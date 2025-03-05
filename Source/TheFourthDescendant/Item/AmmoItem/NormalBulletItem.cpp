// Fill out your copyright notice in the Description page of Project Settings.


#include "NormalBulletItem.h"
#include "TheFourthDescendant/Player/PlayerCharacter.h"
#include "TheFourthDescendant/Weapon/WeaponBase.h"


// Sets default values
ANormalBulletItem::ANormalBulletItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	ItemType = "NormalAmmo";
	AmmoAmount = 30;
}

void ANormalBulletItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	
	if (Activator && Activator->ActorHasTag("Player"))
	{		
		if (APlayerCharacter* Player = Cast<APlayerCharacter>(Activator))
		{
			Player->AddAmmo(EAmmoType::Normal, AmmoAmount);
		}
		DestroyItem();
	}
}


