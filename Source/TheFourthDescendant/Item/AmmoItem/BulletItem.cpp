// Fill out your copyright notice in the Description page of Project Settings.

#include "BulletItem.h"
#include "TheFourthDescendant/Player/PlayerCharacter.h"
#include "TheFourthDescendant/Weapon/WeaponBase.h"

// Sets default values
ABulletItem::ABulletItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ABulletItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
}


