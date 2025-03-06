// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTakeDamageDispatcher, float, HPPercent, float, SPPercent, float, ActualDamage);

USTRUCT(BlueprintType)
struct FStatus
{
	GENERATED_BODY()

	FStatus();
	
	/** 캐릭터의 현재 체력 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Health;
	/** 캐릭터의 최대 체력 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHealth;
	/** 캐릭터의 현재 실드량 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Shield;
	/** 캐릭터의 최대 실드량 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxShield;

	/** 캐릭터의 방어력 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Defense;
	/** 캐릭터의 이동 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WalkSpeed;
};

UCLASS(Abstract)
class THEFOURTHDESCENDANT_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();

public:
	UPROPERTY(BlueprintAssignable, Category = "Character|Event")
	FTakeDamageDispatcher DamageDispatcher;
	
	/** 캐릭터의 현재 체력을 반환*/
	UFUNCTION(BlueprintPure, Category = "Character|Status")
	int32 GetHealth() const;
	/** 캐릭터의 최대 체력을 반환*/
	UFUNCTION(BlueprintPure, Category = "Character|Status")
	int32 GetMaxHealth() const;
	/** 캐릭터의 현재 실드량을 반환 */
	UFUNCTION(BlueprintPure, Category = "Character|Status")
	int32 GetShield() const;
	/** 캐릭터의 최대 실드량을 반환 */
	UFUNCTION(BlueprintPure, Category = "Character|Status")
	int32 GetMaxShield() const;
	
protected:
	/** 캐릭터의 상태 정보 */
	UPROPERTY(EditAnywhere, Category = "Character")
	FStatus Status;

	/** 데미지를 받았을 때 호출되는 함수 */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};
