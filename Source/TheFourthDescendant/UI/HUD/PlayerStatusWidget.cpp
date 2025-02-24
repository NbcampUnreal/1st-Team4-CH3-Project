// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatusWidget.h"

void UPlayerStatusWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	BulletInMag = 0;
	TotalMagsBullet = 0;
	HPPercent = 1.f;
	SPPercent = 1.f;
}

void UPlayerStatusWidget::IncreaseBulletInMag()
{
	++BulletInMag;
}

void UPlayerStatusWidget::DecreaseBulletInMag()
{
	--BulletInMag;
}

void UPlayerStatusWidget::SettingBulletInMag(int32 NewBulletCountInMag)
{
	if(0 > NewBulletCountInMag)
	{
		NewBulletCountInMag = 0;
	}
	BulletInMag = NewBulletCountInMag;
}

void UPlayerStatusWidget::SettingTotalMagsBullet(int32 NewTotalMagsBulletCount)
{
	if(0 > NewTotalMagsBulletCount)
	{
		NewTotalMagsBulletCount = 0;
	}
	TotalMagsBullet = NewTotalMagsBulletCount;
}

void UPlayerStatusWidget::Reload(int32 DesireMagBulletCount)
{	
	if(0 > DesireMagBulletCount)
	{
		DesireMagBulletCount = 0;
	}
	
	// 예외처리, 남은 총알보다 재장전을 원하는 총알이 클 수 있으므로
	if(DesireMagBulletCount > TotalMagsBullet)
	{
		DesireMagBulletCount = TotalMagsBullet;		
	}

	TotalMagsBullet -= DesireMagBulletCount;
	BulletInMag += DesireMagBulletCount;
}

void UPlayerStatusWidget::SetHPPercentTwoParams(float CurrentHP, float MaxHP)
{
	if(CurrentHP <= 0.f)
	{
		CurrentHP = 0.f;
	}
	if(MaxHP <= 0.f)
	{
		CurrentHP = 0.f;
		MaxHP = 1.f;
	}
	
	HPPercent = CurrentHP / MaxHP;
	
	if(HPPercent > 1.f)
	{
		HPPercent = 1.f;
	}
}

void UPlayerStatusWidget::SetHPPercentOneParams(float Percent)
{
	if(Percent < 0.f)
	{
		Percent = 0.f;
	}
	if(Percent > 1.f)
	{
		Percent = 1.f;
	}
	
	HPPercent = Percent;
}

float UPlayerStatusWidget::GetHPPercent() const
{
	return HPPercent;
}

void UPlayerStatusWidget::SetSPPercentTwoParams(float CurrentSP, float MaxSP)
{
	if(CurrentSP <= 0.f)
	{
		CurrentSP = 0.f;
	}
	if(MaxSP <= 0.f)
	{
		CurrentSP = 0.f;
		MaxSP = 1.f;
	}
	
	SPPercent = CurrentSP / MaxSP;
}

void UPlayerStatusWidget::SetSPPercentOneParams(float Percent)
{
	if(Percent < 0.f)
	{
		Percent = 0.f;
	}
	if(Percent > 1.f)
	{
		Percent = 1.f;
	}
	
	SPPercent = Percent;
}

float UPlayerStatusWidget::GetSPPercent() const
{
	return SPPercent;
}