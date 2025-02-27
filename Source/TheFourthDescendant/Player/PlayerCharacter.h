// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "TheFourthDescendant/Abstracts/CharacterBase.h"
#include "TheFourthDescendant/Weapon/WeaponBase.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;

UCLASS()
class THEFOURTHDESCENDANT_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	APlayerCharacter();

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthAndShieldChanged, int, Health, int, Shield);
	/** 체력이나 실드가 변경되었을 때 호출되는 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Player|Events")
	FOnHealthAndShieldChanged OnHealthAndShieldChanged;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipWeapon, AWeaponBase*, Weapon);
	/** 무기가 변경되었을 때 호출되는 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Player|Events")
	FOnEquipWeapon OnEquipWeapon;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTotalAmmoChanged, EAmmoType, AmmoType, int, TotalAmmo);
	/** 최대 탄약이 변경되었을 때 호출되는 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Player|Events")
	FOnTotalAmmoChanged OnTotalAmmoChanged;

	/** 장전 애니메이션 재생 여부, ABP에서 값을 전달 받는다.*/
	UPROPERTY(BlueprintReadWrite, Category = "Player|Animation")
	bool bIsOnAttackAnimState; 
protected:
	
	/** 달리기 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Locomotion")
	float SprintSpeed;
	/** 숙이기 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Locomotion")
	float CrouchSpeed;
	/** 캐릭터가 달리고 있는지 여부 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Player|Locomotion")
	bool bIsSprinting;
	/** 캐릭터가 숙이고 있는 중인지 여부 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Player|Locomotion")
	bool bIsCrouching;
	/** 캐릭터가 조준 중인지 여부, 왼클릭해서 공격 중이거나 오른 클릭해서 조준 중이거나 둘 중 하나이다.
	 * ABP에서 사용되는 변수이다.
	 * 몽타주가 재생 중일 수 있으므로 실질적으로 조준 중인지를 확인하기 위해서는 bIsOnAttackAnimState를 확인해야 한다.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Player|Locomotion")
	bool bIsAiming;
	/** 캐릭터가 공격 버튼을 눌렀을 경우 */
	bool bIsShooting;
	/** 캐릭터가 오른쪽 마우스 버튼을 눌러서 조준을 하고 있을 경우 */
	bool bIsManualAiming;
	/** 재장중인지 여부 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Player|Locomotion")
	bool bIsReloading;
	/** UpperBody Slot에서 몽타주 재생 여부 */
	bool bIsUpperBodyActive;
	/** 캐릭터 이동 입력 여부 */
	bool bIsMoving;

	/** 조준을 하지 않았을 때 카메라 암 길이 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Attack")
	float NormalSpringArmLength;
	/* 조준 했을 때 카메라 암 길이 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Attack")
	float AimSpringArmLength;
	/* 조준 보간 속도 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Attack")
	float ZoomInterpSpeed;
	
	/** 오른손 무기 장착 소켓 이름 */
	static const FName RWeaponSocketName;
	/** 왼손 무기 장착 소켓 이름 */
	static const FName LWeaponSocketName;
	/** 초기 소지 장비 클래스 */
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> StartWeaponClass;
	/** 현재 장착된 무기 */
	UPROPERTY(Transient, BlueprintReadOnly)
	AWeaponBase* CurrentWeapon;
	/** 재장전 UI를 업데이트 할 Timer의 핸들 */
	FTimerHandle ReloadUIUpdateTimerHandle;
	/** 재장전 UI를 업데이트 할 주기 */
	float ReloadUIUpdateInterval;
	/** 재장전 진행 경과 시간 */
	float ReloadElapsedTime;
	/** 총알 소지 개수*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	TMap<EAmmoType, int32> AmmoInventory;

	/** 최소 발소리 재생 간격, 다리 움직임이 블렌드되면서 빠르게 여러번 재생 되는 현상을 방지 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	float FootStepInterval;
	/** 마지막 발소리 재생 시간, 재생 간격을 위한 변수 */
	float LastFootStepTime;
	/** 점프 시에 Skip할 발자국 간격, 점프 이후에 바로 발소리가 출력되어서 소리가 겹치는 현상을 방지*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	int OnLandMoveSkipCount;
	/** 걷기 발소리 사운드*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	class USoundBase* WalkFootStepSound;
	/** 뛰기 발소리 사운드*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	class USoundBase* RunFootStepSound;
	/** 질주 발소리 사운드*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	class USoundBase* SprintFootStepSound;
	/** 착지 사운드*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	class USoundBase* LandSound;
	
private:
	/** TPS 카메라 컴포넌트 */
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* CameraComponent;
	/** 카메라가 부착될 스프링 암 컴포넌트 */
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArmComponent;

