// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SCharacterBase.h"
#include "Stealth/FObjectives.h"
#include "SPlayerCharacter.generated.h"

class UWeaponInventory;

UCLASS()
class STEALTH_API ASPlayerCharacter: public ASCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	//Vault
	UFUNCTION(BlueprintImplementableEvent)
	void StartVault(FVector StartLocation,FVector MidLocation, FVector EndLocation);

	void Vault();

	//Climbing Movement

	UFUNCTION(BlueprintImplementableEvent)
	void StartClimbLedge(FVector StartLocation, FVector EndLocation, FRotator Rotation);

	UFUNCTION(BlueprintImplementableEvent)
	void HangClimbLedgeMotionWarp(FVector StartLocation, FVector EndLocation, FRotator Rotation);

	UFUNCTION(BlueprintImplementableEvent)
	void JumpToNextMotionWarp(UAnimMontage* MontageToPlay, const FVector TargetLocation, const FRotator TargetRotation);

	UFUNCTION(BlueprintImplementableEvent)
	void KickInDoorWarp(UAnimMontage* MontageToPlay, const FVector TargetLocation, const FRotator TargetRotation);

	//Dash
	void Dash();
	void DeactivateDashEffect();
	void DashCooldown();

	

	void KickInDoor();
	

	//Weapon
	virtual void StartShooting() override;
	virtual void StopShooting() override;
	void StartAim();
	void StopAim();
	virtual void ReloadWeapon() override;
	void PickupWeapon();

	void GetFocusedCharacter();
	FRotator GetControlRotationFocused();
	void StartFocusCharacter(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWeaponSlotWidget(class AWeaponBase* Weapon);

	//Close Combat
	void Punch();
	void ResetPunching();
	
	//TakeDown
	void TakeDownEnemy();

	bool IsFocusedCharacterDead(ACharacter* FocusedACharacter) const;

	UFUNCTION(BlueprintImplementableEvent)
	void TakeDownMotionWarp();

	//Ledge Climbing
	void ClimbLedgeRight(float Value);
	void FindLedgeUp(float Value);

	void JumpToNextHangPoint();
	void StopJumpNextHangPoint();

	void StopClimbing();

	//UI
	void CreateTakeItemWidget();

	//Basic Movement
	void MoveForward(float value);
	void MoveRight(float value);

	void StartRunning();
	void StopRunning();
	void StartJump();
	
	//Mouse 
	void LookUp(float value);
	void LookRight(float value);
	
	//Pause Game
	void PauseGame();
	
	UFUNCTION(BlueprintCallable, Category=UI)
	void UnPauseGame();


	//Mission
	void InteractMissionActor();

	void PauseMissionInteraction();
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement")
	float fMouseSensitivity;

	UPROPERTY(BlueprintReadOnly, Category = "Components")
	class UInventoryComponent* Inventory;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent* DashEffectComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWeaponInventory* WeaponInventory;
	
	FVector StartingPoint;
	FVector MidPoint;
	FVector EndPoint;

	bool bClimbing;

	bool bCanMoveRightLedge;

	

	//Dash
	UPROPERTY(EditDefaultsOnly, Category="Abilities")
	float fDashStrength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Abilities")
	float DashCooldownTime;
	
	UPROPERTY(BlueprintReadWrite, Category="Abilities")
	bool bCanDash;

	UPROPERTY(BlueprintReadOnly)
	FTimerHandle DashCooldown_TimerHandle;
	
	FTimerHandle DashEffect_TimerHandle;

	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundBase* DashSound;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAiming;

	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, Category=FOV)
	float ZoomedInFOV;

	UPROPERTY(EditDefaultsOnly, Category=FOV)
	float ZoomInterpSpeed;

	UPROPERTY(EditDefaultsOnly, Category=UI)
	UUserWidget* WeaponWheel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UClimbingComponent* ClimbingComponent;

	UPROPERTY()
	class APlayerController* PlayerController;

	UPROPERTY(BlueprintReadOnly)
	ACharacter* FocusedCharacter;
	
	UPROPERTY(BlueprintReadOnly)
	class ADoor* CurrentDoor;

	UPROPERTY(BlueprintReadOnly)
	float ForwardAxisValue;

	UPROPERTY(BlueprintReadOnly)
	float RightAxisValue;

	FTimerHandle PunchReset_TimerHandle;
	bool LeftHandPunch;

	bool bIsFocused;
	
	//Animation Montages
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Animations)
	UAnimMontage* RightHandPunchMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Animations)
	UAnimMontage* LeftHandPunchMontage;
	
	//UI
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<class UUserWidget> TakeWidgetBP;
	
	UPROPERTY()
	class UUserWidget* TakeWidgetInstance;

	//Pause Menu Widget
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<class UUserWidget> PauseWidget;
	
	UPROPERTY(BlueprintReadWrite, Category=UI)
	class UUserWidget* PauseWidgetInstance;
	
	//Mission
	UPROPERTY(BlueprintReadWrite)
	UUMissionComponent* CurrentMissionComponent;

	UPROPERTY(BlueprintReadOnly)
	class AMissionInteractionBase* CurrentMissionInterActor;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	
	FORCEINLINE UCameraComponent* GetCamera() const { return CameraComp; }
	
	UFUNCTION(BlueprintCallable)
	class UInventoryComponent* GetInventoryComponent() const { return Inventory; }

	FORCEINLINE UWeaponInventory* GetWeaponInventory() const { return WeaponInventory; }

	FORCEINLINE bool IsFocusingPlayer() const { return bIsFocused; }
	FORCEINLINE void SetIsFocused(bool Focused) { bIsFocused =  Focused; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetInputAxis() const { return GetInputAxisValue("Climb Right"); }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool CanMoveRightLedge() const { return bCanMoveRightLedge; }
	
	UFUNCTION(BlueprintCallable)
	bool HasWeaponInHand();
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetClimbing() const { return bClimbing; }
	
	FORCEINLINE void SetCurrentDoor(class ADoor* Door) { CurrentDoor = Door; }

	FORCEINLINE void SetIsAiming(bool Aiming) { bIsAiming = Aiming; }
	
	FORCEINLINE void DeleteFocusedCharacter() { FocusedCharacter = nullptr; }

	FORCEINLINE UUMissionComponent* GetCurrentMissionComp() const {return CurrentMissionComponent; }
	
	FORCEINLINE void SetCurrentMissionComp(UUMissionComponent* CurrentMissionComp)
	{
		if(CurrentMissionComponent != nullptr)
		{
			CurrentMissionComponent = nullptr;
		}
		CurrentMissionComponent = CurrentMissionComp;
	}

	UFUNCTION(BlueprintCallable)
	void SetTakeWidgetVisibility(ESlateVisibility Visibility);

	void SetMissionInteractableActor(AMissionInteractionBase* Object) { CurrentMissionInterActor = Object; }
};






