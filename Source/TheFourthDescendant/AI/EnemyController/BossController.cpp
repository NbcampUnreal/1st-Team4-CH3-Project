#include "BossController.h"
#include "Navigation/PathFollowingComponent.h"


void ABossController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	// BehaviorTree가 존재하는지 확인
	if (BehaviorTree == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "ABossController OnPossess : BehaviorTree is Null !");
		return;
	}

	// 해당 BehaviorTree 실행
	RunBehaviorTree(BehaviorTree);

	// OnMoveComplete 함수 바인딩
	GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &ABossController::OnMoveCompletedCallback);
}



EPathFollowingRequestResult::Type ABossController::MoveToTargetActor(const AActor* Target)
{
	if (!GetPawn()) return EPathFollowingRequestResult::Type::Failed;
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);

	return MoveTo(MoveRequest);
}



EPathFollowingRequestResult::Type ABossController::MoveToTargetLocation(const FVector& TargetLocation)
{
	if (!GetPawn()) return EPathFollowingRequestResult::Type::Failed;
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(TargetLocation);

	return MoveTo(MoveRequest);
}



void ABossController::OnMoveCompletedCallback(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (Result.Code == EPathFollowingResult::Success)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "ABossController::OnMoveCompletedCallback : Success");
	}
}

