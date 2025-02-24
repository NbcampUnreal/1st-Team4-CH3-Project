// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

UCLASS()
class THEFOURTHDESCENDANT_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMainGameInstance();

private:
	/** 처치한 총 몬스터 수 */
	UPROPERTY(VisibleAnywhere, Category = "Result")
	int32 KilledEnemyCount;
	/** 플레이 타임 */
	UPROPERTY(VisibleAnywhere, Category = "Result")
	int32 PlayTime;
	/** 플레이어가 발사한 총알 수 */
	UPROPERTY(VisibleAnywhere, Category = "Result")
	int32 ShootProjectileCount;
	/** 몬스터에게 받은 피해량 */
	UPROPERTY(VisibleAnywhere, Category = "Result")
	int32 ReceivedDamageByEnemy;
	/** 몬스터에게 입힌 피해량 */
	UPROPERTY(VisibleAnywhere, Category = "Result")
	int32 DealtDamageToEnemy;
	/** 플레이어가 적에게 적중한 총알 수 */
	UPROPERTY(VisibleAnywhere, Category = "Result")
	int32 HitProjectileCount;
	/** 사망한 횟수 */
	UPROPERTY(VisibleAnywhere, Category = "Result")
	int32 DeathCount;
	/** 플레이어가 적의 공격을 회피한 횟수 */
	UPROPERTY(VisibleAnywhere, Category = "Result")
	int32 EvasionAttackCount;
	/** 플레이 타임 체크용 타이머 핸들 */
	FTimerHandle PlayTimeTimer;

public:
	virtual void Init();
	
	/** 처치한 몬스터 수 증가 */
	void AddKilledEnemyCount(const int32 Amount);
	/** 발사한 총알 수 증가 */
	void AddShootProjectileCount(const int32 Amount);
	/** 받은 피해량 증가 */
	void AddReceivedDamageByEnemy(const int32 Amount);
	/** 입힌 피해량 증가 */
	void AddDealtDamageToEnemy(const int32 Amount);
	/** 적중한 총알 수 증가 */
	void AddHitProjectileCount(const int32 Amount);
	/** 사망한 횟수 증가 */
	void AddDeathCount(const int32 Amount);
	/** 회피한 횟수 증가 */
	void AddEvasionAttackCount(const int32 Amount);
	/** 회피한 횟수 감소 */
	void SubtractEvasionAttackCount(const int32 Amount);

private:
	/** 1초마다 플레이 타임 1초 증가 */
	void AddPlayTimeOneSecond();
	
};
