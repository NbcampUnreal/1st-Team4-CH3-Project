// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"

#include "Kismet/GameplayStatics.h"
#include "TheFourthDescendant/AI/EnemyController/EnemyController.h"

AMonster::AMonster()
{
	AttackPower = 0;
	bCanAttack = false;
	bIsSpawned = false;
	bIsDead = false;

	Player = nullptr;
}

void AMonster::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어 캐스팅
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	if (Player == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Monster BeginPlay : Character Casting Failed !");
	}

	// AIController 캐스팅
	AEnemyController* EnemyController = Cast<AEnemyController>(GetController());
	if (EnemyController == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Monster BeginPlay : AIController Casting Failed !");
	}

	// 블랙보드 할당
	Blackboard = EnemyController->GetBlackboardComponent();
}


void AMonster::Attack()
{
}