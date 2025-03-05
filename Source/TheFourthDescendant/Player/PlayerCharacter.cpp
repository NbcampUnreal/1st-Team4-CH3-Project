// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "ShooterPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TheFourthDescendant/GameManager/MainGameInstance.h"
#include "TheFourthDescendant/Weapon/WeaponBase.h"

const FName APlayerCharacter::LWeaponSocketName(TEXT("LHandWeaponSocket"));
const FName APlayerCharacter::RWeaponSocketName(TEXT("RHandWeaponSocket"));
const int APlayerCharacter::MaxWeaponSlotCount = 3;

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 300.0f;
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	bInvincible = false;
	bDodgeInvincible = false;
	bIsDeath = false;

	ShieldRechargeRate = 10.0f;
	ShieldRechargeInterval = 1.0f;
	ShieldRechargeDelay = 5.0f;
	CurrentShieldFloatRemain = 0.0f;
	
	// StateMachineContext.Character = this;
	// StateMachineContext.Weapon = nullptr;
	
	Status.WalkSpeed = 600.0f;
	SprintSpeed = 1200.0f;
	bIsSprinting = false;
	bIsAiming = false;
	bIsShooting = false;
	bIsManualAiming = false;
	bIsReloading = false;
	bIsMoving = false;
	NormalSpringArmLength = 280.0f;
	AimSpringArmLength = 80.0f;
	ZoomInterpSpeed = 5.0f;
	
	bIsUpperBodyActive = false;
	bIsOnAttackAnimState = false;

	StartWeaponClasses.SetNum(MaxWeaponSlotCount);
	
	DodgeSpeed = 2000.0f;
	DodgeUpdateInterval = 0.01f;
	DodgeElapsedTime = 0.0f;
	DodgeDirection = FVector::ZeroVector;
	
	ReloadUIUpdateInterval = 0.1f;
	ReloadElapsedTime = 0.0f;

	FootStepInterval = 0.3f;
	MinFallSpeedForLandSound = 400.0f;
	bShouldHandGrab = true;

	DodgeSoundProbability = 0.5f;
	ReloadSoundProbability = 0.8f;
	ReloadWordMinAmmo = 15;

	ShieldBrokenSoundCoolDown = 10.0f;
	bCanPlayShieldBrokenSound = true;

	DamageSoundProbability = 0.5f;
	DamageSoundCoolDown = 2.0f;
	bCanPlayDamageSound = true;
	
	Tags.Add(TEXT("Player"));
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AShooterPlayerController* PlayerController = GetController<AShooterPlayerController>())
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&APlayerCharacter::Move
				);

				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Completed,
					this,
					&APlayerCharacter::StopMove
				);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&APlayerCharacter::TriggerJump
				);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&APlayerCharacter::StopJump
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&APlayerCharacter::Look
				);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Started,
					this,
					&APlayerCharacter::ToggleSprint
				);
			}

			if (PlayerController->DodgeAction)
			{
				EnhancedInput->BindAction(
					PlayerController->DodgeAction,
					ETriggerEvent::Started,
					this,
					&APlayerCharacter::Dodge
				);
			}

			if (PlayerController->CrouchAction)
			{
				EnhancedInput->BindAction(
					PlayerController->CrouchAction,
					ETriggerEvent::Started,
					this,
					&APlayerCharacter::ToggleCrouch
				);
			}

			if (PlayerController->InteractionAction)
			{
				EnhancedInput->BindAction(
					PlayerController->InteractionAction,
					ETriggerEvent::Triggered,
					this,
					&APlayerCharacter::Interaction
				);
			}

			if (PlayerController->FireAction)
			{
				EnhancedInput->BindAction(
					PlayerController->FireAction,
					ETriggerEvent::Started,
					this,
					&APlayerCharacter::StartShoot
				);

				EnhancedInput->BindAction(
					PlayerController->FireAction,
					ETriggerEvent::Triggered,
					this,
					&APlayerCharacter::TriggerShoot
				);

				EnhancedInput->BindAction(
					PlayerController->FireAction,
					ETriggerEvent::Completed,
					this,
					&APlayerCharacter::StopShoot
				);
			}

			if (PlayerController->AimAction)
			{
				EnhancedInput->BindAction(
					PlayerController->AimAction,
					ETriggerEvent::Started,
					this,
					&APlayerCharacter::StartAim
				);

				EnhancedInput->BindAction(
					PlayerController->AimAction,
					ETriggerEvent::Completed,
					this,
					&APlayerCharacter::StopAim
				);
			}

			if (PlayerController->ReloadAction)
			{
				EnhancedInput->BindAction(
					PlayerController->ReloadAction,
					ETriggerEvent::Started,
					this,
					&APlayerCharacter::Reload
				);
			}

			if (PlayerController->EquipWeapon1Action)
			{
				EnhancedInput->BindAction(
					PlayerController->EquipWeapon1Action,
					ETriggerEvent::Started,
					this,
					&APlayerCharacter::EquipWeaponSlot1
				);
			}

			if (PlayerController->EquipWeapon2Action)
			{
			    EnhancedInput->BindAction(
			        PlayerController->EquipWeapon2Action,
			        ETriggerEvent::Started,
			        this,
			        &APlayerCharacter::EquipWeaponSlot2
			    );
			}
			
			if (PlayerController->EquipWeapon3Action)
			{
			    EnhancedInput->BindAction(
			        PlayerController->EquipWeapon3Action,
			        ETriggerEvent::Started,
			        this,
			        &APlayerCharacter::EquipWeaponSlot3
			    );
			}
		}
	}
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateIsAiming();
	UpdateYawControl();
	UpdateCameraArmLength(DeltaSeconds);
	PrevShield = Status.Shield;
}

