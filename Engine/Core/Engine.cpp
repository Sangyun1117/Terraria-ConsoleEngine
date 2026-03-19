#include "Engine.h"
#include <iostream>
#include <Windows.h>

#include "Level/Level.h"
#include "Utils/Utils.h"
#include "Input.h"

Engine* Engine::instance = nullptr;

//Windows 콘솔 이벤트(종료)가 실행되면 자동으로 호출되는 함수.
BOOL WINAPI Engine::ConsoleHandler(DWORD signal)
{
	if (signal == CTRL_CLOSE_EVENT || signal == CTRL_LOGOFF_EVENT || signal == CTRL_SHUTDOWN_EVENT)
	{
		Engine::Get().CleanUp();

		//메모리 누수 체크
		_CrtDumpMemoryLeaks();
		return TRUE;
	}

	return FALSE;
}

Engine::Engine()
{
	instance = this;

	// 콘솔 커서 끄기.
	CONSOLE_CURSOR_INFO info;
	info.bVisible = false;
	info.dwSize = 1;

	SetConsoleCursorInfo(
		GetStdHandle(STD_OUTPUT_HANDLE),
		&info
	);
	// 엔진 설정 로드.
	LoadEngineSettings();

	// 랜덤 종자값(seed) 설정.
	srand(static_cast<unsigned int>(time(nullptr)));

	// 이미지 버퍼 생성.
	Vector2 screenSize(settings.width, settings.height);
	imageBuffer = new CHAR_INFO[(screenSize.x + 1) * screenSize.y + 1];

	// 버퍼 초기화 (문자 버퍼).
	ClearImageBuffer();

	// 두 개의 버퍼 생성.
	renderTargets[0] = new ScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE), screenSize);
	renderTargets[1] = new ScreenBuffer(screenSize);

	// 버퍼 교환.
	Present();

	// 화면 세팅
	ScreenSettings();

	// 콘솔 창 이벤트 등록.
	SetConsoleCtrlHandler(ConsoleHandler, TRUE);

	//빠른 편집 모드 삭제(드래그 방지)
	DisableQuickEditMode();
}

Engine::~Engine()
{
	CleanUp();
}

void Engine::Run()
{
	// 밀리 세컨드 단위로 현재 시간을 알려줌.
	//float currentTime = timeGetTime();
	LARGE_INTEGER currentTime;
	LARGE_INTEGER previousTime;
	QueryPerformanceCounter(&currentTime);
	previousTime = currentTime;

	// 하드웨어 시계의 정밀도(주파수) 가져오기.
	// 나중에 초로 변환하기 위해.
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	// 타겟 프레임.
	float targetFrameRate = settings.framerate == 0.0f ? 60.0f : settings.framerate;

	// 타겟 한 프레임 시간.
	float oneFrameTime = 1.0f / targetFrameRate;

	// GameLoop.
	while (true)
	{
		// 엔진 종료 여부 확인.
		if (isQuit)
		{
			// 루프 종료.
			break;
		}

		// 프레임 시간 계산.
		// (현재 시간 - 이전 시간) / 주파수 = 초단위.
		QueryPerformanceCounter(&currentTime);

		// 프레임 시간.
		float deltaTime = (currentTime.QuadPart - previousTime.QuadPart) / (float)frequency.QuadPart;

		this->deltaTime = deltaTime;

		// 입력은 최대한 빨리.
		input.ProcessInput();

		// 고정 프레임.
		if (deltaTime >= oneFrameTime)
		{
			BeginPlay();
			Tick(deltaTime);
			Render();

			// 시간 업데이트.
			previousTime = currentTime;

			// 현재 프레임의 입력을 기록.
			input.SavePreviouseKeyStates();
			input.SavePreviouseMouseStates();

			// 이전 프레임에 추가 및 삭제 요청된 액터 처리.
			if (mainLevel)
			{
				mainLevel->ProcessAddAndDestroyActors();
			}
		}
	}

	// 정리(텍스트 색상 원래대로 돌려놓기).
	Utils::SetConsoleTextColor(
		FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED
	);
}

