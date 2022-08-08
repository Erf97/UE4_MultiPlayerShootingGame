// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "AttributeComponent.h"
#include "TPSCharacter.h"
#include "TPSPlayerCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
                            FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// TODO: Apply damage
	// GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Hit!");
	if (Other && Other != GetInstigator() && HasAuthority())
	{
		UAttributeComponent* Attributes = Cast<UAttributeComponent>(
			Other->GetComponentByClass(UAttributeComponent::StaticClass()));
		if (Attributes)
		{
			float Damage = BodyDamage; // TODO: get damage by bone
			ATPSCharacter* Character = Cast<ATPSCharacter>(Other);
			FVector Force = GetActorForwardVector() * Damage * ForceCoefficient;
			if (Character && Character->GetMesh()->IsSimulatingPhysics(Hit.BoneName))
			{
				Character->GetMesh()->AddImpulse(Force, Hit.BoneName, true);
			}
			// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Bone 1 " + Hit.BoneName.ToString());
			Attributes->ApplyHealthChange(-Damage, GetInstigator(), Force, Hit.BoneName);
			// bool bIsDead = false;
			// bool bIsDamaged = Attributes->ApplyHealthChange(-Damage, bIsDead, GetInstigator(), Force, Hit.BoneName);
			//
			// ATPSPlayerCharacter* ThisCharacter = Cast<ATPSPlayerCharacter>(GetInstigator());
			// if (ThisCharacter && bIsDamaged)
			// {
			// 	ThisCharacter->OnHitOtherCharacter.Broadcast(bIsDead);
			// }
		}
	}
	Destroy();
}

void AProjectile::Destroyed()
{
	Super::Destroyed();
	if (ExplodeEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplodeEffect, GetActorLocation(), GetActorRotation());
	}
}
