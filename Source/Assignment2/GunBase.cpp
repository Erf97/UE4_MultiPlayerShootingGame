// Fill out your copyright notice in the Description page of Project Settings.


#include "GunBase.h"

#include "Camera/CameraComponent.h"


// Sets default values
AGunBase::AGunBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetOnlyOwnerSee(false); // otherwise won't be visible in the multiplayer
	GunMesh->bCastDynamicShadow = false;
	GunMesh->CastShadow = false;
	GunMesh->SetupAttachment(RootComponent);

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(GunMesh);
	MuzzleLocation->SetRelativeLocation(RelativeLocationVector);

	GunArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("GunArrow"));
	GunArrow->SetupAttachment(GunMesh);

	FireAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	FireAudio->SetupAttachment(GunMesh);

	ShootingOffset = FRotator(0, 0, 0);
	AutoFireCount = 0;

	bReplicates = true;
}

// Called when the game starts or when spawned
void AGunBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGunBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// void AGunBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
// 	DOREPLIFETIME(AGunBase, GunMesh);
// }

void AGunBase::Fire(FRotator Direction, FVector Location)
{
	// GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Blue, "Fire!");
	if (ProjectileClass != nullptr)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Blue, ProjectileClass->GetClass()->GetName());
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			// GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Blue, "World!");
			// const FRotator SpawnRotation = GunOwner->GetControlRotation();
			// const FRotator SpawnRotation = GunArrow->GetComponentRotation();
			
			// const FVector SpawnLocation = (MuzzleLocation->GetComponentLocation());
			const FVector SpawnLocation = Location;
			Direction += ShootingOffset;
			// UCameraComponent* Camera = GunOwner->Camera
			
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AProjectileBase* Proj = World->SpawnActor<AProjectileBase>(ProjectileClass, SpawnLocation, Direction, ActorSpawnParams);
			Proj->Gun = this;
		}
	}
}

void AGunBase::ConsumeAmmo()
{
	CurrAmmo -= 1;
}

void AGunBase::ReloadAmmo()
{
	int ConsumedAmmo = MaxAmmo - CurrAmmo;
	if(ConsumedAmmo >= SpareAmmo)
	{
		CurrAmmo += SpareAmmo;
		SpareAmmo = 0;
	}
	else
	{
		SpareAmmo -= ConsumedAmmo;
		CurrAmmo = MaxAmmo;
	}
}

void AGunBase::CalcShootingOffset_Implementation()
{
}

void AGunBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AGunBase, GunMesh);
}

// bool AGunBase::Fire_Validate(FRotator Direction)
// {
// }

// void AGunBase::Fire(FRotator Direction)
// {
// 	
// }
