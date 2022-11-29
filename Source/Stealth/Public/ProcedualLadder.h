// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProcedualLadder.generated.h"


UCLASS()
class STEALTH_API AProcedualLadder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProcedualLadder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, Category="Components")
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "Components")
	class UArrowComponent* ArrowComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	class USceneComponent* LadderClimbingOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UBoxComponent* BoxComp;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Ladder Settings", Meta=(MakeEditWidget = true))
	FVector LadderEndPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Ladder Settings")
	float fLadderSize;

	UPROPERTY(EditDefaultsOnly, Category = "Ladder Settings")
	float fOffset;

public:	
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	class UStaticMeshComponent* GetMesh() const { return MeshComp; }

	class USceneComponent* GetClimbingOffset() const { return LadderClimbingOffset; }
};
