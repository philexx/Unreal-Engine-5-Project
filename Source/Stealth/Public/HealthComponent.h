// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHelathChangeSignature, class UHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*,DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEALTH_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	//Checks if the killed Actor is an Actor for the mission
	void CheckMissionKill(AActor* DamageCauser, AActor* DamagedActor) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	uint8 TeamNum;	
	
	UPROPERTY(BlueprintReadWrite, Category=HealthComponent)
	float fHealth;

	UPROPERTY(EditDefaultsOnly, Category=HealthComponent)
	float DefaultHealth;
	
	UPROPERTY(BlueprintReadWrite, Category=HealthComponent)
	bool bDead;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHelathChangeSignature OnHealthChanged;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE bool IsDead() const { return bDead; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsFriendly(AActor* ActorA, AActor* ActorB);
	
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	FORCEINLINE float GetHealth() const { return fHealth; }
};
