#include "MoveToTarget.h"
#include "AIController.h"
#include "TheFourthDescendant/Monster/Boss/Boss.h"

UMoveToTarget::UMoveToTarget()
{
	Boss = nullptr;
	NodeName = "MoveToTarget";
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

	Boss->MoveToTarget();

	return EBTNodeResult::Succeeded;
}