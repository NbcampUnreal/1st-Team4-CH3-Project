// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskRangedMove.h"
#include "AIController.h"

UBTTaskRangedMove::UBTTaskRangedMove()
{
	Monster = nullptr;

	NodeName = "RangedMove";
}

EBTNodeResult::Type UBTTaskRangedMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// 몬스터 캐스팅 실패 시 Return
	Monster = Cast<ARangedMonster>(OwnerComp.GetAIOwner()->GetCharacter());
	if (Monster == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "ExecuteTask : Monster Casting Failed !");
		return EBTNodeResult::Failed;
	}
	// 캐스팅에 성공할 경우 Monster의 Move 함수 호출
	Monster->Move();

	return EBTNodeResult::Succeeded;
}
