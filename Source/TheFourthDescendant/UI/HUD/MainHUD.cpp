// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"
#include "CrossHairWidget.h"
#include "PlayerStatusWidget.h"

void UMainHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UMainHUD::Shoot()
{
	if(!IsValid(CrossHairWidget))
	{
		return;
	}

	CrossHairWidget->Shoot();	
}

void UMainHUD::IncreaseBulletInMag()
{
	if(!IsValid(PlayerStatusWidget))
	{
		return;
	}

	PlayerStatusWidget->IncreaseBulletInMag();
}

void UMainHUD::DecreaseBulletInMag()
{
	if(!IsValid(PlayerStatusWidget))
	{
		return;
	}

	PlayerStatusWidget->DecreaseBulletInMag();
}

void UMainHUD::SettingBulletInMag(int32 NewBulletCountInMag)
{
	if(!IsValid(PlayerStatusWidget))
	{
		return;
	}

	PlayerStatusWidget->SettingBulletInMag(NewBulletCountInMag);
}

void UMainHUD::SettingTotalMagsBullet(int32 NewTotalMagsBulletCount)
{
	if(!IsValid(PlayerStatusWidget))
	{
		return;
	}

	PlayerStatusWidget->SettingTotalMagsBullet(NewTotalMagsBulletCount);
}

void UMainHUD::Reload(int32 DesireMagBulletCount)
{
	if(!IsValid(PlayerStatusWidget))
	{
		return;
	}

	PlayerStatusWidget->Reload(DesireMagBulletCount);
}

void UMainHUD::SetHPPercentTwoParams(float CurrentHP, float MaxHP)
{	
	if(!IsValid(PlayerStatusWidget))
	{
		return;
	}

	PlayerStatusWidget->SetHPPercentTwoParams(CurrentHP, MaxHP);
}

void UMainHUD::SetHPPercentOneParams(float Percent)
{
	if(!IsValid(PlayerStatusWidget))
	{
		return;
	}

	PlayerStatusWidget->SetHPPercentOneParams(Percent);
}

float UMainHUD::GetHPPercent() const
{
	if(!IsValid(PlayerStatusWidget))
	{
		return 0.f;
	}

	return PlayerStatusWidget->GetHPPercent();
}

void UMainHUD::SetSPPercentTwoParams(float CurrentSP, float MaxSP)
{
	if(!IsValid(PlayerStatusWidget))
	{
		return;
	}

	PlayerStatusWidget->SetSPPercentTwoParams(CurrentSP, MaxSP);
}

void UMainHUD::SetSPPercentOneParams(float Percent)
{
	if(!IsValid(PlayerStatusWidget))
	{
		return;
	}

	PlayerStatusWidget->SetSPPercentOneParams(Percent);
}

float UMainHUD::GetSPPercent() const
{
	if(!IsValid(PlayerStatusWidget))
	{
		return 0.f;
	}

	return PlayerStatusWidget->GetSPPercent();
}