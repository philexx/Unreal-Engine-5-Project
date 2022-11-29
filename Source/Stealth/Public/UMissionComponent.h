// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Stealth/FObjectives.h"
#include "UMissionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionFinished, bool, Finished);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEALTH_API UUMissionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUMissionComponent();

	UFUNCTION(BlueprintCallable)
	bool FindNextTask();

	UFUNCTION(BlueprintCallable)
	void FinishedMission();

	void CreateMissionCompleteWidget();

	void CreateMissionStartWidget();

	void PlaySoundMissionStart() const;

	void PlaySoundMissionEnd() const;

	void DestroyMissionCompleteWidget();

	void DestroyMissionStartWidget();

	void SetTasks();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Settings)
	TArray<UObjective*> Objectives;
	
	uint8 Tasks;

	UFUNCTION(BlueprintCallable)
	void StartMission();

	UPROPERTY(EditDefaultsOnly, Category=Widget)
	TSubclassOf<UUserWidget> MissionComplete;

	UPROPERTY(EditDefaultsOnly, Category=Widget)
	TSubclassOf<UUserWidget> MissionStart;
	
	UPROPERTY(BlueprintReadOnly)
	UUserWidget* MissionCompleteWidget;

	UPROPERTY(BlueprintReadOnly)
	UUserWidget* MissionStartWidget;

	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundBase* SoundMissionStart;

	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundBase* SoundMissionComplete;

	FTimerHandle TimerHandle_MissionComplete;

	FTimerHandle TimerHandle_MissionStart;

	UPROPERTY(EditDefaultsOnly, Category=Widget)
	float WidgetDisplayTime;

	UPROPERTY(BlueprintAssignable, Category=Events)
	FOnMissionFinished OnMissionFinished;

protected:
	UPROPERTY(BlueprintReadWrite)
	bool bOnMission{false};
	
	UPROPERTY(BlueprintReadWrite)
	bool bMissionCompleted{false};

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UObjective* GetObjectiveByEnum(const EMissionType MissionType);

	FORCEINLINE bool GetOnMission() const { return bOnMission; }
	
};
