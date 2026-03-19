//홈레벨은 게임시작 화면 입니다.
//start를 누르면 게임화면으로 이동하고 exit나 esc를 누르면 프로그램을 종료합니다.
#pragma once
#include "Level/Level.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include <vector>

//홈 화면 메뉴
struct HomeMenuItem
{
	typedef void (*OnSelected)();

	HomeMenuItem(const char* text, OnSelected onSelected) : onSelected(onSelected) {
		size_t length = strlen(text) + 1;
		menuText = new char[length];
		strcpy_s(menuText, length, text);
	}
	~HomeMenuItem() {
		SafeDeleteArray(menuText);
	}

	//메뉴 텍스트
	char* menuText = nullptr;

	//메뉴 선택 시 실행할 동작
	OnSelected onSelected = nullptr;
};


class HomeLevel : public Level {
	RTTI_DECLARATIONS(HomeLevel, Level)
public:
	HomeLevel();
	~HomeLevel();

	virtual void Tick(float deltaTime) override;
	virtual void Render() override;
private:
	//배경 설정
	void SettingBackground();
private:
	//아이템 선택 색상
	bool isHoverGameStart = false;
	bool isHoverExit = false;
	Color selectedTextColor = Color::Green;
	Color selectedBagroundColor = Color::Blue;
	//아이템 선택 안됐을 때 색상
	Color unselectedTextColor = Color::White;
	Color unselectedBagroundColor = Color::Gray;
	//아아템 배열
	std::vector<HomeMenuItem*> items;
	//메뉴 아이템 수
	int length = 0;

	//버튼 위치
	Vector2 gameStartButtonLeftTopXY;
	Vector2 gameStartButtonRightBottomXY;
	Vector2 exitButtonLeftTopXY;
	Vector2 exitButtonRightBottomXY;

	//메인 타이틀
	const std::vector<std::vector<char>>* titleImage = nullptr;
	const std::vector<std::vector<Color>>* titleFgs = nullptr;
	const std::vector<std::vector<Color>>* titleBgs = nullptr;

};

