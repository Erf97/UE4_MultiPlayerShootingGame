// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "GunBase.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "TPSCharacterBase.generated.h"

UCLASS()
class ASSIGNMENT2_API ATPSCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSCharacterBase();

	UPROPERTY(BlueprintReadWrite, Category=Gun, Replicated)
	AGunBase* Gun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AProjectileBase> GrenadeClass;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UCameraComponent* Camera;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=GotKilled)
	// bool bIsDead;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_CurrentHealth)
	int CurrHealth;

	UPROPERTY(VisibleAnywhere)
	int MaxHealth;

	UPROPERTY(VisibleAnywhere, Replicated)
	int Kills;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bIsAlive;

	// UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Dead)
	// bool bIsDead;

	UFUNCTION()
	void OnRep_CurrentHealth();

	void OnHealthUpdate();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void SetGun(AGunBase* GunActor);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void DiscardGun();

	UFUNCTION(BlueprintCallable)
	FRotator GetAimmingDirection();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Throw(const FVector Location, const FRotator Direction);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void StartFire(FRotator Direction, FVector Location);

	// UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	// void GotKilled();

	/** 最大生命值的取值函数。*/
	UFUNCTION(BlueprintPure, Category="Health")
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	/** 当前生命值的取值函数。*/
	UFUNCTION(BlueprintPure, Category="Health")
	FORCEINLINE float GetCurrentHealth() const { return CurrHealth; }

	UFUNCTION(BlueprintPure, Category="Health")
	FORCEINLINE bool GetIsAlive() const { return bIsAlive; }

	UFUNCTION(BlueprintCallable, Category="Health")
	void SetCurrentHealth(float Health);

	// UFUNCTION(BlueprintCallable, Category = "Health")
	// float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator,
	//                  AActor* DamageCauser) override;

	// UFUNCTION(BlueprintCallable, Category = "Health")
	// float InternalTakePointDamage(float Damage, FPointDamageEvent const& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
