// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualLadder.h"

#include "SCharacterBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AProcedualLadder::AProcedualLadder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComp->SetupAttachment(RootComponent);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComp->SetupAttachment(MeshComp);
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AProcedualLadder::OnOverlapBegin);
	BoxComp->OnComponentEndOverlap.AddDynamic(this, &AProcedualLadder::OnOverlapEnd);

	LadderClimbingOffset = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	LadderClimbingOffset->SetupAttachment(MeshComp);
	fOffset = 50.0f;
}

// Called when the game starts or when spawned
void AProcedualLadder::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProcedualLadder::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);

	float LadderNum = LadderEndPoint.Size() / 50.0f;
	for (auto i = 0; i <= LadderNum; i++)
	{
		UStaticMeshComponent* temp = NewObject<UStaticMeshComponent>(this);
		temp->SetupAttachment(MeshComp);
		temp->RegisterComponent();

		temp->SetStaticMesh(MeshComp->GetStaticMesh());
		float fPosition = (i * 50.0f) * (-1);
		FVector Position = FVector(0.0f, 0.0f, fPosition);
		temp->SetRelativeLocation(Position);

	}
	
	float BoxSize = (LadderNum * 50.0f) / 2;
	BoxComp->SetBoxExtent(FVector(20.0f,25.0f, BoxSize));
	BoxComp->SetRelativeLocation(FVector( 20.0f, 0.0f, BoxSize * (-1) + 60.0f));

}

void AProcedualLadder::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ASCharacterBase* Player = Cast<ASCharacterBase>(OtherActor);
		if (Player)
		{
			//Player->ToggleLadder(true, this);
		}
	}
}

void AProcedualLadder::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
	}
}


