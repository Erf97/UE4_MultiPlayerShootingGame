// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SphereCollision->SetupAttachment(GetRootComponent());

	ProjMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ProjMesh->SetupAttachment(SphereCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	if(GetLocalRole() == ROLE_Authority)
	{
		SphereCollision->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
	}
	
	bReplicates = true;

	DamageType = UDamageType::StaticClass();
	Damage = 20.0f;
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// void AProjectileBase::Destroyed()
// {
// 	FVector SpawnLocation = GetActorLocation();
// 	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, SpawnLocation, FRotator::ZeroRotator, true,
// 	                                         EPSCPoolMethod::AutoRelease);
// }





void AProjectileBase::OnHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}
