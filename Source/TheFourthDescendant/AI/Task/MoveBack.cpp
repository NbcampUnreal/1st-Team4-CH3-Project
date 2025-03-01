#include "MoveBack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TheFourthDescendant/Monster/Boss/Boss.h"

UMoveBack::UMoveBack()
{
	Boss = nullptr;
	NodeName = "MoveBack";
	bNotifyTick = true;
}

EBTNodeResult::Type UMoveBack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	Boss = Cast<ABoss>(OwnerComp.GetAIOwner()->GetCharacter());
	if (Boss == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "MoveBack : Boss Casting Failed !");
		return EBTNodeResult::Failed;
	}
	
	// 이동이 완료될 때까지 실행 유지
	return EBTNodeResult::InProgress;
}

void UMoveBack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// 보스가 BackMoving 상태가 끝났다면 성공 반환
	if (Boss->MovementState != EBossMovementState::BackMoving)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("IsMoving"), false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	if (Boss)
	{
		Boss->MoveBack();
	}
}