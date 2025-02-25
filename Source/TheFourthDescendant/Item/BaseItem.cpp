// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseItem.h"

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = false;
}

// 플레이어가 아이템 범위에 들어왔을 때 동작
void ABaseItem::OnItemOverlap(AActor* OverlapActor)
{
}

// 플레이어가 아이템 범위를 벗어났을 때 동작
void ABaseItem::OnItemEndOverlap(AActor* OverlapActor)
{
}

// 아이템이 사용(Activate)되었을 때 동작
void ABaseItem::ActivateItem(AActor* Activator)
{
}

// 아이템 유형을 반환
FName ABaseItem::GetItemType() const
{
	return ItemType;
}

// 아이템을 파괴(제거)하는 함수
void ABaseItem::DestroyItem()
{
	Destroy();
}
