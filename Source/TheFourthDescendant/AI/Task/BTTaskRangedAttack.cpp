// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskRangedAttack.h"
#include "AIController.h"

UBTTaskRangedAttack::UBTTaskRangedAttack()
{
	Monster = nullptr;

	NodeName = "RangedAttack";
}

EBTNodeResult::Type UBTTaskRangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// 몬스터 캐스팅 실패 시 Return
	Monster = Cast<ARangedMonster>(OwnerComp.GetAIOwner()->GetCharacter());
	if (Monster == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "ExecuteTask : Monster Casting Failed !");
		return EBTNodeResult::Failed;
	}

	// 캐스팅에 성공할 경우 Monster의 Attack 함수 호출
	Monster->Attack();
	
	return EBTNodeResult::Succeeded;
}

