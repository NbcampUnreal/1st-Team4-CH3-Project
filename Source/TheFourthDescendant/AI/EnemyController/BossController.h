// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BossController.generated.h"

UCLASS()
class THEFOURTHDESCENDANT_API ABossController : public AAIController
{
	GENERATED_BODY()

	
protected:
	/** Boss에게 적용할 BT */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AI")
	UBehaviorTree* BehaviorTree;


public:
	virtual void OnPossess(APawn* InPawn);

	/** 액터에게 MoveTo를 사용할 때 */
	EPathFollowingRequestResult::Type MoveToTargetActor(const AActor* Target);
	/** FVector로 MoveTo를 사용할 때 */
	EPathFollowingRequestResult::Type MoveToTargetLocation(const FVector& TargetLocation);
	/** MoveTo 성공 시 실행할 함수 */
	void OnMoveCompletedCallback(FAIRequestID RequestID, const FPathFollowingResult& Result);
};