//한 문장을 받아서 버퍼에 저장
void Engine::WriteToBuffer(const Vector2& position, const char* image, Color fgColor, Color bgColor)
{
	int length = static_cast<int>(strlen(image));

	for (int ix = 0; ix < length; ++ix)
	{
		int x = position.x + ix;
		int y = position.y;

		// 콘솔 화면 범위 밖이면 건너뜀
		if (x < 0 || x >= settings.width || y < 0 || y >= settings.height)
			continue;

		int index = (y * settings.width) + x;

		wchar_t wch = static_cast<wchar_t>(image[ix]); //아스키코드 유니코드로 단순 변환
		imageBuffer[index].Char.UnicodeChar = wch;
		//imageBuffer[index].Char.AsciiChar = image[ix];
		imageBuffer[index].Attributes = (WORD)fgColor | ((WORD)bgColor << 4);
	}
}

//전체 텍스트를 받아서 한 단어씩 버퍼에 저장
void Engine::WriteToBuffer(
	const Vector2& position,
	const std::vector<std::vector<char>>& image,
	const std::vector<std::vector<Color>>& fgColors,
	const std::vector<std::vector<Color>>& bgColors)
{
	int height = static_cast<int>(image.size());
	if (height == 0) return;
	int width = static_cast<int>(image[0].size());

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int drawX = position.x + x;
			int drawY = position.y + y;

			// 범위 밖 체크
			if (drawX < 0 || drawX >= settings.width || drawY < 0 || drawY >= settings.height)
				continue;

			// 공백이면 아무 것도 안 그리기
			if (image[y][x] == ' ')
				continue;

			int index = drawY * settings.width + drawX;

			//imageBuffer[index].Char.AsciiChar = image[y][x];
			wchar_t wch = static_cast<wchar_t>(image[y][x]); // 단순 변환
			imageBuffer[index].Char.UnicodeChar = wch;
			WORD attr = static_cast<WORD>(fgColors[y][x]) | (static_cast<WORD>(bgColors[y][x]) << 4);
			imageBuffer[index].Attributes = attr;
		}
	}
}

//한 문장을 받아서 버퍼에 저장(유니코드)
void Engine::WriteToWcharBuffer(const Vector2& position, const wchar_t* image, Color fgColor, Color bgColor)
{
	int length = static_cast<int>(wcslen(image));

	for (int ix = 0; ix < length; ++ix)
	{
		int x = position.x + ix;
		int y = position.y;

		// 콘솔 화면 범위 밖이면 건너뜀
		if (x < 0 || x >= settings.width || y < 0 || y >= settings.height)
			continue;

		int index = (y * settings.width) + x;

		imageBuffer[index].Char.UnicodeChar = image[ix];
		imageBuffer[index].Attributes = (WORD)fgColor | ((WORD)bgColor << 4);
	}
}

//레벨 추가, 각 씬을 레벨로 관리
void Engine::AddLevel(Level* newLevel)
{
	// 기존에 있던 레벨은 제거.
	if (mainLevel)
	{
		delete mainLevel;
	}

	mainLevel = newLevel;
}

void Engine::CleanUp()
{
	// 렌더 타겟 삭제.
	SafeDelete(renderTargets[0]);
	SafeDelete(renderTargets[1]);

	//레벨 삭제
	SafeDelete(mainLevel);

	// 문자 버퍼 삭제.
	SafeDeleteArray(imageBuffer);
}

void Engine::Quit()
{
	// 종료 플래그 설정.
	isQuit = true;
}

Engine& Engine::Get()
{
	return *instance;
}

int Engine::GetScreenWidth() const
{
	return settings.width;
}

int Engine::GetScreenHeight() const
{
	return settings.height;
}

