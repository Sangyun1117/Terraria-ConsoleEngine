#include "Level.h"
#include "Actor/Actor.h"
#include "Utils/Utils.h"
#include <iostream>
#include <algorithm>

Level::Level()
{
}

Level::~Level()
{
	// 메모리 해제.
	for (Actor* actor : actors)
	{
		SafeDelete(actor);
	}

	for (Actor* actor : addRequstedActors)
	{
		SafeDelete(actor);
	}

	// std::vector 정리.
	actors.clear();

	for (UIElement* ui : uiElements)
	{
		ui->SetOwner(nullptr);
		SafeDelete(ui);
	}
	uiElements.clear();
}

// 레벨에 액터를 추가할 때 사용.
void Level::AddActor(Actor* newActor)
{
	// 대기 배열에 추가.
	addRequstedActors.emplace_back(newActor);

	// 오너십 설정.
	newActor->SetOwner(this);
}

void Level::DestroyActor(Actor* destroyedActor)
{
	// 삭제 대기 배열에 추가.
	destroyRequstedActors.emplace_back(destroyedActor);
}

// 엔진 이벤트 함수.
void Level::BeginPlay()
{
	for (Actor* const actor : actors)
	{
		if (actor == nullptr)
		{
			// 로그 남기고 건너뜀
			OutputDebugStringA("이미 삭제된 actor");
			continue;
		}
		// 액터 처리 여부 확인.
		if (!actor->isActive || actor->isExpired)
		{
			continue;
		}

		// 이미 호출된 개체는 건너뛰기.
		if (actor->HasBeganPlay())
		{
			continue;
		}

		actor->BeginPlay();
	}
}

void Level::Tick(float deltaTime)
{
	for (Actor* const actor : actors)
	{
		// 액터 처리 여부 확인.
		if (!actor->isActive || actor->isExpired)
		{
			continue;
		}

		actor->Tick(deltaTime);
	}
}

void Level::Render()
{
	// 그리기 전에 정렬 순서 기준으로 재배치(정렬).
	SortActorsBySortingOrder();

	// Render Pass.
	for (Actor* const actor : actors)
	{
		// 액터 처리 여부 확인.
		if (!actor->isActive || actor->isExpired)
		{
			continue;
		}

		// 검사 (같은 위치에 정렬 순서 높은 액터가 있는지 확인).
		Actor* searchedActor = nullptr;
		for (Actor* const otherActor : actors)
		{
			// 같은 액터면 무시.
			if (actor == otherActor)
			{
				continue;
			}

			// 위치가 같은 액터 확인.
			if (actor->GetPosition() == otherActor->GetPosition())
			{
				// 정렬 순서 비교 후 액터 저장.
				if (actor->sortingOrder < otherActor->sortingOrder)
				{
					// 저장 및 루프 종료.
					searchedActor = otherActor;
					break;
				}
			}
		}

		// 어떤 액터와 같은 위치에 정렬 순서가 더 높은 액터가 있으면,
		// 그리지 않고 건너뛰기.
		if (searchedActor)
		{
			continue;
		}
		// 드로우 콜.
		actor->Render();
	}

	// UI 렌더링
	for (UIElement* const ui : uiElements)
	{
		if (!ui->isVisible) continue;
		ui->Render();
	}

}

void Level::ProcessAddAndDestroyActors()
{
	// actors 배열에서 제외 처리.
	for (auto iterator = actors.begin(); iterator != actors.end();)
	{
		// 삭제 요청된 액터인지 확인 후 배열에서 제외
		if ((*iterator)->isExpired)
		{
			// erase 함수를 사용하면 iterator가 무효화되기 때문에
			// 반환되는 값을 저장해야함.
			iterator = actors.erase(iterator);
			continue;
		}

		++iterator;
	}

	// destroyRequstedActors 배열을 순회하면서 액터 delete.
	for (auto* actor : destroyRequstedActors)
	{
		// 리소스 해제.
		actor->SetOwner(nullptr);
		SafeDelete(actor);
	}

	// 배열 초기화 -> 크기가 0.
	destroyRequstedActors.clear();

	// addRequstedActors 배열을 순회하면서 새로운 액터 추가.
	for (auto* const actor : addRequstedActors)
	{
		actors.emplace_back(actor);
		actor->SetOwner(this);
	}

	// 배열 초기화.
	addRequstedActors.clear();
}

void Level::AddUI(UIElement* newUI)
{
	uiElements.push_back(newUI);
	newUI->SetOwner(this);
}

void Level::SortActorsBySortingOrder()
{
	//우선순위에 맞게 정렬
	std::sort(actors.begin(), actors.end(),
		[](Actor* a, Actor* b) {
			return a->sortingOrder < b->sortingOrder;
		});
}