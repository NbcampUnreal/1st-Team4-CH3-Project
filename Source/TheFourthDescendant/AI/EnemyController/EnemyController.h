// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "EnemyController.generated.h"

UCLASS()
class THEFOURTHDESCENDANT_API AEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	/** 플레이어 접근 상태 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion")
	bool bIsArrived;

protected:
	/** Monster에게 적용할 BT */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AI")
	UBehaviorTree* BehaviorTree;
	/** MoveTo의 AcceptanceRadius로 지정할 값 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Stat")
	float LimitRadius;

	
public:
	virtual void OnPossess(APawn* InPawn);

	/** 액터에게 MoveTo를 사용할 때 */
	EPathFollowingRequestResult::Type MoveToTargetActor(const AActor* Target);
	/** FVector로 MoveTo를 사용할 때 */
	EPathFollowingRequestResult::Type MoveToTargetLocation(const FVector& TargetLocation);
	
	void OnMoveCompletedCallback(FAIRequestID RequestID, const FPathFollowingResult& Result);

};