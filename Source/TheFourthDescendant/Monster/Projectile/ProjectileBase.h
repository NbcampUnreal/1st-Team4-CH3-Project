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
	/** Overlap시 출력할 파티클 이펙트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* ExplosionParticle;
	/** Overlap시 출력할 사운드 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* ExplosionSound;
	/** 충돌 체크용 콜리전 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Projectile|Component")
	USphereComponent* CollisionComp;
	/** 해당 영역 내에 있는 액터에게 데미지 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Projectile|Component")
	USphereComponent* AttackRangeCollisionComp;
	/** 미사일 스태틱 메시 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Projectile|Component")
	UStaticMeshComponent* MeshComp;
	/** 미사일 스켈레탈 메시 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Projectile|Component")
	USkeletalMeshComponent* SkeletalMesh;
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
