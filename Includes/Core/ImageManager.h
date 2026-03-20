/******************************************************************************
 * ImageManager.h
 *
 * 모든 이미지 파일을 관리하는 클래스
 *
 * 이미지와 컬러파일은 모두 텍스트파일로 관리된다.
 * 이미지는 아스키코드로 저장되어짐
 * 컬러파일은 0~15까지 16진수로 저장되어서 사용됨.
 * 해시테이블을 이용해 이미지 경로로 특정 이미지와 컬러에 접근가능하다.
 *
 ******************************************************************************/
#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include "Math/Color.h"
#include <filesystem>

class ImageManager {
public:
	// 싱글톤 인스턴스 얻기
	static ImageManager& Get() {
		static ImageManager instance;
		return instance;
	}

	std::string NormalizePath(const std::string& path) { //경로가 \\로 들어와서 /로 바꾸는 함수가 필요했음
		std::string normalized = path;
		std::replace(normalized.begin(), normalized.end(), '\\', '/');
		return normalized;
	}

	//경로에 있는 폴더를 읽음. 이미지와 컬러를 구분해서 저장하는데 하드코딩이라 수정필요
	bool LoadImagesFromFolder(const std::string& imageFolderPath, const std::string& colorFolderPath) {
		bool allLoaded = true;

		// 이미지 파일들 로드
		for (const auto& entry : std::filesystem::directory_iterator(imageFolderPath)) {
			if (entry.is_regular_file() && entry.path().extension() == ".txt") {
				const std::string pathStr = NormalizePath(entry.path().string());
				if (!LoadImage(pathStr)) {
					allLoaded = false;
				}
			}
		}

		// 컬러 파일들 로드
		for (const auto& entry : std::filesystem::directory_iterator(colorFolderPath)) {
			if (entry.is_regular_file() && entry.path().extension() == ".txt") {
				const std::string pathStr = NormalizePath(entry.path().string());
				if (!LoadColor(pathStr)) {
					allLoaded = false;
				}
			}
		}

		return allLoaded;
	}

	// 특정 이미지 파일 로드
	bool LoadImage(const std::string& filePath) {
		m_images[filePath] = LoadTextFile(filePath);
		if (m_images[filePath].empty()) {
			std::cerr << "Failed to load image: " << filePath << std::endl;
			return false;
		}
		return true;
	}

	// 특정 컬러 파일 로드
	bool LoadColor(const std::string& filePath) {
		m_colors[filePath] = LoadColorsImage(filePath);
		if (m_colors[filePath].empty()) {
			std::cerr << "Failed to load image: " << filePath << std::endl;
			return false;
		}
		return true;
	}

	// 이미지 가져오기
	const std::vector<std::vector<char>>& GetImage(const std::string& filePath) const {
		static const std::vector<std::vector<char>> empty;
		auto it = m_images.find(filePath);
		return it != m_images.end() ? it->second : empty;
	}

	//컬러 가져오기
	const std::vector<std::vector<Color>>& GetColor(const std::string& filePath) const {
		static const std::vector<std::vector<Color>> empty;
		auto it = m_colors.find(filePath);
		return it != m_colors.end() ? it->second : empty;
	}

private:
	ImageManager() = default;
	~ImageManager() = default;
	ImageManager(const ImageManager&) = delete;
	ImageManager& operator=(const ImageManager&) = delete;

	// 텍스트 파일(이미지) 로드
	std::vector<std::vector<char>> LoadTextFile(const std::string& filePath) {
		std::ifstream file(filePath);
		std::vector<std::vector<char>> result;  //여기에 저장하고 리턴

		if (!file.is_open()) {
			std::cerr << "파일을 열 수 없습니다: " << filePath << std::endl;
			return result;
		}

		std::string line;
		size_t maxWidth = 0;
		std::vector<std::string> tempLines;

		// 먼저 줄들을 읽고 최대 너비 구하기
		while (std::getline(file, line)) {
			if (line.size() > maxWidth) maxWidth = line.size();
			tempLines.push_back(line);
		}

		// 공백으로 패딩해서 result에 저장
		for (const auto& l : tempLines) {
			std::vector<char> row;
			for (size_t i = 0; i < maxWidth; ++i) {
				if (i < l.size())
					row.push_back(l[i]);
				else
					row.push_back(' ');
			}
			result.push_back(std::move(row));
		}

		return result;
	}

	//텍스트 파일(컬러) 로드
	std::vector<std::vector<Color>> LoadColorsImage(const std::string& filePath)
	{
		std::ifstream file(filePath);
		std::vector<std::vector<Color>> result;
		if (!file.is_open()) {
			std::cerr << "컬러 파일을 열 수 없습니다: " << filePath << std::endl;
			return result;
		}

		std::string line;
		size_t maxWidth = 0;
		std::vector<std::string> tempLines;

		// 1. 파일에서 모든 줄 읽기 & 최대 너비 계산
		while (std::getline(file, line)) {
			if (line.size() > maxWidth) maxWidth = line.size();
			tempLines.push_back(line);
		}

		// 2. 패딩 처리하여 결과 생성
		for (const auto& l : tempLines) {
			std::vector<Color> row;
			for (size_t i = 0; i < maxWidth; ++i) {
				if (i < l.size()) {
					row.push_back(ConvertHexCharToColor(l[i]));
				}
				else {
					row.push_back(Color::Transparent); // 패딩 부분은 투명 처리
				}
			}
			result.push_back(std::move(row));
		}

		return result;
	}
private:
	std::unordered_map<std::string, std::vector<std::vector<char>>> m_images;
	std::unordered_map<std::string, std::vector<std::vector<Color>>> m_colors;
};