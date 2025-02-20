// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// BehaviorTree가 존재하는지 확인
	if (!BehaviorTree) return; 

	// 해당 BehaviorTree 실행
	RunBehaviorTree(BehaviorTree);
}
