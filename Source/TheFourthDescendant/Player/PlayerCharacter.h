// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "TheFourthDescendant/Abstracts/CharacterBase.h"
#include "TheFourthDescendant/Weapon/WeaponBase.h"
#include "PlayerCharacter.generated.h"

class APlayerCharacter;
struct FInputActionValue;
struct FStateMachineContext
{
	APlayerCharacter* Character;
	AWeaponBase* Weapon;
	TMap<EAmmoType, int32>* AmmoInventory;
};

USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

	FDamageInfo()
		: MaxHealth(0.f), MaxShield(0.f), Health(0.f), Shield(0.f), LostShield(0.f), LostHealth(0.f) {}
	FDamageInfo(const FStatus& Status)
		: MaxHealth(Status.MaxHealth), MaxShield(Status.MaxShield), Health(Status.Health), Shield(Status.Shield), LostShield(0.f), LostHealth(0.f) {}
	UPROPERTY(BlueprintReadOnly)
	float MaxHealth;
	UPROPERTY(BlueprintReadOnly)
	float MaxShield;
	UPROPERTY(BlueprintReadOnly)
	float Health;
	UPROPERTY(BlueprintReadOnly)
	float Shield;
	UPROPERTY(BlueprintReadOnly)
	float LostShield;
	UPROPERTY(BlueprintReadOnly)
	float LostHealth;
};

USTRUCT(BlueprintType)
struct FDurableChangeInfo
{
	GENERATED_BODY()

	FDurableChangeInfo()
		: MaxHealth(0.f), Health(0.f), MaxShield(0.f), Shield(0.f) {}

	// Broadcast(Status) 형식으로 암시적 캐스팅으로 넘어가는데 명확한 표현을 위해 암시적 캐스팅을 제거한다.
	explicit FDurableChangeInfo(const FStatus& Status)
		: MaxHealth(Status.MaxHealth), Health(Status.Health), MaxShield(Status.MaxShield), Shield(Status.Shield) {}
	
	UPROPERTY(BlueprintReadOnly)
	float MaxHealth;
	UPROPERTY(BlueprintReadOnly)
	float Health;
	UPROPERTY(BlueprintReadOnly)
	float MaxShield;
	UPROPERTY(BlueprintReadOnly)
	float Shield;
};

/**
 * 플레이어 캐릭터 클래스
 * 캐릭터는 다음과 같은 상태를 가진다.
 * - Locomotion : 이동, 점프, 사격이 가능
 * - Reload(UpperBody) : 상체만 재장전 애니메이션을 재생,(Locomotion->Reload->Locomotion)
 * - WeaponExchange(UpperBody) : 상체만 재장전 애니메이션을 재생
 * - Dodge(FullBody) : 구르기 애니메이션을 재생
 * - Parkour(FullBody) : 파쿠르 애니메이션을 재생
 * - Grapple(FullBody) : 그래플링 애니메이션을 재생
 */
UCLASS()
class THEFOURTHDESCENDANT_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:

	APlayerCharacter();
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTakeDamage, FDamageInfo, DamageInfo);
	/** 피격되었을 때 호출되는 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Player|Events")
	FOnTakeDamage OnTakeDamage;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthAndShieldChanged, FDurableChangeInfo, HealthAndShield);
	/** 피격 없이 체력이나 실드량이 변경됬을 때 호출되는 이벤트 */
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
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerBecomeDeath);
	/** 플레이어가 사망했을 때 호출되는 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Player|Events")
	FOnPlayerBecomeDeath OnPlayerBecomeDeath;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerCompleteDeath);
	/** 사망 몽타주가 종료되었을 때 호출되는 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Player|Events")
	FOnPlayerCompleteDeath OnPlayerCompleteDeath;

	/** 장전 애니메이션 재생 여부, ABP에서 값을 전달 받는다.*/
	UPROPERTY(BlueprintReadWrite, Category = "Player|Animation")
	bool bIsOnAttackAnimState;
