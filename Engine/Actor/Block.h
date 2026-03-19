//Block는 정적객체에서 사용할 함수, 변수를 정의해놓은 클래스
#pragma once
#include "Core.h"
#include "Math/Vector2.h"
#include "Math/Color.h"
#include "RTTI.h"
#include <vector>
#include <string>

// 전방 선언.
class Level;
class Engine_API Block : public RTTI
{
	friend class Level;
	RTTI_DECLARATIONS(Block, RTTI)

public:
	Block(const std::string& imageLink, const Vector2& position, const std::vector<std::vector<char>>& images, const std::vector<std::vector<Color>>& bgs, const std::vector<std::vector<Color>>& fgs);
	virtual ~Block();

	// 프레임 마다 호출. 사용하지 않을것을 권장
	virtual void Tick(float deltaTime);

	// 그리기 함수.
	virtual void Render();

	// 위치 설정/값 읽는 함수.
	void SetPosition(const Vector2& newPosition);
	void SetPosition(int x, int y);
	Vector2 GetPosition() const;
	void SetItemNum(int n) { itemNum = n; }
	int GetItemNum() { return itemNum; }
protected:
	// 개체의 위치.
	Vector2 position;

	const std::vector<std::vector<char>>& asciiImages;
	const std::vector<std::vector<Color>>& fgs;
	const std::vector<std::vector<Color>>& bgs;
	std::string imageLink;

	int itemNum; //블럭의 식별자
};