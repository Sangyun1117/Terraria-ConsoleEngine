#include "Player.h"
#include "GrassBlock.h"
#include "GroundBlock.h"
#include "Engine.h"
#include "Utils/Utils.h"
#include "Settings/ActionDefines.h"
#include "Game/Game.h"
#include "Level/GameLevel.h"
#include "Math/Vector2.h"
#include "Math/Color.h"
#include "Settings//ObjectDefines.h"
#include "Core/ImageManager.h"
#include <windows.h>
#include <cstdio>
#include <vector>
#include <fstream>
#include <iostream>

Player::Player(int x, int y) : Actor(std::string("../Assets/Images/AmongUs.txt"), Vector2(x, y), ImageManager::Get().GetImage("../Assets/Images/AmongUs.txt"), ImageManager::Get().GetColor("../Assets/Colors/PlayerFgColors.txt"), ImageManager::Get().GetColor("../Assets/Colors/PlayerFgColors.txt"))
{
	SetSize(PLAYER_WIDTH, PLAYER_HEIGHT);
	//아이템 이미지 가져오기
	itemsImage = ImageManager::Get().GetImage(itemImageLink);
	itemsFgColors = ImageManager::Get().GetColor(itemFgLink);
	itemsBgColors = ImageManager::Get().GetColor(itemBgLink);
	actionLevel = ACTION_IDLE;

	SetSortingOrder(20);
	InventoryReset(); //인벤토리 초기화

}

