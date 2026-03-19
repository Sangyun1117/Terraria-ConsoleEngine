//게임레벨은 게임이 실제 진행되는 핵심적인 레벨입니다.
//방향키로 플레이어를 이동시키고 마우스 왼쪽버튼으로 공격,건축 등이 가능합니다.
//숫자키 0~9를 누르면 아이템을 변경합니다.
//esc를 누르면 메뉴레벨로 이동합니다.
#pragma once
#include "Level/Level.h"
#include "Math/Vector2.h"
#include "Actor/Actor.h"
#include "Actor/Player.h"
#include <vector>
#include <string>
class GameLevel : public Level {
	RTTI_DECLARATIONS(GameLevel, Level)
public:
	GameLevel();
	~GameLevel();

	// 게임 루프 단계별 오버라이드 함수
	virtual void BeginPlay() override; //초기화 시 호출
	virtual void Tick(float deltaTime) override; //매 프레임 호출
	virtual void Render() override; //화면 렌더링
	//플레이어 월드좌표 반환
	Vector2 GetPlayerPosition() { return player->GetPosition(); };
	//모든 액터 반환
	std::vector<Actor*>& GetActors() { return actors; };
private:
	//맵 파일을 읽어서 게임 객체 생성하는 함수
	void ReadMapFile(const char* filename);
	//y값에 맞게 배경색 그려주는 함수.
	void SettingBackground();
public:
	Player* player;
	std::vector<std::vector<Block*>> mapData;
};