// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"

#include "SPlayerCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bCanKickDoor = false;
	bIsOpen = false;
	bIsKickedIn = false;

	KickInDoorTime = 5.0f;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComponent->SetupAttachment(GetRootComponent());

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComponent->SetupAttachment(MeshComponent);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
	ArrowComponent->SetupAttachment(MeshComponent);
	
}

void ADoor::TimeLineProgress(float Value)
{
	const FRotator NewRotation = FMath::Lerp(DefaultRotation, DefaultRotation + FRotator(0.0f, -180.0f, 0.0f), Value);
	SetActorRotation(NewRotation);
}

void ADoor::Interact()
{
	OpenDoor();
}

void ADoor::OpenDoor()
{
	if(bIsLocked)
		return;
	
	if(!TimelineOpenDoor.IsPlaying())
	{
		if(bIsOpen)
		{
			TimelineOpenDoor.ReverseFromEnd();
		}
		else if(!bIsOpen)
		{
			TimelineOpenDoor.PlayFromStart();
		}
		bIsOpen = !bIsOpen;
	}
}

void ADoor::MoveDoor()
{
	TimelineKickInDoor.PlayFromStart();
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if(Player)
	{
		PlayEffects(Player);
	}
	GetWorld()->GetTimerManager().ClearTimer(KickInDoor_TimerHandle);
}

void ADoor::StartKickDoor(ASPlayerCharacter* Player)
{
	if(!AnimMontage || bIsOpen)
		return;
	
	if(bCanKickDoor && !bIsKickedIn && Player)
	{
		Player->KickInDoorWarp(AnimMontage,ArrowComponent->GetComponentLocation(), GetActorRotation()  + FRotator(0.0f,180.0f, 0.0f));
		GetWorld()->GetTimerManager().SetTimer(KickInDoor_TimerHandle, this, &ADoor::MoveDoor, KickInDoorTime, false);
		bIsKickedIn = true;
	}
}

void ADoor::OpenDoorTimelineInit()
{
	if(OpenDoorFloatCurve)
	{
		FOnTimelineFloat TimelineProg;
		TimelineProg.BindUFunction(this, FName("TimeLineProgress"));
		TimelineOpenDoor.SetLooping(false);
		TimelineOpenDoor.AddInterpFloat(OpenDoorFloatCurve, TimelineProg);
	}
}

void ADoor::KickInDoorTimelineInit()
{
	if(KickInCurveFloat)
	{
		FOnTimelineFloat TimelineProg;
		TimelineProg.BindUFunction(this, FName("TimeLineProgress"));
		TimelineKickInDoor.SetLooping(false);
		TimelineKickInDoor.AddInterpFloat(KickInCurveFloat, TimelineProg);
	}
}

void ADoor::PlayEffects(ACharacter* Player)
{
	APlayerController* PC = Cast<APlayerController>(Player->GetController());
	if(PC)
	{
		PC->ClientStartCameraShake(CameraShake);
		if(KickDoorSound)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), KickDoorSound, GetActorLocation(), GetActorRotation());
	}
}

void ADoor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASPlayerCharacter* Player = Cast<ASPlayerCharacter>(OtherActor);
	if(Player)
	{
		bCanKickDoor = true;
		Player->SetCurrentDoor(this);
	}
}

void ADoor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	ASPlayerCharacter* Player = Cast<ASPlayerCharacter>(OtherActor);
	if(Player)
	{
		bCanKickDoor = false;
		Player->SetCurrentDoor(nullptr);
	}
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OnOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ADoor::EndOverlap);
	
	OpenDoorTimelineInit();
	KickInDoorTimelineInit();

	DefaultRotation = GetActorRotation();
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimelineOpenDoor.TickTimeline(DeltaTime);
	TimelineKickInDoor.TickTimeline(DeltaTime);

}

