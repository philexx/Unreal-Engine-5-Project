// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponSlot.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "WeaponWheel.generated.h"

/**
 * 
 */
UCLASS()
class STEALTH_API UWeaponWheel : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite)
	int32 SlotCount;

	float Offset;

	UPROPERTY(EditDefaultsOnly)
	float SlotRadius;

	UPROPERTY(EditAnywhere, Category=Settings)
	float WheelRadius;

	UPROPERTY(BlueprintReadWrite ,meta = (BindWidget))
	UCanvasPanel* CanvasPanel;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWeaponSlot> WeaponSlot;

	FTimerHandle Init_TimerHandle;

	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundBase* HoverSound;

	UPROPERTY(BlueprintReadOnly)
	UWidget* LastSelected;
	
	UPROPERTY(BlueprintReadOnly)
	UWeaponSlot* CurrentSlot;
	
	FOnInputAction Callback;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentIndex;

	UPROPERTY(BlueprintReadWrite)
	bool IsOpen;

	UPROPERTY()
	TArray<UWeaponSlot*> WeaponSlots;

public:
	UWeaponWheel(const FObjectInitializer& ObjectInitializer);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void UpdateCurrentSlot();
	
	UFUNCTION(BlueprintCallable)
	FVector2D DetermineSlotPosition(const int32 Index) const;

	UFUNCTION(BlueprintCallable)
	float GetAngleFromPos(FVector2D Position);

	FVector2D GetViewportCenter();

	UFUNCTION(BlueprintCallable)
	UWidget* FindItem();

	void FindItembyIndex();
	
	UFUNCTION(BlueprintCallable)
	void SelectItem();

	UFUNCTION(BlueprintCallable)
	void InitSlots();

	void PlayHoverSound();

	UFUNCTION(BlueprintCallable)
	UWeaponSlot* DetermineRightSlot(class AWeaponBase* Weapon);

	UFUNCTION()
	void CycleForward();
	UFUNCTION()
	void CycleBack();

	void Show();

	void Hide();
	
};
