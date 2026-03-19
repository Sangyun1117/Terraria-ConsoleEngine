//고스트(몬스터)의 행동과 렌더링을 정의한 클래스
#pragma once
#include "Actor/Actor.h"
#include "Settings/ObjectDefines.h"

class Ghost : public Actor {
	RTTI_DECLARATIONS(Ghost, Actor)
public:
	Ghost(int x = 0, int y = 0);
	//게임루프
	virtual void Tick(float deltaTime) override;
	virtual void Render() override;
	//이동
	void Move(Vector2 delta) override;
private:
	//전투
	void Attack(Actor* other);
	void OnAttacked(int damage);
private:
	//이동과 관련한 변수
	float moveSpeed = 0.2f; //이동속도
	float xTemp = 0.0f;
	float yTemp = 0.0f;

	//이미지
	std::string imageLink = "../Assets/Images/Ghost.txt";
	std::string fgColorsImageLink = "../Assets/Colors/GhostColors.txt";
	std::string bgColorsImageLink = "../Assets/Colors/GhostColors.txt";
	//캐릭터 좌우 판단
	bool isRight = true;
	bool isHit = false;
	//타이머
	float hitTimer = 0.0f; //피격 지속 시간 타이머
	float hitDuration = 0.5f;
	//캐릭터 이동 애니메이션
	int runFrame = 0;
	float runTimer = 0.0f;
	const float frameDelay = 0.05f; //프레임 딜레이 시간
	bool isRunning = false;

	//캐릭터 공격
	int attackDamage = 50;
	float attackTimer = 0.0f;  // 액션 지속 시간 타이머
	float attackDuration = 0.5f;
	bool isAttack = false;
	bool canAttack = true;
	float cooldownTimer = 0.0f;
	float cooldownDuration = 1.0f; //공격 후 대기시간
};