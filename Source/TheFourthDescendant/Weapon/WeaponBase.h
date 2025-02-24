// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS(Abstract)
class THEFOURTHDESCENDANT_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeaponBase();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int, CurrentAmmo, int, TotalAmmo);
	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnAmmoChanged OnAmmoChanged;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShootTriggered);
	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnShootTriggered OnShootTriggered;
	
protected:
	/** ApplyRecoil을 호출해서 반동 감쇄*/
	virtual void Tick(float DeltaSeconds) override;

	/** 발사 소켓 이름을 반환 */
	FString GetFireSocketName() const { return FireSocketName; }
	
	/** 무기의 공격력 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WeaponAttackPower;
	/** 무기의 발사 간격 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FireInterval;
	/** 무기의 발사 타이머 핸들 */
	FTimerHandle FireTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	/** 사격 시에 반동으로 올라가는 값. CurrentRecoilOffset에 더해지고 이 값을 바탕으로 총구를 위로 올린다. */
	float RecoilAmount;
	/** 반동 후에 원래대로 돌아가는 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RecoilRecoverySpeed;
	/** 최대 반동 값. 너무 빠른 사격으로 인한 반동이 너무 커지는 것을 방지하기 위함 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxRecoilAmount;
	/** 반동으로 현재 프레임 상승 각도, 매 프레임 RecoverySpeed로 인해 감소된다. */
	float CurrentRecoilOffset;

	/** 탄창에 들어가는 최대 탄약 수 */
	int MaxAmmoInMagazine;
	/** 현재 탄약 수 */
	int CurrentAmmo;
	/** 탄약의 총 수 */
	int TotalAmmo;

	/** 발사 사운드 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* FireSfx;
	/** 사격 중에 발생하는 카메라 흔들림*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCameraShakeBase> FireCameraShake;
	
private:
	/** 사격이 발생하는 소켓 이름*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FString FireSocketName;
	
public:
	/** 무기를 발사 */
	void StartShoot();
	/** 무기 발사 중지 */
	void StopShoot();
	/** 탄약 재장전 */
	void Reload();

protected:
	/** 무기의 공격 동작, 공격 실행, 사운드 재생, 효과음 재생 등 */
	UFUNCTION()
	void Attack();

	/** 공격 실행, 실질적인 공격을 발생 */
	virtual void PerformAttack();

	/** 반동 수치를 감쇄한다. */
	void ApplyRecoil(float DeltaTime);
	
	/** 무기 메쉬를 반환 */
	UFUNCTION(BlueprintCallable)
	UMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	
private:
	/** 무기 메쉬 */
	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* WeaponMesh;
};
