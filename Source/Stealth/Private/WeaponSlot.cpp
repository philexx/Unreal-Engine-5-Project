// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSlot.h"
#include "SPlayerCharacter.h"
#include "WeaponBase.h"
#include "WeaponInventory.h"
#include "Kismet/GameplayStatics.h"

class UWeaponInventory;

UWeaponSlot::UWeaponSlot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CurrentIndex = 0;

	IsSelected = false;
}

void UWeaponSlot::NativeConstruct()
{
	Super::NativeConstruct();
	
	Update();
}

void UWeaponSlot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
}

void UWeaponSlot::CycleWeaponBackward()
{
	int32 Temp = CurrentIndex - 1;
	if(CurrentIndex <= 0)
	{
		Temp = WeaponReference.Num() -1;
	}
	Update(Temp);
	PlayCycleSound();
}

void UWeaponSlot::CycleWeaponForward()
{
	int32 Temp = CurrentIndex + 1;
	if(WeaponReference.Num() <= Temp)
	{
		Temp = 0;
	}
	Update(Temp);
	PlayCycleSound();
}

void UWeaponSlot::PlayCycleSound() const
{
	if(CycleSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), CycleSound);
	}
}

void UWeaponSlot::Update(int32 Index)
{
	if(SlotBackground)
		SlotBackground->SetRenderTransformAngle(ImageAngle);

	if(SelectedImg)
		SelectedImg->SetRenderTransformAngle(ImageAngle);
	
	if(!WeaponReference.IsValidIndex(Index))
	{
		WeaponThumbnail->SetRenderOpacity(0.0f);
		return;
	}
	
	CurrentIndex = Index;
	if(!WeaponReference.IsEmpty())
	{
		WeaponThumbnail->SetBrushFromTexture(WeaponReference[CurrentIndex]->WeaponThumbnail());
		WeaponThumbnail->SetRenderOpacity(1.0f);

		CurrentWeapon = WeaponReference[CurrentIndex];
	}
}

void UWeaponSlot::UpdateWeaponReference(TArray<AWeaponBase*> Weapons)
{
	if(!WeaponReference.IsEmpty())
	{
		WeaponReference.Empty();
	}

	SetWeaponReference(Weapons);
}

void UWeaponSlot::RemoveWeapon()
{
	ASPlayerCharacter* Player = Cast<ASPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if(Player)
	{
		UWeaponInventory* WeaponInventory = Player->FindComponentByClass<UWeaponInventory>();
		if(WeaponInventory)
			WeaponInventory->DropWeapon();
	}
	WeaponReference.RemoveSwap(CurrentWeapon);
	CurrentWeapon = nullptr;
	
	Update(0);
}

void UWeaponSlot::AddWeapon(AWeaponBase* Weapon)
{
	AWeaponBase* NewWeapon = Weapon;
	if(NewWeapon)
	{
		WeaponReference.AddUnique(Weapon);
		Update(WeaponReference.Num() -1);
	}
}
