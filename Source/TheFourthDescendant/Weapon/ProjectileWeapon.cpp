// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"

#include "ProjectileBase.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AProjectileWeapon::AProjectileWeapon()
{
	AttackRange = 3000.0f;
	ProjectileSpeed = 1000.0f;
	ExplosionRadius = 100.0f;
}

FShootResult AProjectileWeapon::PerformAttack()
{
	Super::PerformAttack();

	FShootResult ShootResult;
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		const FVector StartLocation = GetWeaponMesh()->GetSocketLocation(FName(GetFireSocketName()));
		const FVector TargetLocation = CalculateTargetPoint(PlayerController, AttackRange);
		FVector LaunchVelocity;

		UGameplayStatics::FSuggestProjectileVelocityParameters Params(
			this,
			StartLocation,
			TargetLocation,
			ProjectileSpeed
		);
		Params.bDrawDebug = true;
		Params.bFavorHighArc = false;
		Params.ActorsToIgnore = { GetOwner(), this };
		Params.TraceOption = ESuggestProjVelocityTraceOption::OnlyTraceWhileAscending;
		Params.bAcceptClosestOnNoSolutions = true;
		
		if (UGameplayStatics::SuggestProjectileVelocity(Params, LaunchVelocity))
		{
			if (ProjectileClass)
			{
				UE_LOG(LogTemp, Display, TEXT("Projectile velocity suggested: %s"), *LaunchVelocity.ToString());
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = GetInstigator();
				if (AProjectileBase* Projectile = GetWorld()->SpawnActor<AProjectileBase>(ProjectileClass, StartLocation, FRotator::ZeroRotator, SpawnParams))
				{
					Projectile->LaunchProjectile(LaunchVelocity, WeaponAttackPower, ExplosionRadius);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to suggest projectile velocity"));
		}
	}

	return ShootResult;
}

