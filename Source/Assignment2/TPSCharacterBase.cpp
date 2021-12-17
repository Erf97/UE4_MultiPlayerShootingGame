// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATPSCharacterBase::ATPSCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->TargetArmLength = 300.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->AttachToComponent(SpringArm, FAttachmentTransformRules::KeepRelativeTransform);

	MaxHealth = 100;
	CurrHealth = MaxHealth;

	bIsAlive = true;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ATPSCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATPSCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATPSCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATPSCharacterBase::SetGun_Implementation(AGunBase* GunActor)
{
	if(GunActor)
	{
		Gun = GunActor;
		USkeletalMeshComponent* GunMesh = Gun->GunMesh;
		GunMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		GunMesh->SetSimulatePhysics(false);
		GunMesh->AttachToComponent(GetMesh(),
								   FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
								   TEXT("index_03_r"));
		GunMesh->SetRelativeTransform(Gun->GunTrans);
		Gun->GunOwner = this;
	}
}


// void ATPSCharacterBase::SetGun(AGunBase* GunActor)
// {
// 	// Gun = GunActor;
// 	// USkeletalMeshComponent* GunMesh = Gun->GunMesh;
// 	// GunMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
// 	// GunMesh->SetSimulatePhysics(false);
// 	// GunMesh->AttachToComponent(GetMesh(),
// 	//                            FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
// 	//                            TEXT("index_03_r"));
// 	// GunMesh->SetRelativeTransform(Gun->GunTrans);
// 	// Gun->GunOwner = this;
// }

void ATPSCharacterBase::DiscardGun_Implementation()
{
	USkeletalMeshComponent* GunMesh = Gun->GunMesh;
	Gun->GunOwner = nullptr;
	Gun->SetActorEnableCollision(true);
	GunMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GunMesh->SetCollisionResponseToAllChannels(ECR_Block);
	GunMesh->SetSimulatePhysics(true);
	GunMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
}

// void ATPSCharacterBase::DiscardGun()
// {
// 	USkeletalMeshComponent* GunMesh = Gun->GunMesh;
// 	Gun->GunOwner = nullptr;
// 	Gun->SetActorEnableCollision(true);
// 	GunMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
// 	GunMesh->SetCollisionResponseToAllChannels(ECR_Block);
// 	GunMesh->SetSimulatePhysics(true);
// 	GunMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
// }

FRotator ATPSCharacterBase::GetAimmingDirection()
{
	FVector TraceStartPos = Camera->GetComponentLocation();
	FVector TraceEndPos = TraceStartPos + Camera->GetForwardVector() * 10000.f;

	FVector TargetPos = TraceEndPos;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;

	FHitResult HitRes;

	if (GetWorld()->LineTraceSingleByChannel(HitRes, TraceStartPos, TraceEndPos, ECC_Visibility, QueryParams))
	{
		TargetPos = HitRes.ImpactPoint;

		// GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Blue, HitRes.Actor->GetName());
	}

	return UKismetMathLibrary::FindLookAtRotation(Gun->MuzzleLocation->GetComponentLocation(), TargetPos);
}

void ATPSCharacterBase::Throw_Implementation(const FVector Location, const FRotator Direction)
{
	if (GrenadeClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			AProjectileBase* Grenade = World->SpawnActor<AProjectileBase>(
				GrenadeClass, Location, Direction, ActorSpawnParams);
			Grenade->GrenadeOwner = this;
			Grenade->ProjMesh->AddImpulse(Direction.Vector() * 3000.f);
		}
	}
}

// void ATPSCharacterBase::Throw(const FVector Location, const FRotator Direction)
// {
// 	
// }

void ATPSCharacterBase::StartFire_Implementation(FRotator Direction, FVector Location)
{
	if(Gun != nullptr)
	{
		Gun->Fire(Direction, Location);
	}
}

void ATPSCharacterBase::SetCurrentHealth(float Health)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrHealth = FMath::Clamp<int>(Health, 0, MaxHealth);
		OnHealthUpdate();
	}
}


// float ATPSCharacterBase::TakeDamage(float DamageTaken, FDamageEvent const& DamageEvent, AController* EventInstigator,
// 	AActor* DamageCauser)
// {
// 	int DamageApplied = CurrHealth - DamageTaken;
// 	SetCurrentHealth(DamageApplied);
// 	return DamageApplied;
// }

// float ATPSCharacterBase::InternalTakePointDamage(float Damage, FPointDamageEvent const& PointDamageEvent,
// 	AController* EventInstigator, AActor* DamageCauser)
// {
// 	
// }

void ATPSCharacterBase::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void ATPSCharacterBase::OnHealthUpdate()
{
	if(CurrHealth<=0)
	{
		if(IsLocallyControlled())
		{
			APlayerController* PController = Cast<APlayerController>(GetController());
			DisableInput(PController);
		}
		if(GetLocalRole() == ROLE_Authority)
		{
			DiscardGun();
			bIsAlive = false;
		}
		GetMesh()->SetSimulatePhysics(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetCanBeDamaged(false);
	}
}

// void ATPSCharacterBase::GotKilled_Implementation()
// {
// 	APlayerController* PController = Cast<APlayerController>(GetController());
// 	DisableInput(PController);
// 	
//
// 	// TODO ?
// }

void ATPSCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ATPSCharacterBase, CurrHealth);
	DOREPLIFETIME(ATPSCharacterBase, Gun);
}

// bool ATPSCharacterBase::GotDamage_Implementation(int Damage)
// {
// 	if (CurrHealth - Damage <= 0)
// 	{
// 		bIsDead = true;
// 	}
// 	else
// 	{
// 		CurrHealth -= Damage;
// 	}
// 	return bIsDead;
// }
