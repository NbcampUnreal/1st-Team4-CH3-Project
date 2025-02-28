#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissileProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class THEFOURTHDESCENDANT_API AMissileProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AMissileProjectile();

public:
	/** 스피어 콜리전 컴포넌트 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Projectile|Component")
	USphereComponent* CollisionComp;
	/** 미사일 스태틱 메시 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Projectile|Component")
	USkeletalMeshComponent* MeshComp;
	/** Projectile movement 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Component")
	UProjectileMovementComponent* ProjectileMovement;
	/** 하늘로 미사일 날아가는 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Stat")
	float FireSpeedIntoTheSky;

public:
	/** 하늘로 미사일 발사하는 로직 */
	void FireMissileIntoTheSky(FVector& Direction);

};
