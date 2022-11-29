// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InterpToComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEALTH_API UInterpToComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInterpToComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool bStarted;

	FVector vEndLocation;
	FRotator rEndRotation;

	float fOverTime;

	float fTimeElapsed;

	class ASCharacterBase* Player;

public:
	void MoveCharacterTo(class ASCharacterBase* MyPawn, const FVector EndLocation, const FRotator EndRotation, float overTime = 5.0f);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
