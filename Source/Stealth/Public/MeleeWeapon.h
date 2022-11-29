// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "GameFramework/Character.h"
#include "MeleeWeapon.generated.h"


/**
 * 
 */
UCLASS()
class STEALTH_API AMeleeWeapon : public AWeaponBase
{
	GENERATED_BODY()
public:
	AMeleeWeapon();

	virtual void StartAttack() override;

	void TakeDown(class ASPlayerCharacter* Player, ACharacter* FocusedACharacter);

	void OnTakeDownFinished();

	bool CheckAttackRange(float Distance);
	
	bool CheckAttackRange(ACharacter* Player, ACharacter* FocusedACharacter, float Distance);
	
	virtual void Tick(float DeltaSeconds) override;
protected:
		
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* KnifeTakeDownMontage;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* KnifeTakeDownEnemyMontage;

	FTimerHandle KnifeTakeDownEnd_TimerHandle;

	UPROPERTY()
	ACharacter* FocusedCharacter;

	UPROPERTY(EditDefaultsOnly, Category="AttackSettings")
	float fAttackDistance;
	
};