protected:
	// FStateMachineContext StateMachineContext;
	FTimerHandle DeathMontageTimerHandle;

	/** 무적 상태 여부 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Player|Status")
	bool bInvincible;
	/** 회피 무적 여부 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Player|Status")
	bool bDodgeInvincible;
	/** 현재 사망 상태 여부 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Player|Status")
	bool bIsDeath;

	/** 1초 동안 회복되는 실드의 양 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Status")
	float ShieldRechargeRate;
	/** 실드가 회복될 때까지 기다려야 하는 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Status")
	float ShieldRechargeInterval;
	/** 실드가 회복할 때 기다려야 하는 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Status")
	float ShieldRechargeDelay;
	/** 현재 실드 회복 수치, 실드가 int 단위이기에 소수점을 따로 저장한다. */
	float CurrentShieldFloatRemain;
	/** 실드 회복 타이머, 피격이 될 경우, 실드 딜레이던, 실드 회복이던 취소가 되기 때문에 하나의 타이머를 이용해서 구현할 수 있다. */
	FTimerHandle ShieldRechargeTimerHandle;
	
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
	/** 전신 슬롯에서 몽타주 재생 여부 */
	bool bIsFullBodyActive;
	/** UpperBody Slot에서 몽타주 재생 여부 */
	bool bIsUpperBodyActive;
	/** 캐릭터 이동 입력 여부 */
	bool bIsMoving;
	UPROPERTY(BlueprintReadOnly, Category = "Player|Locomotion")
	bool bShouldHandGrab;

	/** 구르기 기준점이 되는 속도, Curve의 값에 곱해진다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Dodge")
	float DodgeSpeed;
	/** 구르기 속도를 조절하는 커브, DodgeSpeed에 곱해진다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Dodge")
	FRuntimeFloatCurve DodgeCurve;
	/** 구르기 상태를 갱신하는 타이머 핸들 */
	FTimerHandle DodgeUpdateTimerHandle;
	/** 구르기 속도 갱신 주기 */
	float DodgeUpdateInterval;
	/** 구르기 진행도를 추적하기 위한 구르기 시간 */
	float DodgeElapsedTime;
	/** 구르기 시작했을 때의 방향, 월드 좌표계*/
	FVector DodgeDirection;
	/** Forward 방향으로 구르기 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Dodge")
	UAnimMontage* DodgeForwardMontage;
	/** Backward 방향으로 구르기 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Dodge")
	UAnimMontage* DodgeBackwardMontage;
	/** Left 방향으로 구르기 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Dodge")
	UAnimMontage* DodgeLeftMontage;
	/** Right 방향으로 구르기 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Dodge")
	UAnimMontage* DodgeRightMontage;
	/** Forward Left 방향으로 구르기 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Dodge")
	UAnimMontage* DodgeForwardLeftMontage;
	/** Forward Right 방향으로 구르기 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Dodge")
	UAnimMontage* DodgeForwardRightMontage;
	/** Backward Left 방향으로 구르기 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Dodge")
	UAnimMontage* DodgeBackwardLeftMontage;
	/** Backward Right 방향으로 구르기 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Dodge")
	UAnimMontage* DodgeBackwardRightMontage;
	/** 현재 진행 중인 구르기 몽타주 */
	TWeakObjectPtr<UAnimMontage> CurrentDodgeMontage;
	
	/** 조준을 하지 않았을 때 카메라 암 길이 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Attack")
	float NormalSpringArmLength;
	/* 조준 했을 때 카메라 암 길이 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Attack")
	float AimSpringArmLength;
	/* 조준 보간 속도 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Attack")
	float ZoomInterpSpeed;
	
	/** 최대 무기 슬롯 개수 */
	static const int MaxWeaponSlotCount;
	/** 소지 하고 있는 무기 */
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Player|Weapon")
	TArray<AWeaponBase*> WeaponSlots;
	/** 시작 무기 리스트 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Weapon")
	TArray<TSubclassOf<AWeaponBase>> StartWeaponClasses;
	/** 무기 타입에 따른 장착 애니메이션 몽타주 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Animation")
	TMap<EWeaponType, UAnimMontage*> EquipMontages;
	/** 무기 타입에 따른 탈착 애니메이션 몽타주 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Animation")
	TMap<EWeaponType, UAnimMontage*> UnEquipMontages;
	/** 현재 무기 교체 애니메이션 재생 중인지 여부 */
	bool bIsExchangeWeapon;
	/** 오른손 무기 장착 소켓 이름 */
	static const FName RWeaponSocketName;
	/** 왼손 무기 장착 소켓 이름 */
	static const FName LWeaponSocketName;
	/** 초기 소지 장비 클래스 */
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Animation")
	TMap<EWeaponType, UAnimMontage*> DeathMontages;

	/** 최소 발소리 재생 간격, 다리 움직임이 블렌드되면서 빠르게 여러번 재생 되는 현상을 방지 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	float FootStepInterval;
	/** 착지 소리가 들리기 위한 최소 속도, >0, 너무 낮은 높이에서 떨어져도 소리가 재싱되는 현상을 방지*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	float MinFallSpeedForLandSound;
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
	/** 구르기 사운드 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	class USoundBase* DodgeSound;
	/** 구르기 사운드 재생 확률 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	float DodgeSoundProbability;
	/** 장전할 때 소리, 단어가 아님 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	class USoundBase* ReloadEffectSound;
	/** 장전할 떄 소리, 단어 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	class USoundBase* ReloadWordSound;
	/** 장전했을 때 소리 재생 확률 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	float ReloadSoundProbability;
	/** 장전 소리 재생했을 때 단어를 말하기 위한 최소 장전 총알 개수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	int32 ReloadWordMinAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	class USoundBase* ShieldBrokenSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	class USoundBase* ShieldBrokenWordSound;
	/** 실드 파괴 사운드 재생 확률 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	float ShieldBrokenSoundProbability;
	/** 실드 파괴 시에 다시 실드 파괴 사운드를 재생할 때까지의 쿨타임*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	float ShieldBrokenSoundCoolDown;
	/** 실드 파괴 단어 재생 여부 */
	bool bCanPlayShieldBrokenSound;
	/** bCanPlayShieldBrokenSound 재생을 업데이트할 타이머 핸들 */
	FTimerHandle ShieldBrokenSoundTimerHandle;
	float PrevShield;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	class USoundBase* DamageSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	float DamageSoundProbability;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	float DamageSoundCoolDown;
	FTimerHandle DamageSoundTimerHandle;
	bool bCanPlayDamageSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Sound")
	USoundBase* WaterDragSound;
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

	UFUNCTION(BlueprintCallable)
	void SetInvincibility(bool bEnable);
	/** 현재 무적 여부 확인 */
	UFUNCTION(BlueprintCallable)
	bool IsInvincible() const;
	/** 무적 상태, 구르기 무적 상태를 기준으로 현재 무적 상태 갱신 */
	void UpdateInvincible();
	
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
	/** Amount 만큼 데미지를 적용, 실드가 있을 경우 실드를 먼저 감소, 대미지가 아니고 바로 호출되는 것에 유의 */
	UFUNCTION(BlueprintCallable)
	void ApplyDamage(const int Amount);
	void Die();
	void OnDeathMontageEnded();

	/** ShieldRechargeDelay 이후에 실드 재생을 시작하는 함수*/
	void StartRechargeShield();
	/** 실제 실드량을 회복시키는 함수 */
	void RechargeShield();

	/** 피격 시 재생할 몽타주 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Status")
	UAnimMontage* HitMontage;
	
	/** 무기 인벤토리를 초기 무기로 초기화한다.*/
	void InitWeaponInventory();
	/** I번째 무기 슬롯의 무기를 장착한다.*/
	void EquipWeaponByIndex(int I);
	/** I번째 무기 슬롯 교체 애니메이션 시작*/
	void StartExchangeAnim(int I);
	/** 무기 장착 해제 몽타주 종료*/
	void OnUnEquipMontageEnded(UAnimMontage* AnimMontage, bool bArg, int32 WeaponSlotIndex);
	/** 무기 장착 몽타주 종료*/
	void OnEquipMontageEnded(UAnimMontage* AnimMontage, bool bArg);
	/** UpperBody Slot에서 몽타주 진행 여부 설정 */
	void SetUpperBodyActive(bool bActive) { bIsUpperBodyActive = bActive; }
	/** FullBody Slot에서 몽타주 진행 여부 설정 */
	void SetFullBodyActive(bool bActive) { bIsFullBodyActive = bActive; }
	/** 무기를 장착한다. 애니메이션 출력과는 관계 없다. */
	void Equip(class AWeaponBase* Weapon);
	/** 현재 장착 무기에 따른 Aim Offset을 반환 */
	virtual FRotator GetBaseAimRotation() const override;
	/** 탄약 추가 */
	UFUNCTION(BlueprintCallable)
	void AddAmmo(EAmmoType AmmoType, int Amount);
	EPhysicalSurface GetSurfaceTypeOnFoot();
	USoundBase* GetLandFootSound(float Speed) const;
	/** 탄약의 총 개수를 반환 */
	UFUNCTION(BlueprintPure, Category = "Player|Weapon")
	int GetTotalAmmo(EAmmoType AmmoType) const { return AmmoInventory[AmmoType]; }

	USoundBase* GetFootStepSound(float Speed, EPhysicalSurface PhysicalSurface);
	/** 발소리 재생, 너무 빠르게 연속으로 재생하지는 않는다. AnimNotify로 BP로 실행 중 */
	UFUNCTION(BlueprintCallable)
	void PlayFootStepSound();
	
