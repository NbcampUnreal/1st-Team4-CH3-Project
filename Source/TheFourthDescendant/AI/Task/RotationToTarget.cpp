#include "RotationToTarget.h"
#include "AIController.h"
#include "TheFourthDescendant/Monster/Boss/Boss.h"

URotationToTarget::URotationToTarget()
{
	Boss = nullptr;
	NodeName = "RotationToTarget";
	bNotifyTick = true;
}



EBTNodeResult::Type URotationToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	Boss = Cast<ABoss>(OwnerComp.GetAIOwner()->GetCharacter());
	if (Boss == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "MoveBack : Boss Casting Failed !");
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}



void URotationToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (Boss)
	{
		Boss->RotationToTarget(DeltaSeconds);
	}
}