// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerCharacter.h"
#include <Weapontype.h>
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InventoryComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ClimbingComponent.h"
#include "Door.h"
#include "HealthComponent.h"
#include "MeleeWeapon.h"
#include "MissionInteractionBase.h"
#include "WeaponBase.h"
#include "WeaponInventory.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Stealth/Stealth.h"


// Sets default values
ASPlayerCharacter::ASPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 350.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 200.0f;
	
	DashEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara Comp"));
	DashEffectComponent->SetupAttachment(GetMesh());
	DashEffectComponent->bAutoActivate = false;
	
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	WeaponInventory = CreateDefaultSubobject<UWeaponInventory>(TEXT("Weapon Inventory"));

	ClimbingComponent = CreateDefaultSubobject<UClimbingComponent>(TEXT("Climbing Component"));
	
	fMouseSensitivity = 0.9f;
	
	
	fDashStrength = 15.0f;
	DashCooldownTime = 20.0f;
	bCanDash = true;

	ZoomInterpSpeed = 20.0f;
	ZoomedInFOV = 70.0f;

	bIsAiming = false;
	bIsFocused = false;
	
	TakeWidgetInstance = nullptr;
}

// Called when the game starts or when spawned
void ASPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	DefaultFOV = CameraComp->FieldOfView;

	if (WeaponWheel)
	{
		WeaponWheel->AddToViewport();
		//WeaponWheel->SetVisibility(ESlateVisibility::Hidden);
	}
	
	CreateTakeItemWidget();

	if(PlayerController == nullptr)
		PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
}

// Called every frame
void ASPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	const float TargetFOV = (bIsAiming) ? ZoomedInFOV : DefaultFOV;

	const float NextFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NextFOV);

	StartFocusCharacter(DeltaTime);
	
}


void ASPlayerCharacter::ReloadWeapon()
{
	AWeaponBase* Weapon = WeaponInventory->GetCurrentWeapon();
	if(Weapon)
	{
		Weapon->StartReload();
	}
}

void ASPlayerCharacter::PickupWeapon()
{
	if(WeaponInventory)
	{
		WeaponInventory->PickupWeapon();
		SetTakeWidgetVisibility(ESlateVisibility::Hidden);
	}
}

void ASPlayerCharacter::GetFocusedCharacter()
{
	FHitResult HitResult;
	const FVector TraceStart = CameraComp->GetComponentLocation();
	const FVector TraceEnd = TraceStart + CameraComp->GetForwardVector() * 2000.0f;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	if(GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WeaponTrace, QueryParams))
	{
		ACharacter* OtherCharacter = Cast<ACharacter>(HitResult.GetActor());
		if(OtherCharacter)
		{
			if(!IsFocusedCharacterDead(OtherCharacter))
			{
				bIsFocused = true;
				GetCharacterMovement()->bOrientRotationToMovement = false;
			}
			FocusedCharacter = OtherCharacter;
		}
	}
}

FRotator ASPlayerCharacter::GetControlRotationFocused()
{
	if(FocusedCharacter)
	{
		const FVector SocketLocation = FocusedCharacter->GetMesh()->GetSocketLocation(FName("FocusSocket"));
		const FVector CameraLocation = CameraComp->GetComponentLocation();
		return UKismetMathLibrary::FindLookAtRotation(CameraLocation, SocketLocation);
	}
	return  FRotator::ZeroRotator;
}

void ASPlayerCharacter::StartFocusCharacter(float DeltaTime)
{
	if(bIsFocused)
	{
		if(PlayerController)
		{
			const FRotator NewControlRotation = FMath::RInterpTo(PlayerController->GetControlRotation(),  GetControlRotationFocused(), DeltaTime, 50.0f );
			
			if(NewControlRotation != FRotator::ZeroRotator)
			{
				PlayerController->SetControlRotation(NewControlRotation.GetNormalized());
				SetActorRotation(FRotator(0.0f, NewControlRotation.Yaw, 0.0f) + FRotator(0.0f, 35.0f, 0.0f));
			}
		}
	}
}


