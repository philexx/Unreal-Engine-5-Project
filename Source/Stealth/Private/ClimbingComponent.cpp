// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbingComponent.h"

#include "NetworkMessage.h"
#include "SPlayerCharacter.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Stealth/Stealth.h"

// Sets default values for this component's properties
UClimbingComponent::UClimbingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	bMontagePlaying = false;
	bClimbing = false;
	bCanJumpToNearbyHangPoint = false;
	RightAxisValue = 0;
	ClimbingSpeed = 100.0f;
	DistanceBetweenTraces = 20.0f;
	HangOffset = 90.0f;
	DistanceOffset = 45.0f;
	ForwardTraceDistance = 150.0f;
	IgnoredActors.Add(GetOwner());
	bCanClimbRight = false;
	
}

void UClimbingComponent::PlayAnimationMontage(UAnimMontage* AnimationMontage, FTimerHandle TimerHandle, const FTimerDelegate::TMethodPtr<UClimbingComponent> TimerFunction, bool AllowInput)
{
	if(AnimationMontage)
	{
		if(!AllowInput)
		{
			ComponentOwner()->DisableInput(UGameplayStatics::GetPlayerController(GetWorld(),0));
		}
		const float AnimationPlayRate = ComponentOwner()->PlayAnimMontage(AnimationMontage);
		ComponentOwner()->GetWorldTimerManager().SetTimer(TimerHandle, this, TimerFunction, AnimationPlayRate, false );
		CurrentTimerHandle = &TimerHandle;
	}
}

void UClimbingComponent::PlayLedgeJumpAnimationMontage(int32 Index)
{
	if(LedgeJumpAnimMontages.IsValidIndex(Index))
	{
		if(LedgeJumpAnimMontages[Index])
			PlayAnimationMontage(LedgeJumpAnimMontages[Index],ShimmyJumpMontageTHandle,&UClimbingComponent::MontageFinished);
	}
}

void UClimbingComponent::PlayLedgeClimbAnimationMontage(int32 Index)
{
	if(LedgeClimbingAnimMontages.IsValidIndex(Index))
	{
		if(LedgeClimbingAnimMontages[Index])
			PlayAnimationMontage(LedgeClimbingAnimMontages[Index],ShimmyJumpMontageTHandle,&UClimbingComponent::MontageFinished);
	}
}

void UClimbingComponent::ChoseLedgeClimbAnimation(const float RightInputAxisValue, const float UpInputAxisValue)
{
	if(UpInputAxisValue == 1) //Up
	{
		NextHangPoint.AnimationIndex = ELegdeClimbingAnimations::Climb_Jump_UP;
	}
	if(UpInputAxisValue == -1) //Down
	{
		NextHangPoint.AnimationIndex = ELegdeClimbingAnimations::Climb_Jump_Down;
	}
	if(RightInputAxisValue == 1) //Right
	{
		NextHangPoint.AnimationIndex = ELegdeClimbingAnimations::Climb_Jump_Right;
	}
	if(RightInputAxisValue == -1) //Left
	{
		NextHangPoint.AnimationIndex = ELegdeClimbingAnimations::Climb_Jump_Left;
	}
}


void UClimbingComponent::DisableClimbing()
{
	ComponentOwner()->GetCharacterMovement()->bOrientRotationToMovement = true;
	bClimbing = false;
}

void UClimbingComponent::EnableClimbing(bool MovingComponent, const FVector TargetLocation, const FRotator TargetRotation)
{
	FLatentActionInfo LatInf;
	LatInf.CallbackTarget = GetOwner();
	bClimbing = true;
	ComponentOwner()->GetCharacterMovement()->StopMovementImmediately();
	ComponentOwner()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	ComponentOwner()->GetCharacterMovement()->bOrientRotationToMovement = false;
	ComponentOwner()->GetCharacterMovement()->MaxFlySpeed = ClimbingSpeed;
	ComponentOwner()->GetCharacterMovement()->BrakingDecelerationFlying = 1000.0f;
	
	if(MovingComponent)
		UKismetSystemLibrary::MoveComponentTo(ComponentOwner()->GetCapsuleComponent(), TargetLocation, TargetRotation, true, true, 0.5f, true, EMoveComponentAction::Move, LatInf);
	
}

