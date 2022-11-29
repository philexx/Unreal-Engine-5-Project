// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWheel.h"

#include "WeaponBase.h"
#include "WeaponInventory.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "SMouseBoundarie.h"
#include "SPlayerCharacter.h"
#include "Widgets/SWeakWidget.h"

UWeaponWheel::UWeaponWheel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
	WheelRadius = 300.0f;
	SlotCount = 6;
	Offset = 90.0f;
	SlotRadius = 12.0f;
	
}

void UWeaponWheel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(IsOpen)
		FindItembyIndex();
}


void UWeaponWheel::NativeConstruct()
{
	Super::NativeConstruct();

	GetWorld()->GetTimerManager().SetTimer(Init_TimerHandle,this, &UWeaponWheel::InitSlots, 0.2f);

	Callback.BindUFunction(this, FName("CycleForward"));
	this->ListenForInputAction(FName("Cycle Weapons Forward"), IE_Pressed, false, Callback);

	Callback.BindUFunction(this, FName("CycleBack"));
	this->ListenForInputAction(FName("Cycle Weapons Back"), IE_Pressed, false, Callback);
}

void UWeaponWheel::UpdateCurrentSlot()
{
	ASPlayerCharacter* Player = Cast<ASPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0 ));
	if(Player)
	{
		UWeaponInventory* WeaponInv = Player->GetWeaponInventory();
		if(WeaponInv)
		{
			TArray<FWeaponArray*> Wpn = WeaponInv->GetWeaponSlots();
			WeaponSlots[CurrentIndex]->SetWeaponReference(Wpn[CurrentIndex]->Weapons);
		}
			
	}
}

FVector2D UWeaponWheel::DetermineSlotPosition(const int32 Index) const
{
	const float SlotAngle = (Index * (360.0f / SlotCount)) - Offset;

	const float X = WheelRadius * UKismetMathLibrary::DegCos(SlotAngle);
	const float Y = WheelRadius * UKismetMathLibrary::DegSin(SlotAngle);

	return FVector2d(X,Y);
}

float UWeaponWheel::GetAngleFromPos(FVector2d Position)
{
	const float X = Position.X - GetViewportCenter().X;
	const float Y = Position.Y - GetViewportCenter().Y;

	return FMath::Fmod(UKismetMathLibrary::DegAtan2(Y,X) + (360.0f + Offset), 360.0f);
}

FVector2D UWeaponWheel::GetViewportCenter()
{
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	return FVector2D(ViewportSize.X/2, ViewportSize.Y/2);
}

void UWeaponWheel::SelectItem()
{
	UWidget* CurrentSelected = FindItem();
	if(CurrentSelected && CurrentSelected != LastSelected)
	{
		PlayHoverSound();
	}
	LastSelected = CurrentSelected;
	CurrentSlot = Cast<UWeaponSlot>(CurrentSelected);
}

void UWeaponWheel::InitSlots()
{
	if(!WeaponSlot)
		return;
	
	ASPlayerCharacter* Player = Cast<ASPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0 ));
	if(Player)
	{
		UWeaponInventory* WeaponInv = Player->GetWeaponInventory();
		if(WeaponInv)
		{
			int32 Index = 0;
			for(const auto ItemSlot : WeaponInv->GetWeaponSlots())
			{
				UWeaponSlot* UW = CreateWidget<UWeaponSlot>(GetWorld(),WeaponSlot);
				if(UW)
				{
					const FVector2D SlotPosition = GetViewportCenter() + DetermineSlotPosition(Index) - FVector2D(115.0f,60.0f);
					UW->SetWeaponReference(ItemSlot->Weapons);
					UW->SetWeaponType(Index);
					UW->SetIndexInWeaponWheel(Index);
					UW->SetImageAngle(GetAngleFromPos(SlotPosition + FVector2D(115.0f,60.0f)));
					UW->SetOwningPlayer(UGameplayStatics::GetPlayerController(this, 0));
					CanvasPanel->AddChild(UW);
					if(UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(UW->Slot))
					{
						CanvasPanelSlot->SetPosition(SlotPosition);
					}
					WeaponSlots.Add(UW);
				}
				Index++;
			}
		}
	}
	CurrentIndex = 0;
}

