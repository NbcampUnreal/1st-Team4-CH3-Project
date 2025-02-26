// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BulletItem.h"
#include "NormalBulletItem.generated.h"

UCLASS()
class THEFOURTHDESCENDANT_API ANormalBulletItem : public ABulletItem
{
	GENERATED_BODY()

public:
	ANormalBulletItem();

	virtual void ActivateItem(AActor* Activator) override;

};
