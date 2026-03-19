// 프로젝트에서 다양하게 사용할 유틸리티 함수 모음.
#pragma once
#include <Windows.h>
#include "Math/Vector2.h"
#include "Math/Color.h"

namespace Utils
{
	// 두 값을 서로 바꿀 때 사용하는 함수.
	template<typename T>
	void Swap(T& a, T& b)
	{
		T temp = a;
		a = b;
		b = temp;
	}

	// 콘솔 출력 핸들 가져오는 함수.
	inline HANDLE GetConsoleHandle()
	{
		return GetStdHandle(STD_OUTPUT_HANDLE);
	}

	// 콘솔 텍스트 색상 설정 함수.
	inline void SetConsoleTextColor(WORD color)
	{
		static HANDLE handle = GetConsoleHandle();
		SetConsoleTextAttribute(handle, color);
	}

	inline void SetConsoleTextColor(Color color)
	{
		SetConsoleTextColor(static_cast<WORD>(color));
	}

	//벡터 잘라주는 함수
	template<typename T>
	std::vector<std::vector<T>> Slice2DVector(
		const std::vector<std::vector<T>>& src,
		int startRow, int startCol,
		int height, int width)
	{
		std::vector<std::vector<T>> result;
		int srcHeight = (int)src.size();
		int srcWidth = (int)(src.empty() ? 0 : src[0].size());

		for (int r = startRow; r < startRow + height && r < srcHeight; ++r)
		{
			std::vector<T> row;
			for (int c = startCol; c < startCol + width && c < srcWidth; ++c)
			{
				row.push_back(src[r][c]);
			}
			result.push_back(std::move(row));
		}

		return result;
	}
}