void UClimbingComponent::FindHangPoint()
{
	FHitResult HitResult;

	const FVector SphereTraceStart = GetOwner()->GetActorLocation() + (GetOwner()->GetActorForwardVector() * 30.0f) + (GetOwner()->GetActorUpVector() * 70.0f);
	const FVector SphereTraceEnd = SphereTraceStart + GetOwner()->GetActorForwardVector() * ForwardTraceDistance;

	if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), SphereTraceStart, SphereTraceEnd, 50.0f, TraceTypeQuery_Climbable, false, IgnoredActors, EDrawDebugTrace::None, HitResult, true))
	{
		if (HitResult.GetActor()->ActorHasTag(FName("Climbable")))
		{
			FHitResult LedgeHit;
			const FVector TraceStart = HitResult.ImpactPoint + FVector(0.0f, 0.0f, 50.0f) + (HitResult.Normal * (-1)) * 1.5f;
			const FVector TraceEnd = HitResult.ImpactPoint + (HitResult.Normal * (-1)) * 3.0f;
			if (GetWorld()->LineTraceSingleByChannel(LedgeHit, TraceStart, TraceEnd, ECC_GameTraceChannel4))
			{
				const FVector TargetLoc = LedgeHit.Location + (LedgeHit.GetActor()->GetActorForwardVector() * DistanceOffset) - FVector(0.0f, 0.0f, HangOffset);
				const FRotator TargetRot = HitResult.GetActor()->GetActorRotation() + FRotator(0.0,180.0f,0.0f);
				EnableClimbing(true, TargetLoc, TargetRot );
			}
		}
	}
}

void UClimbingComponent::FindNearbyHangPoints(float ForwardInputAxisValue, float RightInputAxisValue) //Finds close Ledges when Climbing
{
	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> hits;
	FHitResult ForwardHit;
	const float TraceScale = (ForwardInputAxisValue != 0 && RightInputAxisValue != 0) ? 130.0f : 170.0f;
	const FVector TraceStart = ComponentOwner()->GetActorLocation() + ComponentOwner()->GetActorUpVector() * 100.0f + ComponentOwner()->GetActorForwardVector() * 40.0f;
	const FVector TraceEnd = TraceStart + ((ComponentOwner()->GetActorUpVector() * ForwardInputAxisValue) * TraceScale) + ((ComponentOwner()->GetActorRightVector() * RightInputAxisValue) * TraceScale);

	//Trace to Ignore the Current grabbed Ledge
	if(UKismetSystemLibrary::SphereTraceSingle(GetWorld(), TraceStart, TraceStart + ComponentOwner()->GetActorForwardVector() * 100.0f,8.0f, TraceTypeQuery_Climbable, false, IgnoredActors, EDrawDebugTrace::None, ForwardHit, true))
	{
		ActorsToIgnore.Add(ForwardHit.GetActor());
	}

	//Directional Sphere Trace to Find a ledge Up/Down/Left/Right - 8 Directions 
	if(UKismetSystemLibrary::SphereTraceMulti(GetWorld(), TraceStart, TraceEnd, 30.0f, TraceTypeQuery_Climbable, false, ActorsToIgnore, EDrawDebugTrace::None, hits, true))
	{
		FLedge Ledge = DetermineClosestLedge(hits, ComponentOwner()->GetActorLocation());

		if(!Ledge.Actor)
			return;
		
		FHitResult FinalLocCalc_HitResult;
		TArray<FHitResult> FinalPos_Calc_HitResults;
		FVector MoveToTraceStart = Ledge.ImpactPoint + ((ComponentOwner()->GetActorUpVector() * 20.0f) + (Ledge.Actor->GetActorRightVector() * -1) * 100.0f);
		FVector MoveToTraceEnd = Ledge.ImpactPoint + (Ledge.Actor->GetActorRightVector() * -1) * 100.0f;

		//Sphere traces to find the Center Point of a legde 
		for(auto i = 0; i< 30; i++)
		{
			if(UKismetSystemLibrary::SphereTraceSingle(GetWorld(), MoveToTraceStart, MoveToTraceEnd, 3.0f, TraceTypeQuery_Climbable, false, IgnoredActors,EDrawDebugTrace::None, FinalLocCalc_HitResult, true))
			{
				if(FinalLocCalc_HitResult.GetActor() == Ledge.Actor)
					FinalPos_Calc_HitResults.Add(FinalLocCalc_HitResult);
			}
			MoveToTraceEnd += (ComponentOwner()->GetActorRightVector() * -1) * 5.0f;
			MoveToTraceStart += (ComponentOwner()->GetActorRightVector() * -1) * 5.0f;
		}

		if(!FinalPos_Calc_HitResults.IsEmpty())
		{
			FHitResult FinalLocationHitResult;

			const FVector Center = ((FinalPos_Calc_HitResults[0].ImpactPoint + FinalPos_Calc_HitResults.Last().ImpactPoint) / 2);
			const FVector FinalTraceStart = (FinalPos_Calc_HitResults.Num() > 1) ? Center + ComponentOwner()->GetActorUpVector() * 10.0f : FinalPos_Calc_HitResults[0].ImpactPoint + ComponentOwner()->GetActorUpVector() * 10.0f;
			const FVector FinalTraceEnd =	(FinalPos_Calc_HitResults.Num() > 1) ? Center : FinalPos_Calc_HitResults[0].ImpactPoint;
			
			//Spheretrace to get the point where to move the Character
			if(UKismetSystemLibrary::SphereTraceSingle(GetWorld(), FinalTraceStart, FinalTraceEnd,5.0f, TraceTypeQuery_Climbable,false, ActorsToIgnore, EDrawDebugTrace::None, FinalLocationHitResult, true))
			{
				NextHangPoint.TargetLocation = FinalLocationHitResult.ImpactPoint + ((FinalLocationHitResult.GetActor()->GetActorForwardVector() * DistanceOffset - 5.0f ) - FVector(0.0f, 0.0f, 200.0f));
				NextHangPoint.TargetRotation = FinalLocationHitResult.GetActor()->GetActorRotation() + FRotator(0.0, 180.0f, 0.0f);
				NextHangPoint.AxisValue = (RightInputAxisValue != 0 ) ? RightInputAxisValue : ForwardInputAxisValue;
				
				const float Distance = FVector::Distance(ComponentOwner()->GetActorLocation() + (ComponentOwner()->GetActorUpVector() * 100.0f), FinalLocationHitResult.ImpactPoint);
				if(Distance < 90.0f)
				{
					ChoseLedgeClimbAnimation(RightInputAxisValue, ForwardInputAxisValue);
					MoveToCloseHangPoint();
				}
				else if(Distance > 90.0f)
				{
					ChoseLedgeClimbAnimation(RightInputAxisValue, ForwardInputAxisValue);
					JumpToNextHangPoint();
					bCanJumpToNearbyHangPoint = true;
				}
			}
		}
	}
}

