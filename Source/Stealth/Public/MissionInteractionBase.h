// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "MissionInteractionBase.generated.h"

UCLASS()
class STEALTH_API AMissionInteractionBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMissionInteractionBase();

	void Interact(ASPlayerCharacter* InteractingCharacter);

	void InteractionEnd();

	void PauseInteraction();

	void PlaySoundDefuse();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxComponent;

	UPROPERTY()
	ASPlayerCharacter* InteractingPlayer;

	UPROPERTY(EditDefaultsOnly, Category=Animation)
	class UAnimMontage* InteractionMontage;
	
	FTimerHandle TimerHandle_Interaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Settings)
	float InteractionTime{2.0f};

	bool InteractionPaused{false};

	bool InteractedWith{false};
	
	UPROPERTY(EditDefaultsOnly, Category=UI)
	class UWidgetComponent* WidgetComponent;

	UPROPERTY(BlueprintReadOnly)
	FTimerHandle TimerHandle_PlaySound;
	
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundBase* InteractionSound;

	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<UUserWidget> WidgetAsset;

	UPROPERTY()
	UUserWidget* PressEWidgetInst;

public:	
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FORCEINLINE bool wasInteractedWith() const {return InteractedWith;}
};


