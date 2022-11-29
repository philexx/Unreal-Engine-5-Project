// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacterBase.h"

#include "InteractableInterface.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Stealth/Stealth.h"

// Sets default values
ASCharacterBase::ASCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 270.0f;
	SpringArm->bUsePawnControlRotation = true;


	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	CameraComp->SetFieldOfView(110.0f);
	CameraComp->bUsePawnControlRotation = false;

	fTraceDistance = 5000.0f;

}


FIKBones ASCharacterBase::ClimbBoneIK(const FName& RightSocketName, const FName& LeftSocketName)
{
	FIKBones IKLoc;
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	FHitResult RightHandHitResult, RightHandLedgeHitResult, LeftHandHitResult, LeftHandLedgeHitResult;

	FVector LineStart = GetMesh()->GetSocketLocation(RightSocketName);
	FVector LineEnd = LineStart + GetActorForwardVector() * 60.0f;
	
	if (UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(), LineStart,LineEnd, 10.0f, TraceTypeQuery_Climbable, false, IgnoredActors, EDrawDebugTrace::ForOneFrame,
		RightHandHitResult, true))
	{
		LineStart = RightHandHitResult.Location + FVector(0.0f, 0.0f, 20.0f);
		LineEnd = RightHandHitResult.Location - FVector(0.0f, 0.0f, 2.0f);
		
		if (UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(), LineStart,LineEnd, 5.0f, TraceTypeQuery_Climbable, false,
			IgnoredActors, EDrawDebugTrace::ForOneFrame, RightHandLedgeHitResult, true))
		{
			const FVector Location = RightHandHitResult.ImpactPoint + RightHandHitResult.ImpactNormal;
			IKLoc.Rhs = FVector(Location.X, Location.Y, Location.Z);
			DrawDebugSphere(GetWorld(),IKLoc.Rhs, 5.0f, 20, FColor::Green, false, 0.1f);
		}
	}

	LineStart = GetMesh()->GetSocketLocation(LeftSocketName);
	LineEnd = LineStart + GetActorForwardVector() * 60.0f;
	
	if (UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(), LineStart,LineEnd, 5.0f, ETraceTypeQuery::TraceTypeQuery6, false, IgnoredActors, EDrawDebugTrace::ForOneFrame,
		LeftHandHitResult, true))
	{
		if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), LeftHandHitResult.Location + FVector(0.0f, 0.0f, 40.0f),
		                                            LeftHandHitResult.Location - FVector(0.0f, 0.0f, 2.0f), 5.0f,
		                                            ETraceTypeQuery::TraceTypeQuery6, false, IgnoredActors,
		                                            EDrawDebugTrace::ForOneFrame, LeftHandLedgeHitResult, true))
		{
			const FVector Location = LeftHandHitResult.Location + LeftHandHitResult.ImpactNormal;
			IKLoc.Lhs = FVector(Location.X, Location.Y, LeftHandLedgeHitResult.Location.Z);
			DrawDebugSphere(GetWorld(), IKLoc.Lhs, 20.0f, 20, FColor::Green, false, 0.1f);
		}
	}
	return IKLoc;
}

void ASCharacterBase::StartShooting()
{
}

void ASCharacterBase::StopShooting()
{
}

void ASCharacterBase::ReloadWeapon()
{
}


void ASCharacterBase::TakeCover()
{
	FVector TraceStart =  GetActorLocation() + (GetActorUpVector() * 50.0f);
	const FVector TraceEnd = TraceStart + GetActorForwardVector() * 150.0f;
	FHitResult Hit;
	
	const FVector StartLocation = GetActorLocation() + (GetActorRightVector() * 50.0f); 
	const FRotator StartRotation = GetActorRotation() - FRotator(0.0f, 90.0f, 0.0f);
	
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	if(UKismetSystemLibrary::SphereTraceSingle(GetWorld(), TraceStart, TraceEnd, 30.0f, ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::ForDuration, Hit, true ))
	{
		FVector	TargetLoc = Hit.ImpactPoint + (Hit.Normal  * 40.0f);
		TargetLoc = FVector(TargetLoc.X, TargetLoc.Y, GetActorLocation().Z);
		const FRotator TargetRot = Hit.Normal.Rotation();
		TakeCoverMotionWarp(StartLocation, StartRotation, TargetLoc, TargetRot);
		bTakingCover = true;
	}
}

void ASCharacterBase::Interact()
{
	if (CameraComp)
	{
		const FVector TraceStart = CameraComp->GetComponentLocation();
		const FVector TraceEnd = TraceStart + (CameraComp->GetComponentRotation().Vector() * fTraceDistance);
		FHitResult HitResult;
		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Interactable))
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor)
			{
				IInteractableInterface* Interface = Cast<IInteractableInterface>(HitActor);
				if (Interface)
				{
					CurrentInteractableItem = HitActor;
					Interface->Interact();
					Interface->Execute_InteractBP(HitActor);
				}
			}
		}
	}
}

void ASCharacterBase::PlayReloadMontage()
{
	if(ReloadingMontage)
		PlayAnimMontage(ReloadingMontage);
}

void ASCharacterBase::PlayEquipMontage()
{
}

void ASCharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	//Apply Fall Damage
	const FVector Velocity = GetCharacterMovement()->GetLastUpdateVelocity();
	const float VelocityZ = FMath::Abs(Velocity.Z);

	double Damage = UKismetMathLibrary::NormalizeToRange(VelocityZ, 1000.0f, 1600.0f);
	Damage = FMath::Clamp(Damage, 0.0f,1.0f);

	UGameplayStatics::ApplyDamage(this, Damage * 100.0f, nullptr, this, nullptr);
}



// Called when the game starts or when spawned
void ASCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FVector ASCharacterBase::GetPawnViewLocation() const
{
	if(CameraComp)
		return CameraComp->GetComponentLocation();
	return Super::GetPawnViewLocation();
}

