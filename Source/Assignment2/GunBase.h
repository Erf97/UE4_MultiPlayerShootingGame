// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Components/ArrowComponent.h"
#include "Components/AudioComponent.h"
#include "GunBase.generated.h"

UCLASS()
class ASSIGNMENT2_API AGunBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGunBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category=Mesh)
	USkeletalMeshComponent* GunMesh;

	UPROPERTY(EditAnywhere, Category=Projectile)
	TSubclassOf<class AProjectileBase> ProjectileClass;

	UPROPERTY(EditAnywhere, Category=Mesh)
	FTransform GunTrans;
	// 0, 8, -3
	// 0, 0, 200
	// 1, 1, 1
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Mesh)
	USceneComponent* MuzzleLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UArrowComponent* GunArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UAudioComponent* FireAudio;
	
	UPROPERTY(EditAnywhere, Category=Mesh)
	FVector RelativeLocationVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ACharacter* GunOwner;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int CurrAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int SpareAmmo;

	UPROPERTY(BlueprintReadWrite, Replicated)
	int AutoFireCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAuto;

	UPROPERTY(BlueprintReadWrite, Replicated)
	FRotator ShootingOffset;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void Fire(FRotator Direction, FVector Location);

	UFUNCTION(BlueprintCallable)
	void ConsumeAmmo();

	UFUNCTION(BlueprintCallable)
	void ReloadAmmo();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CalcShootingOffset();
};
