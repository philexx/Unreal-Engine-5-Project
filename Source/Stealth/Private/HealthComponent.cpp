// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "SPlayerCharacter.h"
#include "UMissionComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	TeamNum = 255;
	DefaultHealth = 100.0f;
	bDead = false;
	
	
	// ...
}

void UHealthComponent::CheckMissionKill(AActor* DamageCauser, AActor* DamagedActor) const
{
	if(ASPlayerCharacter* Player = Cast<ASPlayerCharacter>(DamageCauser))
	{
		if(Player == DamagedActor)
			return;
			
		UUMissionComponent* MissionComponent = Player->GetCurrentMissionComp();
		if(!MissionComponent)
			return;

		UObjective* Objective = MissionComponent->GetObjectiveByEnum(EMissionType::Kill);
		if(Objective->ActorsToKill.Num() <= 0)
		{
			Objective->OnTaskStateChanged.Broadcast(0);
			return;
		}
			
		for(const auto DamagedMissionActor : Objective->ActorsToKill)
		{
			if(DamagedActor == DamagedMissionActor)
			{
				Objective->ActorsToKill.Remove(DamagedActor);
				const uint8 AliveActorsLeft = Objective->ActorsToKill.Num();
				Objective->OnTaskStateChanged.Broadcast(AliveActorsLeft);
				break;
			}
		}
	}
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if(Owner)
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
	}
	fHealth = DefaultHealth;
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if(ActorA == nullptr || ActorB == nullptr)
		return true;

	UHealthComponent const* HealthComponentA = Cast<UHealthComponent>(ActorA->GetComponentByClass(UHealthComponent::StaticClass()));
	UHealthComponent const* HealthComponentB = Cast<UHealthComponent>(ActorB->GetComponentByClass(UHealthComponent::StaticClass()));

	if(HealthComponentA == nullptr || HealthComponentB == nullptr)
		return true;

	return HealthComponentA->TeamNum == HealthComponentB->TeamNum;
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bDead)
		return;

	if(DamageCauser != GetOwner()  && IsFriendly(GetOwner(), DamageCauser))
		return;

	fHealth = FMath::Clamp(fHealth - Damage, 0.0f, DefaultHealth );

	bDead = fHealth <= 0.0f;
	OnHealthChanged.Broadcast(this, fHealth, Damage, DamageType, InstigatedBy, DamageCauser);
	if(bDead)
	{
		CheckMissionKill(DamageCauser, DamagedActor);
	}
}
