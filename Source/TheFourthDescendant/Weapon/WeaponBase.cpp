// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(Root);

	WeaponAttackPower = 10;
	FireInterval = 0.2f;
	RecoilAmount = 2.0f;
	RecoilRecoverySpeed = 5.0f;
	MaxRecoilAmount = 15.0f;
	CurrentRecoilOffset = 0.0f;
	
	MaxAmmoInMagazine = 30;
	CurrentAmmo = MaxAmmoInMagazine;
}

void AWeaponBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ApplyRecoil(DeltaSeconds);
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

void AWeaponBase::Reload(int& TotalAmmo)
{
	if (TotalAmmo <= 0) return;
	
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

// TO-DO : Attack 실행을 Animation Event에서 호출하도록 수정
void AWeaponBase::Attack()
{
	if (CurrentAmmo <= 0)
	{
		StopShoot();
		return;
	}

	PerformAttack();
	CurrentAmmo--;
	CurrentRecoilOffset = FMath::Clamp(CurrentRecoilOffset + RecoilAmount, 0.0f, MaxRecoilAmount);
	
	// SFX, VFX...
	if (FireSfx)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSfx, GetActorLocation());
	}

	if (FireCameraShake)
	{
		// @Improve : Recoil에 따라 카메라 쉐이크의 강도를 강하게 적용하도록 수정
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(FireCameraShake);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Attack : Current Ammo: %d"), CurrentAmmo);
}

void AWeaponBase::PerformAttack()
{
	// 세부적인 공격 구현은 상속 받은 클래스에서 구현한다.
}

void AWeaponBase::ApplyRecoil(float DeltaTime)
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		PlayerController && CurrentRecoilOffset > 0.0f)
	{
		FRotator ControlRotation = PlayerController->GetControlRotation();

		// 반동 적용
		const float NewPitch = FMath::ClampAngle(ControlRotation.Pitch + CurrentRecoilOffset * DeltaTime, -88.0f, 88.0f);
		ControlRotation.Pitch = NewPitch;
		PlayerController->SetControlRotation(ControlRotation);

		// FInterpTo는 Distance에 비례해서 움직이기 때문에 처음 움직임이 빠르다. 이러한 움직임을 만들어내기에 적합하다.
		CurrentRecoilOffset = FMath::FInterpTo(CurrentRecoilOffset, 0.0f, DeltaTime, RecoilRecoverySpeed);
	}
}
