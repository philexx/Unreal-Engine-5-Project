// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_PlayAnimMontage.generated.h"

/**
 * 
 */
UCLASS()
class STEALTH_API UBTT_PlayAnimMontage : public UBTTaskNode
{
	GENERATED_BODY()

	explicit UBTT_PlayAnimMontage(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category=Node)
	UAnimMontage* AnimMontage;
};
