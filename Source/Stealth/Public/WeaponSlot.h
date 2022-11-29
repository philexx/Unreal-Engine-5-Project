// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Weapontype.h>

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "WeaponSlot.generated.h"

/**
 * 
 */
UCLASS()
class STEALTH_API UWeaponSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UWeaponSlot(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void CycleWeaponForward();

	UFUNCTION(BlueprintCallable)
	void CycleWeaponBackward();

	void PlayCycleSound()const;
	
	UFUNCTION(BlueprintCallable)
	void Update(int32 Index = 0);
	
	UFUNCTION(BlueprintCallable)
	void UpdateWeaponReference(TArray<class AWeaponBase*>Weapons);

	UFUNCTION(BlueprintCallable)
	void RemoveWeapon();

	UFUNCTION(BlueprintCallable)
	void AddWeapon(class AWeaponBase* Weapon);
protected:
	
	UPROPERTY(BlueprintReadWrite)
	TArray<class AWeaponBase*> WeaponReference;

	UPROPERTY(meta = (BindWidget))
	UImage* SlotBackground;
	
	UPROPERTY(meta = (BindWidget))
	UImage* SelectedImg;
	
	UPROPERTY(meta = (BindWidget))
	UImage* WeaponThumbnail;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentIndex;

	UPROPERTY(BlueprintReadOnly)
	int32 IndexInWeaponWheel;
	
	float ImageAngle;

	UPROPERTY(BlueprintReadOnly)
	bool IsSelected;

	FTimerDelegate TimerDelegate;
	
	FTimerHandle Init_TimerHandle;

	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundBase* CycleSound;

	UPROPERTY(BlueprintReadOnly)
	class AWeaponBase* CurrentWeapon;

	int32 WeaponType;
	
	
public:
	FORCEINLINE void SetWeaponReference(TArray<AWeaponBase*> const& Weapons) { WeaponReference = Weapons;}

	FORCEINLINE TArray<class AWeaponBase*> &WeaponArrayRef() { return WeaponReference; }

	FORCEINLINE void SetImageAngle(float Angle) { ImageAngle = Angle; }

	FORCEINLINE AWeaponBase* GetCurrentWeapon() const { if(WeaponReference.IsValidIndex(CurrentIndex)){ return WeaponReference[CurrentIndex]; } return nullptr; }

	FORCEINLINE UImage* GetBackGround() const { return SlotBackground; }

	FORCEINLINE float GetAngle() const { return ImageAngle; }

	FORCEINLINE void SetSelected(bool Selected) { IsSelected = Selected; }

	FORCEINLINE int32 GetWeaponType() const { return WeaponType; }

	FORCEINLINE void SetWeaponType(int32 WType) { WeaponType = WType; }

	FORCEINLINE int32 GetIndexInWeaponWheel() const { return IndexInWeaponWheel; }

	FORCEINLINE void SetIndexInWeaponWheel(int32 Index) { IndexInWeaponWheel = Index; }
};
