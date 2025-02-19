// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheFourthDescendant/Monster/Monster.h"
#include "RangedMonster.generated.h"

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
	/** OnMoveCompleted 함수가 불린 후에 true로 전환 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion")
	bool bIsMoved;
	/** 전진 상태 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion")
	bool bIsMovingFront;
	/** 왼쪽으로 움직임 상태 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion")
	bool bIsMovingLeft;
	/** 오른쪽으로 움직임 상태 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion")
	bool bIsMovingRight;
	/** 후진 상태 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion")
	bool bIsMovingBack;
	
};
