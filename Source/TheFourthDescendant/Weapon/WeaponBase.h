// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class UNiagaraSystem;

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	Normal,
	Speed,
	Power,
	Count UMETA(Hidden),
};
ENUM_RANGE_BY_COUNT(EAmmoType, EAmmoType::Count);

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	AR,
	Lnc,
	Count UMETA(Hidden),
};
ENUM_RANGE_BY_COUNT(EWeaponType, EWeaponType::Count);

USTRUCT(BlueprintType)
struct FShootResult
{
	GENERATED_BODY()
public:
	FShootResult();
	/* Hit Marker를 켜는지 여부 */
	UPROPERTY(BlueprintReadOnly)
	bool bShouldHitMarkerOn = false;
};

UCLASS(Abstract, BlueprintType)
class THEFOURTHDESCENDANT_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	// @To-Do Muzzle Flash 같은 효과를 총기 애니메이션 쪽에서 처리할 수 있도록 수정
	
	// Sets default values for this actor's properties
	AWeaponBase();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoChanged, int, CurrentAmmo);
	/** 총알 개수가 변경 됬을 때 발생할 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnAmmoChanged OnAmmoChanged;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShootTriggered, FShootResult, ShootResult);
	/** 사격 발생 시에 발생할 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnShootTriggered OnShootTriggered;

protected:
	/** ApplyRecoil을 호출해서 반동 감쇄*/
	virtual void Tick(float DeltaSeconds) override;

	/** 발사 소켓 이름을 반환 */
	FString GetFireSocketName() const { return FireSocketName; }
	/** 머즐 플래시가 발생할 소켓 이름을 반환*/
	FString GetMuzzleSocketName() const { return MuzzleFlashSocketName; }

	/** 사격 가능 여부 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Weapon|Fire")
	bool bCanFire;
	
	/** 무기가 사용하는 탄약 타입 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Fire")
	EAmmoType AmmoType;
	/** 무기의 공격력 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Fire")
	int32 WeaponAttackPower;
	/** 무기의 발사 간격 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Fire")
	float FireInterval;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Fire")
	float RattleInterval;
	/** 무기의 발사 타이머 핸들 */
	FTimerHandle FireTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Fire")
	/** 사격 시에 반동으로 올라가는 값. CurrentRecoilOffset에 더해지고 이 값을 바탕으로 총구를 위로 올린다. */
	float RecoilAmount;
	/** 반동 후에 원래대로 돌아가는 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Fire")
	float RecoilRecoverySpeed;
	/** 최대 반동 값. 너무 빠른 사격으로 인한 반동이 너무 커지는 것을 방지하기 위함 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Fire")
	float MaxRecoilAmount;
	/** 반동으로 현재 프레임 상승 각도, 매 프레임 RecoverySpeed로 인해 감소된다. */
	float CurrentRecoilOffset;

	/** 탄창에 들어가는 최대 탄약 수 */
	int MaxAmmoInMagazine;
	/** 현재 탄약 수 */
	int CurrentAmmo;

	/** 발사 사운드 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Sound")
	USoundBase* FireSfx;
	/** 총알이 없을 때 사운드 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Sound")
	USoundBase* RattleSfx;
	
	/** 사격 중에 발생하는 카메라 흔들림*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	TSubclassOf<UCameraShakeBase> FireCameraShake;
	/** 리로드 애니메이션 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Animation")
	UAnimMontage* ReloadMontage;
	/** 머즐 플래시 Vfx */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	UNiagaraSystem* MuzzleFlashVfx;
private:
	/** 무기의 타입*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category="Weapon")
	EWeaponType WeaponType;
	/** 사격이 발생하는 소켓 이름*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category="Weapon|Fire")
	FString FireSocketName;
	/** 머즐 플래시가 생성되는 소켓 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category="Weapon|Fire")
	FString MuzzleFlashSocketName;
	
public:
	/** 무기의 타입을 반환 */
	EWeaponType GetWeaponType() const { return WeaponType; }
	/** 무기가 사용하는 타입을 반환 */
	EAmmoType GetAmmoType() const { return AmmoType; }
	/** 현재 탄약 수를 반환 */
	UFUNCTION(BlueprintPure)
	int GetCurrentAmmo() const { return CurrentAmmo; }

	/** 무기를 발사 */
	void StartShoot();
	/** 총알이 없을 경우 사운드를 재생 */
	void PlayRattleSound();
	/** 탄약 재장전, 총 탄환 수는 호출 후에 갱신된다. */
	void Reload(int& TotalAmmo);
	/** 재장전 애니메이션 몽타주 반환 */
	UFUNCTION(BlueprintCallable)
	UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	/** 총알이 가득 찼는지 반환 */
	UFUNCTION(BlueprintCallable)
	bool IsMagazineFull() const { return CurrentAmmo == MaxAmmoInMagazine; }

	virtual FRotator GetAimRotation(const APawn* TargetPawn) const;

protected:
	/** 무기의 공격 동작, 공격 실행, 사운드 재생, 효과음 재생 등 */
	UFUNCTION()
	void Attack();
	/** 쿨타임 종료 타이머*/
	UFUNCTION()
	void OnCooldownFinished();

	/** 공격 실행, 실질적인 공격을 발생 */
	virtual FShootResult PerformAttack();

	/** 반동 수치를 감쇄한다. */
	void ApplyRecoil(float DeltaTime);
	/** 카메라로부터 화면 중앙까지 TraceDist 만큼 레이 트레이스 했을 떄 위치를 추적
	 * 만약에 충돌이 없으면 TraceDist만큼 떨어진 지점을 반환
	 */
	FVector CalculateTargetPoint(const AController* PlayerController, float TraceDist) const;
	
	/** 무기 메쉬를 반환 */
	UFUNCTION(BlueprintCallable)
	UMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	
private:
	/** 무기 메쉬 */
	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* WeaponMesh;
};
