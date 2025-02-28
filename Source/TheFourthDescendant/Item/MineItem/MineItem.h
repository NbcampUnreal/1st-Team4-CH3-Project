#pragma once

#include "CoreMinimal.h"
#include "TheFourthDescendant/Item/BaseItem.h"
#include "MineItem.generated.h"


UCLASS()
class THEFOURTHDESCENDANT_API AMineItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AMineItem();

public:
	/** 폭발 딜레이 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionDelay;
	/** 폭발 데미지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionDamage;
	/** 폭발 반경 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionRadius;
	/** 폭발 시 출력될 파티클 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* ExplosionParticle;
	/** 폭발 시 출력될 사운드 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* ExplosionSound;

private:
	/** 폭발 딜레이를 체크할 타이머 */
	FTimerHandle ExplosionTimerHandle;
	/** 폭발 여부 */
	bool bHasExploded;

public:
	virtual void BeginPlay() override;
	/** 폭발 로직 */
	void Explode();
};