void Player::Tick(float deltaTime)
{
	if (hp <= 0) {
		static_cast<Game&>(Engine::Get()).QuitLevel(LEVEL_NUM_GAME, LEVEL_NUM_HOME);
		return;
	}
	// 중력 처리
	velocityY += gravity;
	yFloat += velocityY;

	int newY = (int)yFloat;
	int dy = newY - position.y;

	if (dy != 0)
	{
		Vector2 oldPos = position;
		Move(Vector2(0, dy));

		// 이동에 실패한 경우 -> yFloat 롤백하고 속도 0, 땅 위에 있다고 판단
		if (position.y == oldPos.y) {
			yFloat = (float)position.y;
			velocityY = 0;
			isOnGround = true;
		}
		else {
			isOnGround = false;
		}
	}

	//키보드 입력
	if (Input::Get().GetKeyDown(VK_SPACE) && isOnGround)
	{
		velocityY = jumpPower; // 위로 점프
		isOnGround = false;
	}

	isRunning = false;
	if (Input::Get().GetKey('A'))
	{
		Move(Vector2::Left);
		isRight = false;
		isRunning = true;
	}

	if (Input::Get().GetKey('D'))
	{
		Move(Vector2::Right);
		isRight = true;
		isRunning = true;
	}

	if (isRunning) {
		runTimer += Engine::Get().GetDeltaTime();
		if (runTimer >= frameDelay) {
			runFrame = (runFrame + 1) % 4; //프레임 0~3;
			runTimer = 0.0f;
		}
	}
	else {
		runTimer = 0.0f;
		runFrame = 0;
	}


	for (int i = 0; i <= 9; i++) { //인벤토리 아이템 선택
		if (Input::Get().GetKey('0' + i)) {
			itemLevel = i;
			break;  // 여러 개 눌렸을 경우 첫 번째 것만 적용
		}
	}

	//마우스 입력
	if (Input::Get().GetMouseLeftDown())
	{
		GameLevel* mygl = dynamic_cast<GameLevel*>(GetOwner());

		//현재 아이템이 곡괭이라면 클릭지점에 블럭이 있을 경우 파괴하고 인벤토리에 추가
		if ( inventory[itemLevel].itemName == ITEM_PICKAXE) {
			isItemAction = true;
			actionLevel = ACTION_ATTACK;
			actionTimer = 0.0f; //타이머 초기화
			actionDuration = 0.2f; //지속시간 설정

			Vector2 nowMousePosition = Input::Get().GetMousePosition();
			Vector2 realPosition = Engine::Get().cameraPos + nowMousePosition;
			int searchBlockX = realPosition.x / 10;
			int searchBlockY = realPosition.y / 5;

			//플레이어 중앙 위치
			int playerCenterX = (position.x + PLAYER_WIDTH / 2) / BLOCKSIZE_WIDTH;
			int playerCenterY = (position.y + PLAYER_HEIGHT / 2) / BLOCKSIZE_HEIGHT;

			// 플레이어 기준 ±2 칸 이내만 허용
			constexpr int BUILD_LIMIT = 2;

			int buildRangeX = abs(searchBlockX - playerCenterX);
			int buildRangeY = abs(searchBlockY - playerCenterY);
			if (buildRangeX > BUILD_LIMIT || buildRangeY > BUILD_LIMIT) {
				return; // 너무 멀어서 금지
			}


			int minX = max(0, playerCenterX - BUILD_LIMIT);
			int maxX = min(MAP_WIDTH - 1, playerCenterX + BUILD_LIMIT);
			int minY = max(0, playerCenterY - BUILD_LIMIT);
			int maxY = min(MAP_HEIGHT - 1, playerCenterY + BUILD_LIMIT);
			// searchBlock이 유효한지 다시 제한된 범위 안에서 검사
			if (searchBlockX >= minX && searchBlockX <= maxX &&
				searchBlockY >= minY && searchBlockY <= maxY)
			{
				if (mygl->mapData[searchBlockY][searchBlockX] != nullptr) {
					for (int i = 1; i < inventory.size()+1; ++i) { //인벤토리 1,2,3~0순서로 하기 위해
						int num = i % 10;
						if (inventory[num].itemName == ITEM_HAND) {
							switch (mygl->mapData[searchBlockY][searchBlockX]->GetItemNum())
							{
							case ITEM_GRASS_BLOCK:
								inventory[num] = { ITEM_GRASS_BLOCK, 1 };
								break;
							case ITEM_GROUND_BLOCK:
								inventory[num] = { ITEM_GROUND_BLOCK, 1 };
								break;
							default:
								break;
							}

							break;
						}
						if (inventory[num].itemName == mygl->mapData[searchBlockY][searchBlockX]->GetItemNum() && inventory[num].count < 64) {
							inventory[num].count++;
							break;
						}
					}
					SafeDelete(mygl->mapData[searchBlockY][searchBlockX]);
				}
			}
		}
		//현재 아이템이 블럭이라면 설치가능한지 판단 후 블럭 설치
		else if (inventory[itemLevel].itemName == ITEM_GRASS_BLOCK || inventory[itemLevel].itemName == ITEM_GROUND_BLOCK) {
			isItemAction = true;
			actionLevel = ACTION_ATTACK;
			actionTimer = 0.0f; //타이머 초기화
			actionDuration = 0.2f; //지속시간 설정

			Vector2 nowMousePosition = Input::Get().GetMousePosition();
			Vector2 realPosition = Engine::Get().cameraPos + nowMousePosition;
			int searchBlockX = realPosition.x / 10;
			int searchBlockY = realPosition.y / 5;

			// 플레이어의 블록 범위
			int playerLeft = position.x / BLOCKSIZE_WIDTH;
			int playerRight = (position.x + PLAYER_WIDTH - 1) / BLOCKSIZE_WIDTH;
			int playerTop = position.y / BLOCKSIZE_HEIGHT;
			int playerBottom = (position.y + PLAYER_HEIGHT - 1) / BLOCKSIZE_HEIGHT;

			// 1. 블럭과 플레이어가 겹치는지 확인
			if (searchBlockX >= playerLeft && searchBlockX <= playerRight &&
				searchBlockY >= playerTop && searchBlockY <= playerBottom) {
				return; // 플레이어랑 겹치므로 설치 금지
			}

			//플레이어 중앙 위치
			int playerCenterX = (position.x + PLAYER_WIDTH / 2) / BLOCKSIZE_WIDTH;
			int playerCenterY = (position.y + PLAYER_HEIGHT / 2) / BLOCKSIZE_HEIGHT;


			// 2. 플레이어와 2블록 이상 떨어지면 금지
			int buildRangeX = abs(searchBlockX - playerCenterX);
			int buildRangeY = abs(searchBlockY - playerCenterY);
			if (buildRangeX > 2 || buildRangeY > 2) {
				return; // 너무 멀어서 설치 금지
			}
			// 3. 맵 경계 내부인지 검사
			if (searchBlockX < 0 || searchBlockX >= MAP_WIDTH ||
				searchBlockY < 0 || searchBlockY >= MAP_HEIGHT) {
				return;
			}
			// 4. 설치
			if (mygl && mygl->mapData[searchBlockY][searchBlockX] == nullptr) {
				Block* block = nullptr;
				switch (inventory[itemLevel].itemName) {
				case ITEM_GRASS_BLOCK:
					block = new GrassBlock(searchBlockX * BLOCKSIZE_WIDTH, searchBlockY * BLOCKSIZE_HEIGHT);
					break;
				case ITEM_GROUND_BLOCK:
					block = new GroundBlock(searchBlockX * BLOCKSIZE_WIDTH, searchBlockY * BLOCKSIZE_HEIGHT);
					break;
				default:
					break;
				}
				mygl->mapData[searchBlockY][searchBlockX] = block; 
				inventory[itemLevel].count--;
				if (inventory[itemLevel].count <= 0) {
					inventory[itemLevel] = { ITEM_HAND, 0 };
					itemLevel = ITEM_HAND;
				}
			}
		}
		//현재 아이템이 검이면 공격범위에 몬스터가 있는지 확인 후 공격
		else if (inventory[itemLevel].itemName == ITEM_SWORD) {
			isItemAction = true;
			actionLevel = ACTION_ATTACK;
			actionTimer = 0.0f; //타이머 초기화
			actionDuration = 0.2f; //지속시간 설정

			for (Actor* other : mygl->GetActors()) { //자기자신 제외
				if (other == this)
					continue;
				//범위 지정
				Vector2 myTL = isRight ? Vector2(position.x + width - BLOCKSIZE_WIDTH / 2, position.y) : Vector2(position.x - BLOCKSIZE_WIDTH / 2, position.y);
				Vector2 myBR = isRight ? Vector2(position.x + width + BLOCKSIZE_WIDTH / 2, position.y + height) : Vector2(position.x + BLOCKSIZE_WIDTH / 2, position.y + height);
				Vector2 otherTL = other->GetPosition();
				Vector2 otherBR = { other->GetPosition().x + other->GetWidth(), other->GetPosition().y + other->GetHeight() };

				if (isIntersect(myTL, myBR, otherTL, otherBR))
					Attack(other);
			}
		}
	}

	//현재 애니메이션 중이라면 시간체크
	if (actionLevel != ACTION_IDLE) {
		actionTimer += Engine::Get().GetDeltaTime();
		if (actionTimer >= actionDuration) {
			isItemAction = false;
			actionLevel = ACTION_IDLE;
			actionTimer = 0.0f;
		}
	}
}

