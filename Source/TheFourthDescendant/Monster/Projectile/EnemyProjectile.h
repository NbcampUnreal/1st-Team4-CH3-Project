// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class THEFOURTHDESCENDANT_API AEnemyProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemyProjectile();

protected:
	/** 스피어 콜리전 컴포넌트 */
	UPROPERTY(VisibleDefaultsOnly, Category="Projectile")
	USphereComponent* CollisionComp;
	/** 총알 모양 스태틱 메시 */
	UPROPERTY(VisibleDefaultsOnly, Category="Projectile")
	UStaticMeshComponent* MeshComp;
	/** 총알 데미지 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	int32 ProjectileDamage;
	/** Projectile movement 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovement;

public:
	/** 발사 방향으로의 발사체 속도를 초기화하는 함수 */
	void FireInDirection(const FVector& ShootDirection);
	
protected:
	/** 총알이 충돌을 감지할 경우 호출 */
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	               int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
