#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class THEFOURTHDESCENDANT_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectileBase();

public:
	/** 스피어 콜리전 컴포넌트 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Projectile|Component")
	USphereComponent* CollisionComp;
	/** 미사일 스태틱 메시 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Projectile|Component")
	UStaticMeshComponent* MeshComp;
	/** Projectile movement 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Component")
	UProjectileMovementComponent* ProjectileMovement;
	/** 발사 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Stat")
	float FireSpeed;
	/** 데미지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Stat")
	float ProjectileDamage;

public:
	/** 총알을 발사하는 로직 */
	virtual void Fire(FVector& Direction);
	/** 총알이 충돌을 감지할 경우 호출 */
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
				   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
