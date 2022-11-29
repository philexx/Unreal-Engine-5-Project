// Fill out your copyright notice in the Description page of Project Settings.


#include "InterpToComponent.h"
#include "TimerManager.h"
#include "SCharacterBase.h"


// Sets default values for this component's properties
UInterpToComponent::UInterpToComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	bStarted = false;
}


// Called when the game starts
void UInterpToComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
	// ...

}

void UInterpToComponent::MoveCharacterTo(ASCharacterBase* MyPawn, const FVector EndLocation, const FRotator EndRotation, float overTime)
{
	SetComponentTickEnabled(true);
	bStarted = true;
	vEndLocation = EndLocation;
	rEndRotation = EndRotation;
	Player = MyPawn;
	fOverTime = overTime;
}

// Called every frame
void UInterpToComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Blue, FString("Tick is turned on"));
	if(bStarted)
	{
		if (Player)
		{
			if(fTimeElapsed < fOverTime)
			{
				Player->SetActorLocationAndRotation(FMath::Lerp(Player->GetActorLocation(), vEndLocation, fTimeElapsed / fOverTime), FMath::Lerp(Player->GetActorRotation(), rEndRotation, (DeltaTime / fTimeElapsed)));
				fTimeElapsed += DeltaTime;
				GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Blue, FString("Started"));
			}
			else if(fTimeElapsed >= fOverTime)
			{
				SetComponentTickEnabled(false);
				Player = nullptr;
				bStarted = false;
				fTimeElapsed = 0;
				GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Blue, FString("OFF"));
			}
		}
	}
}