void APlayerCharacter::SetInvincibility(bool bEnable)
{
	const bool bWasInvincible = IsInvincible();
	bInvincible = bEnable;
	if (bWasInvincible == IsInvincible())
	{
		return;
	}

	UpdateInvincible();
}

bool APlayerCharacter::IsInvincible() const
{
	return bInvincible || bDodgeInvincible;
}

void APlayerCharacter::UpdateInvincible()
{
	if (IsInvincible())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->RecreatePhysicsState();
	}
	else
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
	}
}

void APlayerCharacter::IncreaseHealth(const int Amount)
{
	if (Amount <= 0) return;
	
	Status.Health += Amount;
	Status.Health = FMath::Clamp(Status.Health, 0, Status.MaxHealth);

	OnHealthAndShieldChanged.Broadcast(FDurableChangeInfo(Status));
}

void APlayerCharacter::DecreaseHealth(const int Amount)
{
	if (Amount <= 0) return;
	
	Status.Health -= Amount;
	Status.Health = FMath::Clamp(Status.Health, 0, Status.MaxHealth);

	OnHealthAndShieldChanged.Broadcast(FDurableChangeInfo(Status));
	// 사망 처리
	if (Status.Health <= 0)
	{
		Die();
	}
}

void APlayerCharacter::IncreaseShield(const int Amount)
{
	if (Amount <= 0) return;
	
	Status.Shield += Amount;
	Status.Shield = FMath::Clamp(Status.Shield, 0, Status.MaxShield);

	OnHealthAndShieldChanged.Broadcast(FDurableChangeInfo(Status));
}

void APlayerCharacter::DecreaseShield(const int Amount)
{
	if (Amount <= 0) return;
	
	Status.Shield -= Amount;
	Status.Shield = FMath::Clamp(Status.Shield, 0, Status.MaxShield);

	OnHealthAndShieldChanged.Broadcast(FDurableChangeInfo(Status));

	if (Status.Shield < Status.MaxShield)
	{
		GetWorldTimerManager().SetTimer(ShieldRechargeTimerHandle, this, &APlayerCharacter::StartRechargeShield, ShieldRechargeDelay, false);
	}
}

void APlayerCharacter::ApplyDamage(const int Amount)
{
	if (Amount <= 0) return;

	Status.Shield -= Amount;
	if (Status.Shield < 0)
	{
		Status.Health += Status.Shield;
		Status.Shield = 0;
	}

	if (Status.Shield < Status.MaxShield)
	{
		GetWorldTimerManager().SetTimer(ShieldRechargeTimerHandle, this, &APlayerCharacter::StartRechargeShield, ShieldRechargeDelay, false);
	}
	OnHealthAndShieldChanged.Broadcast(FDurableChangeInfo(Status));

	// 사망 처리
	if (Status.Health <= 0)
	{
		Die();
	}
}