void UClimbingComponent::MontageFinished()
{
	if(CurrentTimerHandle)
		ComponentOwner()->EnableInput(UGameplayStatics::GetPlayerController(GetWorld(),0));
		ComponentOwner()->GetWorldTimerManager().ClearTimer(*CurrentTimerHandle);
		bCanJumpToNearbyHangPoint = false;
}

void UClimbingComponent::FindUpHangPoint(float Value, float Value1)
{
	FindNearbyHangPoints(Value, Value1);
}

void UClimbingComponent::JumpToNextReleased()
{
	bCanJumpToNearbyHangPoint = false;
}

void UClimbingComponent::StopClimbing()
{
	DisableClimbing();
	ComponentOwner()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

bool UClimbingComponent::CanClimb(float value)
{
	if (value > 0 || value < 0)
	{
		const FVector TraceStart = GetOwner()->GetActorLocation() + GetOwner()->GetActorUpVector() * 105.0f + ((GetOwner()->GetActorRightVector() * value) * 40.0f);
		const FVector TraceEnd = TraceStart + GetOwner()->GetActorForwardVector() * 200.0f;
		if(UKismetSystemLibrary::SphereTraceSingle(GetWorld(), TraceStart, TraceEnd, 5.0f, TraceTypeQuery_Climbable, false,IgnoredActors, EDrawDebugTrace::None, Climbing_HitResult,true))
		{
			return true;
		}
		ComponentOwner()->GetCharacterMovement()->StopMovementImmediately();
		return false;
	}
	return false;
}


bool UClimbingComponent::IsClimbing() const
{
	if(bClimbing)
	{
		return true;
	}
	return false;
}

ASPlayerCharacter* UClimbingComponent::ComponentOwner() const
{
	return Cast<ASPlayerCharacter>(GetOwner());
}
FLedge UClimbingComponent::DetermineClosestLedge(const TArray<FHitResult> HitResult, const FVector LocationToCompare) const
{
	FLedge Ledge;
	Ledge.Actor = nullptr;
	Ledge.ImpactPoint = FVector(200000.0f,200000.0f,200000.0f);
	
	for (const auto Actor : HitResult)
	{
		if( FVector::Distance(Actor.ImpactPoint, LocationToCompare) < Ledge.ImpactPoint.Length())
		{
			Ledge.Actor = Actor.GetActor();
			Ledge.ImpactPoint = Actor.ImpactPoint;
		}
	}
	return Ledge;
}

void UClimbingComponent::MoveCapsule()
{
	CurveTimeline.PlayFromStart();
}

void UClimbingComponent::ClimbRight(float Value, const FVector DirectionVector)
{
	if(bClimbing )
	{
		bCanClimbRight = CanClimb(Value);
		if(bCanClimbRight)
		{
			RightAxisValue = Value;
			ComponentOwner()->AddMovementInput(ComponentOwner()->GetActorRightVector(), Value);
		}
		else
		{
			RightAxisValue = 0;
		}
	}
}

void UClimbingComponent::MoveToCloseHangPoint()
{
	if(bClimbing)
	{
		if(!LedgeClimbingAnimMontages[NextHangPoint.AnimationIndex])
			return;
		
		ComponentOwner()->JumpToNextMotionWarp(LedgeClimbingAnimMontages[NextHangPoint.AnimationIndex], NextHangPoint.TargetLocation, NextHangPoint.TargetRotation);
		ComponentOwner()->DisableInput(UGameplayStatics::GetPlayerController(GetWorld(),0));
	}
}

void UClimbingComponent::JumpToNextHangPoint()
{
	if(bCanJumpToNearbyHangPoint && bClimbing)
	{
		if(!LedgeJumpAnimMontages[NextHangPoint.AnimationIndex])
			return;
		ComponentOwner()->JumpToNextMotionWarp(LedgeJumpAnimMontages[NextHangPoint.AnimationIndex], NextHangPoint.TargetLocation, NextHangPoint.TargetRotation);
		ComponentOwner()->DisableInput(UGameplayStatics::GetPlayerController(GetWorld(),0));
	}
}

void UClimbingComponent::JumpOnCurb()
{
	if(bClimbing)
	{
		HangClimbLedge();
		return;
	}

	
	FHitResult WallHitSingle;
	TArray<FHitResult> HitResults;
	FHitResult WallHit_Result;
	FHitResult LedgeHit_Result;
	FRotator ClimbRotation = ComponentOwner()->GetActorRotation();
	
	if (GetWorld()->LineTraceSingleByChannel(WallHitSingle, ComponentOwner()->GetActorLocation(),
											 ComponentOwner()->GetActorLocation() + ComponentOwner()->GetActorForwardVector() * 200.0f, ECC_Visibility))
	{
		ClimbRotation = UKismetMathLibrary::MakeRotFromX(WallHitSingle.Normal * -1);
	}

	FVector WallTraceStart = ComponentOwner()->GetActorLocation() + FVector(0.0f, 0.0f, 30.0f);
	FVector WallTraceEnd = WallTraceStart + ComponentOwner()->GetActorForwardVector() * 100.0f;
	for (auto i = 0; i <= 9; i++)
	{
		if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), WallTraceStart, WallTraceEnd, 10.0f,
													ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors,
													EDrawDebugTrace::None, WallHit_Result, true))
		{
			HitResults.Add(WallHit_Result);
			if (UKismetSystemLibrary::SphereTraceSingle(
				GetWorld(), WallHit_Result.ImpactPoint + FVector(0.0f, 0.0f, 20.0f), WallHit_Result.ImpactPoint, 10.0f,
				ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, LedgeHit_Result,
				true))
			{
				if (!LedgeHit_Result.bStartPenetrating)
				{
					break;
				}
			}
		}
		WallTraceStart += FVector(0.0f, 0.0f, 20.0f);
		WallTraceEnd += FVector(0.0f, 0.0f, 20.0f);
	}
	if (HitResults.Num() <= 0)
	{
		return;
	}
	
	if (HitResults.Num() <= 3)
	{
		ComponentOwner()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		ComponentOwner()->StartClimbLedge(WallHit_Result.Location + FVector(0.0f, 0.0f, 100.0f),LedgeHit_Result.Location + WallHitSingle.Normal * (-100.0f), ClimbRotation);
		DisableClimbing();
	}
}

