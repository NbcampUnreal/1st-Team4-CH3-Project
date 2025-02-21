// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeMonster.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"


void AMeleeMonster::Attack()
{
	// 공격 로직이 한 번 실행됐을 경우 return
	if (bIsAttacked) return;
	
	bIsAttacked = true;
	
	FHitResult HitResult;
	FCollisionQueryParams Params;

	// 자기 자신은 무시
	Params.AddIgnoredActor(this);

	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;
	bool HitDetected = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(AttackRange),
		Params
		);

	if (HitDetected)
	{
		if (HitResult.GetActor() == Player)
		{
			// 공격
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Attack : MeleeMonster Attack to Player !");
			UGameplayStatics::ApplyDamage(Player, AttackPower, EnemyController, this, UDamageType::StaticClass());
		}
	}
	
}

void AMeleeMonster::Move()
{
	if (EnemyController->bIsArrived)
	{
		bCanAttack = true;
		Blackboard->SetValueAsBool(FName("IsAttacking"), true);
		return;
	}
	EnemyController->MoveToTargetActor(Player);
}

void AMeleeMonster::AttackCompleted()
{
	bCanAttack = false;
	bIsAttacked = false;
	Blackboard->SetValueAsBool(FName("IsAttacking"), false);
	EnemyController->bIsArrived = false;
}