void APlayerCharacter::Die()
{
	if (bIsDeath) return;
	
	bIsDeath = true;
	bIsFullBodyActive = true;
	bIsAiming = false;
	OnPlayerBecomeDeath.Broadcast();

	GetWorldTimerManager().ClearAllTimersForObject(this);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	if (!AnimInstance) return;
	AnimInstance->StopAllMontages(false);
	
	EWeaponType CurrentWeaponType = CurrentWeapon ? CurrentWeapon->GetWeaponType() : EWeaponType::None;
	UAnimMontage** DeathMontage = DeathMontages.Find(CurrentWeaponType);
	if (DeathMontage && *DeathMontage)
	{
		const float MontageLength = AnimInstance->Montage_Play(*DeathMontage);

		// Auto Blend Out을 비활성화했을 때 BlendOut, End가 호출이 안 된다.
		// 따라서 Timer를 이용해서 직접 종료를 처리하기로 했다.
		GetWorldTimerManager().SetTimer(DeathMontageTimerHandle, this, &APlayerCharacter::OnDeathMontageEnded, MontageLength, false);
	}
	else
	{
		OnDeathMontageEnded();
	}

	if (UMainGameInstance* MainGameInstance = GetGameInstance<UMainGameInstance>())
	{
		MainGameInstance->AddDeathCount(1);
	}
}

void APlayerCharacter::OnDeathMontageEnded()
{
	// Death Montage 종류 후
	OnPlayerCompleteDeath.Broadcast();
}

void APlayerCharacter::StartRechargeShield()
{
	CurrentShieldFloatRemain = 0.0f;
	GetWorldTimerManager().SetTimer(ShieldRechargeTimerHandle, this, &APlayerCharacter::RechargeShield, ShieldRechargeInterval, true);
}

void APlayerCharacter::RechargeShield()
{
	CurrentShieldFloatRemain += ShieldRechargeRate * ShieldRechargeInterval;
	const int32 ShieldRechargeAmount = FMath::FloorToInt(CurrentShieldFloatRemain);
	if (ShieldRechargeAmount <= 0)
	{
		return;
	}

	Status.Shield = FMath::Clamp(Status.Shield + ShieldRechargeAmount, 0, Status.MaxShield);
	CurrentShieldFloatRemain -= ShieldRechargeAmount;
	if (Status.Shield >= Status.MaxShield)
    {
        GetWorldTimerManager().ClearTimer(ShieldRechargeTimerHandle);
    }
	
	OnHealthAndShieldChanged.Broadcast(FDurableChangeInfo(Status));
}

void APlayerCharacter::InitWeaponInventory()
{
	StartWeaponClasses.RemoveAll([](const TSubclassOf<AWeaponBase>& WeaponClass) { return WeaponClass == nullptr; });
	const int ValidWeaponCount = FMath::Min(StartWeaponClasses.Num(), MaxWeaponSlotCount);
	for (int i = 0; i < ValidWeaponCount; i++)
	{
		FActorSpawnParameters Param;
		Param.Owner = this;

		if (AWeaponBase* NewWeapon = GetWorld()->SpawnActor<AWeaponBase>(StartWeaponClasses[i]))
		{
			NewWeapon->SetActorHiddenInGame(true);
			WeaponSlots.Add(NewWeapon);
		}
	}
}

// 장착 모션 자체는 재장전 로직과 동일하다. 하나의 차이가 있다면 재장전은 모션이 취소되면 끊기지만 무기 교체는 이미 끝났다는 것이다.
void APlayerCharacter::EquipWeaponByIndex(int I)
{
	if (!Controller || bIsFullBodyActive) return;
	if (!WeaponSlots.IsValidIndex(I) || CurrentWeapon == WeaponSlots[I] ||  bIsExchangeWeapon) return;

	bIsExchangeWeapon = true;
	bIsUpperBodyActive = true;
	bShouldHandGrab = false;

	StartExchangeAnim(I);
}

void APlayerCharacter::StartExchangeAnim(int I)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	UAnimMontage* UnEquipMontage = UnEquipMontages[CurrentWeapon->GetWeaponType()];
	if (AnimInstance && UnEquipMontage)
	{
		AnimInstance->Montage_Play(UnEquipMontage);

		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &APlayerCharacter::OnUnEquipMontageEnded, I);
		AnimInstance->Montage_SetBlendingOutDelegate(MontageEndedDelegate);
	}
}

void APlayerCharacter::OnUnEquipMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted, int32 WeaponSlotIndex)
{
	AWeaponBase* NextWeapon = WeaponSlots[WeaponSlotIndex];
	Equip(NextWeapon);
	bShouldHandGrab = true;

	if (bInterrupted)
	{
		bIsExchangeWeapon = false;
		bIsUpperBodyActive = false;
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	UAnimMontage* EquipMontage = EquipMontages[NextWeapon->GetWeaponType()];
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);

		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &APlayerCharacter::OnEquipMontageEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate);
	}
}

