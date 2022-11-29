// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "BaseItem.generated.h"

UCLASS()
class STEALTH_API ABaseItem : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, Category="MeshComp")
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly, Category="Item")
	FName ItemName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Item")
	int32 Amount;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void UseItem();

	void InteractBP_Implementation() override;

	virtual void Interact() override;

	FName GetItemName() const { return ItemName; }

	int32 GetAmount() const { return Amount; }

	void SetAmount(const int32 ItemAmount) { Amount = ItemAmount; }
};
