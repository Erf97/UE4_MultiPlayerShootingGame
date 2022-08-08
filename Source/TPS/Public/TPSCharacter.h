// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeComponent.h"
#include "AI/TPSAIController.h"

#include "GameFramework/Character.h"
#include "TPSCharacter.generated.h"

class AGun;
class USpringArmComponent;
class UCameraComponent;
class UInteractComponent;
class UAnimStatusComponent;


UCLASS()
class TPS_API ATPSCharacter : public ACharacter
{
	GENERATED_BODY()
private:
public:
	// Sets default values for this character's properties
	ATPSCharacter();

	UFUNCTION(BlueprintNativeEvent)
	void OnDead(APawn* InstigatorPawn, const FVector& Force, FName Bone);

	UFUNCTION(BlueprintNativeEvent)
	void OnHealthChange(APawn* InstigatorPawn, float Delta, float CurrentHealth, const FVector& Force, FName Bone);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category=Mesh, Replicated)
	UStaticMeshComponent* GunMesh;

	UPROPERTY(VisibleAnywhere, Category=Interact)
	UInteractComponent* InteractComponent;

	UPROPERTY(EditAnywhere, Category=Gun)
	FName GunGridSocketName;

	UPROPERTY(VisibleAnywhere, Category=Anim)
	UAnimStatusComponent* AnimStatus;

	UPROPERTY(ReplicatedUsing=OnRep_SetGunStatus)
	AGun* Gun;

	UPROPERTY(EditDefaultsOnly, Category=Attribute)
	UAttributeComponent* Attributes;

	UPROPERTY(Replicated)
	bool bIsAlive;

	UPROPERTY(Replicated)
	bool bHasWeapon;

	UPROPERTY()
	TArray<ATPSAIController*> SensedAIs;

	UPROPERTY(EditDefaultsOnly, Category=GamePlay)
	float PendindDestoryTime = 4.5;

	FTimerHandle PendingDestoryHandle;

	virtual void SetMovementMode(int Mode)
	{
	}

	UFUNCTION(Server, Unreliable)
	virtual void ServerInteract();

	UFUNCTION(Server, Unreliable)
	virtual void ServerGetGun(AGun* NewGun);

	UFUNCTION(Server, Unreliable)
	virtual void ServerDiscardGun();

	UFUNCTION()
	virtual void OnRep_SetGunStatus();

	UFUNCTION()
	void OnMontageEnd(UAnimMontage* InMontage, bool bInterrupted);
	
	void PendingDestory();


public:
	// 角色操作输入相关
	void MoveForward(const float Value);

	void MoveRight(const float Value);

	virtual void ReloadAmmo();

	virtual void Interact();

	virtual void Aim();

	virtual void AimRelease();

	virtual void Fire();

	virtual void FireRelease();

	virtual void Rush();

	virtual void RushRelease();

	virtual void TPSCrouch();

	virtual void CrouchRelease();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static enum EMovementMode
	{
		Walking,
		Jogging,
		Rushing
	};

	virtual void GetGun(AGun* NewGun);

	void DestroyThis() { Destroy(); }
	
	// UFUNCTION()
	virtual void DiscardGun();

	virtual float CalcDamageByBone(float Damage, FName Bone);

	UPROPERTY(EditDefaultsOnly, Category=Movement, BlueprintReadOnly)
	float WalkSpeed = 300;

	UPROPERTY(EditDefaultsOnly, Category=Movement, BlueprintReadOnly)
	float JogSpeed = 600;

	UPROPERTY(EditDefaultsOnly, Category=Movement, BlueprintReadOnly)
	float RushSpeed = 1200;

	UPROPERTY(EditAnywhere, Category=MultiPlayer)
	int Team;

	UFUNCTION(BlueprintCallable)
	UAnimStatusComponent* GetAnimStatus() const
	{
		return AnimStatus;
	}

	UFUNCTION(BlueprintCallable)
	UAttributeComponent* GetAttributes() const
	{
		return Attributes;
	}

	UFUNCTION(BlueprintCallable)
	AGun* GetGunActor() const
	{
		return Gun;
	};

	TArray<ATPSAIController*>& GetSensedAIs()
	{
		return SensedAIs;
	}
};
