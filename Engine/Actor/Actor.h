//Actor는 동적객체에서 사용할 함수, 변수를 정의해놓은 클래스
#pragma once
#include "Core.h"
#include "Math/Vector2.h"
#include "Math/Color.h"
#include "RTTI.h"
#include <vector>
#include <string>

// 전방 선언.
class Level;
class Engine_API Actor : public RTTI
{
	friend class Level;
	RTTI_DECLARATIONS(Actor, RTTI)

public:
	Actor(const std::string& imageLink, const Vector2& position, const std::vector<std::vector<char>>& images, const std::vector<std::vector<Color>>& bgs, const std::vector<std::vector<Color>>& fgs);
	virtual ~Actor();

	// 객체 생애주기(Lifetime)에 1번만 호출됨 (초기화 목적).
	virtual void BeginPlay();

	// 프레임 마다 호출 (반복성 작업/지속성이 필요한 작업).
	virtual void Tick(float deltaTime);

	// 그리기 함수.
	virtual void Render();

	// BeginPlay 호출 여부 확인.
	inline bool HasBeganPlay() const { return hasBeganPlay; }

	//데미지 받았을 때 처리 함수
	virtual void OnAttacked(int damage);
	//위치 이동 시키는 함수
	virtual void Move(Vector2 delta);
	// 위치 설정/값 읽는 함수.
	void SetPosition(const Vector2& newPosition);
	void SetPosition(int x, int y);
	Vector2 GetPosition() const;
	//크기 관련 함수
	int GetWidth() { return width; };
	int GetHeight() { return height; };
	void SetSize(int w, int h) { width = w; height = h; };
	//스탯 관련 함수
	int GetHp() { return hp; };
	void SetHp(int newHp) { hp = newHp; };
	// Sorting Order 설정.
	void SetSortingOrder(unsigned int sortingOrder);
	unsigned int GetSortingOrder() { return sortingOrder; }
	// 오너십 설정.
	void SetOwner(Level* newOwner);
	Level* GetOwner();

	// 객체 삭제 함수.
	void Destroy();

	bool isIntersect(Vector2 myIntersectTL, Vector2 myIntersectBR, Vector2 otherRangeTR, Vector2 otherRangeBR);

	// 게임 종료 요청 함수.
	void QuitGame();

public:
	// 액터가 활성 상태인지 알려주는 변수.
	bool isActive = true;

	// 삭제 요청됐는지 알려주는 변수.
	bool isExpired = false;


protected:
	// 개체의 위치.
	Vector2 position;
	int width = 0;
	int height = 0;
	int hp = 100;

	//이미지 아스키 아트, 전경색, 배경색, 경로
	const std::vector<std::vector<char>>& asciiImages;
	const std::vector<std::vector<Color>>& fgs;
	const std::vector<std::vector<Color>>& bgs;
	std::string imageLink;

	//액션레벨에 따라 보여지는 이미지, 행동 등에 사용
	int actionLevel = 0;

	// BeginPlay 호출이 되었는지 확인.
	bool hasBeganPlay = false;

	// 정렬 순서.
	unsigned int sortingOrder = 0;

	// 소유 레벨(오너십).
	Level* owner = nullptr;
};