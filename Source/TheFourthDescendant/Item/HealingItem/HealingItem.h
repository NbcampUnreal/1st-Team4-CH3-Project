// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheFourthDescendant/Item/BaseItem.h"
#include "HealingItem.generated.h"

UCLASS()
class THEFOURTHDESCENDANT_API AHealingItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AHealingItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float HealAmount;

	virtual void ActivateItem(AActor* Activator) override;
};