void APlayerCharacter::OnEquipMontageEnded(UAnimMontage* AnimMontage, bool bArg)
{
	bIsExchangeWeapon = false;
	bIsUpperBodyActive = false;
}

void APlayerCharacter::Equip(class AWeaponBase* Weapon)
{
	if (Weapon == CurrentWeapon) return;
	// @To-Do : 기존 무기가 있으면 탈착
	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->SetActorHiddenInGame(true);
		CurrentWeapon = nullptr;
	}

	if (Weapon != nullptr)
	{
		CurrentWeapon = Weapon;
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RWeaponSocketName);
		Weapon->SetOwner(this);
		Weapon->SetActorHiddenInGame(false);
		bIsOnAttackAnimState = false;
		OnEquipWeapon.Broadcast(Weapon);
	}
}

FRotator APlayerCharacter::GetBaseAimRotation() const
{
	return CurrentWeapon ?
		CurrentWeapon->GetAimRotation(this) :
		Super::GetBaseAimRotation();
}

void APlayerCharacter::AddAmmo(EAmmoType AmmoType, int Amount)
{
	if (Amount <= 0) return;

	// C++ STL과 다르게 없는 키에 접근하는 것은 허용되지 않는다.
	if (AmmoInventory.Contains(AmmoType))
	{
		AmmoInventory[AmmoType] += Amount;
	}
	else
	{
		AmmoInventory.Add(AmmoType, Amount);
	}

	OnTotalAmmoChanged.Broadcast(CurrentWeapon->GetAmmoType(), AmmoInventory[AmmoType]);
}


EPhysicalSurface APlayerCharacter::GetSurfaceTypeOnFoot()
{
	FHitResult LandHitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.AddIgnoredActor(this);
	FVector StartLocation = GetActorLocation() + FVector::UpVector * 10.0f;
	FVector EndLocation = GetActorLocation() + FVector::DownVector * 100.0f;
	if (GetWorld()->LineTraceSingleByChannel(LandHitResult, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, QueryParams))
	{
		return  UGameplayStatics::GetSurfaceType(LandHitResult);
	}
	return EPhysicalSurface::SurfaceType_Default;
}

USoundBase* APlayerCharacter::GetLandFootSound(const float Speed) const
{
	if (Speed < 250.f)
	{
		return  WalkFootStepSound;
	}
	if (Speed < 650.f)
	{
		return  RunFootStepSound;
	}
	if (Speed > 650.f)
	{
		return  SprintFootStepSound;
	}
	return nullptr;
}

USoundBase* APlayerCharacter::GetWaterFootSound(float Speed)
{
	if (Speed < 450.0f)
	{
		return WaterDragSound;
	}
	if (Speed <= 800.0f)
	{
		return WaterRunSound;
	}
	
	return nullptr;
}

USoundBase* APlayerCharacter::GetFootStepSound(float Speed, EPhysicalSurface PhysicalSurface)
{
	if (PhysicalSurface == EPhysicalSurface::SurfaceType_Default)
	{
		return GetLandFootSound(Speed);
	}
	if (PhysicalSurface == EPhysicalSurface::SurfaceType1)
	{
		return  GetWaterFootSound(Speed);
	}
	return nullptr;
}

void APlayerCharacter::PlayFootStepSound()
{
	if (GetWorld()->GetTimeSeconds() - LastFootStepTime < FootStepInterval)
	{
		return;;
	}

	LastFootStepTime = GetWorld()->GetTimeSeconds();

	const float Speed = GetVelocity().Size();
	EPhysicalSurface PhysicalSurface = GetSurfaceTypeOnFoot();
	if (USoundBase* FootStepSound = GetFootStepSound(Speed, PhysicalSurface))
	{
		UGameplayStatics::PlaySoundAtLocation(this, FootStepSound, GetActorLocation());
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = Status.WalkSpeed;
	SpringArmComponent->TargetArmLength = NormalSpringArmLength;

	InitAmmoInventory();
	InitWeaponInventory();
	if (WeaponSlots.Num() > 0 && WeaponSlots[0])
	{
		Equip(WeaponSlots[0]);
	}

	PrevShield = Status.Shield;
	OnHealthAndShieldChanged.Broadcast(FDurableChangeInfo(Status));
}

void APlayerCharacter::OnDamageSoundCoolDown()
{
	bCanPlayDamageSound = true;
}

void APlayerCharacter::PlayDamageSound()
{
	if (DamageSound && bCanPlayDamageSound && FMath::FRand() < DamageSoundProbability)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DamageSound, GetActorLocation());
		bCanPlayDamageSound = false;
		GetWorldTimerManager().SetTimer(DamageSoundTimerHandle, this, &APlayerCharacter::OnDamageSoundCoolDown, DamageSoundCoolDown, false);
	}

	if (HitMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(HitMontage);
		}
	}
}

