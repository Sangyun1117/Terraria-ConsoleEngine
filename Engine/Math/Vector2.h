// 2차원 벡터(정수형 x, y 좌표)를 표현하는 클래스.
#pragma once
#include "Core.h"
#include <Windows.h>

class Engine_API Vector2
{
public:
	Vector2(int x = 0, int y = 0);
	~Vector2();

	Vector2 operator+(const Vector2& other) const;
	Vector2 operator-(const Vector2& other) const;
	bool operator==(const Vector2& other) const;

	// 형변환 연산자 오버로딩.
	operator COORD();
	operator COORD() const;

	static Vector2 Zero;
	static Vector2 One;
	static Vector2 Up;
	static Vector2 Down;
	static Vector2 Right;
	static Vector2 Left;

public:
	// X/Y 좌표.
	int x = 0;
	int y = 0;
};