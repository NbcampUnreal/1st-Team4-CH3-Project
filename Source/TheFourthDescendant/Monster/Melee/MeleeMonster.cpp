// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeMonster.h"


void AMeleeMonster::Attack()
{
	// Todo 플레이어에게 데미지 들어가는 로직 구현
}

void AMeleeMonster::Move()
{
	if (EnemyController->bIsArrived)
	{
		bCanAttack = true;
		Blackboard->SetValueAsBool(FName("IsAttacking"), true);
	}
	EnemyController->MoveToTargetActor(Player);
}

void AMeleeMonster::AttackCompleted()
{
	bCanAttack = false;
	Blackboard->SetValueAsBool(FName("IsAttacking"), false);
	EnemyController->bIsArrived = false;
}
