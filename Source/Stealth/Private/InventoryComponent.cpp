// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "BaseItem.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UInventoryComponent::AddItem(ABaseItem* Item)
{
	ABaseItem* NewItem = Item;
	if (NewItem)
	{
		const FName ItemName = NewItem->GetItemName();
		const int32 Amount = NewItem->GetAmount();
		if (ItemAmount.Find(ItemName))
		{
			ItemAmount[ItemName] += Amount;
		}
		else
		{
			ItemAmount.Add(ItemName, Amount);
			Inventory.Add(ItemName, NewItem);
			GEngine->AddOnScreenDebugMessage(20, 2.0f, FColor::Green, "Item Added");
		}
		OnInventoryChanged(ItemName, NewItem, Amount);
	}
}

void UInventoryComponent::RemoveItem(const FName& ItemName)
{
	if(!ItemAmount.Find(ItemName) && !Inventory.Find(ItemName))
	{
		return;
	}

	ItemAmount.Remove(ItemName);
	Inventory.Remove(ItemName);
}

void UInventoryComponent::UseItem(const FName& ItemName)
{
	if(ItemAmount.Find(ItemName) && Inventory.Find(ItemName))
	{
		ABaseItem* Item = Inventory[ItemName];
		if(Item)
		{
			Item->UseItem();
			ItemAmount[ItemName] -= 1;
		}
		if(ItemAmount[ItemName] <= 0)
		{
			RemoveItem(ItemName);
			OnInventoryChanged("", nullptr, 0);
		}
	}
}

void UInventoryComponent::OnInventoryChanged(const FName ItemName, ABaseItem* Item, int32 Amount)
{
	InventoryChanged.Broadcast(ItemName, Item, Amount);
}



