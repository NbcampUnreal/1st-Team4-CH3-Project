// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheFourthDescendant/Abstracts/CharacterBase.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;

UCLASS()
class THEFOURTHDESCENDANT_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

private:
	/** TPS 카메라 컴포넌트 */
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* CameraComponent;
	/** 카메라가 부착될 스프링 암 컴포넌트 */
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArmComponent;

public:
	/** Mapping Context에 따라 입력을 바인딩*/
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	/** IA_Move 바인딩 함수 */
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	/** IA_Jump(Started) 바인딩 함수 */
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	/** IA_Look(Triggered) 바인딩 함수 */
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	/** IA_Sprint(Started) 바인딩 함수 */
	UFUNCTION()
	void Sprint(const FInputActionValue& Value);
	/** IA_Dodge(Started) 바인딩 함수 */
	UFUNCTION()
	void Dodge(const FInputActionValue& Value);
	/** IA_Crouch(Started) 바인딩 함수 */
	UFUNCTION()
	void StartCrouch(const FInputActionValue& Value);
	/** IA_Interaction(Started) 바인딩 함수 */
	UFUNCTION()
	void Interaction(const FInputActionValue& Value);
	/** IA_Fire(Started) 바인딩 함수 */
	UFUNCTION()
	void StartShoot(const FInputActionValue& Value);
	/** IA_Fire(Triggered) 바인딩 함수 */
	UFUNCTION()
	void TriggerShoot(const FInputActionValue& Value);
	/** IA_Fire(Completed) 바인딩 함수 */
	UFUNCTION()
	void StopShoot(const FInputActionValue& Value);
	/** IA_Aim(Started) 바인딩 함수 */
	UFUNCTION()
	void StartAim(const FInputActionValue& Value);
	/** IA_Aim(Completed) 바인딩 함수 */
	UFUNCTION()
	void StopAim(const FInputActionValue& Value);
	/** IA_Reload(Started) 바인딩 함수 */
	UFUNCTION()
	void Reload(const FInputActionValue& Value);
};
