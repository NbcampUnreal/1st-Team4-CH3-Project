// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskMeleeMove.h"

UBTTaskMeleeMove::UBTTaskMeleeMove()
{
	Monster = nullptr;

	NodeName = "MeleeMove";
}

EBTNodeResult::Type UBTTaskMeleeMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// 몬스터 캐스팅 실패 시 Return
	Monster = Cast<AMeleeMonster>(OwnerComp.GetAIOwner()->GetCharacter());
	if (Monster == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "ExecuteTask : Monster Casting Failed !");
		return EBTNodeResult::Failed;
	}
	// 캐스팅에 성공할 경우 Monster의 Move 함수 호출
	Monster->Move();

	return EBTNodeResult::Succeeded;
}