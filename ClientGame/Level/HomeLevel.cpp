#include "HomeLevel.h"
#include "Utils/Utils.h"
#include "Game/Game.h"
#include "Core/ImageManager.h"
#include <iostream>
#include "Input.h"
HomeLevel::HomeLevel()
{
	//메뉴 아이템 추가

	items.emplace_back(new HomeMenuItem("Resume Game", []() { static_cast<Game&>(Engine::Get()).ChangeLevel(LEVEL_NUM_GAME); }));

	items.emplace_back(new HomeMenuItem("Quit Game", []() { Game::Get().Quit(); }));

	//아이템 수 미리 저장
	length = static_cast<int>(items.size());

	// 메인 타이틀 로딩
	titleImage = &ImageManager::Get().GetImage("../Assets/Images/MainTitle.txt");
	titleFgs = &ImageManager::Get().GetColor("../Assets/Colors/MainTitleColors.txt");
	titleBgs = &ImageManager::Get().GetColor("../Assets/Colors/MainTitleColors.txt");

	//버튼 기본 문자열
	const wchar_t* startLine = L"         GAME START         ";
	const wchar_t* exitLine  = L"            EXIT            ";

	//버튼 중앙 좌표 계산
	int textLength = static_cast<int>(wcslen(startLine));
	int startX = (Engine::Get().GetScreenWidth() - textLength) / 2;
	int startY = Engine::Get().GetScreenHeight() - 12;

	// 좌표 저장
	gameStartButtonLeftTopXY = { startX, startY };
	gameStartButtonRightBottomXY = { startX + textLength, startY + 2 };

	exitButtonLeftTopXY = { startX, startY + 3 };
	exitButtonRightBottomXY = { startX + textLength, startY + 5 };
}

HomeLevel::~HomeLevel()
{
	for (HomeMenuItem* item : items) {
		SafeDelete(item);
	}
	items.clear();
}

void HomeLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);
	//ESC 누르면 종료
	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		items[1]->onSelected();
	}

	//마우스가 버튼 위에 있는지 확인
	isHoverGameStart = Input::Get().IsMouseOver(gameStartButtonLeftTopXY, gameStartButtonRightBottomXY);
	isHoverExit = Input::Get().IsMouseOver(exitButtonLeftTopXY, exitButtonRightBottomXY);
	//마우스 입력 처리
	if (Input::Get().GetMouseLeftDown() &&
		Input::Get().IsMouseOver(gameStartButtonLeftTopXY, gameStartButtonRightBottomXY))
	{
		items[0]->onSelected();
	}

	if (Input::Get().GetMouseLeftDown() &&
		Input::Get().IsMouseOver(exitButtonLeftTopXY, exitButtonRightBottomXY))
	{
		items[1]->onSelected();
	}
}

void HomeLevel::Render()
{
	//배경
	SettingBackground();

	//메인 타이틀 출력
	Engine::Get().WriteToBuffer(Vector2(10, 5), *titleImage, *titleFgs, *titleBgs);

	const wchar_t* startLine = isHoverGameStart ? L"     ➡️  GAME START         "  : L"         GAME START         ";
	const wchar_t* exitLine = isHoverExit ?       L"       ➡️   EXIT            " :  L"            EXIT            ";

	// 메뉴 아이템 렌더링.
	Color buttonTextColor = isHoverGameStart ? Color::Red : Color::Black;
	Engine::Get().WriteToWcharBuffer(
		{ gameStartButtonLeftTopXY.x, gameStartButtonLeftTopXY.y + 1 },
		startLine, buttonTextColor, Color::BrightYellow
	);

	buttonTextColor = isHoverExit ? Color::Yellow : Color::Black;
	Engine::Get().WriteToWcharBuffer(
		{ exitButtonLeftTopXY.x, exitButtonLeftTopXY.y + 1 },
		exitLine, buttonTextColor, Color::BrightYellow
	);

}

void HomeLevel::SettingBackground() {
	WORD backgoroundColor = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;

	int screenWidth = Engine::Get().GetScreenWidth();
	int screenHeight = Engine::Get().GetScreenHeight();

	for (int y = 0; y < Engine::Get().GetScreenHeight(); ++y)
	{
		for (int x = 0; x < screenWidth; ++x)
		{
			CHAR_INFO& buffer = Engine::Get().imageBuffer[(y * (screenWidth)) + x];
			buffer.Char.UnicodeChar = ' ';
			buffer.Attributes = backgoroundColor;
		}

		// 각 줄 끝에 개행 문자 추가.
		CHAR_INFO& buffer = Engine::Get().imageBuffer[(y * (screenWidth)) + screenWidth];
		buffer.Char.UnicodeChar = '\n';
		buffer.Attributes = backgoroundColor;
	}

	// 마지막에 널 문자 추가.
	CHAR_INFO& buffer = Engine::Get().imageBuffer[(screenWidth)*screenHeight];
	buffer.Char.UnicodeChar = '\0';
	buffer.Attributes = backgoroundColor;
}