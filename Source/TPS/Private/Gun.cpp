// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

#include "TPSCharacter.h"
#include "TPSPlayerCharacter.h"
#include "AI/TPSAICharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"


void AGun::ConsumeAmmo(int ConsumingAmmo)
{
	SetAmmo(CurrAmmo - ConsumingAmmo, BackupAmmo);
}

void AGun::AddRecoilForceToPlayer()
{
	ATPSCharacter* Character = Cast<ATPSCharacter>(GetOwner());
	Character->AddControllerPitchInput(-RecoilForce);
}

FRotator AGun::GetAimmingDirection()
{
	ATPSPlayerCharacter* Character = Cast<ATPSPlayerCharacter>(GetOwner());

	if (!Character) return FRotator::ZeroRotator;

	UCameraComponent* Camera = Character->GetCamera();
	FVector TraceStartPos = Camera->GetComponentLocation();
	FVector TraceEndPos = TraceStartPos + Camera->GetForwardVector() * ShotRange;

	FVector TargetPos = TraceEndPos;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Character);
	QueryParams.bTraceComplex = true;

	FHitResult HitRes;

	if (GetWorld()->LineTraceSingleByChannel(HitRes, TraceStartPos, TraceEndPos, ECC_Visibility, QueryParams))
	{
		TargetPos = HitRes.ImpactPoint;
	}

	return UKismetMathLibrary::FindLookAtRotation(FireLocation->GetComponentLocation(), TargetPos);
}

void AGun::ServerFire_Implementation(UWorld* World, const FVector& Location, const FRotator& Direction)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = Cast<APawn>(GetOwner());
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, GetNameSafe(GetOwner()));
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ConsumeAmmo(ConsumedAmmo);
	MulticastMuzzleFlash(Location, Direction);
	World->SpawnActor(ProjectileClass, &Location, &Direction, SpawnParameters);
}

FRotator AGun::Offset_Implementation(int AimingStatus)
{
	FRotator Direction;
	switch (AimingStatus)
	{
	case 0: // None
	case 1: // Hold
		Direction = GetAimmingDirection();
		break;
	case 2: // Tap
		Direction = FireLocation->GetComponentRotation();
		break;
	default:
		break;
	}
	return Direction;
}

// Sets default values
AGun::AGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>("Gun");
	GunCamera1P = CreateDefaultSubobject<UCameraComponent>("GunCamera");

	GunCamera1P->SetupAttachment(GunMesh);

	FireLocation = CreateDefaultSubobject<UArrowComponent>("FireLocation");
	FireLocation->SetupAttachment(GunMesh);

	// MuzzleFlashComp = CreateDefaultSubobject<UParticleSystemComponent>("MuzzleFlashParticle");
	// MuzzleFlashComp->SetupAttachment(FireLocation);

	SetReplicates(true);
}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGun, OffsetDirection);
	DOREPLIFETIME(AGun, CurrAmmo);
	DOREPLIFETIME(AGun, BackupAmmo);
	DOREPLIFETIME(AGun, MaxDivAmmo);
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);
}

void AGun::SetAmmo(int NewCurrAmmo, int NewBackupAmmo)
{
	NewCurrAmmo = NewCurrAmmo < 0 ? 0 : NewCurrAmmo;
	NewBackupAmmo = NewBackupAmmo < 0 ? 0 : NewBackupAmmo;
	CurrAmmo = NewCurrAmmo;
	BackupAmmo = NewBackupAmmo;
	if(HasAuthority())
	{
		OnRep_AmmoChange();
	}
}

void AGun::ServerSetAmmo_Implementation(int NewCurrAmmo, int NewBackupAmmo)
{
	SetAmmo(NewCurrAmmo, NewBackupAmmo);
}

void AGun::MulticastMuzzleFlash_Implementation(const FVector& Location, const FRotator& Direction)
{
	if(MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, FireLocation);
	}
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGun::AddSmoothRecoilForceToPlayer_Implementation()
{
}

void AGun::OnAIFire_Implementation(ATPSCharacter* TargetCharacter)
{
	ATPSAICharacter* ThisAI = Cast<ATPSAICharacter>(GetOwner());
	if (TargetCharacter && ThisAI)
	{
		if (ProjectileClass)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, TEXT("Fire!"));
				FVector Location = FireLocation->GetComponentLocation();
				FRotator Direction = UKismetMathLibrary::FindLookAtRotation(
					Location, TargetCharacter->GetActorLocation());
				const float RandomOffsetX = FMath::RandRange(-ThisAI->FireDirectionOffsetMax, ThisAI->FireDirectionOffsetMax);
				const float RandomOffsetY = FMath::RandRange(-ThisAI->FireDirectionOffsetMax, ThisAI->FireDirectionOffsetMax);
				Direction += FRotator(RandomOffsetY, RandomOffsetX, 0);
				ServerFire(World, Location, Direction);
			}
		}
	}
}

void AGun::Interact_Implementation(APawn* InstigatorPawn)
{
	if (GetOwner())
	{
		return;
	}
	IInteractInterface::Interact_Implementation(InstigatorPawn);
	ATPSCharacter* Character = Cast<ATPSCharacter>(InstigatorPawn);
	Character->GetGun(this);
}

void AGun::ReloadAmmo()
{
	int NewCurrAmmo, NewBackupAmmo;
	if(BackupAmmo >= MaxDivAmmo - CurrAmmo)
	{
		NewCurrAmmo = MaxDivAmmo;
		NewBackupAmmo = BackupAmmo - MaxDivAmmo + CurrAmmo;
	}
	else
	{
		NewCurrAmmo = BackupAmmo + CurrAmmo;
		NewBackupAmmo = 0;
	}
	ServerSetAmmo(NewCurrAmmo, NewBackupAmmo);

	ATPSAICharacter* AICharacter = Cast<ATPSAICharacter>(GetOwner());
	if(AICharacter)
	{
		ATPSAIController* AIC = Cast<ATPSAIController>(AICharacter->GetController());
		if(AIC)
		{
			AIC->UpdateHasCurrAmmo(true);
		}
	}
}

void AGun::OnRep_AmmoChange()
{
	OnAmmoChange.Broadcast(CurrAmmo, BackupAmmo);
}

void AGun::ClientOnAmmoChange_Implementation()
{
	OnAmmoChange.Broadcast(CurrAmmo, BackupAmmo);
}

void AGun::OnPlayerFire_Implementation(const int AimingStatus)
{
	if (ProjectileClass && GetOwner())
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FVector Location = FireLocation->GetComponentLocation();
			ServerFire(World, Location, Offset(AimingStatus));
			// AddRecoilForceToPlayer();
			AddSmoothRecoilForceToPlayer();
		}
	}
}