void ASPlayerCharacter::Punch()
{
	if(LeftHandPunchMontage == nullptr || RightHandPunchMontage == nullptr)
		return;

	
	UAnimMontage* MontageToPlay = RightHandPunchMontage;
	if(LeftHandPunch)
	{
		MontageToPlay = LeftHandPunchMontage;
	}

	if(MontageToPlay)
	{
		PlayAnimMontage(MontageToPlay);
		LeftHandPunch = !LeftHandPunch;
	}

	// Clear Timer if Active
	if(GetWorldTimerManager().IsTimerActive(PunchReset_TimerHandle))
		GetWorldTimerManager().ClearTimer(PunchReset_TimerHandle);
	
	GetWorldTimerManager().SetTimer(PunchReset_TimerHandle, this, &ASPlayerCharacter::ResetPunching, 0.5f, false);
	
}

void ASPlayerCharacter::ResetPunching()
{
	LeftHandPunch = false;
	GetWorldTimerManager().ClearTimer(PunchReset_TimerHandle);
}

void ASPlayerCharacter::TakeDownEnemy()
{
	if(!FocusedCharacter || IsFocusedCharacterDead(FocusedCharacter))
		return;
	
	if(WeaponInventory->GetCurrentWeapon() && WeaponInventory->GetCurrentWeapon()->GetWeaponType() == EWeapontype::MELEE)
	{
		AMeleeWeapon* Melee = Cast<AMeleeWeapon>(WeaponInventory->GetCurrentWeapon());
		if(Melee)
		{
			Melee->TakeDown(this,FocusedCharacter);
			FCollisionResponseContainer CollisionResponseContainer;
			CollisionResponseContainer.SetResponse(ECC_Pawn, ECollisionResponse::ECR_Ignore);
			FocusedCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	
}

bool ASPlayerCharacter::IsFocusedCharacterDead(ACharacter* FocusedACharacter) const
{
	if(FocusedACharacter)
	{
		UHealthComponent* FocusedCharacterHealth = Cast<UHealthComponent>(FocusedACharacter->GetComponentByClass(UHealthComponent::StaticClass()));
		if(FocusedCharacterHealth)
		{
			return FocusedCharacterHealth->IsDead();
		}
	}
	return false;
}

void ASPlayerCharacter::MoveForward(float value)
{
	if (value != 0 && !ClimbingComponent->IsClimbing())
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FVector Direction = FRotationMatrix(FRotator(0.0f, Rotation.Yaw, 0.0f)).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, value);
	}
}

void ASPlayerCharacter::MoveRight(float value)
{
	if (value != 0 && !ClimbingComponent->IsClimbing())
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FVector Direction = FRotationMatrix(FRotator(0.0f, Rotation.Yaw, 0.0f)).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, value);
	}
}

void ASPlayerCharacter::LookUp(float value)
{
	AddControllerYawInput(fMouseSensitivity * value);
}

void ASPlayerCharacter::LookRight(float value)
{
	AddControllerPitchInput(fMouseSensitivity * value);
}

void ASPlayerCharacter::PauseGame()
{
	if(PauseWidget)
		if(PauseWidgetInstance == nullptr)
		{
			PauseWidgetInstance = CreateWidget(GetWorld(), PauseWidget);
		}

	if(PauseWidgetInstance)
	{
		PauseWidgetInstance->AddToViewport(0);
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController, PauseWidgetInstance);
	}
	PlayerController->bShowMouseCursor = true;
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void ASPlayerCharacter::UnPauseGame()
{
	if(PauseWidgetInstance)
	{
		PauseWidgetInstance->RemoveFromParent();
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
	}
	PlayerController->bShowMouseCursor = false;
	UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void ASPlayerCharacter::InteractMissionActor()
{
	if(CurrentMissionInterActor)
	{
		if(!CurrentMissionInterActor->wasInteractedWith())
			CurrentMissionInterActor->Interact(this);
	}
}

void ASPlayerCharacter::PauseMissionInteraction()
{
	if(CurrentMissionInterActor)
	{
		if(!CurrentMissionInterActor->wasInteractedWith())
			CurrentMissionInterActor->PauseInteraction();
	}
}

void ASPlayerCharacter::StartRunning()
{
	if (!bIsAiming)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600;
	}
}

