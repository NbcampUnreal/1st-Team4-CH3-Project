#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "HomingProjectile.generated.h"

UCLASS()
class THEFOURTHDESCENDANT_API AHomingProjectile : public AProjectileBase
{
	GENERATED_BODY()

public:
	AHomingProjectile();

private:
	/** Target 설정 지연 시간 측정용 */
	FTimerHandle SetHomingTimer;

public:
	/** 발사 함수 */
	virtual void Fire(FVector& Direction) override;
	/** 추격 대상 설정 */
	void SetHomingTarget();
	/** 총알이 충돌을 감지할 경우 호출 */
	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
				   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
};
