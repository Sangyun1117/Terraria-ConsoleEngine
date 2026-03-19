#pragma once

#include "Core.h"
#include "Input.h"
#include "Math/Vector2.h"
#include "Render/ScreenBuffer.h"
#include "Math/Color.h"
#include <Windows.h>
#include <vector>
// 엔진 설정 구조체.
struct EngineSettings
{
	// 콘솔 화면 가로 크기.
	int width = 0;

	// 콘솔 화면 세로 크기.
	int height = 0;

	// 타겟 프레임 속도.
	float framerate = 0.0f;
};

class Level;
class Engine_API Engine
{
public:

	// 생성자.
	Engine();

	// 소멸자.
	virtual ~Engine();

	// 엔진 실행 함수.
	void Run();

	// 문자열 그리기 요청 함수.
	void WriteToBuffer(const Vector2& position, const char* image, Color fgColor = Color::White, Color bgColor = Color::Black);
	void WriteToBuffer(
		const Vector2& position,
		const std::vector<std::vector<char>>& image,
		const std::vector<std::vector<Color>>& fgColors,
		const std::vector<std::vector<Color>>& bgColors);
	void WriteToWcharBuffer(const Vector2& position, const wchar_t* image, Color fgColor = Color::White, Color bgColor = Color::Black);
	// 레벨 추가 함수.
	void AddLevel(Level* newLevel);

	// 메모리 해제 함수.
	virtual void CleanUp();

	// 종료 함수.
	void Quit();

	// 싱글톤 접근 함수.
	static Engine& Get();

	// 화면 가로/세로 크기 반환 함수.
	int GetScreenWidth() const;
	int GetScreenHeight() const;

	//프레임 시간을 넘겨줌
	float GetDeltaTime() const { return deltaTime; }

	static BOOL WINAPI ConsoleHandler(DWORD signal);
protected:
	void BeginPlay();
	void Tick(float deltaTime = 0.0f);

	// 화면 지우는 함수 (전체를 빈 문자열로 설정).
	void Clear(); 
	// 액터 그리기 함수 (백버퍼에 기록).
	void Render();
	// 버퍼 교환 함수 (프론트 버퍼 <-> 백버퍼)
	void Present();

	// 엔진 설정 로드 함수.
	void LoadEngineSettings();

	// 백버퍼 렌더 타겟 반환 함수.
	ScreenBuffer* GetRenderer() const;

	// 글자 버퍼 지우는 함수.
	void ClearImageBuffer();

	//화면 세팅
	void ScreenSettings();

	//빠른편집모드(마우스 클릭 시 드래그 가능) 끄기
	void DisableQuickEditMode();
public:
	Vector2 cameraPos = { 0,0 };
	CHAR_INFO* imageBuffer = nullptr; // 백버퍼(프레임).
protected:
	// 엔진 종료 플래그.
	bool isQuit = false;
	
	// 메인 레벨.
	Level* mainLevel = nullptr;

	// 입력 관리자.
	Input input;

	// 엔진 설정.
	EngineSettings settings;

	//더블버퍼링을 위한 변수
	ScreenBuffer* renderTargets[2] = { }; // 이중 버퍼.
	int currentRenderTargetIndex = 0; // 백버퍼로 사용하는 렌더 타겟의 인덱스.
	

	//애니메이션을 위한 시간을 넘겨주기위한 변수
	float deltaTime = 0.0f;

	// 싱글톤 변수.
	static Engine* instance;
};