float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                   class AController* EventInstigator, AActor* DamageCauser)
{
	float Amount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	OnTakeDamage.Broadcast(FDamageInfo(Status));

	if (PrevShield > 0 && Status.Shield == 0)
	{
		PlayShieldBrokenSound();
	}
	PlayDamageSound();
	
	if (Status.Shield < Status.MaxShield)
	{
		GetWorldTimerManager().SetTimer(ShieldRechargeTimerHandle, this, &APlayerCharacter::StartRechargeShield, ShieldRechargeDelay, false);
	}

	if (UMainGameInstance* MainGameInstance = GetGameInstance<UMainGameInstance>())
	{
		MainGameInstance->AddHitProjectileCount(1);
	}
	
	if (Status.Health <= 0)
	{
		// 사망 처리
		Die();
	}

	return  Amount;
}

void APlayerCharacter::UpdateIsAiming()
{
	if (bIsDeath)
	{
		bIsAiming = false;
		return;
	}
	
	if (CurrentWeapon)
	{
		bIsAiming = bIsManualAiming || bIsShooting;	
	}
	else
	{
		bIsAiming = false;
	}
}

void APlayerCharacter::UpdateYawControl()
{
	// 상태가 많아지면서 if문이 증가하고 있다. State Machine에서 어떻게 처리할 수 있을지에 대해서 고민할 필요가 있다.
	// 현재의 난관은 이동하면서 다른 동작이 가능해서 State를 명확하게 표현하기 어렵다는 점이고 Animation이나 다른 것에서 상태 전이를 해야 된다는 점이다.

	// 이동 중이거나 실제 조준 중일 때는 회전
	if (bIsMoving || (bIsAiming && !bIsUpperBodyActive))
	{
		bUseControllerRotationYaw = true;
	}
	else
	{
		bUseControllerRotationYaw = false;
	}
}

void APlayerCharacter::UpdateCameraArmLength(float DeltaSeconds)
{
	if (bIsManualAiming && !bIsUpperBodyActive && !bIsFullBodyActive)
	{
		SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, AimSpringArmLength, DeltaSeconds, ZoomInterpSpeed);
	}
	else
	{
		SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, NormalSpringArmLength, DeltaSeconds, ZoomInterpSpeed);
	}
}

void APlayerCharacter::PlayLandSound()
{
	// 착지 다음에 다음 발소리가 바로 출력되서 소리가 겹치는 것을 방지한다.
	LastFootStepTime = GetWorld()->GetTimeSeconds() + FootStepInterval * 2;

	// @To-Do : 속도에 따른 착지 소리 재생, 가령 높은 곳에서 추락을 구현 가능
	// const float FallSpeed = -GetVelocity().Z;
	// USoundBase* LandSoundToPlay = nullptr;
	// if (FallSpeed < MinFallSpeedForLandSound)
	// {
	// 	LandSoundToPlay = nullptr;
	// }
	// else
	// {
	// 	LandSoundToPlay = LandSound;
	// }
	EPhysicalSurface PhysicalSurface = GetSurfaceTypeOnFoot();
	if (USoundBase* LandSound = GetLandSound(PhysicalSurface))
	{
		UGameplayStatics::PlaySoundAtLocation(this, LandSound, GetActorLocation());
	}
}

void APlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	if (-GetVelocity().Z > MinFallSpeedForLandSound)
	{
		PlayLandSound();
	}
}

USoundBase* APlayerCharacter::GetLandSound(EPhysicalSurface PhysicalSurface) const
{
	if (PhysicalSurface == EPhysicalSurface::SurfaceType_Default)
	{
		return  GroundLandSound;
	}
	if (PhysicalSurface == EPhysicalSurface::SurfaceType1)
	{
		return  WaterLandSound;
	}
	return nullptr;
}

