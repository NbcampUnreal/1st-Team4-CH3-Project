// Fill out your copyright notice in the Description page of Project Settings.

#include "HealingItem.h"
#include "TheFourthDescendant/Player/PlayerCharacter.h"

AHealingItem::AHealingItem()
{
	HealAmount = 20;
	ItemType = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	
	if (Activator && Activator->ActorHasTag("Player"))
	{		
		if (APlayerCharacter* Player = Cast<APlayerCharacter>(Activator))
		{
			Player->IncreaseHealth(HealAmount);
		}
		DestroyItem();
	}
}

