#include "Input.h"
#include <Windows.h>

// static 변수 정의.
Input* Input::instance = nullptr;

Input::Input()
{
	// 싱글톤 실행을 위해 instance 변수 설정.
	instance = this;
}

bool Input::IsMouseOver(const Vector2& topLeft, const Vector2& bottomRight) const
{
	const Vector2& m = mouseState.position;
	return (m.x >= topLeft.x && m.x <= bottomRight.x &&
		m.y >= topLeft.y && m.y <= bottomRight.y);
}

void Input::ProcessInput()
{
	// 키보드 입력 처리
	for (int ix = 0; ix < 255; ++ix)
	{
		keyStates[ix].isKeyDown
			= GetAsyncKeyState(ix) & 0x8000;
	}

	// 마우스 입력 처리
	POINT p;
	GetCursorPos(&p);
	HWND hwnd = GetConsoleWindow();
	ScreenToClient(hwnd, &p);

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_FONT_INFO fontInfo;
	GetCurrentConsoleFont(hOut, FALSE, &fontInfo);
	COORD fontSize = GetConsoleFontSize(hOut, fontInfo.nFont);

	if (fontSize.X == 0) fontSize.X = 8; // 기본 글자 폭(px)
	if (fontSize.Y == 0) fontSize.Y = 12; // 기본 글자 높이(px)
	mouseState.position.x = p.x / fontSize.X;
	mouseState.position.y = p.y / fontSize.Y;

	bool nowLeft = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
	bool nowRight = (GetAsyncKeyState(VK_RBUTTON) & 0x8000);

	mouseState.leftPressed = (!mouseState.prevLeftDown && nowLeft);
	mouseState.rightPressed = (!mouseState.prevRightDown && nowRight);

	mouseState.leftDown = nowLeft;
	mouseState.rightDown = nowRight;
}

void Input::SavePreviouseKeyStates()
{
	for (int ix = 0; ix < 255; ++ix)
	{
		keyStates[ix].previouseKeyDown
			= keyStates[ix].isKeyDown;
	}
}

void Input::SavePreviouseMouseStates()
{
	mouseState.prevLeftDown = mouseState.leftDown;
	mouseState.prevRightDown = mouseState.rightDown;
}

bool Input::GetKey(int keyCode)
{
	return keyStates[keyCode].isKeyDown;
}

bool Input::GetKeyDown(int keyCode)
{
	return !keyStates[keyCode].previouseKeyDown
		&& keyStates[keyCode].isKeyDown;
}

bool Input::GetKeyUp(int keyCode)
{
	return keyStates[keyCode].previouseKeyDown
		&& !keyStates[keyCode].isKeyDown;
}

Input& Input::Get()
{
	return *instance;
}