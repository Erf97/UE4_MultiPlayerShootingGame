// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// #include "TPSCharacterBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "ProjectileBase.generated.h"

UCLASS()
class ASSIGNMENT2_API AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	USphereComponent* SphereCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* ProjMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AActor* Gun;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ACharacter* GrenadeOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	class UParticleSystem* ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UDamageType> DamageType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage")
	float Damage;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// virtual void Destroyed() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent)
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	           FVector NormalImpulse,
	           const FHitResult& Hit);
};