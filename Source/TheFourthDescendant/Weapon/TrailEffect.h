// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TrailEffect.generated.h"

/**
 * 
 */
UCLASS()
class THEFOURTHDESCENDANT_API ATrailEffect : public AActor
{
	GENERATED_BODY()

public:
	ATrailEffect();
	
	void StopAndDestroy(float DelayTime);
protected:
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* TrailVfx;
};
