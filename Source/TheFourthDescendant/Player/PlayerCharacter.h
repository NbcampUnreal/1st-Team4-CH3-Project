// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "TheFourthDescendant/Abstracts/CharacterBase.h"
#include "PlayerCharacter.generated.h"

class AWeaponBase;
struct FInputActionValue;

UCLASS()
class THEFOURTHDESCENDANT_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	APlayerCharacter();

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthAndShieldChanged, int, Health, int, Shield);
	UPROPERTY(BlueprintAssignable, Category = "Player|Events")
	FOnHealthAndShieldChanged OnHealthAndShieldChanged;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipWeapon, AWeaponBase*, Weapon);
	UPROPERTY(BlueprintAssignable, Category = "Player|Events")
	FOnEquipWeapon OnEquipWeapon;
	
protected:
	
	/** 달리기 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Locomotion")
	float SprintSpeed;
	/** 숙이기 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Locomotion")
	float CrouchSpeed;
	/** 캐릭터가 달리고 있는지 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Locomotion")
	bool bIsSprinting;
	/** 캐릭터가 숙이고 있는 중인지 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Locomotion")
	bool bIsCrouching;
	/** 캐릭터가 조준 중인지 여부, 왼클릭해서 공격 중이거나 오른 클릭해서 조준 중이거나 둘 중 하나이다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Locomotion")
	bool bIsAiming;
	/** 캐릭터가 공격 버튼을 눌렀을 경우 */
	bool bIsShooting;
	/** 캐릭터가 오른쪽 마우스 버튼을 눌러서 조준을 하고 있을 경우 */
	bool bIsManualAiming;
	/** 재장중인지 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Locomotion")
	bool bIsReloading;

	/** 오른손 무기 장착 소켓 이름 */
	static const FName RWeaponSocketName;
	/** 왼손 무기 장착 소켓 이름 */
	static const FName LWeaponSocketName;
	/** 초기 소지 장비 클래스 */
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> StartWeaponClass;
	/** 현재 장착된 무기 */
	UPROPERTY()
	AWeaponBase* CurrentWeapon;
	/** 재장전 UI를 업데이트 할 Timer의 핸들 */
	FTimerHandle ReloadUIUpdateTimerHandle;
	/** 재장전 UI를 업데이트 할 주기 */
	float ReloadUIUpdateInterval;
	/** 재장전 진행 경과 시간 */
	float ReloadElapsedTime;

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
	
protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/** bIsShooting, bIsRightButtonAiming을 이용해서 최종 Aim 여부를 결정 */
	void UpdateIsAiming();

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
	/** IA_Aim(Started, Completed) 바인딩 함수, IsAiming을 토글해서 Aim 애니메이션을 재생한다. */
	UFUNCTION()
	void StartAim(const FInputActionValue& Value);
	/** IA_Aim(Completed) 바인딩 함수 */
	UFUNCTION()
	void StopAim(const FInputActionValue& Value);
	/** IA_Reload(Started) 바인딩 함수 */
	UFUNCTION()
	void Reload(const FInputActionValue& Value);
};