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
	FString GetFireSocketName() const { return FireSocketName; }
	
	/** 무기의 공격력 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WeaponAttackPower;
	/** 무기의 발사 간격 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FireInterval;
	/** 무기의 발사 타이머 핸들 */
	FTimerHandle FireTimerHandle;

	/** 탄창에 들어가는 최대 탄약 수 */
	int MaxAmmoInMagazine;
	/** 현재 탄약 수 */
	int CurrentAmmo;
	/** 탄약의 총 수 */
	int TotalAmmo;

	/** 발사 사운드 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* FireSfx;

private:
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

	UFUNCTION(BlueprintCallable)
	UMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	
private:
	/** 무기 메쉬 */
	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* WeaponMesh;
};