void Player::Render()
{
	Vector2 actorPos = { position.x - Engine::Get().cameraPos.x, position.y - Engine::Get().cameraPos.y };

	if (asciiImages.empty() || asciiImages[0].empty())
	{
		return;
	}

	//애니메이션 프레임 계산
	int direction = isRight ? 0 : 50;
	int animationLevel = 0;
	if (isRunning)
	{
		animationLevel = (runFrame + 1) * 10;
	}

	int startRow = direction + animationLevel;

	// 이미지와 색상에서 필요한 부분만 자른 후 캐릭터 렌더링
	auto sliceImage = Utils::Slice2DVector(asciiImages, startRow, 0, PLAYER_HEIGHT, (int)asciiImages[0].size());
	auto sliceFg = Utils::Slice2DVector(fgs, startRow, 0, PLAYER_HEIGHT, (int)fgs[0].size());
	auto sliceBg = Utils::Slice2DVector(bgs, startRow, 0, PLAYER_HEIGHT, (int)bgs[0].size());
	Engine::Get().WriteToBuffer(actorPos, sliceImage, sliceFg, sliceBg);


	if (inventory[itemLevel].itemName == ITEM_HAND)
	{
		return;
	}

	//아이템 위치 계산
	int itemDirection = isRight ? 0 : 5;
	int itemPosX = isRight ? 3 : -3;
	int itemPosY = isItemAction ? 6 : 3;
	int itemActionPoint = isItemAction ? 10 : 0;
	Vector2 itemDrawPos = { actorPos.x + itemPosX, actorPos.y + itemPosY };

	int itemStartRow = itemDirection + (inventory[itemLevel].itemName - 1) * 20 + itemActionPoint;
	int itemHeight = 5; //아이템 높이

	// 이미지와 색상에서 필요한 부분만 자른 후 아이템 렌더링
	auto itemSliceImage = Utils::Slice2DVector(itemsImage, itemStartRow, 0, itemHeight, (int)itemsImage[0].size());
	auto itemSliceFg = Utils::Slice2DVector(itemsFgColors, itemStartRow, 0, itemHeight, (int)itemsFgColors[0].size());
	auto itemSliceBg = Utils::Slice2DVector(itemsBgColors, itemStartRow, 0, itemHeight, (int)itemsBgColors[0].size());
	Engine::Get().WriteToBuffer(itemDrawPos, itemSliceImage, itemSliceFg, itemSliceBg);
}

void Player::Move(Vector2 delta)
{
	Vector2 nextPos = position + delta;

	//1. 정적 객체 충돌 확인 (맵 기준)
	GameLevel* mygl = dynamic_cast<GameLevel*>(GetOwner());
	//모서리 4곳 검사
	int left = nextPos.x / 10;
	int right = (nextPos.x + PLAYER_WIDTH - 1) / BLOCKSIZE_WIDTH;
	int top = nextPos.y / 5;
	int bottom = (nextPos.y + PLAYER_HEIGHT - 1) / BLOCKSIZE_HEIGHT;

	// 전체 범위 검사
	for (int y = top; y <= bottom; ++y) {
		for (int x = left; x <= right; ++x) {
			if (y >= 0 && y < (int)mygl->mapData.size() &&
				x >= 0 && x < (int)mygl->mapData[y].size() &&
				mygl->mapData[y][x] != nullptr) {
				return;
			}
		}
	}

	//Todo: 2. 동적객체충돌 처리 (몬스터, npc 등)

	//충돌 없으면 이동
	position = position + delta;
	SetPosition(position);
	Engine::Get().cameraPos = Engine::Get().cameraPos + delta;
}

//공격 - 몬스터 이동시키고 데미지 줌
void Player::Attack(Actor* other)
{
	other->Move(isRight ? Vector2(BLOCKSIZE_WIDTH, 0) : Vector2(BLOCKSIZE_WIDTH * (-1), 0));
	other->OnAttacked(attackDamage);
}

//피격 - 체력감소
void Player::OnAttacked(int damage)
{
	SetHp(hp - damage);
}

void Player::InventoryReset() {
	for (int i = 0; i <= 9; ++i) {
		inventory.push_back({ ITEM_HAND, 0 });
	}
	inventory[1] = { ITEM_PICKAXE, 1 };
	inventory[2] = { ITEM_SWORD, 1 };
}
