// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeWeapon.h"
#include <Weapontype.h>
#include "SPlayerCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"



AMeleeWeapon::AMeleeWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	WeaponType = EWeapontype::MELEE;

	fAttackDistance = 250.0f;
}

void AMeleeWeapon::StartAttack()
{
	
}

void AMeleeWeapon::TakeDown(ASPlayerCharacter* Player,ACharacter* FocusedACharacter)
{
	FocusedCharacter = FocusedACharacter;
	//Pointer to Player and FocusedACharacter being checked in CheckAttackRange()
	const bool IsInRange = CheckAttackRange(Player, FocusedCharacter, fAttackDistance);
	if(IsInRange)
	{
		if(KnifeTakeDownMontage && KnifeTakeDownEnemyMontage)
		{
			Player->GetCharacterMovement()->DisableMovement();
			Player->SetIsAiming(false);
			Player->SetIsFocused(false);
			Player->TakeDownMotionWarp();
			const float MontageTime = FocusedCharacter->PlayAnimMontage(KnifeTakeDownEnemyMontage);
			GetWorld()->GetTimerManager().SetTimer(KnifeTakeDownEnd_TimerHandle, this, &AMeleeWeapon::OnTakeDownFinished, MontageTime);
		}
	}
}

void AMeleeWeapon::OnTakeDownFinished()
{
	if(FocusedCharacter)
	{
		ACharacter* WeaponOwner = Cast<ACharacter>(GetOwner());
		if(WeaponOwner)
			WeaponOwner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		UGameplayStatics::ApplyDamage(Cast<AActor>(FocusedCharacter), 150.0f, nullptr, GetOwner(), nullptr);
		FocusedCharacter->GetMesh()->SetSimulatePhysics(true);
		FocusedCharacter->GetCharacterMovement()->DisableMovement();
		GetWorld()->GetTimerManager().ClearTimer(KnifeTakeDownEnd_TimerHandle);
		FocusedCharacter = nullptr;
	}
}

bool AMeleeWeapon::CheckAttackRange(float Distance)
{
	return CheckAttackRange(Cast<ACharacter>(GetOwner()), FocusedCharacter, Distance);
}

bool AMeleeWeapon::CheckAttackRange(ACharacter* Player, ACharacter* FocusedACharacter, float Distance)
{
	if(Player && FocusedACharacter)
	{
		const FVector Dist = Player->GetActorLocation() - FocusedCharacter->GetActorLocation();
		return  Dist.Length() <= Distance;
	}
	return false;
}



void AMeleeWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}



