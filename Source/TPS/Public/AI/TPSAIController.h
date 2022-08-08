// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MultiPlayerGamePlayComponent.h"
#include "TPSAIController.generated.h"

/**
 * 
 */
class ATPSCharacter;

UCLASS()
class TPS_API ATPSAIController : public AAIController
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, Category=AI)
	UBehaviorTree* BehaviorTree;

	FName TargetLocationName = "TargetLocation";

	FName TargetCharacterName = "TargetCharacter";

	FName TargetActorName = "TargetActor";

	FName HasWeaponName = "HasWeapon";

	FName HasSightName = "HasSight";
	
	FName HasCurrAmmoName = "HasCurrAmmo";

	FName HasTotalAmmoName = "HasTotalAmmo";

public:

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;
	
	void UpdateTargetLocation(const FVector& TargetLocation);

	void UpdateTargetCharacter(AActor* Actor);

	void ResetTargetCharacter();

	void UpdateTargetActor(AActor* Actor);

	void UpdateHasWeapon(bool bHasWeapon);

	void UpdateHasSight(bool bHasSight);

	void UpdateHasCurrAmmo(bool bHasCurrAmmo);

	void UpdateHasTotalAmmo(bool bHasTotalAmmo);

	void ResetMemory();

	AActor* GetTargetActor();

	ATPSCharacter* GetTargetCharacter();

	bool GetHasSight();

	int GetTeam() const;

	void SetTeam(const int _Team);
};