protected:
	virtual void BeginPlay() override;
	void OnDamageSoundCoolDown();
	void PlayDamageSound();
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
	/** 착지 소리 재생, 착지 속력에 따라 소리를 재생한다. */
	void PlayLandSound();
	/** 사격 가능 여부를 확인*/
	bool CanFire() const;

	/** 로컬 좌표계에서의 이동 방향을 이용해서 구르기 몽타주를 반환 */
	UAnimMontage* GetDodgeMontage(const FVector& LocalDodgeDirection) const;
	/** 구르기가 종료되었을 때 호출되는 함수, BlendOut이 사작될 때 호출된다. */
	UFUNCTION()
	void OnDodgeMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted);
	/** 구르기 동안 속도를 갱신하는 함수, Curve 데이터를 이용해서 현재 시간의 속도를 반영한다. */
	void OnDodgeUpdate();
	/** 구르기 무적 설정 */
	void SetDodgeInvincible(bool bEnable);
	
	/** 소지하고 있지 않은 탄환을 0으로 초기화*/
	void InitAmmoInventory();
	/** 리로드 UI 업데이트 콜백 함수 */
	UFUNCTION()
	void OnReloadUIUpdate();
	/** 리로드 애니메이션 종료 콜백 함수 */
	UFUNCTION()
	void OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void OnShieldBrokenSoundCoolDown();
	void PlayShieldBrokenSound();
	
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
	UFUNCTION()
	void EquipWeaponSlot1(const FInputActionValue& InputActionValue);
	UFUNCTION()
	void EquipWeaponSlot2(const FInputActionValue& InputActionValue);
	UFUNCTION()
	void EquipWeaponSlot3(const FInputActionValue& InputActionValue);
};