void UClimbingComponent::Vault()
{
	TArray<FHitResult> HitResultsWall;
	TArray<FHitResult> HitResultsLedge;
	FHitResult FloorHit, WallHit, LedgeHit;
	bool bBlockingHit = false;

	FVector WallTraceStart = ComponentOwner()->GetActorLocation();
	FVector WallTraceEnd = WallTraceStart + ComponentOwner()->GetActorForwardVector() * 200.0f;
	FVector WallHitPos;

	for (auto i = 0; i < 10; i++)
	{
		if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), WallTraceStart, WallTraceEnd, 10.0f,
		                                            ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors,
		                                            EDrawDebugTrace::None, WallHit, true))
		{
			WallHitPos = WallHit.Location;
			HitResultsWall.Add(WallHit);
			bBlockingHit = true;
		}
		WallTraceStart += FVector(0.0f, 0.0f, 30.0f);
		WallTraceEnd += FVector(0.0f, 0.0f, 30.0f);
	}

	if (bBlockingHit == false)
	{
		return;
	}

	FVector LedgeTraceStart = WallHitPos + (ComponentOwner()->GetActorForwardVector() * 10.0f) + FVector(0.0f, 0.0f, 150.0f);
	FVector LedgeTraceEnd = WallHitPos + (ComponentOwner()->GetActorForwardVector() * 10.0f) - FVector(0.0f, 0.0f, 100.0f);
	for (auto i = 0; i <= 8; i++)
	{
		if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), LedgeTraceStart, LedgeTraceEnd, 10.0f,
		                                            ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors,
		                                            EDrawDebugTrace::None, LedgeHit, true))
		{
			HitResultsLedge.Add(LedgeHit);
		}
		LedgeTraceStart += ComponentOwner()->GetActorForwardVector() * 40.0f;
		LedgeTraceEnd += ComponentOwner()->GetActorForwardVector() * 40.0f;
	}

	FVector StartingPoint;
	if (HitResultsLedge.IsValidIndex(1))
	{
		StartingPoint = HitResultsLedge[1].Location;
	}
	if (HitResultsLedge.Num() <= 8 && HitResultsWall.Num() <= 4)
	{
		
		const FVector MidPoint = HitResultsLedge.Last().Location;
		FVector EndPoint;
		
		FVector FloorTraceStart = MidPoint + ComponentOwner()->GetActorForwardVector() * 150.0f;
		FVector FloorTraceEnd = FloorTraceStart - FVector(0.0f, 0.0f, 1000.0f);
		if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), FloorTraceStart, FloorTraceEnd, 10.0f,
		                                            ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors,
		                                            EDrawDebugTrace::None, FloorHit, true))
		{
			EndPoint = FloorHit.Location;
		}
		ComponentOwner()->StartVault(StartingPoint, MidPoint, EndPoint);
		ComponentOwner()->GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

