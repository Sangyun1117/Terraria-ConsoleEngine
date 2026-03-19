#include "Actor.h"
#include "Utils/Utils.h"
#include "Engine.h"
#include "Level/Level.h"
#include "Math/Vector2.h"
#include "ImageManager.h"
#include <Windows.h>
#include <iostream>
#include <fstream>

Actor::Actor(const std::string& imageLink, const Vector2& position, const std::vector<std::vector<char>>& images, const std::vector<std::vector<Color>>& bgs, const std::vector<std::vector<Color>>& fgs)
	: imageLink(imageLink), position(position), asciiImages(images), bgs(bgs), fgs(fgs) {
}

Actor::~Actor()
{
}

// 객체 생애주기(Lifetime)에 1번만 호출됨 (초기화 목적).
void Actor::BeginPlay()
{
	hasBeganPlay = true;
}

// 프레임 마다 호출 (반복성 작업/지속성이 필요한 작업).
void Actor::Tick(float deltaTime)
{
	if (hp <= 0) {
		Destroy();
	}
}

// 그리기 함수.
void Actor::Render()
{
	Vector2 actorPos = { position.x - Engine::Get().cameraPos.x, position.y - Engine::Get().cameraPos.y };

	// 화면 밖이면 아예 그리지 않음(필요시 영역 검사)
	if (actorPos.x >= Engine::Get().GetScreenWidth() || actorPos.y >= Engine::Get().GetScreenHeight())
		return;

	//버퍼에 저장
	Engine::Get().WriteToBuffer(actorPos, asciiImages, fgs, bgs);
}

void Actor::OnAttacked(int damage) {}

void Actor::Move(Vector2 delta) { }

void Actor::SetPosition(const Vector2& newPosition)
{
	position = newPosition;
}
void Actor::SetPosition(int x, int y) {
	position = { x,y };
}
Vector2 Actor::GetPosition() const
{
	return position;
}

void Actor::SetSortingOrder(unsigned int sortingOrder)
{
	this->sortingOrder = sortingOrder;
}

void Actor::SetOwner(Level* newOwner)
{
	owner = newOwner;
}

Level* Actor::GetOwner()
{
	return owner;
}

void Actor::Destroy()
{
	// 중복 삭제 방지 처리.
	if (isExpired)
	{
		return;
	}

	// 삭제 요청 되었다고 설정.
	isExpired = true;

	// 레벨에 삭제 요청.
	owner->DestroyActor(this);
}

//겹치는지 확인
bool Actor::isIntersect(Vector2 myIntersectTL, Vector2 myIntersectBR, Vector2 otherRangeTL, Vector2 otherRangeBR)
{
	if (myIntersectTL.x > otherRangeBR.x || myIntersectBR.x < otherRangeTL.x || myIntersectTL.y > otherRangeBR.y || myIntersectBR.y < otherRangeTL.y)
		return false;
	return true;
}

void Actor::QuitGame()
{
	Engine::Get().Quit();
}