// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSCharacter.h"
#include "TPSPlayerCharacter.generated.h"

/**
 * 
 */

class ATPSPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFireChanged, int, AimState, bool, bIsStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetGun, AGun*, NewGun);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiscardGun, AGun*, OldGun);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitOtherCharacter, bool, bIsOtherCharacterKilled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAimStateChanged, int, AimState);


UCLASS()
class TPS_API ATPSPlayerCharacter : public ATPSCharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category=Camera)
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, Category=Camera)
	UCameraComponent* Camera;

	bool bDontAim = false;

	void AimHoldImpl();

	UFUNCTION(Server, Reliable)
	void ServerAimHoldImpl();

	void AimTapImpl();

	UFUNCTION(Server, Reliable)
	void ServerAimTapImpl();

	void AimHoldReleaseImpl();

	UFUNCTION(Server, Reliable)
	void ServerAimHoldReleaseImpl();

	void AimTapReleaseImpl();

	UFUNCTION(Server, Reliable)
	void ServerAimTapReleaseImpl();

	void Enter8DirMode();

	void Exit8DirMode();

	void FireImpl();

	void AimModeChangeBroadcast();

	void FireChangeBroadCast(bool BIsStart);

	int GetAimState();

	UFUNCTION(Server, Reliable)
	void ServerTPSCrouch();

	UFUNCTION(Server, Reliable)
	void ServerCrouchRelease();

	UPROPERTY()
	UUserWidget* CrossHairWidget;

protected:

	virtual void SetMovementMode(int Mode) override;

	UFUNCTION(Server, Reliable)
	void ServerSetMovementMode(int Mode);

	virtual void OnRep_SetGunStatus() override;

public:
	
	virtual void Interact() override;

	virtual void ReloadAmmo() override;

	virtual void Aim() override;

	virtual void AimRelease() override;

	virtual void Fire() override;

	virtual void FireRelease() override;
	
	virtual void Rush() override;

	virtual void RushRelease() override;

	virtual void TPSCrouch() override;

	virtual void CrouchRelease() override;

public:

	ATPSPlayerCharacter();
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnDead_Implementation(APawn* InstigatorPawn, const FVector& Force, FName Bone) override;

	virtual void GetGun(AGun* NewGun) override;

	virtual void DiscardGun() override;

	UFUNCTION(Client, Reliable)
	void ClientOnGetGun(AGun* NewGun);
	
	UFUNCTION(Client, Reliable)
	void ClientOnDiscardGun(AGun* OldGun);

	UFUNCTION(Client, Reliable)
	void ClientOnHitOtherCharacter(bool bIsOtherCharacterKilled);

	ATPSPlayerController* GetPlayerController() const; 

	UPROPERTY(BlueprintAssignable)
	FOnFireChanged OnFireChanged;

	UPROPERTY(BlueprintAssignable)
	FOnHitOtherCharacter OnHitOtherCharacter;

	UPROPERTY(BlueprintAssignable)
	FOnAimStateChanged OnAimStateChanged;

	UPROPERTY(BlueprintAssignable)
	FOnGetGun OnGetGun;

	UPROPERTY(BlueprintAssignable)
	FOnDiscardGun OnDiscardGun;
	
	UCameraComponent* GetCamera() const
	{
		return Camera;
	}

};
