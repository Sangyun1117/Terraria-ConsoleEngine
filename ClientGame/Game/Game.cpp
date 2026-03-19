#include "Game.h"
#include "Level/GameLevel.h"
#include "Level/HomeLevel.h"
#include "Level/MenuLevel.h"
#include "Core/ImageManager.h"

Game::Game()
{
    // 이미지 매니저 가져오기
    ImageManager& imgMgr = ImageManager::Get();
    //이미지들 로드
    if (!imgMgr.LoadImagesFromFolder("../Assets/Images", "../Assets/Colors")) {
        std::cerr << "일부 이미지 로드 실패!\n";
    }

	instance = this;

    homeLevel = new HomeLevel();
    gameLevel = new GameLevel();
    menuLevel = new MenuLevel();
	AddLevel(homeLevel);

}

Game::~Game()
{
	CleanUp();
}

//레벨 변경
void Game::ChangeLevel(int changeLevel)
{
    nowLevel = changeLevel;
    switch (nowLevel)
    {
    case LEVEL_NUM_HOME:
        mainLevel = homeLevel;
        break;
    case LEVEL_NUM_GAME:
        mainLevel = gameLevel;
        break;
    case LEVEL_NUM_MENU:
        mainLevel = menuLevel;
        break;
    default:
        break;
    }
}

//정리
void Game::CleanUp()
{
    SafeDelete(homeLevel);
    SafeDelete(gameLevel);
    SafeDelete(menuLevel);
    homeLevel = nullptr;
    gameLevel = nullptr;
    menuLevel = nullptr;
    mainLevel = nullptr;
}

//레벨 종료
void Game::QuitLevel(int deleteLevel, int nextLevel)
{
    switch (deleteLevel) {
    case LEVEL_NUM_HOME:
        SafeDelete(homeLevel);
        homeLevel = new HomeLevel();
        break;
    case LEVEL_NUM_GAME:
        SafeDelete(gameLevel);
        gameLevel = new GameLevel();
        break;
    case LEVEL_NUM_MENU:
        SafeDelete(menuLevel);
        menuLevel = new MenuLevel();
        break;
   }

    switch (nextLevel)
    {
    case LEVEL_NUM_HOME:
        mainLevel = homeLevel;
        break;
    case LEVEL_NUM_GAME:
        mainLevel = gameLevel;
        break;
    case LEVEL_NUM_MENU:
        mainLevel = menuLevel;
        break;
    default:
        break;
    }

    nowLevel = nextLevel;
}