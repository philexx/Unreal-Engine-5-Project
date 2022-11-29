// Fill out your copyright notice in the Description page of Project Settings.


#include "DetectionSystem.h"
#include "SCharacterBase.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ADetectionSystem::ADetectionSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(RootComponent);
	
	CheckFrequency = 0.5f;

	TraceDist = 700.0f;

	ConeHalfDeg = 3.0f;
}

void ADetectionSystem::CheckForPlayer()
{
	GetWorld()->GetTimerManager().SetTimer(CheckTimerHandle, this, &ADetectionSystem::ExecuteCheck, CheckFrequency, true);
}

void ADetectionSystem::ExecuteCheck()
{
	TArray<AActor*>IgnoredActors;
	IgnoredActors.Add(this);

	const FVector TraceStart = GetActorLocation();
	FVector Direction = GetActorForwardVector();

	const float ConeHalfRad = FMath::DegreesToRadians(ConeHalfDeg);
	Direction = FMath::VRandCone(Direction, ConeHalfRad, ConeHalfRad);

	const FVector TraceEnd = TraceStart + Direction * TraceDist;

	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red,false, 1.0f);
	
	FHitResult HitResult;
	if(GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Pawn))
	{
		ASCharacterBase* Player = Cast<ASCharacterBase>(HitResult.GetActor());
		if(Player)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("PlayerHit"));
		}
	}
	//GetWorld()->GetTimerManager().ClearTimer(CheckTimerHandle);
}

// Called when the game starts or when spawned
void ADetectionSystem::BeginPlay()
{
	Super::BeginPlay();

	CheckForPlayer();
	
}

// Called every frame
void ADetectionSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

