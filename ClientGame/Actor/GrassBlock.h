//잔디블럭 클래스. 별 기능은 없다.
#pragma once
#include "Actor/Actor.h"
#include "Actor/Block.h"

class GrassBlock : public Block {
public:
	GrassBlock(int x = 0, int y = 0);
	virtual ~GrassBlock();
private:
	std::string bgColorsImageLink = "../Assets/Colors/GrassBlockBgColors.txt";
};