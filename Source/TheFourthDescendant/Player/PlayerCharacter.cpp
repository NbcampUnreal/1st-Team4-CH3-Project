// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "ShooterPlayerController.h"
#include "EnhancedInputComponent.h"
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
	PrimaryActorTick.bCanEverTick = true;

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
	bIsMoving = false;
	NormalSpringArmLength = 280.0f;
	AimSpringArmLength = 80.0f;
	ZoomInterpSpeed = 5.0f;
	
	bIsUpperBodyActive = false;
	bIsOnAttackAnimState = false;

	ReloadUIUpdateInterval = 0.1f;
	ReloadElapsedTime = 0.0f;

	FootStepInterval = 0.3f;
	MinFallSpeedForLandSound = 400.0f;

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

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateIsAiming();
	UpdateYawControl();
	UpdateCameraArmLength(DeltaSeconds);	
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
	OnHealthAndShieldChanged.Broadcast(Status.Health, Status.Shield);
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
	bIsOnAttackAnimState = false;
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
	else if (Speed < 650.f)
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
	SpringArmComponent->TargetArmLength = NormalSpringArmLength;

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
	if (bIsManualAiming && !bIsUpperBodyActive)
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
	
	if (LandSound)
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

bool APlayerCharacter::CanFire() const
{
	if (!CurrentWeapon || !bIsOnAttackAnimState) return false;
	if (bIsUpperBodyActive) return false;

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
	if (!Controller || !CurrentWeapon || bIsReloading) return;
	if (CurrentWeapon->IsMagazineFull() || AmmoInventory[CurrentWeapon->GetAmmoType()] <= 0) return;

	bIsReloading = true;
	bIsUpperBodyActive = true;
	
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
