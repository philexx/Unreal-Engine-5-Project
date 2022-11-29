
#include "FObjectives.h"

#include "SPlayerCharacter.h"
#include "UMissionComponent.h"
#include "Components/BoxComponent.h"



UObjective::UObjective()
{
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComp->SetBoxExtent(FVector(25.0f, 25.0f,25.0f));
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &UObjective::OnComponentOverlap);
	
	OnTaskStateChanged.AddDynamic(this, &UObjective::TaskStateChanged);
}

void UObjective::TaskStateChanged(uint8 ObjectiveTaskLeft)
{
	if(ObjectiveTaskLeft <= 0)
	{
		MissionFinished = true;
		if(OwnerComponent)
		{
			OwnerComponent->FinishedMission();
		}
	}
}

void UObjective::StartTask(UUMissionComponent* OwnerComp)
{
	if(OwnerComp)
		OwnerComponent = OwnerComp;

	if(MissionType == EMissionType::GoTo)
	{
		BoxComp->SetWorldLocation(GoToLocation);
	}
}

void UObjective::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASPlayerCharacter* Player = Cast<ASPlayerCharacter>(OtherActor);
	if(Player && MissionType == EMissionType::GoTo && MissionFinished == false && Player->GetCurrentMissionComp()->GetOnMission() == true)
	{
		MissionFinished = true;
		OwnerComponent->FinishedMission();
	}
}