void ASPlayerCharacter::StopRunning()
{
	if (!bIsAiming)
	{
		GetCharacterMovement()->MaxWalkSpeed = 350;
	}
}

void ASPlayerCharacter::StartJump()
{
	ClimbingComponent->JumpOnCurb();
	Jump();
}

void ASPlayerCharacter::Dash()
{
	if(ClimbingComponent->IsClimbing())
		return;
	
	if (DashEffectComponent)
	{
		UCharacterMovementComponent* MovementComp = GetCharacterMovement();
		if(!MovementComp || MovementComp->IsFalling() || !bCanDash)
			return;

		if(DashSound)
			UGameplayStatics::PlaySound2D(GetWorld(), DashSound);

		bCanDash = false;
		DashEffectComponent->Activate(true);
		GetMesh()->SetHiddenInGame(true);
		const FVector Vel = MovementComp->Velocity;
		const FVector DashDirection = FVector(Vel.X * fDashStrength,
		                                      Vel.Y * fDashStrength,
		                                      0.0f);
		LaunchCharacter(DashDirection, false, false);
		MovementComp->bOrientRotationToMovement = true;
		
		GetWorldTimerManager().SetTimer(DashEffect_TimerHandle, this, &ASPlayerCharacter::DeactivateDashEffect, 0.7f);
		GetWorldTimerManager().SetTimer(DashCooldown_TimerHandle, this, &ASPlayerCharacter::DashCooldown, DashCooldownTime);
	}
}

void ASPlayerCharacter::DeactivateDashEffect()
{
	if (DashEffectComponent)
	{
		GetMesh()->SetHiddenInGame(false);
		DashEffectComponent->Deactivate();
		GetWorldTimerManager().ClearTimer(DashEffect_TimerHandle);
	}
}

void ASPlayerCharacter::DashCooldown()
{
	bCanDash = true;
	GetWorld()->GetTimerManager().ClearTimer(DashCooldown_TimerHandle);
}


void ASPlayerCharacter::KickInDoor()
{
	if(bIsAiming)
		bIsAiming = false;
	
	if(CurrentDoor)
	{
		CurrentDoor->StartKickDoor(this);
	}
}

void ASPlayerCharacter::StartShooting()
{
	if (WeaponInventory)
	{
		if(HasWeaponInHand())
		{
			if (!WeaponInventory->CanShoot)
				return;

			AWeaponBase* Wpn = WeaponInventory->GetCurrentWeapon();
			if(Wpn)
			{
				Wpn->StartAttack();
			}
		}
		else
		{
			Punch();
		}
	}
}

void ASPlayerCharacter::StopShooting()
{
	if (WeaponInventory)
	{
		AWeaponBase* Wpn = WeaponInventory->GetCurrentWeapon();
		if(Wpn)
		{
			Wpn->StopAttack();
		}
	}
}

void ASPlayerCharacter::StartAim()
{
	if(ClimbingComponent->IsClimbing())
		return;

	bIsAiming = true;
	AWeaponBase* CurrentWeapon =  WeaponInventory->GetCurrentWeapon();
	if(CurrentWeapon && CurrentWeapon->GetWeaponType() != EWeapontype::MELEE)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->MaxWalkSpeed = 250.0f;
	}
	else if( !CurrentWeapon || (CurrentWeapon && CurrentWeapon->GetWeaponType() == EWeapontype::MELEE) )
	{
		GetFocusedCharacter();
	}
}

void ASPlayerCharacter::StopAim()
{
	if(ClimbingComponent->IsClimbing())
		return;
	
	bIsAiming = false;
	bIsFocused = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxWalkSpeed = 350.0f;
	if(FocusedCharacter)
	{
		FocusedCharacter = nullptr;
	}
}

