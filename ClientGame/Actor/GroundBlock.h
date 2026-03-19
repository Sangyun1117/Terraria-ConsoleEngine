//땅 블럭 클래스. 별 기능은 없다.
#pragma once
#include "Actor/Block.h"
class GroundBlock : public Block {
public:
	GroundBlock(int x = 0, int y = 0);
	virtual ~GroundBlock();
private:
};