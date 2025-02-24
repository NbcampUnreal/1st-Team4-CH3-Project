// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameInstance.h"

UMainGameInstance::UMainGameInstance()
{
	KilledEnemyCount = 0;
	PlayTime = 0;
	ShootProjectileCount = 0;
	ReceivedDamageByEnemy = 0;
	DealtDamageToEnemy = 0;
	HitProjectileCount = 0;
	DeathCount = 0;
	EvasionAttackCount = 0;
}

void UMainGameInstance::Init()
{
	Super::Init();

	// 플레이 타임 타이머 가동
	GetTimerManager().SetTimer(PlayTimeTimer, this, &UMainGameInstance::AddPlayTimeOneSecond, 1.0f, true);
}

// 구현
void UMainGameInstance::AddKilledEnemyCount(const int32 Amount)
{
	KilledEnemyCount += Amount;
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, FString::Printf(TEXT("KilledEnemyCount : %d"),KilledEnemyCount));
}

void UMainGameInstance::AddShootProjectileCount(const int32 Amount)
{
	ShootProjectileCount += Amount;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, FString::Printf(TEXT("ShootProjectileCount : %d"),ShootProjectileCount));
}

// 구현
void UMainGameInstance::AddReceivedDamageByEnemy(const int32 Amount)
{
	ReceivedDamageByEnemy += Amount;
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, FString::Printf(TEXT("ReceivedDamageByEnemy : %d"),ReceivedDamageByEnemy));
}

void UMainGameInstance::AddDealtDamageToEnemy(const int32 Amount)
{
	DealtDamageToEnemy += Amount;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, FString::Printf(TEXT("DealtDamageToEnemy : %d"),DealtDamageToEnemy));
}

void UMainGameInstance::AddHitProjectileCount(const int32 Amount)
{
	HitProjectileCount += Amount;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, FString::Printf(TEXT("HitProjectileCount : %d"),HitProjectileCount));
}

void UMainGameInstance::AddDeathCount(const int32 Amount)
{
	DeathCount += Amount;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, FString::Printf(TEXT("DeathCount : %d"),DeathCount));
}

// 구현
void UMainGameInstance::AddEvasionAttackCount(const int32 Amount)
{
	EvasionAttackCount += Amount;
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, FString::Printf(TEXT("EvasionAttackCount : %d"),EvasionAttackCount));
}

// 구현
void UMainGameInstance::SubtractEvasionAttackCount(const int32 Amount)
{
	EvasionAttackCount -= Amount;
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, FString::Printf(TEXT("EvasionAttackCount : %d"),EvasionAttackCount));
}

void UMainGameInstance::AddPlayTimeOneSecond()
{
	PlayTime += 1;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, FString::Printf(TEXT("PlayTime : %d"), PlayTime));
}


