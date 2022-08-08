// Fill out your copyright notice in the Description page of Project Settings.
//
// switch (AnimStatus->State)
// {
// case EAnimStates::Idle:
// case EAnimStates::Jogging:
// case EAnimStates::Crouch:
// case EAnimStates::CrouchWalk:
// case EAnimStates::IdleAiming:
// case EAnimStates::JogAiming:
// case EAnimStates::RunJump:
// case EAnimStates::Jump:
// case EAnimStates::Rush:
// default:
// 	break;
// }

#include "TPSCharacter.h"

#include "Gun.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InteractComponent.h"
#include "AnimStatusComponent.h"
#include "TPSPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


//TODO： 这部分操作感觉之后挪到别处会比较好


// Sets default values
ATPSCharacter::ATPSCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractComponent = CreateDefaultSubobject<UInteractComponent>("InteractComp");

	AnimStatus = CreateDefaultSubobject<UAnimStatusComponent>("AnimStatus");

	Gun = CreateDefaultSubobject<AGun>("Gun");

	Attributes = CreateDefaultSubobject<UAttributeComponent>("Attributes");

	bHasWeapon = false;

	SetReplicates(true);
}

void ATPSCharacter::OnDead_Implementation(APawn* InstigatorPawn, const FVector& Force, FName Bone)
{
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Super"));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Character->GetMesh()->AddImpulse(Force, Bone, true);
	DiscardGun();
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::SanitizeFloat(SensedAIs.Num()));
	if (HasAuthority())
	{
		for (auto AI : SensedAIs)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, GetNameSafe(AI));
			AI->ResetTargetCharacter();
		}
	}
	PendingDestory();
}

void ATPSCharacter::OnHealthChange_Implementation(APawn* InstigatorPawn, float Delta, float CurrentHealth, const FVector& Force, FName Bone)
{
	ATPSPlayerCharacter* InstigatorPlayerCharacter = Cast<ATPSPlayerCharacter>(InstigatorPawn);
	// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "HealthChange 2! : " + FString::SanitizeFloat(CurrentHealth));
	if(InstigatorPlayerCharacter && HasAuthority())
	{
		if(Delta < 0)
		{
			InstigatorPlayerCharacter->ClientOnHitOtherCharacter(CurrentHealth <= 0.0f);
			InstigatorPlayerCharacter->ClientOnHitOtherCharacter_Implementation(CurrentHealth <= 0.0f);
		}
	}
}

void ATPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPSCharacter, Gun);
	DOREPLIFETIME(ATPSCharacter, bIsAlive);
	DOREPLIFETIME(ATPSCharacter, bHasWeapon);
	DOREPLIFETIME(ATPSCharacter, GunMesh);
	// DOREPLIFETIME(ATPSCharacter, SensedAIs);
}

// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	Attributes->OnDead.AddDynamic(this, &ATPSCharacter::OnDead);

	Attributes->OnHealthChange.AddDynamic(this, &ATPSCharacter::OnHealthChange);

	GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ATPSCharacter::OnMontageEnd);
}

// Called every frame
void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATPSCharacter::GetGun(AGun* NewGun)
{
	ServerGetGun(NewGun);
}

void ATPSCharacter::ServerGetGun_Implementation(AGun* NewGun)
{
	if (HasAuthority())
	{
		if (bHasWeapon)
		{
			DiscardGun();
		}
		if (NewGun)
		{
			Gun = NewGun;
			Gun->SetOwner(this);
			bHasWeapon = true;
			OnRep_SetGunStatus();
			// Gun->ClientOnAmmoChange();
			// Gun->ClientOnAmmoChange_Implementation();
		}
	}
}


void ATPSCharacter::DiscardGun()
{
	ServerDiscardGun();
}

float ATPSCharacter::CalcDamageByBone(float Damage, FName Bone)
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "2 " + Bone.ToString());
	if(Bone == FName("head"))
	{
		Damage *= 5;
	}
	else if(GetMesh()->BoneIsChildOf(Bone, FName("upperarm_r")) || GetMesh()->BoneIsChildOf(Bone, FName("upperarm_l")))
	{
		Damage *= .5;
	}
	else if(GetMesh()->BoneIsChildOf(Bone, FName("thigh_r")) || GetMesh()->BoneIsChildOf(Bone, FName("thigh_l")))
	{
		Damage *= .5;
	}
	return Damage;
}

void ATPSCharacter::ServerDiscardGun_Implementation()
{
	if (bHasWeapon)
	{
		if (HasAuthority())
		{
			Gun->SetOwner(nullptr);
			Gun = nullptr;
			bHasWeapon = false;
			OnRep_SetGunStatus();
		}
	}
}

void ATPSCharacter::OnRep_SetGunStatus()
{
	if (bHasWeapon)
	{
		GunMesh = Gun->GetGunMesh();
		// GunMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		GunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GunMesh->SetSimulatePhysics(false);
		GunMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		                           GunGridSocketName);
		GunMesh->SetRelativeTransform(Gun->GetAttachTransform());
	}
	else
	{
		GunMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GunMesh->SetSimulatePhysics(true);
		GunMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	}
}

void ATPSCharacter::OnMontageEnd(UAnimMontage* InMontage, bool bInterrupted)
{
	if(InMontage == AnimStatus->GetReloadAnim())
	{
		if(Gun)
		{
			Gun->ReloadAmmo();
		}
	}
}

void ATPSCharacter::PendingDestory()
{
	GetWorld()->GetTimerManager().SetTimer(PendingDestoryHandle, this, &ATPSCharacter::DestroyThis, PendindDestoryTime, false);
}


void ATPSCharacter::Interact()
{
	// ServerInteract();
	InteractComponent->Interact();
}

void ATPSCharacter::Aim()
{
}

void ATPSCharacter::AimRelease()
{
}

void ATPSCharacter::Fire()
{
}

void ATPSCharacter::FireRelease()
{
}

void ATPSCharacter::Rush()
{
}

void ATPSCharacter::RushRelease()
{
}

void ATPSCharacter::TPSCrouch()
{
}

void ATPSCharacter::CrouchRelease()
{
}


void ATPSCharacter::ServerInteract_Implementation()
{
	InteractComponent->Interact();
}


void ATPSCharacter::MoveForward(const float Value)
{
	FRotator Rotation = FRotator(0, GetControlRotation().Yaw, 0);
	AddMovementInput(Rotation.Vector(), Value);
}

void ATPSCharacter::MoveRight(const float Value)
{
	FRotator Rotation = FRotator(0, GetControlRotation().Yaw + 90.0f, 0);
	AddMovementInput(Rotation.Vector(), Value);
}

void ATPSCharacter::ReloadAmmo()
{
	if(Gun && Gun->GetCurrentAmmo() < Gun->GetMaxDivAmmo())
	{
		if(!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
		{
			AnimStatus->PlayMontage(AnimStatus->GetReloadAnim());
		}
	}
}
