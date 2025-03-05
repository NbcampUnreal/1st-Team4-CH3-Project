// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "TheFourthDescendant/GameManager/MainGameInstance.h"

FShootResult::FShootResult()
{
	bShouldHitMarkerOn = false;
}

// Sets default values
AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(Root);

	bCanFire = true;
	
	WeaponAttackPower = 10;
	FireInterval = 0.2f;
	RattleInterval = 1.0f;
	RecoilAmount = 2.0f;
	RecoilRecoverySpeed = 5.0f;
	MaxRecoilAmount = 15.0f;
	CurrentRecoilOffset = 0.0f;
	
	MaxAmmoInMagazine = 30;
	CurrentAmmo = MaxAmmoInMagazine;

	AttachRotator = FRotator::ZeroRotator;
}

void AWeaponBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ApplyRecoil(DeltaSeconds);
}

void AWeaponBase::StartShoot()
{
	if (bCanFire && CurrentAmmo > 0)
	{
		Attack();
	}
	else if (bCanFire && CurrentAmmo <= 0)
	{
		PlayRattleSound();
	}
}

void AWeaponBase::PlayRattleSound()
{
	if (RattleSfx)
	{
		UGameplayStatics::PlaySoundAtLocation(this, RattleSfx, GetActorLocation());
		bCanFire = false;
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AWeaponBase::OnCooldownFinished, RattleInterval, false);
	}
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
	OnAmmoChanged.Broadcast(CurrentAmmo);
	UE_LOG(LogTemp, Warning, TEXT("Total Ammo: %d, Current Ammo: %d"), TotalAmmo, CurrentAmmo);
}

FRotator AWeaponBase::GetAimRotation(const APawn* TargetPawn) const
{
	return TargetPawn ? TargetPawn->APawn::GetBaseAimRotation() : FRotator::ZeroRotator;
}

// TO-DO : Attack 실행을 Animation Event에서 호출하도록 수정
void AWeaponBase::Attack()
{
	if (CurrentAmmo <= 0)
	{
		return;
	}

	// 공격 실행, 탄환 처리, 반동 처리
	const FShootResult ShootResult = PerformAttack();
	CurrentAmmo--;
	CurrentRecoilOffset = FMath::Clamp(CurrentRecoilOffset + RecoilAmount, 0.0f, MaxRecoilAmount);

	// 발사 간격을 설정하고 발사가 불가능하도록 설정
	bCanFire = false;
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AWeaponBase::OnCooldownFinished, FireInterval, false);
	
	// UI 쪽에서 Shoot Trigger되면 탄약을 갱신하고 있어서 건들지 않는다.
	// OnAmmoChanged.Broadcast(CurrentAmmo);
	OnShootTriggered.Broadcast(ShootResult);
	
	// SFX, VFX...
	if (FireSfx)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSfx, GetActorLocation());
	}

	if (MuzzleFlashVfx)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			MuzzleFlashVfx,
			WeaponMesh,
			FName(GetMuzzleSocketName()),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::Type::SnapToTarget,
			true
		);
	}
	
	if (FireCameraShake)
	{
		// @Improve : Recoil에 따라 카메라 쉐이크의 강도를 강하게 적용하도록 수정
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(FireCameraShake);
	}

	// Game Instance
	if (UMainGameInstance* GameInstance = Cast<UMainGameInstance>(GetGameInstance()))
	{
		GameInstance->AddShootProjectileCount(1);
	}
}

void AWeaponBase::OnCooldownFinished()
{
	bCanFire = true;
}

FShootResult AWeaponBase::PerformAttack()
{
	// 세부적인 공격 구현은 상속 받은 클래스에서 구현한다.
	return FShootResult();
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

FVector AWeaponBase::CalculateTargetPoint(const AController* PlayerController, const float TraceDist) const
{
	if (PlayerController == nullptr) {return FVector::ZeroVector;}
	
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector CameraTraceStart = CameraLocation;
	const FVector CameraTraceEnd = CameraLocation + CameraRotation.Vector() * TraceDist;
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

	return TargetEnd;
}
