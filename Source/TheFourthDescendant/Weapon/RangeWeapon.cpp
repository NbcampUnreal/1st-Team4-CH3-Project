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

void ARangeWeapon::PerformAttack()
{
	Super::PerformAttack();


	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		FVector CameraLocation;
		FRotator CameraRotation;
	    PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

		// 1차 라인 트레이스, 카메라 위치에서 화면 중앙 방향으로 레이 캐스트
		const FVector CameraTraceStart = CameraLocation;
		const FVector CameraTraceEnd = CameraLocation + CameraRotation.Vector() * DefaultRayLength;
		FHitResult CameraHitResult;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		CollisionQueryParams.AddIgnoredActor(GetOwner());
		
		FVector TargetEnd = CameraTraceEnd;
		if (GetWorld()->LineTraceSingleByChannel(CameraHitResult, CameraTraceStart, CameraTraceEnd, ECollisionChannel::ECC_Visibility, CollisionQueryParams))
		{
			// ImpactPoint까지로만 잡으면 부동 소수점 오차로 인해서 2차 라인 트레이싱에서 도달하지 못할 수 있다.
			// 따라서 아주 작은 값 10을 진행 방향으로 더해서 2차 라인 트레이싱에서 다시 도달할 수 있게 보장한다.
			TargetEnd = CameraHitResult.ImpactPoint + CameraRotation.Vector() * 10.f;
		}

		// 2차 라인 트레이스, 무기의 총구 위치에서 타겟 위치로 레이 캐스트
		// 카메라 위치부터 타겟 위치까지 물체가 존재하지 않아도 총구 앞에 다른 물체가 있을 수 있다.
		FCollisionQueryParams EnemyQueryParams;
		EnemyQueryParams.AddIgnoredActor(this);
		EnemyQueryParams.AddIgnoredActor(GetOwner());
		FHitResult HitResult;
		const FVector MuzzleLocation = GetWeaponMesh()->GetSocketLocation(FName(GetFireSocketName()));
		if (GetWorld()->LineTraceSingleByChannel(HitResult, MuzzleLocation, TargetEnd, ECollisionChannel::ECC_Pawn, EnemyQueryParams))
		{
			if (AActor* HitActor = HitResult.GetActor();
				HitActor && HitActor->IsA(AMonster::StaticClass()))
			{
				UGameplayStatics::ApplyDamage(HitActor, WeaponAttackPower, PlayerController, this, UDamageType::StaticClass());
			}
		}
		DrawLineTraces(GetWorld(), MuzzleLocation, TargetEnd, { HitResult }, 5.f);
	}
}
