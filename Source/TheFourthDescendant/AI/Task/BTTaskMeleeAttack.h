// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TheFourthDescendant/Monster/Melee/MeleeMonster.h"
#include "BTTaskMeleeAttack.generated.h"

UCLASS()
class THEFOURTHDESCENDANT_API UBTTaskMeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTaskMeleeAttack();

private:
	/** AI를 적용할 몬스터 */
	AMeleeMonster* Monster;

protected:
	/** Task가 실행되기 시작할 때 호출되는 함수 */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