public:
	/** Mapping Context에 따라 입력을 바인딩*/
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/** 캐릭터의 상태를 업데이트
	 * Trigger Event를 Tick처럼 사용하고 있었지만 Trigger가 겹칠 경우 제대로 변수가 업데이트되지 않을 수 있다.
	 * 따라서 Tick으로 분리하도록 한다.
	 */
	virtual void Tick(float DeltaSeconds) override;

	/** Amount 만큼 체력을 증가 */
	UFUNCTION(BlueprintCallable)
	void IncreaseHealth(const int Amount);
	/** Amount 만큼 체력을 감소, NewHealth = Health - Amount (Amount > 0) */
	UFUNCTION(BlueprintCallable)
	void DecreaseHealth(const int Amount);
	/** Amount 만큼 실드량을 증가 */
	UFUNCTION(BlueprintCallable)
	void IncreaseShield(const int Amount);
	/** Amount 만큼 실드량을 감소, NewShield = Shield - Amount (Amount > 0)
	 * 실드량만을 감소시키며 초과 피해를 체력에 영향을 주고 싶을 경우는 ApplyDamage를 호출 */
	UFUNCTION(BlueprintCallable)
	void DecreaseShield(const int Amount);
	/** Amount 만큼 데미지를 적용, 실드가 있을 경우 실드를 먼저 감소 */
	UFUNCTION(BlueprintCallable)
	void ApplyDamage(const int Amount);

	/** 무기를 장착 */
	void Equip(class AWeaponBase* Weapon);
	/** 탄약 추가 */
	UFUNCTION(BlueprintCallable)
	void AddAmmo(EAmmoType AmmoType, int Amount);
	/** 탄약의 총 개수를 반환 */
	UFUNCTION(BlueprintPure, Category = "Player|Weapon")
	int GetTotalAmmo(EAmmoType AmmoType) const { return AmmoInventory[AmmoType]; }

	/** 발소리 재생, 너무 빠르게 연속으로 재생하지는 않는다. */
	UFUNCTION(BlueprintCallable)
	void PlayFootStepSound();
	
protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/** 입력으로부터 IsAiming변수를 갱신, 공격 중이거나 조준 중이면 True가 된다. */
	void UpdateIsAiming();
	/** 현재 상태에ㅐ서의 Yaw Control을 업데이트
	 * 공격, 이동, 조준일 떄는 같이 회전하고 이동하지 않을 경우는 회전하지 않는다.
	 * IsAiming의 갱신이 필요하다.
	 */
	void UpdateYawControl();
	/* 현재 상태에 맞춰서 CameraArm 길이를 조절한다. */
	void UpdateCameraArmLength(float DeltaSeconds);
	/** 캐릭터가 착지했을 때 호출되는 함수 */
	virtual void Landed(const FHitResult& Hit) override;
	/** 사격 가능 여부를 확인*/
	bool CanFire() const;

	/** 소지하고 있지 않은 탄환을 0으로 초기화*/
	void InitAmmoInventory();
	/** 리로드 UI 업데이트 콜백 함수 */
	UFUNCTION()
	void OnReloadUIUpdate();
	/** 리로드 애니메이션 종료 콜백 함수 */
	UFUNCTION()
	void OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	/** IA_Move 바인딩 함수, WS : X축, AD : Y축, 캐릭터의 X축, Y축과 동일하게 맵핑
	 * 질주 시에는 앞으로는 가능하지만 후방으로는 걷기 속도로 이동한다.
	 */
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	/** IA_Move(Completed) 바인딩 함수 */
	UFUNCTION()
	void StopMove(const FInputActionValue& Value);
	/** IA_Jump(Started) 바인딩 함수 */
	UFUNCTION()
	void TriggerJump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);
	/** IA_Look(Triggered) 바인딩 함수 */
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	/** IA_Sprint(Started) 바인딩 함수 */
	UFUNCTION()
	void ToggleSprint(const FInputActionValue& Value);
	/** IA_Dodge(Started) 바인딩 함수 */
	UFUNCTION()
	void Dodge(const FInputActionValue& Value);
	/** IA_Crouch(Started) 바인딩 함수 */
	UFUNCTION()
	void ToggleCrouch(const FInputActionValue& Value);
	/** IA_Interaction(Started) 바인딩 함수 */
	UFUNCTION()
	void Interaction(const FInputActionValue& Value);
	/** IA_Fire(Started) 바인딩 함수 */
	UFUNCTION()
	void StartShoot(const FInputActionValue& Value);
	/** IA_Fire(Triggered) 바인딩 함수 */
	UFUNCTION()
	void TriggerShoot(const FInputActionValue& Value);
	/** IA_Fire(Completed) 바인딩 함수 */
	UFUNCTION()
	void StopShoot(const FInputActionValue& Value);
	/** IA_Aim(Started) 바인딩 함수, IsAiming을 토글해서 Aim 애니메이션을 재생한다. */
	UFUNCTION()
	void StartAim(const FInputActionValue& Value);
	/** IA_Aim(Completed) 바인딩 함수 */
	UFUNCTION()
	void StopAim(const FInputActionValue& Value);
	/** IA_Reload(Started) 바인딩 함수 */
	UFUNCTION()
	void Reload(const FInputActionValue& Value);
};