bool APlayerCharacter::CanFire() const
{
	if (!CurrentWeapon || !bIsOnAttackAnimState) return false;
	if (bIsUpperBodyActive || bIsFullBodyActive || bIsDeath) return false;

	// !!! Warning
	// 원래라면 이 부분은 애니메이션 슬롯을 확인해서 작동해야 한다.
	// 하지만, 이렇게 하면 애니메이션 몽타주 종료 시점을 확인할 방법이 없다.
	// 이러한 구현의 사이드 이펙트는 Open Close Principle을 어기기 때문에 새로운 상태가 추가될 떄마다 여기가 갱신이 되어야 한다.
	// 이를 일단은 개선하기로 한다.
	// IsUpperBodyActive를 선언하고 다른 모션은 State라고 생각을 하고 State로 들어간다고 가정한다.
	// State에서의 진입 조건은 IsUpperBodyActive가 false일 때이다.
	// 이렇게 하면 각각 상태가 단일하게 존재할 수 있고 상태만 IsUpperBodyActive만 체크하면 되서 확장에 대응할 수 있게 된다.
	// 하지만, 이것은 간의적으로 FSM을 사용하는 것이므로 리펙토링에서 FSM 사용을 고려할 필요가 있다.

	// 재장전 모션 추적에서와 같이 Montage가 완전이 FadeOut되는 것을 확인할 수 없다.
	// 현재 재장전 모션 추적에서 사용되고 있는 것을 이용한다.
	// if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	// {
	// 	if (AnimInstance->IsSlotActive(FName("UpperBody")))
	// 	{
	// 		return false;
	// 	}
	// }

	return true;
}

UAnimMontage* APlayerCharacter::GetDodgeMontage(const FVector& LocalDodgeDirection) const
{
	// 단순한 각도 계산은 부동 소수점 이슈로 정확하게 일치하지 않을 수 있다.
	// 8방향에 대해서 내적을 수행하면 가장 일치하는 방향을 찾을 수 있다.
	static const FVector CardinalDirs[8] = {
		FVector(1.0f, 0.0f, 0.0f),		// Forward
		FVector(1.0f, 1.0f, 0.0f).GetSafeNormal(), // Forward Right
		FVector(0.0f, 1.0f, 0.0f), // Right
		FVector(-1.0f, 1.0f, 0.0f).GetSafeNormal(), // Backward Right
		FVector(-1.0f, 0.0f, 0.0f), // Backward
		FVector(-1.0f, -1.0f, 0.0f).GetSafeNormal(), // Backward Left
		FVector(0.0f, -1.0f, 0.0f), // Left
		FVector(1.0f, -1.0f, 0.0f).GetSafeNormal() // Forward Left
	};
	
	float MaxDot = -1.0f;
	int MaxIndex = 0;
	for (int32 i = 0; i < 8; ++i)
	{
		float Dot = FVector::DotProduct(LocalDodgeDirection, CardinalDirs[i]);
		if (Dot > MaxDot)
		{
			MaxDot = Dot;
			MaxIndex = i;
		}
	}

	switch (MaxIndex)
	{
	case 0:
		return DodgeForwardMontage;
	case 1:
		return DodgeForwardRightMontage;
	case 2:
		return DodgeRightMontage;
	case 3:
		return DodgeBackwardRightMontage;
	case 4:
		return DodgeBackwardMontage;
	case 5:
		return DodgeBackwardLeftMontage;
	case 6:
		return DodgeLeftMontage;
	case 7:
		return DodgeForwardLeftMontage;
	default:
		return DodgeForwardMontage;
	}
}

void APlayerCharacter::OnDodgeMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted)
{
	bIsFullBodyActive = false;
	SetDodgeInvincible(false);

	GetWorldTimerManager().ClearTimer(DodgeUpdateTimerHandle);
}

void APlayerCharacter::OnDodgeUpdate()
{
	DodgeElapsedTime += DodgeUpdateInterval;

	const float PlayLengthWithoutBlending = CurrentDodgeMontage->GetPlayLength() - CurrentDodgeMontage->GetDefaultBlendOutTime();
	const float Alpha = DodgeElapsedTime / PlayLengthWithoutBlending;
	const float CurveValue = DodgeCurve.GetRichCurve()->Eval(Alpha);
	FVector DodgeVelocity = DodgeDirection * DodgeSpeed * CurveValue;
	
	FVector CurrentVelocity = GetVelocity();

	DodgeVelocity.Z = CurrentVelocity.Z;
	GetCharacterMovement()->Velocity = DodgeVelocity;
}

void APlayerCharacter::SetDodgeInvincible(bool bEnable)
{
	bDodgeInvincible = bEnable;
	UpdateInvincible();
}

void APlayerCharacter::InitAmmoInventory()
{
	for (const EAmmoType AmmoType : TEnumRange<EAmmoType>())
	{
		if (!AmmoInventory.Contains(AmmoType))
		{
			AmmoInventory.Add(AmmoType, 0);
		}
	}
}

