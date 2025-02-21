// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedMonster.h"
#include "TheFourthDescendant/Monster/Projectile/EnemyProjectile.h"

ARangedMonster::ARangedMonster()
{
	RangedAttackCount = 0;
	CurrentRangedAttackCount = 0;
	bIsReloading = false;
}

void ARangedMonster::BeginPlay()
{
	Super::BeginPlay();

	// 탄환 수 초기화
	CurrentRangedAttackCount = RangedAttackCount;
	// 소켓 추적용 메시 컴포넌트 초기화
	SkeletalMesh = this->GetMesh();
}


void ARangedMonster::Attack()
{
	// 잔탄이 0발 이하인 경우 return
	if (CurrentRangedAttackCount <= 0)
	{
		bCanAttack = false;
		bIsReloading = true;
		Blackboard->SetValueAsBool(FName("IsReloading"), true);
		return;
	}
	
	// 총구 위치
	const FVector MuzzleLocation = SkeletalMesh->GetSocketLocation(FName("MuzzleSocket"));
	// 플레이어 위치
	const FVector PlayerLocation = Player->GetActorLocation();

	FHitResult HitResult;
	FCollisionQueryParams Params;

	// 자기 자신은 무시
	Params.AddIgnoredActor(this);

	// 총구와 플레이어 사이에 물체가 있는지 확인
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, MuzzleLocation, PlayerLocation, ECC_Visibility, Params);

	// LineTracing 추적 디버그용 
	//DrawDebugLine(GetWorld(), MuzzleLocation, PlayerLocation, FColor::Red, false, 1, 0, 1.0f);
	
	if (bHit)
	{
		// 플레이어가 적중한 경우 데미지 로직 작성
		if (HitResult.GetActor() == Player)
		{
			// 공격 가능한 상태로 전환
			bCanAttack = true;
		}
	}

	// 공격한 상태라면 탄환 수 감소
	if (bCanAttack)
	{
		--CurrentRangedAttackCount;
		const FVector FireDirection = PlayerLocation - MuzzleLocation;
		AEnemyProjectile* Projectile = GetWorld()->SpawnActor<AEnemyProjectile>(ProjectileClass, MuzzleLocation, FRotator::ZeroRotator);
		Projectile->FireInDirection(FireDirection.GetSafeNormal());
	}
}

void ARangedMonster::Move()
{
	EnemyController->MoveToTargetActor(Player);
}

void ARangedMonster::ReloadCompleted()
{
	// 탄환 수 초기화
	CurrentRangedAttackCount = RangedAttackCount;
	// 장전 상태 플래그 비활성화
	bIsReloading = false;
	// AI의 장전 상태 플래그 비활성화
	Blackboard->SetValueAsBool(FName("IsReloading"), false);
}

