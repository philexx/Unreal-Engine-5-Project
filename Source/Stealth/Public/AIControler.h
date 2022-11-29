// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControler.generated.h"

/**
 * 
 */
UCLASS()
class STEALTH_API AAIControler : public AAIController
{
	GENERATED_BODY()

public:
	explicit AAIControler(const FObjectInitializer& ObjectInitializer);

	virtual void OnPossess(APawn* InPawn) override;

protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(Transient)
	class UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(Transient)
	class UBlackboardComponent* BlackboardComponent;

	UPROPERTY()
	class AAICharacter* AICharacter;
	
};
