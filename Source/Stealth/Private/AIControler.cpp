// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControler.h"

#include "AICharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AAIControler::AAIControler(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behaviortree Component"));
	BlackboardComponent =  CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	
}

void AAIControler::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	AAICharacter* Char = Cast<AAICharacter>(InPawn);
	UBehaviorTree const* BT = Char->GetBehaviorTree();
	if(InPawn && BT)
	{
		AICharacter = Char;
		BlackboardComponent->InitializeBlackboard(*BT->BlackboardAsset);
		BehaviorTreeComponent->StartTree(*Char->GetBehaviorTree());
	}
}

void AAIControler::BeginPlay()
{
	Super::BeginPlay();
	if(!AICharacter)
	{
		AAICharacter* Char = Cast<AAICharacter>(GetPawn());
		if(!Char)
			return;
		AICharacter = Char;
	}
}
