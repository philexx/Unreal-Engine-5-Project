// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionInteractionBase.h"

#include "SPlayerCharacter.h"
#include "UMissionComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMissionInteractionBase::AMissionInteractionBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	SetRootComponent(MeshComponent);
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->SetupAttachment(MeshComponent);

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AMissionInteractionBase::OnComponentBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AMissionInteractionBase::OnComponentEndOverlap);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));
	WidgetComponent->SetupAttachment(MeshComponent);
	WidgetComponent->SetVisibility(false);

}

void AMissionInteractionBase::Interact(ASPlayerCharacter* InteractingCharacter)
{
	InteractingPlayer = InteractingCharacter;
	if(InteractingPlayer != nullptr)
	{
		InteractingPlayer->GetCharacterMovement()->DisableMovement();
		if(InteractionMontage)
			InteractingPlayer->PlayAnimMontage(InteractionMontage);
		
		if(InteractionPaused)
		{
			InteractionPaused = false;
			GetWorld()->GetTimerManager().UnPauseTimer(TimerHandle_Interaction);
		}
		else
		{
			WidgetComponent->SetVisibility(true);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_PlaySound, this, &AMissionInteractionBase::PlaySoundDefuse, InteractionTime);
		}
	}
}

void AMissionInteractionBase::InteractionEnd()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Interaction);
	if(InteractingPlayer)
	{
		UUMissionComponent* MissionComp = InteractingPlayer->GetCurrentMissionComp();
		if(!MissionComp)
			return;
		
		UObjective* Task = MissionComp->GetObjectiveByEnum(EMissionType::Interact);
		if(Task)
		{
			Task->ActorsToInteract.Remove(this);
			const uint8 ActorsToInteract = Task->ActorsToInteract.Num();
			Task->OnTaskStateChanged.Broadcast(ActorsToInteract);
			
			InteractingPlayer->SetMissionInteractableActor(nullptr);
			WidgetComponent->SetVisibility(false);
			InteractedWith = true;
		}
		InteractingPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}

void AMissionInteractionBase::PauseInteraction()
{
	if(!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Interaction) || InteractingPlayer == nullptr )
		return;

	InteractionPaused = !InteractionPaused;
	InteractingPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetWorld()->GetTimerManager().PauseTimer(TimerHandle_Interaction);
}

void AMissionInteractionBase::PlaySoundDefuse()
{
	if(InteractionSound)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), InteractionSound, GetActorLocation(), GetActorRotation());

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_PlaySound);
	
	const float SoundDuration = InteractionSound->GetDuration();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Interaction, this, &AMissionInteractionBase::InteractionEnd, SoundDuration );
}

// Called when the game starts or when spawned
void AMissionInteractionBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMissionInteractionBase::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	ASPlayerCharacter* Player = Cast<ASPlayerCharacter>(OtherActor);
	if(Player && InteractedWith == false && Player->GetCurrentMissionComp() != nullptr)
	{
		Player->SetMissionInteractableActor(this);
		if(WidgetAsset)
			PressEWidgetInst = CreateWidget(GetWorld(), WidgetAsset);

		if(PressEWidgetInst)
			PressEWidgetInst->AddToViewport();
	}
}

void AMissionInteractionBase::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASPlayerCharacter* Player = Cast<ASPlayerCharacter>(OtherActor);
	if(Player)
	{
		Player->SetMissionInteractableActor(nullptr);
		if(PressEWidgetInst)
			PressEWidgetInst->RemoveFromParent();
	}
}
