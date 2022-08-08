// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractInterface.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "Projectile.h"
#include "Gun.generated.h"

class UArrowComponent;
class ATPSCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChange, int, NewCurrentAmmo, int, NewBackupAmmo);

UCLASS()
class TPS_API AGun : public AActor, public IInteractInterface
{
	GENERATED_BODY()

private:
	
	UPROPERTY(EditAnywhere, Category=Mesh)
	UStaticMeshComponent* GunMesh;

	UPROPERTY(EditDefaultsOnly, Category=Gun)
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category=Particle)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(Replicated, EditDefaultsOnly, Category=Ammo)
	int MaxDivAmmo = 30;

	UPROPERTY(ReplicatedUsing=OnRep_AmmoChange, EditDefaultsOnly, Category=Ammo)
	int BackupAmmo = 90;

	UPROPERTY(ReplicatedUsing=OnRep_AmmoChange, EditDefaultsOnly, Category=Ammo)
	int CurrAmmo = 30;

	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	int ConsumedAmmo = 1;

	UPROPERTY(EditAnywhere, Category=Gun)
	FTransform AttachTransform;

	UPROPERTY(VisibleAnywhere, Category=Gun)
	UCameraComponent* GunCamera1P;

	UPROPERTY(EditDefaultsOnly, Category=Fire)
	int FireMode = Single;

	UPROPERTY(EditDefaultsOnly, Category=Gun)
	int ShotRange;

	UPROPERTY(EditDefaultsOnly, Category=Fire)
	float AutoFireDelta;

	UPROPERTY(EditDefaultsOnly, Category=Fire)
	UArrowComponent* FireLocation;

	UPROPERTY(EditDefaultsOnly, Category=Fire)
	FFloatCurve HipOffsetCurveX;

	UPROPERTY(EditDefaultsOnly, Category=Fire)
	FFloatCurve HipOffsetCurveY;

	UPROPERTY(EditDefaultsOnly, Category=Fire)
	FFloatCurve AimOffsetCurveX;

	UPROPERTY(EditDefaultsOnly, Category=Fire)
	FFloatCurve AimOffsetCurveY;
	
	int AutoFireCount;

public:
	// Sets default values for this actor's properties
	AGun();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ConsumeAmmo(int ConsumingAmmo);

	void AddRecoilForceToPlayer();
	
	FRotator GetAimmingDirection();
	
	void SetAmmo(int NewCurrAmmo, int NewBackupAmmo);

	UFUNCTION(Server, Reliable)
	void ServerSetAmmo(int NewCurrAmmo, int NewBackupAmmo);

	UFUNCTION(Server, Reliable)
	void ServerFire(UWorld* World, const FVector &Location, const FRotator &Direction);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastMuzzleFlash(const FVector &Location, const FRotator &Direction);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	enum EFireMode
	{
		Auto,
		Single
	};
	
	FTimerHandle AutoFireTimerHandle;

	UPROPERTY(BlueprintReadWrite, Replicated)
	FRotator OffsetDirection;

	UPROPERTY(EditDefaultsOnly, Category=Fire, BlueprintReadOnly)
	float RecoilForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=UI)
	TSubclassOf<UUserWidget> CrossHairWidgetClass;

	UPROPERTY()
	UUserWidget* CrossHairWidget;

	UFUNCTION(BlueprintNativeEvent)
	void OnPlayerFire(int AimingStatus);

	UFUNCTION(BlueprintNativeEvent)
	void OnAIFire(ATPSCharacter* TargetCharacter);

	// Only Called By Player
	UFUNCTION(BlueprintNativeEvent)
	FRotator Offset(int AimingStatus);

	UFUNCTION(BlueprintNativeEvent)
	void AddSmoothRecoilForceToPlayer();

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	UFUNCTION()
	virtual void ReloadAmmo();

	void AddAutoFireCount() { ++AutoFireCount; }

	void ResetAutoFireCount() { AutoFireCount = 0; }

	int GetAutoFireCount() const { return AutoFireCount; }

	int GetFireMode() const { return FireMode; }

	float GetAutoFireDelta() const { return AutoFireDelta; }

	UFUNCTION(BlueprintCallable)
	int GetCurrentAmmo() const { return CurrAmmo; }

	UFUNCTION(BlueprintCallable)
	int GetConsumedAmmo() const { return ConsumedAmmo; }

	UFUNCTION(BlueprintCallable)
	int GetBackupAmmo() const { return BackupAmmo; }

	int GetMaxDivAmmo() const { return MaxDivAmmo; }
	
	UStaticMeshComponent* GetGunMesh() const
	{
		return GunMesh;
	}

	FTransform GetAttachTransform() const
	{
		return AttachTransform;
	}

	UCameraComponent* GetGunCamera() const
	{
		return GunCamera1P;
	}

	UPROPERTY(BlueprintAssignable)
	FOnAmmoChange OnAmmoChange;

	UFUNCTION(Client, Reliable)
	void ClientOnAmmoChange();

	UFUNCTION()
	void OnRep_AmmoChange();
};