void Engine::BeginPlay()
{
	if (mainLevel)
	{
		mainLevel->BeginPlay();
	}
}

void Engine::Tick(float deltaTime)
{
	if (mainLevel)
	{
		mainLevel->Tick(deltaTime);
	}
}

void Engine::Clear() {
	ClearImageBuffer();
	GetRenderer()->Clear();
}

void Engine::Render()
{
	//화면 지우기
	Clear();
	//레벨 그리기
	if (mainLevel)
	{
		mainLevel->Render();
	}
	GetRenderer()->Render(imageBuffer);
	//버퍼 교환
	Present();
}

void Engine::Present()
{
	// 버퍼 교환.
	SetConsoleActiveScreenBuffer(GetRenderer()->buffer);

	// 인덱스 뒤집기. 1->0, 0->1.
	currentRenderTargetIndex = 1 - currentRenderTargetIndex;
}

void Engine::LoadEngineSettings()
{
	// 엔진 설정 파일 열기.
	FILE* file = nullptr;
	fopen_s(&file, "../Settings/EngineSettings.txt", "rt");
	if (file == nullptr)
	{
		std::cout << "Failed to load engine settings.\n";
		__debugbreak();
		return;
	}

	// 로드.

	// FP(File Position) 포인터를 가장 뒤로 옮기기.
	fseek(file, 0, SEEK_END);

	// 이 위치 구하기.
	size_t fileSize = ftell(file);

	// 다시 첫 위치로 되돌리기.
	//fseek(file, 0, SEEK_SET);
	rewind(file);

	// 파일 내용을 저장할 버퍼 할당.
	char* buffer = new char[fileSize + 1];
	memset(buffer, 0, fileSize + 1);

	// 내용 읽기.
	size_t readSize = fread(buffer, sizeof(char), fileSize, file);

	// 파싱(Parcing, 구문 해석->필요한 정보를 얻는 과정).
	char* context = nullptr;
	char* token = nullptr;

	token = strtok_s(buffer, "\n", &context);

	// 구문 분석.
	while (token != nullptr)
	{
		// 키/값 분리.
		char header[10] = { };

		// 아래 구문이 제대로 동작하려면 키와 값 사이의 빈칸이 있어야 함.
		sscanf_s(token, "%s", header, 10);

		// 헤더 문자열 비교.
		if (strcmp(header, "framerate") == 0)
		{
			sscanf_s(token, "framerate = %f", &settings.framerate);
		}
		else if (strcmp(header, "width") == 0)
		{
			sscanf_s(token, "width = %d", &settings.width);
		}
		else if (strcmp(header, "height") == 0)
		{
			sscanf_s(token, "height = %d", &settings.height);
		}

		// 그 다음줄 분리.
		token = strtok_s(nullptr, "\n", &context);
	}


	// 버퍼 해제.
	SafeDeleteArray(buffer);

	// 파일 닫기.
	fclose(file);
}

//백버퍼를 반환한다.
ScreenBuffer* Engine::GetRenderer() const
{
	return renderTargets[currentRenderTargetIndex];
}

void Engine::ClearImageBuffer() //화면 초기화
{
	//기본 배경
	//WORD defaultColor =
	//	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | // 흰 글자
	//	BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY; // 하늘색 배경

	// 글자 버퍼 덮어쓰기.
	for (int y = 0; y < settings.height; ++y)
	{
		for (int x = 0; x < settings.width; ++x)
		{
			CHAR_INFO& buffer = imageBuffer[(y * (settings.width + 1)) + x];
			buffer.Char.AsciiChar = ' ';
			buffer.Attributes = 0;
		}

		// 각 줄 끝에 개행 문자 추가.
		CHAR_INFO& buffer = imageBuffer[(y * (settings.width + 1)) + settings.width];
		buffer.Char.AsciiChar = '\n';
		buffer.Attributes = 0;
	}

	// 마지막에 널 문자 추가.
	CHAR_INFO& buffer = imageBuffer[(settings.width + 1) * settings.height];
	buffer.Char.AsciiChar = '\0';
	buffer.Attributes = 0;
}

