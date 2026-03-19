#include "GameLevel.h"
#include "Game/Game.h"
#include "Actor/Player.h"
#include "Actor/Ghost.h"
#include "Actor/GrassBlock.h"
#include "Actor/GroundBlock.h"
#include "Core/Engine.h"
#include "Settings/ObjectDefines.h"
#include "Core/Core.h"
#include "UI/InventoryUI.h"
#include <iostream>
#include <string>
GameLevel::GameLevel()
{
	//맵 읽기
	ReadMapFile("chunk_0_0.txt");
	//ui 추가
	InventoryUI* hpUI = new InventoryUI(InventoryX, InventoryY);
	AddUI(hpUI);
}

GameLevel::~GameLevel()
{
	for (auto& row : mapData) {
		for (auto& blockPtr : row) {
			SafeDelete(blockPtr); // new Block() 해제
		}
		row.clear();
	}
	mapData.clear();
	player = nullptr;
}

void GameLevel::BeginPlay()
{
	super::BeginPlay();
}

void GameLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);
	//esc 누르면 메뉴레벨로 이동
	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		static_cast<Game&>(Engine::Get()).ChangeLevel(LEVEL_NUM_MENU);
	}
}

void GameLevel::Render()
{
	SettingBackground();

	for (auto blockRow : mapData) {
		for (Block* block : blockRow) {
			if (block != nullptr)
				block->Render();
		}
	}

	// 그리기 전에 정렬 순서 기준으로 재배치(정렬).
	SortActorsBySortingOrder();

	// Render Pass.
	for (Actor* const actor : actors)
	{
		// 액터 처리 여부 확인.
		if (!actor->isActive || actor->isExpired)
		{
			continue;
		}

		// 검사 (같은 위치에 정렬 순서 높은 액터가 있는지 확인).
		Actor* searchedActor = nullptr;
		for (Actor* const otherActor : actors)
		{
			// 같은 액터면 무시.
			if (actor == otherActor)
			{
				continue;
			}

			// 위치가 같은 액터 확인.
			if (actor->GetPosition() == otherActor->GetPosition())
			{
				// 정렬 순서 비교 후 액터 저장.
				if (actor->GetSortingOrder() < otherActor->GetSortingOrder())
				{
					// 저장 및 루프 종료.
					searchedActor = otherActor;
					break;
				}
			}
		}

		// 어떤 액터와 같은 위치에 정렬 순서가 더 높은 액터가 있으면,
		// 그리지 않고 건너뛰기.
		if (searchedActor)
		{
			continue;
		}
		// 드로우 콜.
		actor->Render();
	}

	// UI 렌더링
	for (UIElement* const ui : uiElements)
	{
		if (!ui->isVisible) continue;
		ui->Render();
	}

}

//월드y값 기준 배경색 세팅
void GameLevel::SettingBackground()
{
	WORD skyColor =
		BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY; // 하늘색 배경
	WORD groundColor = 0; // 검정 배경

	WORD nowColor = skyColor;

	int screenWidth = Engine::Get().GetScreenWidth();
	int screenHeight = Engine::Get().GetScreenHeight();
	int groundLine = 55;
	for (int y = 0; y < Engine::Get().GetScreenHeight(); ++y)
	{
		nowColor = (Engine::Get().cameraPos.y + y >= groundLine) ? groundColor : skyColor;
		for (int x = 0; x < screenWidth; ++x)
		{
			CHAR_INFO& buffer = Engine::Get().imageBuffer[(y * (screenWidth)) + x];
			buffer.Char.UnicodeChar = ' ';
			buffer.Attributes = nowColor;
		}

		// 각 줄 끝에 개행 문자 추가.
		CHAR_INFO& buffer = Engine::Get().imageBuffer[(y * (screenWidth)) + screenWidth];
		buffer.Char.UnicodeChar = '\n';
		buffer.Attributes = nowColor;
	}

	// 마지막에 널 문자 추가.
	CHAR_INFO& buffer = Engine::Get().imageBuffer[(screenWidth)*screenHeight];
	buffer.Char.UnicodeChar = '\0';
	buffer.Attributes = nowColor;
}



//맵 읽어오기
void GameLevel::ReadMapFile(const char* filename)
{
	mapData = std::vector<std::vector<Block*>>(MAP_HEIGHT, std::vector<Block*>(MAP_WIDTH, nullptr)); //맵데이터 벡터, 1청크 32x16
	//파일 읽어오기
	char filepath[256] = { };
	sprintf_s(filepath, 256, "../Assets/Maps/%s", filename);
	FILE* file = nullptr;
	fopen_s(&file, filepath, "rt");

	// 예외처리.
	if (file == nullptr)
	{
		std::cout << "맵 파일 읽기 실패: " << filename << "\n";
		__debugbreak();
		return;
	}

	//파일 크기 구하기
	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	rewind(file);

	// 확인한 파일 크기를 활용해 버퍼 할당.
	char* buffer = new char[fileSize + 1];
	memset(buffer, 0, fileSize + 1);
	size_t readSize = fread(buffer, sizeof(char), fileSize, file);

	// 배열 순회를 위한 인덱스 변수.
	int index = 0;
	// 문자열 길이 값.
	int size = (int)readSize;
	// x, y 좌표.
	Vector2 gridPos;
	// 문자 배열 순회.
	while (index < size)
	{
		// 맵 문자 확인.
		char mapCharacter = buffer[index++];

		// 개행 문자 처리.
		if (mapCharacter == '\n')
		{
			// 다음 줄로 넘기면서, x 좌표 초기화.
			gridPos.y++;
			gridPos.x = 0;
			continue;
		}

		// 각 문자 별로 처리.
		switch (mapCharacter)
		{

		case 'B': {
			// 위치에 맞게 mapData에 저장
			if (gridPos.y >= 0 && gridPos.y < (int)mapData.size() &&
				gridPos.x >= 0 && gridPos.x < (int)mapData[0].size()) {
				GrassBlock* block = new GrassBlock(gridPos.x * 10, gridPos.y * 5);
				mapData[gridPos.y][gridPos.x] = block;
			}
			break;
		}
		case 'L': {
			// 위치에 맞게 mapData에 저장
			if (gridPos.y >= 0 && gridPos.y < (int)mapData.size() &&
				gridPos.x >= 0 && gridPos.x < (int)mapData[0].size()) {
				GroundBlock* block = new GroundBlock(gridPos.x * 10, gridPos.y * 5);
				mapData[gridPos.y][gridPos.x] = block;
			}
			break;
		}
		case 'P': {
			player = new Player(gridPos.x * 10, gridPos.y * 5);
			AddActor(player);
			break;
		}
		case 'G': {
			Ghost* ghost = new Ghost(gridPos.x * 10, gridPos.y * 5);
			AddActor(ghost);
			break;
		}
		default:
			break;
		}

		gridPos.x++;
	}

	// 버퍼 해제.
	delete[] buffer;

	// 파일 닫기.
	fclose(file);

	//카메라 좌표를 플레이어가 화면 중심인 화면의 왼쪽상단으로 위치
	Engine::Get().cameraPos.x = player->GetPosition().x - Engine::Get().GetScreenWidth() / 2;
	Engine::Get().cameraPos.y = player->GetPosition().y - Engine::Get().GetScreenHeight() / 2;
}
