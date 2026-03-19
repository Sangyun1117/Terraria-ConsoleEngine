#include "Actor/Ghost.h"
#include "Core/ImageManager.h"
#include "Core/Engine.h"
#include "Level/Level.h"
#include "Level/GameLevel.h"
#include "Settings/ObjectDefines.h"
#include "Utils/Utils.h"
Ghost::Ghost(int x, int y) : Actor(std::string("../Assets/Images/Ghost.txt"), Vector2(x, y), ImageManager::Get().GetImage("../Assets/Images/Ghost.txt"), ImageManager::Get().GetColor("../Assets/Colors/GhostColors.txt"), ImageManager::Get().GetColor("../Assets/Colors/GhostColors.txt")) {
	SetSize(GHOST_WIDTH, GHOST_HEIGHT);
	//좌표를 float형으로 바꿔서 자연스럽게 움직일 수 있도록 함
	xTemp = (float)x;
	yTemp = (float)y;
	SetSortingOrder(15);
}

void Ghost::Tick(float deltaTime) {
	super::Tick(deltaTime);

	GameLevel* mygl = dynamic_cast<GameLevel*>(GetOwner());
	if (!mygl)
		return;

	//플레이어와 몬스터의 위치 계산
	Vector2 playerLeftTop = mygl->GetPlayerPosition();
	Vector2 playerCenter = Vector2(playerLeftTop.x + (PLAYER_WIDTH / 2), playerLeftTop.y + (PLAYER_HEIGHT / 2));
	Vector2 ghostCenter = Vector2(position.x + (GHOST_WIDTH / 2), position.y + (GHOST_HEIGHT / 2));
	
	//플레이어를 바라보는 방향으로 설정
	if (playerCenter.x > ghostCenter.x)
		isRight = true;
	else
		isRight = false;

	//피격 시간 계산
	if (isHit) {
		if (hitTimer >= hitDuration) {
			isHit = false;
			xTemp = (float)position.x;
			yTemp = (float)position.y;
		}
		hitTimer += deltaTime;
		return;
	}

	//공격 시간 계산 - 공격 모션이 끝나고 공격. 
	if (isAttack) {
		if (attackTimer >= attackDuration) {
			isAttack = false;
			attackTimer = 0.0f;
			for (Actor* other : mygl->GetActors()) {
				if (!other->Is(Player::TypeIdClass())) //플레이어인지 확인
					continue;
				Vector2 myTL = isRight ? Vector2(position.x + width - BLOCKSIZE_WIDTH / 2, position.y) : Vector2(position.x - BLOCKSIZE_WIDTH / 2, position.y);
				Vector2 myBR = isRight ? Vector2(position.x + width + BLOCKSIZE_WIDTH / 2, position.y + height) : Vector2(position.x + BLOCKSIZE_WIDTH / 2, position.y + height);
				Vector2 otherTL = other->GetPosition();
				Vector2 otherBR = { other->GetPosition().x + other->GetWidth(), other->GetPosition().y + other->GetHeight() };

				if (isIntersect(myTL, myBR, otherTL, otherBR))//공격범위와 겹치면 공격
					Attack(other);
			}
		}
		attackTimer += deltaTime;
		return;
	}

	//공격 후 대기시간 계산
	if (!canAttack) {
		cooldownTimer += deltaTime;
		if (cooldownTimer >= cooldownDuration) {
			canAttack = true;
			cooldownTimer = 0.0f;
		}
	}

	//공격 사거리
	int xRange = BLOCKSIZE_WIDTH;
	int yRange = BLOCKSIZE_HEIGHT;

	//공격범위에 플레이어가 있으면 공격하고 아니면 이동함.
	if (xRange >= abs(ghostCenter.x - playerCenter.x) && yRange >= abs(ghostCenter.y - playerCenter.y)) {
		if (canAttack) {
			isAttack = true;
			attackTimer = 0.0f;
		}
	}
	else {
		// 유령 중심 기준으로 방향 계산 (정규화된 벡터)
		float dx = (float)(playerCenter.x - ghostCenter.x);
		float dy = (float)(playerCenter.y - ghostCenter.y);
		float length = sqrtf(dx * dx + dy * dy);
		if (length != 0.0f)
		{
			dx /= length;
			dy /= length;
		}

		// 누적 좌표 업데이트 (float)
		xTemp += dx * moveSpeed;
		yTemp += dy * moveSpeed;

		//	// 정수 좌표로 변환 → 이동
		int newX = (int)xTemp;
		int newY = (int)yTemp;
		int deltaX = newX - position.x;
		int deltaY = newY - position.y;

		if (deltaX != 0 || deltaY != 0)
		{
			Vector2 oldPos = position;
			Move(Vector2(deltaX, deltaY));

			// 이동 실패 → 롤백
			if (position == oldPos)
			{
				xTemp = (float)position.x;
				yTemp = (float)position.y;
			}
		}
	}

}
void Ghost::Render() {
	Vector2 actorPos = { position.x - Engine::Get().cameraPos.x, position.y - Engine::Get().cameraPos.y };

	if (asciiImages.empty() || asciiImages[0].empty())
		return;

	//애니메이션 프레임 계산
	int direction = isRight ? 0 : 30;
	int animationLevel = isAttack ? 15 : 0;

	int startRow = direction + animationLevel;

	// 이미지와 색상에서 필요한 부분만 자름
	auto sliceImage = Utils::Slice2DVector(asciiImages, startRow, 0, GHOST_HEIGHT, (int)asciiImages[0].size());
	auto sliceFg = Utils::Slice2DVector(fgs, startRow, 0, GHOST_HEIGHT, (int)fgs[0].size());
	auto sliceBg = Utils::Slice2DVector(bgs, startRow, 0, GHOST_HEIGHT, (int)bgs[0].size());
	Engine::Get().WriteToBuffer(actorPos, sliceImage, sliceFg, sliceBg);

}
void Ghost::Move(Vector2 delta) {
	position = position + delta;
}

//공격 - 플레이어를 이동시키고 데미지 줌
void Ghost::Attack(Actor* other) {
	Player* player = other->As<Player>();
	if (player) {
		canAttack = false;
		player->Move(isRight ? Vector2(BLOCKSIZE_WIDTH, 0) : Vector2(BLOCKSIZE_WIDTH * (-1), 0));
		player->OnAttacked(attackDamage);
	}
}

//피격 - hp하락
void Ghost::OnAttacked(int damage)
{
	isHit = true;
	hitTimer = 0.0f;
	SetHp(hp - damage);
}
