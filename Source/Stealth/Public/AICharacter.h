// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "SCharacterBase.h"
#include "AICharacter.generated.h"

UCLASS()
class STEALTH_API AAICharacter : public ASCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacter();

	//Weapon
	virtual void StartShooting() override;
	virtual void StopShooting() override;
	virtual void ReloadWeapon() override;

	void SpawnWeapon();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category=AI)
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AI)
	class APatrolPath* PatrolPath;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Weapon)
	TSubclassOf<AWeaponBase> DefaultWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Weapon)
	AWeaponBase* CurrentWeapon;

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	bool bHasWeaponInHand;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
		
	virtual FVector GetPawnViewLocation() const override;
};
