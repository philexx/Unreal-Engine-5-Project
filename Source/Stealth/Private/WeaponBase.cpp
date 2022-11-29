// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "Projectile.h"
#include "SCharacterBase.h"
#include "SPlayerCharacter.h"
#include "WeaponInventory.h"
#include "WeaponType.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SlateWrapperTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystem.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Stealth/Stealth.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	MeshComponent->SetupAttachment(RootComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->SetupAttachment(MeshComponent);
	
	ShouldBeVisible = false;

	WeaponName = FName("SW-39");

	WeaponType = EWeapontype::PISTOL;
	
	MuzzleSocketName = FName("muzzleSocket");

	LoadedAmmo = 15;

	MaxLoadedAmmo = 15;

	MagazineAmmo = 120;

	FirstShot = false;
	
	ShotDistance = 20000.0f;

	BaseDamage = 10.0f;

	DamageMultiplier = 2.5f;
	
}


void AWeaponBase::StartAttack()
{
	if(IsReloading)
		return;
	
		if(LoadedAmmo <= 0)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), EmptyMagSound, MeshComponent->GetComponentLocation());
		}
		else
		{
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);
			QueryParams.AddIgnoredActor(GetOwner());
			QueryParams.bReturnPhysicalMaterial = true;
			
			if(ASCharacterBase* Player = Cast<ASCharacterBase>(GetOwner()))
			{
				FHitResult WeaponHitResult;
				FVector Eyelocation;
				FRotator EyeRotation;
				Player->GetActorEyesViewPoint(Eyelocation, EyeRotation);
				const FVector TraceStart = Eyelocation;
				FVector Direction = EyeRotation.Vector();

				const float HalfConeRad = FMath::DegreesToRadians(BulletSpread);
				Direction = FMath::VRandCone(Direction, HalfConeRad, HalfConeRad);
				
				const FVector TraceEnd = TraceStart + (Direction * ShotDistance);
				
				if(GetWorld()->LineTraceSingleByChannel(WeaponHitResult, TraceStart, TraceEnd, ECC_WeaponTrace, QueryParams))
				{
					SpawnHitEffects(WeaponHitResult);
					DealDamage(Direction, WeaponHitResult);
				}

				TracerEndPoint = WeaponHitResult.bBlockingHit ? WeaponHitResult.Location : TraceEnd;
				PlayEffects();
				PlaySound();
				LoadedAmmo--;
				UpdateAmmo(LoadedAmmo, MagazineAmmo);
			}
		}
}

void AWeaponBase::StopAttack()
{
}

void AWeaponBase::StartReload()
{
	if(LoadedAmmo == MaxLoadedAmmo)
		return;
	
	ASCharacterBase* Player = Cast<ASCharacterBase>(GetOwner());
	if(Player)
		Player->PlayReloadMontage(); 
	
	IsReloading = true;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Reload, this, &AWeaponBase::Reload, ReloadingTime);
}

void AWeaponBase::Reload()
{
	const float CurrentAmmo = LoadedAmmo;
	int32 AmmoToReload = MaxLoadedAmmo - CurrentAmmo;
	if(MagazineAmmo >= AmmoToReload)
	{
		LoadedAmmo = LoadedAmmo + AmmoToReload;
		MagazineAmmo = MagazineAmmo - AmmoToReload;
	}
	else
	{
		AmmoToReload = MagazineAmmo;
		LoadedAmmo += AmmoToReload;
		MagazineAmmo -= AmmoToReload;
	}
	UpdateAmmo(LoadedAmmo, MagazineAmmo);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Reload);
	
	IsReloading = false;
}

void AWeaponBase::AddRecoil()
{
	ACharacter* Player = Cast<ACharacter>(GetOwner());
	if(Player)
	{
		const float RecoilAmount = UKismetMathLibrary::RandomFloatInRange(RecoilMin * -1, RecoilMax * -1);
		Player->AddControllerPitchInput(RecoilAmount);
	}
}

void AWeaponBase::SetIndex(int32 Index)
{
	WeaponIndex = Index;
}

void AWeaponBase::SpawnHitEffects(const FHitResult Hit)
{
	//Determine Surface and chose effect to play
	const EPhysicalSurface SurfaceType = UGameplayStatics::GetSurfaceType(Hit); 
	switch (SurfaceType)
	{
	case Surface_FleshDefault:
		SelectedEffect = FleshHitEffect;
		break;
	case Surface_FleshVulnerable:
		SelectedEffect = VulnerableHitEffect;
		break;
	default:
		SelectedEffect = DefaultHitEffect;
		break;
	}

	//Play Effect
	if(SelectedEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
		SelectedEffect = nullptr;
	}
}

void AWeaponBase::DealDamage(const FVector ShotDirection, const FHitResult& HitResult)
{
	const EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
	const float Damage = (SurfaceType == Surface_FleshVulnerable) ? BaseDamage*= DamageMultiplier :  BaseDamage;
	
	UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), Damage, ShotDirection, HitResult, GetOwner()->GetInstigatorController(), GetOwner(), DamageType);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AWeaponBase::OnEndOverlap);
}



// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AWeaponBase::UpdateAmmo(int32 CurrentAmmo, int32 AmmoInMag)
{
	OnAmmoChanged.Broadcast(CurrentAmmo, AmmoInMag);
}

void AWeaponBase::PlayEffects()
{
}

void AWeaponBase::PlaySound()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShootingSound, MeshComponent->GetComponentLocation());
}

void AWeaponBase::SpawnTracer()
{
	if(Projectile == nullptr)
		return;
	
	TArray<AActor*>IgnoredActors;
	IgnoredActors.Add(this);
	const FVector TraceStart = MeshComponent->GetSocketLocation(MuzzleSocketName);
	FHitResult HitResult;
	FVector BulletHitLocation = TracerEndPoint;
	if(UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TracerEndPoint, TraceTypeQuery_Weapon, false, IgnoredActors, EDrawDebugTrace::None, HitResult, true))
	{
		BulletHitLocation = HitResult.Location;
	}
	
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(TraceStart, BulletHitLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<AProjectile>(Projectile,TraceStart, Rotation, SpawnParams);
}

void AWeaponBase::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASPlayerCharacter* Player = Cast<ASPlayerCharacter>(OtherActor);
	if(!IsActive && Player)
	{
		Player->GetWeaponInventory()->SetWeaponToPickup(this);
		Player->SetTakeWidgetVisibility(ESlateVisibility::Visible);
	}
	
}

void AWeaponBase::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASPlayerCharacter* Player = Cast<ASPlayerCharacter>(OtherActor);
	if(!IsActive && Player)
	{
		Player->GetWeaponInventory()->SetWeaponToPickup(nullptr);
		Player->SetTakeWidgetVisibility(ESlateVisibility::Hidden);
	}
}

void AWeaponBase::CameraShake()
{
	ACharacter* Player = Cast<ACharacter>(GetOwner());
	if(Player)
	{
		APlayerController* PC = Cast<APlayerController>(Player->GetController());
		if(PC)
		{
			PC->ClientStartCameraShake(CameraShakeBase);
		}
	}
}
