#include "MenuLevel.h"
#include "Utils/Utils.h"
#include "Game/Game.h"
#include "Core/ImageManager.h"
#include <iostream>
#include "Input.h"
MenuLevel::MenuLevel()
{

	items.emplace_back(new MenuMenuItem("Resume Game", []() { static_cast<Game&>(Engine::Get()).ChangeLevel(LEVEL_NUM_GAME); }));

	items.emplace_back(new MenuMenuItem("Home", []() { static_cast<Game&>(Engine::Get()).QuitLevel(LEVEL_NUM_GAME, LEVEL_NUM_HOME); }));

	//아이템 수 미리 저장
	length = static_cast<int>(items.size());

	// 메뉴 타이틀 로딩
	titleImage = &ImageManager::Get().GetImage("../Assets/Images/MenuTitle.txt");
	titleFgs = &ImageManager::Get().GetColor("../Assets/Colors/MenuTitleColors.txt");
	titleBgs = &ImageManager::Get().GetColor("../Assets/Colors/MenuTitleColors.txt");

	//버튼 기본 문자열
	const wchar_t* startLine = L"         GAME RESUME         ";
	const wchar_t* exitLine =  L"            HOME             ";

	//버튼 중앙 좌표 계산
	int textLength = static_cast<int>(wcslen(startLine));
	int startX = (Engine::Get().GetScreenWidth() - textLength) / 2;
	int startY = Engine::Get().GetScreenHeight() - 12;

	// 좌표 저장
	gameResumeButtonLeftTopXY = { startX, startY };
	gameResumeButtonRightBottomXY = { startX + textLength, startY + 2 };

	homeButtonLeftTopXY = { startX, startY + 3 };
	homeButtonRightBottomXY = { startX + textLength, startY + 5 };
}

MenuLevel::~MenuLevel()
{
	for (MenuMenuItem* item : items) {
		SafeDelete(item);
	}
	items.clear();
}

void MenuLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);
	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		items[0]->onSelected();
	}

	//마우스가 버튼 위에 있는지 확인
	isHoverGameStart = Input::Get().IsMouseOver(gameResumeButtonLeftTopXY, gameResumeButtonRightBottomXY);
	isHoverExit = Input::Get().IsMouseOver(homeButtonLeftTopXY, homeButtonRightBottomXY);
	//마우스 입력 처리
	if (Input::Get().GetMouseLeftDown() &&
		Input::Get().IsMouseOver(gameResumeButtonLeftTopXY, gameResumeButtonRightBottomXY))
	{
		items[0]->onSelected();
	}

	if (Input::Get().GetMouseLeftDown() &&
		Input::Get().IsMouseOver(homeButtonLeftTopXY, homeButtonRightBottomXY))
	{
		items[1]->onSelected();
	}
}

void MenuLevel::Render()
{
	//배경
	SettingBackground();
	//Main Title

	Engine::Get().WriteToBuffer(Vector2(25, 5), *titleImage, *titleFgs, *titleBgs);

	const wchar_t* startLine = isHoverGameStart ? L"     ➡️  GAME RESUME         " : L"         GAME RESUME         ";
	const wchar_t* exitLine = isHoverExit ?       L"        ➡️   HOME            " : L"             HOME            ";

	// 메뉴 아이템 렌더링.
	Color buttonTextColor = isHoverGameStart ? Color::Red : Color::Black;
	Engine::Get().WriteToWcharBuffer(
		{ gameResumeButtonLeftTopXY.x, gameResumeButtonLeftTopXY.y + 1 },
		startLine, buttonTextColor, Color::BrightCyan
	);

	buttonTextColor = isHoverExit ? Color::Yellow : Color::Black;
	Engine::Get().WriteToWcharBuffer(
		{ homeButtonLeftTopXY.x, homeButtonLeftTopXY.y + 1 },
		exitLine, buttonTextColor, Color::BrightCyan
	);
}

void MenuLevel::SettingBackground() {
	WORD backgroundColor = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY;

	int screenWidth = Engine::Get().GetScreenWidth();
	int screenHeight = Engine::Get().GetScreenHeight();
	for (int y = 0; y < Engine::Get().GetScreenHeight(); ++y)
	{
		for (int x = 0; x < screenWidth; ++x)
		{
			CHAR_INFO& buffer = Engine::Get().imageBuffer[(y * (screenWidth)) + x];
			buffer.Char.UnicodeChar = ' ';
			buffer.Attributes = backgroundColor;
		}

		// 각 줄 끝에 개행 문자 추가.
		CHAR_INFO& buffer = Engine::Get().imageBuffer[(y * (screenWidth)) + screenWidth];
		buffer.Char.UnicodeChar = '\n';
		buffer.Attributes = backgroundColor;
	}

	// 마지막에 널 문자 추가.
	CHAR_INFO& buffer = Engine::Get().imageBuffer[(screenWidth)*screenHeight];
	buffer.Char.UnicodeChar = '\0';
	buffer.Attributes = backgroundColor;
}