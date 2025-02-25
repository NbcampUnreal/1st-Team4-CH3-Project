// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheFourthDescendant/Item/BaseItem.h"
#include "AmmoItem.generated.h"

UCLASS()
class THEFOURTHDESCENDANT_API AAmmoItem : public ABaseItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAmmoItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
