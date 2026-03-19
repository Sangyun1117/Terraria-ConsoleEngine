// Game.h
//
// 게임의 전체 흐름을 관리하는 Game 클래스
// 
// 주요 기능:
//  - 레벨(홈, 게임, 메뉴) 관리
//  - 현재 활성화된 레벨 추적
//  - 레벨 변경(ChangeLevel) 및 종료(QuitLevel) 기능 제공


#pragma
#include "Engine.h"
#include "Settings/ObjectDefines.h"
#include <string>
#include <vector>
class Level; //전방선언
class Game : public Engine {
public:
	Game();
	~Game();
	//레벨 변경 함수
	void ChangeLevel(int changeLevel);
	virtual void CleanUp() override;
	//레벨 종료 함수
	void QuitLevel(int deleteLevel, int nextLevel);
	// Getter 함수
	Level* GetHomeLevel() const { return homeLevel; }
	Level* GetGameLevel() const { return gameLevel; }

private:
	//레벨
	Level* homeLevel = nullptr;
	Level* gameLevel = nullptr;
	Level* menuLevel = nullptr;

	int nowLevel = LEVEL_NUM_HOME; 
};