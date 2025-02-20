// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

UCLASS()
class THEFOURTHDESCENDANT_API AEnemyController : public AAIController
{
	GENERATED_BODY()

protected:
	/** Monster에게 적용할 BT */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AI")
	UBehaviorTree* BehaviorTree;
	
public:
	virtual void OnPossess(APawn* InPawn);
	
};