void APlayerCharacter::OnReloadUIUpdate()
{
	if (!CurrentWeapon || !CurrentWeapon->GetReloadMontage())
	{
		return;
	}

	// 현재 재장전 애니메이션의 진행도를 계산
	// Montage_GetPosition은 Blend Out 되는 순간에 0을 반환하기 때문에 Blend를 포함해서 전체 시간을 확인할 수 없다.
	// 따라서 진행 사항을 직접 계산한다.
	UAnimMontage* ReloadMontage = CurrentWeapon->GetReloadMontage();
	ReloadElapsedTime += ReloadUIUpdateInterval;
	float ReloadProgress = ReloadElapsedTime / ReloadMontage->GetPlayLength();
	UE_LOG(LogTemp, Display, TEXT("Reload Progress : %f"), ReloadProgress);
}

void APlayerCharacter::OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsReloading = false;
	bIsUpperBodyActive = false;
	bShouldHandGrab = true;
	
	GetWorldTimerManager().ClearTimer(ReloadUIUpdateTimerHandle);
	if (CurrentWeapon && !bInterrupted)
	{
		EAmmoType WeaponAmmoType = CurrentWeapon->GetAmmoType();
		CurrentWeapon->Reload(AmmoInventory[WeaponAmmoType]);
		OnTotalAmmoChanged.Broadcast(CurrentWeapon->GetAmmoType(), AmmoInventory[WeaponAmmoType]);
	}
}

void APlayerCharacter::OnShieldBrokenSoundCoolDown()
{
	bCanPlayShieldBrokenSound = true;
}

void APlayerCharacter::PlayShieldBrokenSound()
{
	if (bCanPlayShieldBrokenSound && ShieldBrokenWordSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShieldBrokenWordSound, GetActorLocation());
		bCanPlayShieldBrokenSound = false;
		GetWorldTimerManager().SetTimer(ShieldBrokenSoundTimerHandle, this, &APlayerCharacter::OnShieldBrokenSoundCoolDown, ShieldBrokenSoundCoolDown, false);
	}
	else if (ShieldBrokenSound && FMath::FRand() < ShieldBrokenSoundProbability)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ShieldBrokenSound, GetActorLocation());
    }
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;
	// 전체 몸이 사용 중이면 이동을 할 수 없다.
	if (bIsFullBodyActive || bIsDeath) return;

	const FVector2D MoveInput = Value.Get<FVector2D>();
	const FRotator ControllerRotator = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);

	// World에서 Controller의 X축, Y축 방향
	const FVector ForwardVector = FRotationMatrix(ControllerRotator).GetUnitAxis(EAxis::X);
	const FVector RightVector = FRotationMatrix(ControllerRotator).GetUnitAxis(EAxis::Y);

	// 전후 이동
	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(ForwardVector, MoveInput.X);
		
		// 전방으로 이동할 경우에만 Sprint 속도를 적용
		if (MoveInput.X > 0 && bIsSprinting)
		{
			GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = Status.WalkSpeed;
		}
	}
	// 좌우 이동
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(RightVector, MoveInput.Y);
	}

	bIsMoving = true;
}

void APlayerCharacter::StopMove(const FInputActionValue& Value)
{
	bIsMoving = false;
}

void APlayerCharacter::TriggerJump(const FInputActionValue& Value)
{
	if (!Controller || bIsDeath) return;

	if (Value.Get<bool>())
	{
		Jump();
	}
}

