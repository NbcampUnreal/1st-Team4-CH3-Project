// Fill out your copyright notice in the Description page of Project Settings.


#include "TrailEffect.h"

#include "NiagaraComponent.h"

ATrailEffect::ATrailEffect()
{
	USceneComponent* DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	TrailVfx = CreateDefaultSubobject<UNiagaraComponent>("TrailVfx");
	TrailVfx->SetupAttachment(DefaultSceneRoot);
}

void ATrailEffect::StopAndDestroy(float DelayTime)
{
	TrailVfx->Deactivate();
	SetLifeSpan(DelayTime);
}
