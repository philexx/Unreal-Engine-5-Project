// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DetectionSystem.generated.h"

UCLASS()
class STEALTH_API ADetectionSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADetectionSystem();

	void CheckForPlayer();

	void ExecuteCheck();

	void Rotate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle CheckTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category=Settings)
	float CheckFrequency;
	
	UPROPERTY(EditDefaultsOnly, Category=Settings)
	float ConeHalfDeg;
	
	UPROPERTY(EditDefaultsOnly, Category=Settings)
	float TraceDist;

	UPROPERTY(EditDefaultsOnly, Category=Components)
	UStaticMeshComponent* MeshComponent;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
