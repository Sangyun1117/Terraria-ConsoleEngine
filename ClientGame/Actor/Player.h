//플레이어 클래스는 게임의 주체로서 플레이어 캐릭터를 정의함
//플레이어 입력에 따라 이동, 공격, 점프 등의 행동을 수행
//체력이 떨어지면 게임이 종료되고 홈화면으로 이동한다.
#pragma once
#include "Actor/Actor.h"
#include "Settings/ActionDefines.h"
#include "Settings/ObjectDefines.h"

//인벤토리에 들어갈 아이템 구조체, 속성으로 이름과 갯수를 가짐
struct Item {
	int itemName;
	int count;
};

class Player : public Actor {
	RTTI_DECLARATIONS(Player, Actor)
public:
	Player(int x = 0, int y = 0);
	//게임루프 함수
	virtual void Tick(float deltaTime) override;
	virtual void Render() override;
	//이동
	void Move(Vector2 delta) override;
	//피격
	void OnAttacked(int damage) override;
	//인벤토리, UI에서 접근함
	const std::vector<Item>& GetInventory() const {
		return inventory;
	}
private:
	//공격
	void Attack(Actor *other);
	//인벤토리 초기화
	void InventoryReset();
private:
	//중력 변수
	bool isOnGround = false;
	float velocityY = 0.0f; //속도
	float gravity = 0.12f;
	float jumpPower = -2.4f;
	float yFloat = 0.0f; // 실수 누적용
	//캐릭터 좌우 판단
	bool isRight = true;
	int itemLevel = ITEM_HAND;
	//캐릭터 액션
	float actionTimer = 0.0f;  // 액션 지속 시간 타이머
	float actionDuration = 1.0f;  // 액션 지속 시간
	//캐릭터 이동 애니메이션
	int runFrame = 0;
	float runTimer = 0.0f;
	const float frameDelay = 0.05f; //프레임 딜레이 시간
	bool isRunning = false;
	//능력
	int attackDamage = 30;

	//아이템
	std::string itemImageLink = "../Assets/Images/Item.txt";
	std::string itemFgLink = "../Assets/Colors/ItemFgColors.txt";
	std::string itemBgLink = "../Assets/Colors/ItemBgColors.txt";
	std::vector<std::vector<char>> itemsImage;
	std::vector<std::vector<Color>> itemsFgColors;
	std::vector<std::vector<Color>> itemsBgColors;
	std::vector<Item> inventory;
	bool isItemAction = false;
};