// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacterBase.generated.h"

USTRUCT(Blueprintable)
struct FIKBones
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FVector Rhs;
	UPROPERTY(BlueprintReadWrite)
	FVector Lhs;
};

UCLASS()
class STEALTH_API ASCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacterBase();
	
	UFUNCTION(BlueprintCallable)
	FIKBones ClimbBoneIK(const FName& RightSocketName, const FName& LeftSocketName);

	//Weapon
	UFUNCTION(BlueprintCallable, Category=Weapon)
	virtual void StartShooting();
	
	UFUNCTION(BlueprintCallable, Category=Weapon)
	virtual void StopShooting();

	UFUNCTION(BlueprintCallable, Category=Weapon)
	virtual void ReloadWeapon();

	void TakeCover();

	UFUNCTION(BlueprintImplementableEvent)
	void TakeCoverMotionWarp(const FVector TargetLocation,const FRotator TargetRotation, const FVector TargetLocation2 ,const FRotator TargetRotation2);
	
	//Interaction
	void Interact();

	void PlayReloadMontage();
	
	void PlayEquipMontage();

	virtual void Landed(const FHitResult& Hit) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Components")
	class UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, Category = "Components")
	class USpringArmComponent* SpringArm;
	
	UPROPERTY(BlueprintReadWrite, Category=BoneIK)
    FIKBones IKHands;

    UPROPERTY(BlueprintReadWrite, Category=BoneIK)
    FIKBones IKFoots;

	//Trace distance for interactable objects
	float fTraceDistance;

	UPROPERTY(BlueprintReadWrite)
	bool bTakingCover;

	UPROPERTY(BlueprintReadOnly)
	AActor* CurrentInteractableItem;

	UPROPERTY(EditDefaultsOnly, Category=Animations)
	UAnimMontage* ReloadingMontage;

	UPROPERTY(EditDefaultsOnly, Category=Animations)
	UAnimMontage* EquipWeaponMontage;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	FORCEINLINE UAnimMontage* GetEquipAnimMontage() const { return EquipWeaponMontage; }
};
