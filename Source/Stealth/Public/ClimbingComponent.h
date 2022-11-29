// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "ClimbingComponent.generated.h"


class UAnimMontage;
class AClimbingLedge;

USTRUCT()
struct FNextHangPoint
{
	GENERATED_BODY()
	FVector TargetLocation;
	FRotator TargetRotation;
	float AxisValue;
	int32 AnimationIndex;
};

USTRUCT()
struct FLedge
{
	GENERATED_BODY()
	FVector ImpactPoint;
	AActor* Actor;
};

UENUM()
enum ELegdeClimbingAnimations { Climb_Jump_Right = 0, Climb_Jump_Left = 1, Climb_Jump_UP = 2, Climb_Jump_Down = 3, };

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEALTH_API UClimbingComponent : public UActorComponent
{
	GENERATED_BODY()

public:


	// Sets default values for this component's properties
	UClimbingComponent();

	//Animation Montage
	void PlayAnimationMontage(UAnimMontage* AnimationMontage, FTimerHandle TimerHandle, const FTimerDelegate::TMethodPtr<UClimbingComponent> TimerFunction, bool AllowInput = false );
	FTimerHandle* CurrentTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category="Ledge Jump Animations")
	TArray<UAnimMontage*> LedgeJumpAnimMontages;

	UPROPERTY(BlueprintReadWrite, Category=Animation)
	UAnimMontage* CurrentMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="Ledge Climb Animations")
	TArray<UAnimMontage*> LedgeClimbingAnimMontages;

	void PlayLedgeJumpAnimationMontage(int32 Index);
	void PlayLedgeClimbAnimationMontage(int32 Index);

	void ChoseLedgeClimbAnimation(const float RightInputAxisValue, const float UpInputAxisValue);
	
	//
	UFUNCTION(BlueprintCallable)
	void DisableClimbing();
	UFUNCTION(BlueprintCallable)
	void EnableClimbing(bool MovingComponent = false, const FVector TargetLocation = FVector::ZeroVector, const FRotator TargetRotation = FRotator::ZeroRotator);
	
	UFUNCTION(BlueprintCallable)
	void FindHangPoint();

	UFUNCTION(BlueprintCallable)
	void FindNearbyHangPoints(float Value, float Value1);
	
	UFUNCTION(BlueprintCallable)
	void ClimbRight(float Value, const FVector DirectionVector);

	void JumpToNextHangPoint();
	void JumpToNextReleased();

	void StopClimbing();

	void MoveToCloseHangPoint();
	
	bool CanClimb(float Value);

	void MontageFinished();

	void FindUpHangPoint(float ForwardInputAxisValue, float RightInputAxisValue);

	void MoveCapsule();

	void JumpOnCurb();

	void Vault();

	void HangClimbLedge();

	UFUNCTION()
	void TimelineProgress(float Value);

	

protected:

	//Anim
	
	bool bMontagePlaying;
	
	//Climbing
	UPROPERTY(EditDefaultsOnly, Category = "Climbing Settings")
	float DistanceBetweenTraces;

	//Point Climbing

	FNextHangPoint NextHangPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Climbing Settings")
	float HangOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Climbing Settings")
	float DistanceOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Climbing Settings")
	float ForwardTraceDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Climbing Settings")
	float ClimbingSpeed;

	UPROPERTY(BlueprintReadOnly)
	bool bClimbing;
	
	bool bCanJumpToNearbyHangPoint;
	
	UPROPERTY(BlueprintReadOnly)
	bool bCanClimbRight;
	
	FTimerHandle ShimmyJumpMontageTHandle;

	FHitResult Climbing_HitResult;

	TArray<AActor*>IgnoredActors;
	
	FLedge DetermineClosestLedge(TArray<FHitResult> Locations, const FVector LocationToCompare) const;
	
	// Called when the game starts
	virtual void BeginPlay() override;
	
	FTimeline CurveTimeline;

	UPROPERTY(EditAnywhere, Category="Curves")
	UCurveFloat* CurveFloat;

	UPROPERTY(BlueprintReadOnly)
	float RightAxisValue;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	bool IsClimbing()const;

	inline class ASPlayerCharacter* ComponentOwner() const;
};






