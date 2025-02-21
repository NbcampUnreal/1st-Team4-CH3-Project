// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(Root);

	WeaponAttackPower = 10;
	FireInterval = 0.2f;
	
	MaxAmmoInMagazine = 30;
	CurrentAmmo = MaxAmmoInMagazine;
	TotalAmmo = 90;
}

void AWeaponBase::StartShoot()
{
	// To-Do : 피격 시에 총알이 발사되지 않도록 처리
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AWeaponBase::Attack, FireInterval, true);
}

void AWeaponBase::StopShoot()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void AWeaponBase::Reload()
{
	const int AmmoToReload = MaxAmmoInMagazine - CurrentAmmo;
	if (TotalAmmo >= AmmoToReload)
	{
		TotalAmmo -= AmmoToReload;
		CurrentAmmo = MaxAmmoInMagazine;
	}
	else
	{
		CurrentAmmo += TotalAmmo;
		TotalAmmo = 0;
	}
	UE_LOG(LogTemp, Warning, TEXT("Total Ammo: %d, Current Ammo: %d"), TotalAmmo, CurrentAmmo);
}

void AWeaponBase::Attack()
{
	if (CurrentAmmo <= 0)
	{
		StopShoot();
		return;
	}

	PerformAttack();
	CurrentAmmo--;
	
	// SFX, VFX...
	if (FireSfx)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSfx, GetActorLocation());
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Attack : Current Ammo: %d"), CurrentAmmo);
}

void AWeaponBase::PerformAttack()
{
}
