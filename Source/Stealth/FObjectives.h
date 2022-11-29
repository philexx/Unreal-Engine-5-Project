#pragma once



#include "MissionType.h"
#include "Kismet/GameplayStatics.h"
#include "FObjectives.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTaskStateChanged, uint8, ObjectiveTaskLeft);

UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class STEALTH_API UObjective : public UObject
{
	GENERATED_BODY()

	
public:

	UObjective();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Settings)
	TEnumAsByte<EMissionType> MissionType{0};

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Settings)
	FString MissionInstruction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Settings)
	FVector GoToLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Settings)
	TArray<AActor*>ActorsToKill;

	UPROPERTY(EditAnywhere, Category=Settings)
	TArray<AActor*> ActorsToInteract;

	UPROPERTY(BlueprintReadWrite)
	bool MissionFinished{false};

	UPROPERTY(BlueprintAssignable)
	FOnTaskStateChanged OnTaskStateChanged;

	UFUNCTION()
	void TaskStateChanged(uint8 ObjectiveTaskLeft);

	void SetOwnerComponent(class UUMissionComponent* ActorComponent) { OwnerComponent = ActorComponent; }

	UUMissionComponent* GetOwnerComponent() const { return OwnerComponent; }

	void StartTask(UUMissionComponent* OwnerComp);

	UFUNCTION()
	void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxComp;

	UPROPERTY(BlueprintReadOnly)
	class UUMissionComponent* OwnerComponent;
};




