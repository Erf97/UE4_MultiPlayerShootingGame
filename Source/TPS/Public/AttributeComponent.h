// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

class ATPSCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDead, APawn*, Instigator, const FVector&, Force, FName, Bone=NAME_None);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnHealthChange, APawn*, Instigator, float, Delta, float, CurrentHealth, const FVector&, Force, FName, Bone=NAME_None);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()
private:

	UPROPERTY(Replicated, EditAnywhere, Category=Health)
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, Category=Health)
	float HealthMax = 100.0f;

	UPROPERTY(EditAnywhere, Category=Health)
	bool bIsInvincible = false;
	
public:	
	// Sets default values for this component's properties
	UAttributeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool CheckDead(float Delta) const { return !bIsDead; }

	bool CheckInVincible(float Delta) const;

	bool CheckTeamDamage(APawn* InstigatorPawn, float Delta) const;

	bool CanApplyHealthChange(APawn* InstigatorPawn, float Delta) const;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Server, Reliable)
	void ApplyHealthChange(float Delta, APawn* InstigatorPawn, const FVector& Force, FName Bone=NAME_None); // return if the character is dead;

	UPROPERTY(BlueprintAssignable)
	FOnDead OnDead;

	UPROPERTY(BlueprintAssignable)
	FOnHealthChange OnHealthChange;

	bool bIsDead = false;

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const { return HealthMax; }

	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth() const { return Health; }
	
	void OnActorDead(APawn* InstigatorPawn, const FVector& Force, FName Bone=NAME_None);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnActorDead(APawn* InstigatorPawn, const FVector& Force, FName Bone=NAME_None);
	
	void OnActorHealthChange(APawn* InstigatorPawn, float Delta, float CurrentHealth, const FVector& Force, FName Bone=NAME_None);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnActorHealthChange(APawn* InstigatorPawn, float Delta, float CurrentHealth, const FVector& Force, FName Bone=NAME_None);
	
};
