#include "MoveHorizontal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TheFourthDescendant/Monster/Boss/Boss.h"

UMoveHorizontal::UMoveHorizontal()
{
	Boss = nullptr;
	NodeName = "MoveHorizontal";
	bNotifyTick = true;
}



EBTNodeResult::Type UMoveHorizontal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	Boss = Cast<ABoss>(OwnerComp.GetAIOwner()->GetCharacter());
	if (Boss == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "MoveHorizontal : Boss Casting Failed !");
		return EBTNodeResult::Failed;
	}

	// 왼쪽, 오른쪽 중 움직일 방향 택
	DirectionIndex = FMath::RandRange(0, 1);
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Emerald, "Horizontal 1");
	// 이동이 완료될 때까지 실행 유지
	return EBTNodeResult::InProgress;
}



void UMoveHorizontal::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// 보스가 Surrounding 상태가 끝났다면 성공 반환
	if (Boss->MovementState != EBossMovementState::Surrounding)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "MoveHorizontal Finished !");
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("IsMoving"), false);

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	if (Boss)
	{
		Boss->MoveHorizontal(DirectionIndex);
	}
}