void APlayerCharacter::StopJump(const FInputActionValue& Value)
{
	if (!Controller) return;

	if (Value.Get<bool>())
	{
		StopJumping();
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	if (!Controller) return;

	FVector2D LookInput = Value.Get<FVector2D>();
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void APlayerCharacter::ToggleSprint(const FInputActionValue& Value)
{
	bIsSprinting = !bIsSprinting;
}

void APlayerCharacter::Dodge(const FInputActionValue& Value)
{
	// 점프 중에는 회피를 할 수 없다.
	if (GetMovementComponent()->IsFalling()) return;
	if (bIsFullBodyActive || bIsDeath) return;;
	
	bIsFullBodyActive = true;
	SetDodgeInvincible(true);
	if (DodgeSound && FMath::FRand() < DodgeSoundProbability)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DodgeSound, GetActorLocation());
	}
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	// 구르기 방향을 결정, 이동 방향이 없으면 앞으로 구르기
	DodgeDirection = GetLastMovementInputVector().GetSafeNormal();
	if (DodgeDirection.IsNearlyZero())
	{
		DodgeDirection = GetActorForwardVector();
	}
	const FVector LocalDodgeDirection = GetTransform().InverseTransformVector(DodgeDirection).GetSafeNormal();
	
	UAnimMontage* DodgeMontage = GetDodgeMontage(LocalDodgeDirection);
	CurrentDodgeMontage = DodgeMontage;

	if (AnimInstance && DodgeMontage)
	{
		AnimInstance->Montage_Play(DodgeMontage);

		// BlendOut이 되면서 이동이 시작되어야 하므로 End 시점이 아니라 BlendOut시점에서 구르기를 종료한다.
		FOnMontageBlendingOutStarted MontageBlendingOutDelegate;
		MontageBlendingOutDelegate.BindUObject(this, &APlayerCharacter::OnDodgeMontageEnded);
		AnimInstance->Montage_SetBlendingOutDelegate(MontageBlendingOutDelegate);
		
		DodgeElapsedTime = 0.0f;
		GetWorldTimerManager().SetTimer(DodgeUpdateTimerHandle, this, &APlayerCharacter::OnDodgeUpdate, DodgeUpdateInterval, true);
	}
}

void APlayerCharacter::ToggleCrouch(const FInputActionValue& Value)
{
	// UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	// AnimInstance->Montage_Play(HitMontage);
	
	// bIsCrouching = !bIsCrouching;
	// if (bIsCrouching)
	// {
	// 	Crouch();
	// }
	// else
	// {
	// 	UnCrouch();
	// }
}

void APlayerCharacter::Interaction(const FInputActionValue& Value)
{
}

void APlayerCharacter::StartShoot(const FInputActionValue& Value)
{
	if (!Controller) return;

	bIsShooting = true;
}

void APlayerCharacter::TriggerShoot(const FInputActionValue& Value)
{
	if (CurrentWeapon && CanFire())
	{
		CurrentWeapon->StartShoot();
	}
}

void APlayerCharacter::StopShoot(const FInputActionValue& Value)
{
	if (!Controller) return;

	bIsShooting = false;
}

void APlayerCharacter::StartAim(const FInputActionValue& Value)
{
	if (!Controller) return;

	bIsManualAiming = true;
}

void APlayerCharacter::StopAim(const FInputActionValue& Value)
{
	if (!Controller) return;
	
	bIsManualAiming = false;
}

void APlayerCharacter::Reload(const FInputActionValue& Value)
{
	// 상태가 많아지고 있다. FSM 사용을 고려할 것
	if (!Controller || !CurrentWeapon || bIsReloading || bIsFullBodyActive) return;
	if (CurrentWeapon->IsMagazineFull() || AmmoInventory[CurrentWeapon->GetAmmoType()] <= 0) return;

	bIsReloading = true;
	bIsUpperBodyActive = true;
	bShouldHandGrab = false;

	int AmmoToReload = CurrentWeapon->GetMaxAmmoInMagazine() - CurrentWeapon->GetCurrentAmmo();
	if (FMath::FRand() < ReloadSoundProbability)
	{
		if (ReloadWordSound && AmmoToReload >= ReloadWordMinAmmo)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ReloadWordSound, GetActorLocation());
		}
		else if (ReloadEffectSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ReloadEffectSound, GetActorLocation());
		}
	}
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	UAnimMontage* ReloadMontage = CurrentWeapon->GetReloadMontage();

	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		
		FOnMontageEnded MontageEndDelegate;
		// 몽타주가 실행된 다음은 인스턴스가 사라지기 때문에 호출할 때마다 바인딩해야 한다.
		
		MontageEndDelegate.BindUObject(this, &APlayerCharacter::OnReloadMontageEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndDelegate, ReloadMontage);

		ReloadElapsedTime = 0.0f;
		GetWorldTimerManager().SetTimer(ReloadUIUpdateTimerHandle, this, &APlayerCharacter::OnReloadUIUpdate, ReloadUIUpdateInterval, true);
	}
}

void APlayerCharacter::EquipWeaponSlot1(const FInputActionValue& InputActionValue)
{
	if (!Controller || bIsDeath) return;
	EquipWeaponByIndex(0);
}

void APlayerCharacter::EquipWeaponSlot2(const FInputActionValue& InputActionValue)
{
	if (!Controller || bIsDeath) return;
	EquipWeaponByIndex(1);
}

void APlayerCharacter::EquipWeaponSlot3(const FInputActionValue& InputActionValue)
{
	if (!Controller || bIsDeath) return;
	EquipWeaponByIndex(2);
}
