// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class STEALTH_API ADoor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

	UFUNCTION()
	void TimeLineProgress(float Value);

	//Interact ~ Interface
	virtual void Interact() override;

	//Simple Open Door
	void OpenDoor();

	void MoveDoor();
	
	void StartKickDoor(class ASPlayerCharacter* Player);

	void OpenDoorTimelineInit();

	void KickInDoorTimelineInit();

	void PlayEffects(ACharacter* Player);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category=Components)
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, Category=Curve)
	class UCurveFloat* OpenDoorFloatCurve;

	UPROPERTY(EditDefaultsOnly, Category=Curve)
	class UCurveFloat* KickInCurveFloat;

	UPROPERTY(EditDefaultsOnly, Category=Animation)
	class UAnimMontage* AnimMontage;

	UPROPERTY(EditAnywhere, Category=Components)
	class UArrowComponent* ArrowComponent;

	UPROPERTY(EditDefaultsOnly, Category=Effects)
	TSubclassOf<class UCameraShakeBase> CameraShake;
	
	FTimeline TimelineOpenDoor;

	FTimeline TimelineKickInDoor;

	FTimerHandle KickInDoor_TimerHandle;

	//When to move the door "Open"
	UPROPERTY(EditDefaultsOnly)
	float KickInDoorTime;
	
	bool bCanKickDoor;

	bool bIsOpen;

	bool bIsKickedIn;

	bool bIsLocked;

	UPROPERTY(EditAnywhere, Category=Components)
	class UBoxComponent* BoxComponent;

	UPROPERTY()
	FRotator DefaultRotation;

	UPROPERTY(EditDefaultsOnly, Category=Sound)
	class USoundBase* KickDoorSound;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};


