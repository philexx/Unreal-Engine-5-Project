// Fill out your copyright notice in the Description page of Project Settings.


// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Perception/AIPerceptionComponent.h"


// Sets default values
AAICharacter::AAICharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bHasWeaponInHand = false;
	
	
}

void AAICharacter::StartShooting()
{
	if(CurrentWeapon)
	{
		CurrentWeapon->StartAttack();	
	}
}

void AAICharacter::StopShooting()
{
	if(CurrentWeapon)
	{
		CurrentWeapon->StopAttack();
	}
}

void AAICharacter::ReloadWeapon()
{
	if(CurrentWeapon)
	{
		CurrentWeapon->Reload();
	}
}

void AAICharacter::SpawnWeapon()
{
	if(DefaultWeapon)
	{
		CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(DefaultWeapon, GetActorLocation(), GetActorRotation());
		if(CurrentWeapon)
		{
			bHasWeaponInHand = true;
			CurrentWeapon->AttachToComponent(GetMesh(),  FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GripPoint"));
			CurrentWeapon->SetIsActive(true);
			CurrentWeapon->SetOwner(this);
		}
	}
}


// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();
	SpawnWeapon();
}

// Called every frame
void AAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FVector AAICharacter::GetPawnViewLocation() const
{
	if(CameraComp)
			return CameraComp->GetComponentLocation();
	return Super::GetPawnViewLocation();
}