void UWeaponWheel::PlayHoverSound()
{
	if(HoverSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), HoverSound);
	}
}

UWeaponSlot* UWeaponWheel::DetermineRightSlot(AWeaponBase* Weapon)
{
	if(Weapon)
	{
		TArray<UWidget*> Widgets = CanvasPanel->GetAllChildren();
		for(const auto WheelSlot : Widgets)
		{
			UWeaponSlot* WpnSlt = Cast<UWeaponSlot>(WheelSlot);
			if(WpnSlt)
			{
				if(Weapon->GetWeaponType() == WpnSlt->GetWeaponType())
				{
					if(Widgets.IsValidIndex(CurrentIndex))
						if(UWeaponSlot* Temp = Cast<UWeaponSlot>(Widgets[CurrentIndex]))
							Temp->SetSelected(false);

					
					CurrentIndex = WpnSlt->GetIndexInWeaponWheel();
					CurrentSlot = WpnSlt;
					return CurrentSlot;
					break;
				}
			}
		}
	}
	return nullptr;
}

void UWeaponWheel::CycleForward()
{
	if(CurrentSlot && IsOpen)
	{
		CurrentSlot->CycleWeaponForward();
	}
}

void UWeaponWheel::CycleBack()
{
	if(CurrentSlot && IsOpen)
	{
		CurrentSlot->CycleWeaponBackward();
	}
}

void UWeaponWheel::Show()
{
	this->SetVisibility(ESlateVisibility::Visible);
}

void UWeaponWheel::Hide()
{
	this->SetVisibility(ESlateVisibility::Hidden);
	if(CurrentSlot)
	{
		ASPlayerCharacter* Player = Cast<ASPlayerCharacter>( UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if(Player)
		{
			Player->GetWeaponInventory()->EquipWeapon(CurrentSlot->GetCurrentWeapon());
		}
	}
}

UWidget* UWeaponWheel::FindItem()
{
	const FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	const float MouseAngle = GetAngleFromPos(MousePos);
	for (const auto Child : CanvasPanel->GetAllChildren())
	{
		const FVector2D ItemPos = Cast<UCanvasPanelSlot>(Child->Slot)->GetPosition();
		const float SlotAngle = GetAngleFromPos(ItemPos + FVector2D(110.0f, 50.0f));
		
		if(UKismetMathLibrary::InRange_FloatFloat(MouseAngle,SlotAngle - SlotRadius, SlotAngle + SlotRadius))
		{
			if(CurrentSlot)
			{
				CurrentSlot->SetSelected(true);
			}
			return Child;
		}
		if(CurrentSlot)
		{
			CurrentSlot->SetSelected(false);
		}	
	}
	return nullptr;
}

void UWeaponWheel::FindItembyIndex()
{
	FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	const float MouseAngle = GetAngleFromPos(MousePos);
	
	
	ASPlayerCharacter* Player = Cast<ASPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0 ));
	if(Player)
	{
		UWeaponInventory* WeaponInv = Player->GetWeaponInventory();
		if(WeaponInv)
		{
			const float AnglePerSlot = (MouseAngle) / (360.0f / WeaponInv->GetWeaponSlots().Num());
			const int32 NewIndex = (FMath::Floor(AnglePerSlot == 6)) ? FMath::Floor(AnglePerSlot) : FMath::Floor(AnglePerSlot) + 1;
			TArray<UWidget*> Widgets = CanvasPanel->GetAllChildren();
			if(NewIndex != CurrentIndex)
			{
				if(Widgets.IsValidIndex(CurrentIndex) && Widgets.IsValidIndex(NewIndex))
				{
					PlayHoverSound();

					UWeaponSlot* WpnSlot = Cast<UWeaponSlot>(Widgets[CurrentIndex]);
					if(WpnSlot)
						WpnSlot->SetSelected(false);

					WpnSlot = Cast<UWeaponSlot>(Widgets[NewIndex]);
					if(WpnSlot)
						WpnSlot->SetSelected(true);

					
					CurrentSlot = Cast<UWeaponSlot>(Widgets[NewIndex]);
				}
			}
			CurrentIndex = NewIndex;
		}
	}
}

