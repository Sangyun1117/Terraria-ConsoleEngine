//메뉴레벨은 게임 진행 중 들어갈 수 있는 설정화면입니다.
//resume이나 esc을 누르면 다시 게임화면으로 돌아가고 home을 누르면 게임을 종료하고 홈화면으로 이동합니다.
#pragma once
#include "Level/Level.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include <vector>

//메뉴 화면 메뉴
struct MenuMenuItem {
	typedef void (*OnSelected)();

	MenuMenuItem(const char* text, OnSelected onSelected) : onSelected(onSelected) {
		size_t length = strlen(text) + 1;
		menuText = new char[length];
		strcpy_s(menuText, length, text);
	}
	~MenuMenuItem() {
		SafeDeleteArray(menuText);
	}

	//메뉴 텍스트
	char* menuText = nullptr;

	//메뉴 선택 시 실행할 동작
	OnSelected onSelected = nullptr;
};

class MenuLevel : public Level {
	RTTI_DECLARATIONS(MenuLevel, Level)
public:
	MenuLevel();
	~MenuLevel();

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
	std::vector<MenuMenuItem*> items;
	//메뉴 아이템 수
	int length = 0;

	//버튼 위치
	Vector2 gameResumeButtonLeftTopXY;
	Vector2 gameResumeButtonRightBottomXY;
	Vector2 homeButtonLeftTopXY;
	Vector2 homeButtonRightBottomXY;

	//메뉴 타이틀
	const std::vector<std::vector<char>>* titleImage = nullptr;
	const std::vector<std::vector<Color>>* titleFgs = nullptr;
	const std::vector<std::vector<Color>>* titleBgs = nullptr;
};