// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "TPSAICharacter.generated.h"

/**
 * 
 */
class UPawnSensingComponent;
class AMapCheckPoint;

UCLASS()
class TPS_API ATPSAICharacter : public ATPSCharacter
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, Category=Sensing)
	UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(EditDefaultsOnly, Category=Sensing)
	UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY()
	AMapCheckPoint* CurrentCheckPoint;

	UPROPERTY(EditDefaultsOnly, Category=AIFire)
	float AutoFireDuration = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category=AIFire)
	float AutoFireDurationOffsetMax = 0.5f;

	FTimerHandle LoseSight;

	FTimerHandle StopFire;

	void FireImpl();

	void LoseSightOfTarget();

protected:

	virtual void SetMovementMode(int Mode) override;

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION()
	void OnActorSensed(AActor* SensedActor, FAIStimulus AIStimulus);
	
	virtual void ServerInteract_Implementation() override;

public:

	ATPSAICharacter();

	UPROPERTY(EditDefaultsOnly, Category=AIFire)
	float FireDirectionOffsetMax = 2.0f;

	virtual void OnDead_Implementation(APawn* InstigatorPawn, const FVector& Force, FName Bone) override;

	virtual void OnHealthChange_Implementation(APawn* InstigatorPawn, float Delta, float CurrentHealth, const FVector& Force, FName Bone) override;

	virtual void PostInitializeComponents() override;

	virtual void Rush() override;

	virtual void RushRelease() override;

	virtual void Interact() override;

	virtual void Aim() override;

	virtual void AimRelease() override;

	virtual void Fire() override;

	virtual void FireRelease() override;

	virtual void GetGun(AGun* NewGun) override;

	virtual void DiscardGun() override;
	
	AMapCheckPoint* GetCurrentCheckPoint() const
	{
		return CurrentCheckPoint;
	}

	void SetCurrentCheckPoint(AMapCheckPoint* CheckPoint)
	{
		CurrentCheckPoint = CheckPoint;
	}
};
