// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleEmitter.h"
#include "Projectile.generated.h"

class UMovementComponent;
class UProjectileMovementComponent;

UCLASS()
class TPS_API AProjectile : public AActor
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, Category=Mesh)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere, Category=Movement)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, Category=Particle)
	UParticleSystem* ExplodeEffect;
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	UPROPERTY(EditDefaultsOnly, Category=Damage)
	float HeadShotDamage;

	UPROPERTY(EditDefaultsOnly, Category=Damage)
	float BodyDamage;

	UPROPERTY(EditDefaultsOnly, Category=Damage)
	float LimbDamage;
	
	UPROPERTY(EditDefaultsOnly, Category=Damage)
	float ForceCoefficient = 10;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void Destroyed() override;
	
	
};
