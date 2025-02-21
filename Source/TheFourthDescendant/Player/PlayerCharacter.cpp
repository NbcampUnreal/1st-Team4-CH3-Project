// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "ShooterPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
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
					ETriggerEvent::Started,
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
}

void APlayerCharacter::DecreaseHealth(const int Amount)
{
	if (Amount <= 0) return;
	
	Status.Health -= Amount;
	Status.Health = FMath::Clamp(Status.Health, 0, Status.MaxHealth);

	// 사망 처리
}

void APlayerCharacter::IncreaseShield(const int Amount)
{
	if (Amount <= 0) return;
	
	Status.Shield += Amount;
	Status.Shield = FMath::Clamp(Status.Shield, 0, Status.MaxShield);
}

void APlayerCharacter::DecreaseShield(const int Amount)
{
	if (Amount <= 0) return;
	
	Status.Shield -= Amount;
	Status.Shield = FMath::Clamp(Status.Shield, 0, Status.MaxShield);
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
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RWeaponSocketName);
	CurrentWeapon = Weapon;
	Weapon->SetOwner(this);
}


void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = Status.WalkSpeed;

	CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(StartWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
	if (CurrentWeapon)
	{
		Equip(CurrentWeapon);
	}
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
}

void APlayerCharacter::TriggerShoot(const FInputActionValue& Value)
{
	if (!Controller) return;

	//@To-DO : Aiming, Shooting 중 Aim 애니메이션 처리
	bIsAiming = true;
	if (CurrentWeapon)
	{
		CurrentWeapon->StartShoot();
	}
}

void APlayerCharacter::StopShoot(const FInputActionValue& Value)
{
	if (!Controller) return;

	bIsAiming = false;
	if (CurrentWeapon)
	{
		CurrentWeapon->StopShoot();
	}
}

void APlayerCharacter::StartAim(const FInputActionValue& Value)
{
	if (!Controller) return;

	bIsAiming = true;
	bUseControllerRotationYaw = true;
}

void APlayerCharacter::StopAim(const FInputActionValue& Value)
{
	if (!Controller) return;

	bIsAiming = false;
	bUseControllerRotationYaw = false;
}

void APlayerCharacter::Reload(const FInputActionValue& Value)
{
	if (!Controller) return;

	if (CurrentWeapon)
	{
		CurrentWeapon->Reload();
	}
}
