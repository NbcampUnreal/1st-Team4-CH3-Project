// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "RangeWeapon.generated.h"

UCLASS()
class THEFOURTHDESCENDANT_API ARangeWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARangeWeapon();

protected:
	virtual void PerformAttack() override;
};
