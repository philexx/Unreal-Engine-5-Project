// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAmmoChanged, int32, CurrentAmmo, int32, AmmoInMag);

UCLASS()
class STEALTH_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	virtual void StartAttack();

	virtual void StopAttack();

	virtual void StartReload();
	
	virtual void Reload();

	virtual void AddRecoil();

	void SetIndex(int32 Index);
	
	void SpawnHitEffects(const FHitResult Hit);

	void DealDamage(const FVector ShotDirection, const FHitResult& HitResult);

	void CameraShake();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=WeaponSettings)
	FName WeaponName;
	
	UPROPERTY(VisibleDefaultsOnly, Category=WeaponSettings)
	FName MuzzleSocketName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=WeaponSettings)
	uint8 WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Components)
	class USkeletalMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, Category=Components)
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxComponent;

	UPROPERTY(EditDefaultsOnly, Category=Sound)
	class USoundBase* ShootingSound;

	UPROPERTY(EditDefaultsOnly, Category=Sound)
	class USoundBase* EmptyMagSound;

	//Whether the weapon should always be visible when unEqiped
	UPROPERTY(EditDefaultsOnly, Category=WeaponSettings)
	bool ShouldBeVisible;

	UPROPERTY(BlueprintAssignable)
	FAmmoChanged OnAmmoChanged;

	UPROPERTY(BlueprintReadOnly)
	int32 LoadedAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=WeaponSettings)
	int32 MaxLoadedAmmo;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=WeaponSettings)
	int32 MagazineAmmo;

	FTimerHandle TimerHandle_Reload;

	UPROPERTY(EditDefaultsOnly, Category=WeaponSettings)
	float ReloadingTime;

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	bool IsReloading;

	UPROPERTY(EditDefaultsOnly, Category=WeaponSettings, meta=(ClampMin= 0.0))
	float BulletSpread;

	UPROPERTY(EditDefaultsOnly, Category=WeaponSettings)
	float RecoilMin;

	UPROPERTY(EditDefaultsOnly, Category=WeaponSettings)
	float ShotDistance;
	
	UPROPERTY(EditDefaultsOnly, Category=WeaponSettings)
	float RecoilMax;

	UPROPERTY(EditDefaultsOnly, Category=WeaponSettings)
	float BaseDamage;
	
	UPROPERTY(EditDefaultsOnly, Category=WeaponSettings)
	float DamageMultiplier;
	
	UPROPERTY(EditDefaultsOnly, Category=WeaponSettings)
	class UMaterialInterface* DecalMat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Thumbnail)
	class UTexture2D* Thumbnail;

	UPROPERTY(EditDefaultsOnly, Category=Effects)
	UParticleSystem* DefaultHitEffect;

	UPROPERTY(EditDefaultsOnly, Category=Effects)
	UParticleSystem* FleshHitEffect;
	
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	UParticleSystem* VulnerableHitEffect;

	UPROPERTY(EditDefaultsOnly, Category=Effects)
	TSubclassOf<class UCameraShakeBase> CameraShakeBase;

	UPROPERTY(BlueprintReadOnly)
	UParticleSystem* SelectedEffect;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY()
	FVector TracerEndPoint;

	UPROPERTY(EditDefaultsOnly, Category=WeaponSettings)
	TSubclassOf<class AProjectile> Projectile;
	
	int32 WeaponIndex;

	bool IsActive;

	bool FirstShot;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE bool AlwaysVisible() const { return ShouldBeVisible; }

	FORCEINLINE FName GetWeaponName() const { return WeaponName; }

	FORCEINLINE int32 GetWeaponType() const { return WeaponType; }

	FORCEINLINE UTexture2D* WeaponThumbnail() const { return Thumbnail; }

	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return MeshComponent; }

	FORCEINLINE void SetIsActive(bool Active) { IsActive = Active; }

	FORCEINLINE int32 GetIndex() const {return WeaponIndex;}

	FORCEINLINE int32 GetLoadedAmmo() const { return LoadedAmmo;}

	FORCEINLINE int32 GetAmmoInMag() const { return MagazineAmmo; }
 
	virtual void UpdateAmmo(int32 CurrentAmmo, int32 AmmoInMag );

	virtual void PlayEffects();

	virtual void PlaySound();

	virtual void SpawnTracer();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};







