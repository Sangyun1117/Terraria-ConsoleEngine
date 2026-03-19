/******************************************************************************
 * Input.h
 *
 * 키보드와 마우스 입력 상태를 관리하는 싱글톤 클래스
 * 프레임 단위로 입력 상태를 처리하여 "눌림", "떼짐", "지속 눌림" 등을 구분
 * 
 ******************************************************************************/
#pragma once
#include "Core.h"
#include "Math/Vector2.h"

//마우스 상태정보 구조체
struct MouseState
{
	Vector2 position; // 문자 좌표 기준 마우스 위치
	bool prevLeftDown = false; //이전 프레임에 마우스 왼쪽 버튼이 눌려있었는지
	bool prevRightDown = false;
	bool leftDown = false; //현재 프레임에 마우스 왼쪽 버튼이 눌려 있는지
	bool rightDown = false;
	bool leftPressed = false; //이번 프레임에 새로 눌렸는지 leftPressed = !prevLeftDown && leftDown;
	bool rightPressed = false;
};

class Engine_API Input
{
	// friend 선언.
	friend class Engine;

	// 키입력 확인을 위한 구조체 선언.
	struct KeyState
	{
		// 현재 프레임에 키가 눌렸는지 여부.
		bool isKeyDown = false;

		// 이전 프레임에 키가 눌렸는지 여부.
		bool previouseKeyDown = false;
	};

public:
	Input();
	static Input& Get();

	// 키보드 클릭 확인 함수.
	bool GetKey(int keyCode);
	bool GetKeyDown(int keyCode);
	bool GetKeyUp(int keyCode);

	// 마우스 클릭 확인 함수
	const Vector2& GetMousePosition() const { return mouseState.position; }
	bool GetMouseLeft() const { return mouseState.leftDown; } //마우스 클릭되어 있는지
	bool GetMouseLeftDown() const { return mouseState.leftPressed; } //이번 프레임에 새로눌렸는지
	bool GetMouseLeftUp() const { return mouseState.prevLeftDown && !mouseState.leftDown; } //이번 프레임에 마우스를 뗐는지
	bool IsMouseOver(const Vector2& topLeft, const Vector2& bottomRight) const;

private:
	void ProcessInput();
	void SavePreviouseKeyStates();
	void SavePreviouseMouseStates();

private:
	static Input* instance;
	// 키 입력 정보 관리 변수.
	KeyState keyStates[256] = { };
	MouseState mouseState;
};