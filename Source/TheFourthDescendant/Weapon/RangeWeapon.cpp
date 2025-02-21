// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeWeapon.h"

#include "CollisionDebugDrawingPublic.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ARangeWeapon::ARangeWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ARangeWeapon::PerformAttack()
{
	Super::PerformAttack();


	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		FVector CameraLocation;
		FRotator CameraRotation;
	    PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

		const FVector CameraTraceStart = CameraLocation;
		const FVector CameraTraceEnd = CameraLocation + CameraRotation.Vector() * 10000.f;
		FHitResult CameraHitResult;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		CollisionQueryParams.AddIgnoredActor(GetOwner());
		FVector TargetEnd = CameraTraceEnd;
		if (GetWorld()->LineTraceSingleByChannel(CameraHitResult, CameraTraceStart, CameraTraceEnd, ECollisionChannel::ECC_Visibility, CollisionQueryParams))
		{
			TargetEnd = CameraHitResult.ImpactPoint;
		}

		FHitResult HitResult;
		const FVector MuzzleLocation = GetWeaponMesh()->GetSocketLocation(FName(GetFireSocketName()));
		if (GetWorld()->LineTraceSingleByChannel(HitResult, MuzzleLocation, TargetEnd, ECollisionChannel::ECC_Visibility, CollisionQueryParams))
		{
			if (AActor* HitActor = HitResult.GetActor())
			{
				UGameplayStatics::ApplyDamage(HitActor, WeaponAttackPower, PlayerController, this, UDamageType::StaticClass());
			}
		}
		DrawLineTraces(GetWorld(), MuzzleLocation, TargetEnd, { HitResult }, 5.f);
	}
}
