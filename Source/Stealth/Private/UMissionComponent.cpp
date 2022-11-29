// Fill out your copyright notice in the Description page of Project Settings.


#include "UMissionComponent.h"
#include "SPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UUMissionComponent::UUMissionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool UUMissionComponent::FindNextTask()
{
	if(Objectives.IsEmpty())
		return false;
	
	for(const auto Task : Objectives)
	{
		if(!Task->MissionFinished)
			return true;
	}
	return false;
}


// Called when the game starts
void UUMissionComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
	
    }
    
void UUMissionComponent::StartMission()
{
    if(!bOnMission)
    {
        if(FindNextTask())
        {
        	SetTasks();
        	ASPlayerCharacter* Player = Cast<ASPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        	if(Player)
        		Player->SetCurrentMissionComp(this);
        	
        	CreateMissionStartWidget();
        	PlaySoundMissionStart();
        	bOnMission = true;
        }
    }
}
    
void UUMissionComponent::FinishedMission()
{
	if(FindNextTask() == false)
	{
		ASPlayerCharacter* Player = Cast<ASPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if(Player)
			Player->SetCurrentMissionComp(nullptr);
		CreateMissionCompleteWidget();
		PlaySoundMissionEnd();
		bOnMission = false;
		bMissionCompleted = true;
		OnMissionFinished.Broadcast(bMissionCompleted);
	}
	else
	{
		CreateMissionStartWidget();
		PlaySoundMissionStart();
	}
}

void UUMissionComponent::CreateMissionCompleteWidget()
{
	if(!MissionComplete)
		return;
	
	MissionCompleteWidget = CreateWidget(GetWorld(), MissionComplete);
	if(MissionCompleteWidget)
		MissionCompleteWidget->AddToViewport();
		
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_MissionComplete, this, &UUMissionComponent::DestroyMissionCompleteWidget, WidgetDisplayTime);
}

void UUMissionComponent::CreateMissionStartWidget()
{
	if(!MissionStart)
		return;

	if(MissionStartWidget)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_MissionStart);
		MissionStartWidget->RemoveFromParent();
		MissionStartWidget = nullptr;
	}
	
	MissionStartWidget = CreateWidget(GetWorld(), MissionStart);
	if(MissionStartWidget)
		MissionStartWidget->AddToViewport();
		
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_MissionStart, this, &UUMissionComponent::DestroyMissionStartWidget, WidgetDisplayTime);
}

void UUMissionComponent::PlaySoundMissionStart() const
{
	if(SoundMissionStart)
		UGameplayStatics::PlaySound2D(GetWorld(), SoundMissionStart);
}

void UUMissionComponent::PlaySoundMissionEnd() const
{
	if(SoundMissionComplete)
		UGameplayStatics::PlaySound2D(GetWorld(), SoundMissionComplete);
}

void UUMissionComponent::DestroyMissionCompleteWidget()
{
	if(MissionCompleteWidget)
		MissionCompleteWidget->RemoveFromParent();

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_MissionComplete);
}

void UUMissionComponent::DestroyMissionStartWidget()
{
	if(MissionStartWidget)
		MissionStartWidget->RemoveFromParent();

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_MissionStart);
}

void UUMissionComponent::SetTasks()
{
	for(const auto Task : Objectives)
	{
		Task->StartTask(this);
	}
}


// Called every frame
void UUMissionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UObjective* UUMissionComponent::GetObjectiveByEnum(const EMissionType MissionType)
{
	for(auto obj : Objectives)
	{
		if(obj->MissionType == MissionType)
				return obj;
	}
	return nullptr;
}

