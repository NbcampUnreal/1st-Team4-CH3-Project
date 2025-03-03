// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeWeapon.h"

#include "CollisionDebugDrawingPublic.h"
#include "Kismet/GameplayStatics.h"
#include "TheFourthDescendant/Monster/Monster.h"

const int ARangeWeapon::DefaultRayLength = 5000;

// Sets default values
ARangeWeapon::ARangeWeapon()
{
}

FShootResult ARangeWeapon::PerformAttack()
{
	Super::PerformAttack();

	FShootResult ShootResult;
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		FVector TargetEnd = CalculateTargetPoint(PlayerController, DefaultRayLength);

		// 2차 라인 트레이스, 무기의 총구 위치에서 타겟 위치로 레이 캐스트
		// 카메라 위치부터 타겟 위치까지 물체가 존재하지 않아도 총구 앞에 다른 물체가 있을 수 있다.
		FCollisionQueryParams EnemyQueryParams;
		EnemyQueryParams.AddIgnoredActor(this);
		EnemyQueryParams.AddIgnoredActor(GetOwner());
		FHitResult HitResult;
		if (const FVector MuzzleLocation = GetWeaponMesh()->GetSocketLocation(FName(GetFireSocketName()));
			GetWorld()->LineTraceSingleByChannel(HitResult, MuzzleLocation, TargetEnd, ECollisionChannel::ECC_Pawn, EnemyQueryParams))
		{
			if (AActor* HitActor = HitResult.GetActor();
				HitActor && HitActor->IsA(ACharacterBase::StaticClass()))
			{
				UGameplayStatics::ApplyDamage(HitActor, WeaponAttackPower, PlayerController, this, UDamageType::StaticClass());
				ShootResult.bShouldHitMarkerOn = true;
			}
		}
	}

	return ShootResult;
}
