// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

UCLASS()
class STEALTH_API APatrolPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APatrolPath();

	UFUNCTION(BlueprintCallable)
	void GetSplinePoints();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class USplineComponent* SplineComponent;

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector>SplinePoints;

	UPROPERTY(EditAnywhere)
	class UBillboardComponent* BillboardComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
