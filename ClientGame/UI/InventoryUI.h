//인벤토리 UI 클래스
#pragma once
#include "UI/UIElement.h"
#include "Actor/Player.h"

class InventoryUI : public UIElement {
public:
	InventoryUI(int x = 0, int y = 0);
	void Render() override;
	//itemNum에 해당하는 인벤토리 아이템 이미지 반환
	const std::wstring & GetItemName(Item itemNum);
};