void ASPlayerCharacter::ClimbLedgeRight(float Value)
{
	ClimbingComponent->ClimbRight(Value, GetActorRightVector());
}

void ASPlayerCharacter::FindLedgeUp(float Value)
{
	ClimbingComponent->FindUpHangPoint(Value, GetInputAxisValue("Climb Right"));
}

void ASPlayerCharacter::JumpToNextHangPoint()
{
	ClimbingComponent->JumpToNextHangPoint();
}

void ASPlayerCharacter::StopJumpNextHangPoint()
{
	ClimbingComponent->JumpToNextReleased();
}

void ASPlayerCharacter::StopClimbing()
{
	if(ClimbingComponent)
	{
		ClimbingComponent->StopClimbing();
	}
}

void ASPlayerCharacter::CreateTakeItemWidget()
{
	if(!TakeWidgetBP)
		return;
	
	if(TakeWidgetInstance == nullptr)
	{
		TakeWidgetInstance = CreateWidget(GetWorld(), TakeWidgetBP);
		if(TakeWidgetInstance)
		{
			TakeWidgetInstance->AddToViewport();
			TakeWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}


void ASPlayerCharacter::SetTakeWidgetVisibility(ESlateVisibility Visibility)
{
	if(TakeWidgetInstance)
	{
		TakeWidgetInstance->SetVisibility(Visibility);
	}
}

void ASPlayerCharacter::Vault()
{
	ClimbingComponent->Vault();
}


// Called to bind functionality to input
void ASPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Move Forward", this, &ASPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right", this, &ASPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Look Up", this, &ASPlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis("Look Right", this, &ASPlayerCharacter::LookRight);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASPlayerCharacter::StartRunning);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASPlayerCharacter::StopRunning);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASPlayerCharacter::StartJump);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ASPlayerCharacter::Dash);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASPlayerCharacter::Interact);

	PlayerInputComponent->BindAction("Vault", IE_Pressed, this, &ASPlayerCharacter::Vault);

	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &ASPlayerCharacter::StartShooting);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &ASPlayerCharacter::StopShooting);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ASPlayerCharacter::StartAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ASPlayerCharacter::StopAim);

	PlayerInputComponent->BindAction("Kick In Door", IE_Pressed, this, &ASPlayerCharacter::KickInDoor);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASPlayerCharacter::ReloadWeapon);
	PlayerInputComponent->BindAction("PickupWeapon", IE_Pressed, this, &ASPlayerCharacter::PickupWeapon);

	PlayerInputComponent->BindAction("Take Down", IE_Pressed, this, &ASPlayerCharacter::TakeDownEnemy);

	PlayerInputComponent->BindAxis("Climb Right", this, &ASPlayerCharacter::ClimbLedgeRight);
	PlayerInputComponent->BindAxis("Find Ledge Up", this, &ASPlayerCharacter::FindLedgeUp);
	PlayerInputComponent->BindAction("Jump Next HP", IE_Pressed, this, &ASPlayerCharacter::JumpToNextHangPoint);
	PlayerInputComponent->BindAction("Stop Climbing", IE_Pressed, this, &ASPlayerCharacter::StopClimbing);

	PlayerInputComponent->BindAction("Take Cover", IE_Pressed, this, &ASCharacterBase::TakeCover);

	PlayerInputComponent->BindAction("Pause Game", IE_Pressed, this, &ASPlayerCharacter::PauseGame);

	PlayerInputComponent->BindAction("Interact Msn", IE_Pressed, this, &ASPlayerCharacter::InteractMissionActor);
	PlayerInputComponent->BindAction("Pause Msn Interaction", IE_Pressed, this, &ASPlayerCharacter::PauseMissionInteraction);
	//PlayerInputComponent->BindAction("Search For Nearby Ledge", IE_Released, this, &ASPlayerCharacter::StopJumpNextHangPoint);
	
}

bool ASPlayerCharacter::HasWeaponInHand()
{
	if(WeaponInventory->GetCurrentWeapon() != nullptr)
		return true;

	return false;
}
