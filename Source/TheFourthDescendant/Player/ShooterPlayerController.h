﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class THEFOURTHDESCENDANT_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AShooterPlayerController();

	/** Player Input Mapping Context를 반환 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;
	/** IA_Move를 반환 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;
	/** IA_Look를 반환 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;
	/** IA_Jump를 반환 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction;
	/** IA_Sprint를 반환 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* SprintAction;
	/** IA_Dodge를 반환 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* DodgeAction;
	/** IA_Crouch를 반환 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* CrouchAction;
	/** IA_Interact를 반환 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* InteractionAction;
	/** IA_Fire를 반환 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* FireAction;
	/** IA_Aim을 반환 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* AimAction;
	/** IA_Reload를 반환 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* ReloadAction;
	/** IA_Weapon1를 반환 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* EquipWeapon1Action;
	/** IA_Weapon2를 반환 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* EquipWeapon2Action;
	/** IA_Weapon3를 반환 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* EquipWeapon3Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ActivateSkillZAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ActivateSkillXAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ActivateSkillCAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget *HUDWidget;
protected:
	/** InputLocalPlayerSubsystem에 MappingContext를 추가 */
	virtual void BeginPlay() override;
};
