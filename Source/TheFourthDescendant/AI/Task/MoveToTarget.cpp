#include "MoveToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TheFourthDescendant/Monster/Boss/Boss.h"

UMoveToTarget::UMoveToTarget()
{
	Boss = nullptr;
	NodeName = "MoveToTarget";
	bNotifyTick = true;
}

EBTNodeResult::Type UMoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	Boss = Cast<ABoss>(OwnerComp.GetAIOwner()->GetCharacter());
	if (Boss == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "MoveToTarget : Boss Casting Failed !");
		return EBTNodeResult::Failed;
	}
	
	// 이동이 완료될 때까지 실행 유지
	return EBTNodeResult::InProgress;
}

void UMoveToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// 보스가 Approaching 상태가 끝났다면 성공 반환
	if (Boss->MovementState != EBossMovementState::Approaching)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("IsMoving"), false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	if (Boss)
	{
		Boss->MoveToTarget();
	}
}
