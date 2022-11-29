// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInventoryChanged, FName, ItemName, class ABaseItem*, Item, int32, Amount);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEALTH_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInventoryChanged InventoryChanged;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:

	UPROPERTY(BlueprintReadOnly, Category="Inventory")
	TMap<FName, int32> ItemAmount;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TMap<FName, class ABaseItem*> Inventory;

public:

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void AddItem(class ABaseItem* Item);

	void RemoveItem(const FName &ItemName);

	void UseItem(const FName &ItemName);

	UFUNCTION()
	void OnInventoryChanged(const FName ItemName, class ABaseItem* Item ,int32 Amount);
};
