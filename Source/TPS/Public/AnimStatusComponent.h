// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimStatusComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_API UAnimStatusComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	
public:	
	// Sets default values for this component's properties
	UAnimStatusComponent();

	enum EAnimStates
	{
		Idle,
		Jogging,
		Jump,
		Crouch,
		IdleAiming,
		JogAiming,
		CrouchWalk,
		RunJump,
		Rush
	};

	int State;

	UPROPERTY(BlueprintReadOnly, Replicated, Category=Attribute)
	bool bIsAiming;

	UPROPERTY(BlueprintReadOnly, Replicated, Category=Attribute)
	bool bIsAimingHold;

	UPROPERTY(BlueprintReadOnly, Replicated, Category=Attribute)
	bool bIsAimingTap;

	UPROPERTY(BlueprintReadOnly, Replicated, Category=Attribute)
	bool bIsCrouching;

	UPROPERTY(BlueprintReadOnly, Replicated, Category=Attribute)
	bool bIsRushing;

	UPROPERTY(BlueprintReadOnly, Replicated, Category=Attribute)
	float PlayerPitch;

	UPROPERTY(BlueprintReadOnly, Replicated, Category=Attribute)
	float PlayerSpeedX;

	UPROPERTY(BlueprintReadOnly, Replicated, Category=Attribute)
	float PlayerSpeedY;

	UPROPERTY(EditAnywhere, Category=Attribute)
	float ViewChangeBlendTime;

	UPROPERTY(EditAnywhere, Category=Attribute)
	float OnDeadViewChangeBlendTime = 0.5f;

	FTimerHandle AimHoldCheckTimerHandle;

	void PlayMontage(UAnimMontage* AnimMontage);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category=Anim)
	UAnimMontage* FireAnimHip;

	UPROPERTY(EditDefaultsOnly, Category=Anim)
	UAnimMontage* FireAnimAim;

	UPROPERTY(EditDefaultsOnly, Category=Anim)
	UAnimMontage* ReloadAnim;

	bool CanToAim();

	bool CanToRush();

	UFUNCTION(Server, Unreliable)
	void ServerPlayMontage(UAnimMontage* AnimMontage);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayMontage(UAnimMontage* AnimMontage);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UAnimMontage* GetFireAnim();

	UAnimMontage* GetReloadAnim() const { return ReloadAnim; }

	bool SetAnimState(int ToState);
};
