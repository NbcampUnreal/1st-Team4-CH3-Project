// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	// BehaviorTree가 존재하는지 확인
	if (BehaviorTree == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "AEnemyController OnPossess : BehaviorTree is Null !");
		return;
	}

	// 해당 BehaviorTree 실행
	RunBehaviorTree(BehaviorTree);

	// OnMoveComplete 함수 바인딩
	GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AEnemyController::OnMoveCompletedCallback);
}

EPathFollowingRequestResult::Type AEnemyController::MoveToTargetActor(const AActor* Target)
{
	if (!GetPawn()) return EPathFollowingRequestResult::Type::Failed;
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(LimitRadius);

	return MoveTo(MoveRequest);
}



EPathFollowingRequestResult::Type AEnemyController::MoveToTargetLocation(const FVector& TargetLocation)
{
	if (!GetPawn()) return EPathFollowingRequestResult::Type::Failed;
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(TargetLocation);
	MoveRequest.SetAcceptanceRadius(LimitRadius);

	return MoveTo(MoveRequest);
}

void AEnemyController::OnMoveCompletedCallback(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (Result.Code == EPathFollowingResult::Success)
	{
		UE_LOG(LogTemp, Log, TEXT("AI Arrived!"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("AI Arrived!!"));
	}
}