// Called when the game starts

void UClimbingComponent::HangClimbLedge()
{
	FHitResult HitResult;
	const FVector TraceStart = ComponentOwner()->GetActorLocation() + (ComponentOwner()->GetActorUpVector() * 100.0f) + ComponentOwner()->GetActorForwardVector() * 55.0f;
	const FVector TraceEnd = TraceStart + ComponentOwner()->GetActorUpVector() * (- 101.0f);
	
	if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), TraceStart, TraceEnd, 10.0f,
												ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors,
												EDrawDebugTrace::None, HitResult, true))
	{
		ComponentOwner()->HangClimbLedgeMotionWarp(HitResult.Location + ComponentOwner()->GetActorForwardVector() * (-20.0f) - FVector(0.0f, 0.0f, 10.0f),
								 HitResult.Location + ComponentOwner()->GetActorForwardVector() * 55.0f, ComponentOwner()->GetActorRotation());
		ComponentOwner()->GetCharacterMovement()->bOrientRotationToMovement = true;
		DisableClimbing();
	}
}

void UClimbingComponent::TimelineProgress(float Value)
{
	const FVector NewLocation = FMath::Lerp(ComponentOwner()->GetActorLocation(), NextHangPoint.TargetLocation, Value);
	ComponentOwner()->SetActorLocation(NewLocation);
}


void UClimbingComponent::BeginPlay()
{
	Super::BeginPlay();
	if(CurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, "TimelineProgress");
		CurveTimeline.AddInterpFloat(CurveFloat, TimelineProgress);
		CurveTimeline.SetLooping(false);
	}
}


// Called every frame
void UClimbingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	CurveTimeline.TickTimeline(DeltaTime);
}

