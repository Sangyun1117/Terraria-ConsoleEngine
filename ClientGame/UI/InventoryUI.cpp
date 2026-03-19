#include "InventoryUI.h"
#include "UI/UIElement.h"
#include "Core/ImageManager.h"
#include "Level/GameLevel.h"
#include "Settings/ObjectDefines.h"


InventoryUI::InventoryUI(int x, int y)
	: UIElement("../Assets/Images/InventoryUI.txt", Vector2(x, y), ImageManager::Get().GetImage("../Assets/Images/InventoryUI.txt"), ImageManager::Get().GetColor("../Assets/Colors/InventoryUIBgColors.txt"), ImageManager::Get().GetColor("../Assets/Colors/InventoryUIFgColors.txt"))
{

}

void InventoryUI::Render()
{
	if (position.x >= Engine::Get().GetScreenWidth() || position.y >= Engine::Get().GetScreenHeight())
		return;
	Engine::Get().WriteToBuffer(position, asciiImages, fgs, bgs);

	const std::vector<Item>& inv = static_cast<GameLevel*>(owner)->player->GetInventory();

	//인벤토리 아이템 정보 출력 좌표
	Vector2 itemPos = { position.x + 2, position.y + 2 };
	Vector2 itemCountPos = { position.x + 6, position.y + 3 };
	//반복문 돌리며 아이템 출력
	for (int i = 1; i < inv.size()+1; i++) {
		int num = i % 10;
		if (inv[num].itemName != ITEM_HAND) {
			const std::wstring itemName = GetItemName(inv[num]);
			Color itemColor;
			switch (inv[num].itemName)
			{
			case ITEM_PICKAXE:
				itemColor = Color::Blue;
				break;
			case ITEM_SWORD:
				itemColor = Color::Red;
				break;
			case ITEM_GRASS_BLOCK:
				itemColor = Color::BrightGreen;
				break;
			case ITEM_GROUND_BLOCK:
				itemColor = Color::Yellow;
				break;
			default:
				itemColor = Color::Black;
				break;
			}

			Engine::Get().WriteToWcharBuffer(itemPos, itemName.c_str(), itemColor, Color::White);
		}
		//아이템 갯수 출력
		if (inv[num].count > 0) {
			std::wstring str = std::to_wstring(inv[num].count);
			Engine::Get().WriteToWcharBuffer(itemCountPos, str.c_str(), Color::Black, Color::White);
		}
		itemPos.x += 8;
		itemCountPos.x += 8;
	}
}

//해시맵을 이용해 아이템이미지 반환
const std::wstring& InventoryUI::GetItemName(Item item) {
	static const std::unordered_map<int, std::wstring> itemNames = {
		{ ITEM_PICKAXE, L"  ⛏️" },
		{ ITEM_SWORD, L"  ⚔️" },
		{ ITEM_GRASS_BLOCK, L"  ♒ " },
		{ ITEM_GROUND_BLOCK, L"  🟫" }
	};

	auto it = itemNames.find(item.itemName);
	if (it != itemNames.end())
		return it->second;

	static const std::wstring unknown = L"알 수 없음";
	return unknown;
}

