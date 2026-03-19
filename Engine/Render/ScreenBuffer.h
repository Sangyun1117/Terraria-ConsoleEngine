// ScreenBuffer 클래스
// 콘솔 환경에서 더블 버퍼링(Double Buffering)을 구현하기 위한 클래스.
// 문자열/문자 정보(CHAR_INFO)를 버퍼에 기록한 뒤, 최종적으로 콘솔에 출력.
// 깜빡임 없는 화면 갱신을 위해 사용.
#pragma once
#include <Windows.h>
#include "Math/Vector2.h"

class ScreenBuffer
{
	friend class Engine;

public:
	// 새로운 버퍼를 생성할 때 사용할 생성자.
	ScreenBuffer(const Vector2& screenSize);

	// conslle에 전달된 핸들을 사용해 콘솔 버퍼 생성.
	ScreenBuffer(HANDLE console, const Vector2& screenSize);

	// 소멸자.
	~ScreenBuffer();

	// 버퍼 지우기.
	void Clear();

	// 콘솔 버퍼에 문자열 버퍼를 쓸 때 사용하는 함수.
	void Render(CHAR_INFO* charInfo);

	HANDLE GetHandle() const { return buffer; }
private:

	// 콘솔 핸들.
	HANDLE buffer = nullptr;

	// 콘솔 화면 크기.
	Vector2 screenSize;
};