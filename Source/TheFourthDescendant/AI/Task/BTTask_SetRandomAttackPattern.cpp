#include "BTTask_SetRandomAttackPattern.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetRandomAttackPattern::UBTTask_SetRandomAttackPattern()
{
	NodeName = "SetRandomAttackPattern";
}

EBTNodeResult::Type UBTTask_SetRandomAttackPattern::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// 블랙보드 반환에 실패한 경우 Failed 처리
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// 공격 패턴 개수 반환
	const int AttackPatternCount = Blackboard->GetValueAsInt(FName("AttackPatternCount"));

	// 랜덤 공격 패턴 인덱스 할당
	const int RandomAttackIndex = FMath::RandRange(0,AttackPatternCount-1);
	Blackboard->SetValueAsInt(FName("AttackPatternIndex"), RandomAttackIndex);
	
	return EBTNodeResult::Succeeded;
}
