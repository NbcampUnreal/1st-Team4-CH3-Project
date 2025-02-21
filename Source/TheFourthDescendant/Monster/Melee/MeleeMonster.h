// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheFourthDescendant/Monster/Monster.h"
#include "MeleeMonster.generated.h"

UCLASS()
class THEFOURTHDESCENDANT_API AMeleeMonster : public AMonster
{
	GENERATED_BODY()

protected:
	/** 공격 반경 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat")
	float AttackRange;

private:
	/** 공격 로직은 단 한 번 실행하기 위함 */
	bool bIsAttacked;

public:
	/** 공격 함수 */
	UFUNCTION(BlueprintCallable)
	virtual void Attack() override;
	/** 이동 함수 */
	virtual void Move() override;

	/** 공격 완료 로직 */
	UFUNCTION(BlueprintCallable)
	void AttackCompleted();
};
