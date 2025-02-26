#include "MovementStateSetter.h"
#include "AIController.h"
#include "TheFourthDescendant/Monster/Boss/Boss.h"

UMovementStateSetter::UMovementStateSetter()
{
	Boss = nullptr;
	NodeName = "MovementStateSetter";
}

EBTNodeResult::Type UMovementStateSetter::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	Boss = Cast<ABoss>(OwnerComp.GetAIOwner()->GetCharacter());
	if (Boss == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "MovementStateSetter : Boss Casting Failed !");
		return EBTNodeResult::Failed;
	}

	Boss->SetMoveState();

	return EBTNodeResult::Succeeded;
}
