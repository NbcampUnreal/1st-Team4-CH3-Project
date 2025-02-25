// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheFourthDescendant/Item/BaseItem.h"
#include "BulletItem.generated.h"

UCLASS()
class THEFOURTHDESCENDANT_API ABulletItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ABulletItem();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 AmmoAmount;

	virtual void ActivateItem(AActor* Activator) override;
};
