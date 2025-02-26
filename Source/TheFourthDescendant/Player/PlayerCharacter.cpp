// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "ShooterPlayerController.h"
#include "EnhancedInputComponent.h"
#include "GameSessionSettings.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TheFourthDescendant/Weapon/WeaponBase.h"

const FName APlayerCharacter::LWeaponSocketName(TEXT("LHandWeaponSocket"));
const FName APlayerCharacter::RWeaponSocketName(TEXT("RHandWeaponSocket"));

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 300.0f;
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	
	Status.WalkSpeed = 600.0f;
	SprintSpeed = 1200.0f;
	bIsSprinting = false;
	bIsAiming = false;
	bIsShooting = false;
	bIsManualAiming = false;
	bIsReloading = false;

	ReloadUIUpdateInterval = 0.1f;
	ReloadElapsedTime = 0.0f;

	FootStepInterval = 0.3f;

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
		}
	}
}

void APlayerCharacter::IncreaseHealth(const int Amount)
{
	if (Amount <= 0) return;
	
	Status.Health += Amount;
	Status.Health = FMath::Clamp(Status.Health, 0, Status.MaxHealth);

	OnHealthAndShieldChanged.Broadcast(Status.Health, Status.Shield);
}

void APlayerCharacter::DecreaseHealth(const int Amount)
{
	if (Amount <= 0) return;
	
	Status.Health -= Amount;
	Status.Health = FMath::Clamp(Status.Health, 0, Status.MaxHealth);

	// 사망 처리
	OnHealthAndShieldChanged.Broadcast(Status.Health, Status.Shield);
}

void APlayerCharacter::IncreaseShield(const int Amount)
{
	if (Amount <= 0) return;
	
	Status.Shield += Amount;
	Status.Shield = FMath::Clamp(Status.Shield, 0, Status.MaxShield);

	OnHealthAndShieldChanged.Broadcast(Status.Health, Status.Shield);
}

void APlayerCharacter::DecreaseShield(const int Amount)
{
	if (Amount <= 0) return;
	
	Status.Shield -= Amount;
	Status.Shield = FMath::Clamp(Status.Shield, 0, Status.MaxShield);

	OnHealthAndShieldChanged.Broadcast(Status.Health, Status.Shield);
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

	// 사망 처리
}

void APlayerCharacter::Equip(class AWeaponBase* Weapon)
{
	if (Weapon == CurrentWeapon) return;
	// @To-Do : 기존 무기가 있으면 탈착
	if (CurrentWeapon != nullptr)
	{
		
	}

	CurrentWeapon = Weapon;
	// @To-Do : 무기가 있으면 무기 교체, null일 경우 무기 해제
	
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RWeaponSocketName);
	Weapon->SetOwner(this);
	OnEquipWeapon.Broadcast(Weapon);
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


void APlayerCharacter::PlayFootStepSound()
{
	if (GetWorld()->GetTimeSeconds() - LastFootStepTime < FootStepInterval)
	{
		return;;
	}

	LastFootStepTime = GetWorld()->GetTimeSeconds();
	
	const float Speed = GetVelocity().Size();
	USoundBase* FootStepSound = nullptr;
	if (Speed < 250.f)
	{
		FootStepSound = WalkFootStepSound;
	}
	else if (Speed < 600.f)
	{
		FootStepSound = RunFootStepSound;
	}
	else
	{
		FootStepSound = SprintFootStepSound;
	}

	if (FootStepSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FootStepSound, GetActorLocation());
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = Status.WalkSpeed;

	InitAmmoInventory();
	if (StartWeaponClass)
	{
		if (AWeaponBase* StartWeapon = GetWorld()->SpawnActor<AWeaponBase>(StartWeaponClass))
		{
			Equip(StartWeapon);
		}
	}

	OnHealthAndShieldChanged.Broadcast(Status.Health, Status.Shield);
}

float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float Amount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	OnHealthAndShieldChanged.Broadcast(Status.Health, Status.Shield);
	if (Status.Health <= 0)
	{
		// 사망 처리
	}
	
	return  Amount;
}

void APlayerCharacter::UpdateIsAiming()
{
	bIsAiming = bIsManualAiming || bIsShooting;
	bUseControllerRotationYaw = bIsAiming;
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
	GetWorldTimerManager().ClearTimer(ReloadUIUpdateTimerHandle);
	if (CurrentWeapon)
	{
		EAmmoType WeaponAmmoType = CurrentWeapon->GetAmmoType();
		CurrentWeapon->Reload(AmmoInventory[WeaponAmmoType]);
		OnTotalAmmoChanged.Broadcast(CurrentWeapon->GetAmmoType(), AmmoInventory[WeaponAmmoType]);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	const FVector2D MoveInput = Value.Get<FVector2D>();
	const FRotator ControllerRotator = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);

	// World에서 Controller의 X축, Y축 방향
	const FVector ForwardVector = FRotationMatrix(ControllerRotator).GetUnitAxis(EAxis::X);
	const FVector RightVector = FRotationMatrix(ControllerRotator).GetUnitAxis(EAxis::Y);

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
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(RightVector, MoveInput.Y);
	}

	bUseControllerRotationYaw = true;
}

void APlayerCharacter::StopMove(const FInputActionValue& Value)
{
	bUseControllerRotationYaw = false;
}

void APlayerCharacter::TriggerJump(const FInputActionValue& Value)
{
	if (!Controller) return;

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
}

void APlayerCharacter::ToggleCrouch(const FInputActionValue& Value)
{
	bIsCrouching = !bIsCrouching;
	if (bIsCrouching)
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

void APlayerCharacter::Interaction(const FInputActionValue& Value)
{
}

void APlayerCharacter::StartShoot(const FInputActionValue& Value)
{
	if (!Controller) return;

	//@To-DO : Aiming, Shooting 중 Aim 애니메이션 처리
	if (CurrentWeapon)
	{
		bIsShooting = true;
		CurrentWeapon->StartShoot();
	}
	UpdateIsAiming();
}

void APlayerCharacter::TriggerShoot(const FInputActionValue& Value)
{
	
}

void APlayerCharacter::StopShoot(const FInputActionValue& Value)
{
	if (!Controller) return;

	bIsShooting = false;
	if (CurrentWeapon)
	{
		CurrentWeapon->StopShoot();
	}
	UpdateIsAiming();
}

void APlayerCharacter::StartAim(const FInputActionValue& Value)
{
	if (!Controller) return;

	// 무기가 없을 경우 Aim되어서는 안 된다.
	if (CurrentWeapon)
	{
		bIsManualAiming = true;
		bUseControllerRotationYaw = true;
	}
	UpdateIsAiming();
}

void APlayerCharacter::StopAim(const FInputActionValue& Value)
{
	if (!Controller) return;

	bIsManualAiming = false;
	bUseControllerRotationYaw = false;
	UpdateIsAiming();
}

void APlayerCharacter::Reload(const FInputActionValue& Value)
{
	// 상태가 많아지고 있다. FSM 사용을 고려할 것
	if (!Controller || !CurrentWeapon || bIsReloading) return;
	if (CurrentWeapon->IsMagazineFull() || AmmoInventory[CurrentWeapon->GetAmmoType()] <= 0) return;
	
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
