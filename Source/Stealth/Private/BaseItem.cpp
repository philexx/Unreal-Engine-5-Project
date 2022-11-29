// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"

// Sets default values
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	SetRootComponent(MeshComp);

	ItemName = "Base Item";
	Amount = 1;
}

// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseItem::UseItem()
{
	//TO DO:
}

void ABaseItem::InteractBP_Implementation()
{
	GEngine->AddOnScreenDebugMessage(20, 2.0f, FColor::Green, "Blueprint implementation");
}

void ABaseItem::Interact()
{
	GEngine->AddOnScreenDebugMessage(20, 2.0f, FColor::Red, "C++ implementation");
}


