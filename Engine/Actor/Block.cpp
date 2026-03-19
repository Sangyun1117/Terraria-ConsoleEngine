#include "Block.h"
#include "Utils/Utils.h"
#include "Engine.h"
#include "Level/Level.h"
#include "Math/Vector2.h"
#include "ImageManager.h"
#include <Windows.h>
#include <iostream>
#include <fstream>

Block::Block(const std::string& imageLink, const Vector2& position, const std::vector<std::vector<char>>& images, const std::vector<std::vector<Color>>& bgs, const std::vector<std::vector<Color>>& fgs)
	: imageLink(imageLink), position(position), asciiImages(images), bgs(bgs), fgs(fgs) {
	SetItemNum(0);
}

Block::~Block()
{
}

// 프레임 마다 호출 (반복성 작업/지속성이 필요한 작업).
void Block::Tick(float deltaTime) {
}

void Block::Render()
{
	Vector2 blockPos = { position.x - Engine::Get().cameraPos.x, position.y - Engine::Get().cameraPos.y };

	// 화면 밖이면 아예 그리지 않음(필요시 영역 검사)
	if (blockPos.x >= Engine::Get().GetScreenWidth() || blockPos.y >= Engine::Get().GetScreenHeight())
		return;
	//버퍼에 저장
	Engine::Get().WriteToBuffer(blockPos, asciiImages, fgs, bgs);
}

void Block::SetPosition(const Vector2& newPosition)
{
	position = newPosition;
}
void Block::SetPosition(int x, int y) {
	position = { x,y };
}
Vector2 Block::GetPosition() const
{
	return position;
}

