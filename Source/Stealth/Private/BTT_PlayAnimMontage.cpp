// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PlayAnimMontage.h"

#include "AIController.h"
#include "SCharacterBase.h"

UBTT_PlayAnimMontage::UBTT_PlayAnimMontage(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Play Anim Montage";
}

EBTNodeResult::Type UBTT_PlayAnimMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Contr = OwnerComp.GetAIOwner();
	if(!Contr)
		return EBTNodeResult::Failed;

	ASCharacterBase* Bot = Cast<ASCharacterBase>(Contr->GetPawn());
	if(!Bot)
		return EBTNodeResult::Failed;

	if(AnimMontage == nullptr)
		return EBTNodeResult::Failed;
	
	Bot->PlayAnimMontage(AnimMontage);
	return EBTNodeResult::Succeeded;
}
