// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheFourthDescendant/Monster/Monster.h"
#include "RangedMonster.generated.h"

class AEnemyProjectile;

UCLASS()
class THEFOURTHDESCENDANT_API ARangedMonster : public AMonster
{
	GENERATED_BODY()

public:
	ARangedMonster();

protected:
	/** 원거리 공격 반복 횟수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
	int32 RangedAttackCount;
	/** 현재 남은 공격 횟수 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Status")
	int32 CurrentRangedAttackCount;
	/** 장전 상태 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion")
	bool bIsReloading;
	/** 발사할 Projectile */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	TSubclassOf<AEnemyProjectile> ProjectileClass;

private:
	/** 소켓을 추출할 몬스터 스켈레탈 메시 */
	USkeletalMeshComponent* SkeletalMesh;
	/** LineTracing 시 무시할 객체 */
	FCollisionQueryParams Params;

public:
	/** 사망 로직 함수 */
	virtual void OnDeath() override;
	/** 공격 함수 */
	virtual void Attack() override;
	/** 이동 함수 */
	virtual void Move() override;

	/** 장전 완료 로직 */
	UFUNCTION(BlueprintCallable)
	void ReloadCompleted();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** 체력바 UI 제거하는 함수 */
	UFUNCTION(BlueprintImplementableEvent)
	void DestroyWidget();
};