void Engine::ScreenSettings()
{
	for (int i = 0; i < 2; ++i) {
		HANDLE hOut = renderTargets[i]->GetHandle();

		// 1. 윈도우 창 크기를 가장 작게 설정. 윈도우 창이 버퍼보다 크면 오류가 남.
		SMALL_RECT tempWindow = { 0,0,1,1 };
		SetConsoleWindowInfo(hOut, TRUE, &tempWindow);

		// 2. 버퍼 크기 설정
		COORD bufferSize = { static_cast<SHORT>(settings.width), static_cast<SHORT>(settings.height) };
		if (!SetConsoleScreenBufferSize(hOut, bufferSize))
		{
			std::cerr << "SetConsoleScreenBufferSize 실패 buffer" << i << "\n";
			continue;
		}

		// 3. 콘솔 창 크기 설정
		SMALL_RECT windowSize = { 0, 0, static_cast<SHORT>(settings.width - 1), static_cast<SHORT>(settings.height - 1) };
		if (!SetConsoleWindowInfo(hOut, TRUE, &windowSize))
		{
			std::cerr << "SetConsoleWindowInfo 실패\n";
			return;
		}

		if (i == 0) {
			// 4. 콘솔 창 중앙으로 이동
			HWND consoleWindow = GetConsoleWindow();
			if (consoleWindow == nullptr) {
				std::cerr << "콘솔 창 핸들을 가져올 수 없습니다.\n";
				return;
			}

			// 현재 콘솔 폰트의 픽셀 크기 얻기
			CONSOLE_FONT_INFO fontInfo;
			if (!GetCurrentConsoleFont(hOut, FALSE, &fontInfo)) {
				std::cerr << "콘솔 폰트 정보 가져오기 실패\n";
				return;
			}

			COORD fontSize = GetConsoleFontSize(hOut, fontInfo.nFont); // 픽셀 단위

			// 클라이언트 영역 크기 (콘솔 화면 기준)
			int clientWidth = fontSize.X * settings.width;
			int clientHeight = fontSize.Y * settings.height;

			// 전체 윈도우 크기로 확장
			RECT windowRect = { 0, 0, clientWidth, clientHeight };
			if (!AdjustWindowRect(&windowRect, GetWindowLong(consoleWindow, GWL_STYLE), FALSE)) {
				std::cerr << "AdjustWindowRect 실패\n";
				return;
			}

			int totalWindowWidth = windowRect.right - windowRect.left;
			int totalWindowHeight = windowRect.bottom - windowRect.top;

			// 모니터 중앙 좌표 계산
			int screenWidth = GetSystemMetrics(SM_CXSCREEN);
			int screenHeight = GetSystemMetrics(SM_CYSCREEN);
			int x = (screenWidth - totalWindowWidth) / 2;
			int y = (screenHeight - totalWindowHeight) / 2;

			// 콘솔 창 이동 및 크기 설정
			MoveWindow(consoleWindow, x, y, totalWindowWidth, totalWindowHeight, TRUE);


			// 5. 콘솔창 x버튼 지우기
			// 현재 윈도우 스타일 값을 가져옴.
			LONG_PTR style = GetWindowLongPtr(consoleWindow, GWL_STYLE);

			// 제목 표시줄, 크기 조절 테두리, 시스템 메뉴 등의 스타일을 제거
			style &= ~(WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_VSCROLL | WS_HSCROLL);

			// 변경된 스타일 적용
			SetWindowLongPtr(consoleWindow, GWL_STYLE, style);
		}
	}
}

void Engine::DisableQuickEditMode()
{
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hInput, &mode);
	mode &= ~ENABLE_QUICK_EDIT_MODE;
	mode &= ~ENABLE_INSERT_MODE;
	SetConsoleMode(hInput, mode);
}

