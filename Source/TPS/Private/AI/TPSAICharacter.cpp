// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/TPSAICharacter.h"

#include "AnimStatusComponent.h"
#include "BrainComponent.h"
#include "InteractComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "MapCheckPoint.h"
#include "AI/TPSAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gun.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "TPS/TPSGameModeBase.h"

void ATPSAICharacter::FireImpl()
{
	if(GetCurrentMontage() == AnimStatus->GetReloadAnim())
	{
		return;
	}
	UAnimMontage* FireAnim = GetAnimStatus()->GetFireAnim();
	ATPSAIController* AIC = Cast<ATPSAIController>(GetController());
	if (FireAnim && Gun && Gun->GetCurrentAmmo() >= Gun->GetConsumedAmmo())
	{
		if (GetAnimStatus()->bIsRushing)
		{
			SetMovementMode(Jogging);
		}
		PlayAnimMontage(FireAnim);
		
		if (AIC)
		{
			ATPSCharacter* Target = AIC->GetTargetCharacter();
			Gun->OnAIFire(Target);
			Gun->AddAutoFireCount();
		}
	}
	else if(Gun)
	{
		if(Gun->GetCurrentAmmo() <= 0)
		{
			AIC->UpdateHasCurrAmmo(false);
			if(Gun->GetBackupAmmo() <= 0)
			{
				AIC->UpdateHasTotalAmmo(false);
			}
		}
	}
}

void ATPSAICharacter::LoseSightOfTarget()
{
	ATPSAIController* AIController = Cast<ATPSAIController>(GetController());
	if (AIController)
	{
		AIController->UpdateTargetCharacter(nullptr);
	}
}

void ATPSAICharacter::SetMovementMode(int Mode)
{
	switch (Mode)
	{
	case Jogging:
		GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
		break;
	case Walking:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		break;
	case Rushing:
		GetCharacterMovement()->MaxWalkSpeed = RushSpeed;
		break;
	default:
		break;
	}
}

void ATPSAICharacter::OnPawnSeen(APawn* Pawn)
{
	ATPSCharacter* SeenCharacter = Cast<ATPSCharacter>(Pawn);
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("I saw something!"));
	if (SeenCharacter)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("It's a character!"));
		if (SeenCharacter->Team != Team)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("It's an enemy!"));
			ATPSAIController* AIController = Cast<ATPSAIController>(GetController());
			if (AIController)
			{
				GetWorld()->GetTimerManager().ClearTimer(LoseSight);
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Go hunt it!"));
				AIController->UpdateTargetCharacter(SeenCharacter);
				GetWorld()->GetTimerManager().SetTimer(LoseSight, this, &ATPSAICharacter::LoseSightOfTarget, 3.0f);
			}
		}
	}
}

void ATPSAICharacter::OnActorSensed(AActor* SensedActor, FAIStimulus AIStimulus)
{
	if(!HasAuthority()) return;
	ATPSCharacter* SeenCharacter = Cast<ATPSCharacter>(SensedActor);
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("I saw something!"));
	if (SeenCharacter && SeenCharacter->GetAttributes()->bIsDead == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::SanitizeFloat(SeenCharacter->Team) + " " + FString::SanitizeFloat(Team));
		if (SeenCharacter->Team != Team)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("It's an enemy!"));
			ATPSAIController* AIController = Cast<ATPSAIController>(GetController());
			if (AIController)
			{
				if (AIStimulus.WasSuccessfullySensed())
				{
					GetWorld()->GetTimerManager().ClearTimer(LoseSight);
					AIController->UpdateTargetCharacter(SeenCharacter);
					AIController->UpdateHasSight(true);
				}
				else
				{
					AIController->UpdateHasSight(false);
					GetWorld()->GetTimerManager().SetTimer(LoseSight, this, &ATPSAICharacter::LoseSightOfTarget, 3.0f);
				}
			}
		}
	}
}


ATPSAICharacter::ATPSAICharacter() : Super()
{
	// PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensing");
	APlayerState* PState = GetPlayerState();
	if (PState)
	{
		PState->SetIsABot(true);
	}
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerception");
}

void ATPSAICharacter::OnDead_Implementation(APawn* InstigatorPawn, const FVector& Force, FName Bone)
{
	Super::OnDead_Implementation(InstigatorPawn, Force, Bone);

	AIPerceptionComponent->Deactivate();

	ATPSAIController* PController = Cast<ATPSAIController>(GetController());
	
	if (PController && PController->GetBrainComponent())
	{
		PController->GetBrainComponent()->StopLogic("Killed");
		PController->PawnPendingDestroy(this);
	}


	ATPSGameModeBase* GameMode = Cast<ATPSGameModeBase>(UGameplayStatics::GetGameMode(this));

	if (GameMode)
	{
		ATPSCharacter* InstigatorCharacter = Cast<ATPSCharacter>(InstigatorPawn);
		int DeadCause = 0;
		if(Bone == FName("head"))
		{
			DeadCause = 1;
		}
		GameMode->HandleCharacterDead(PController, InstigatorPawn->GetController(), false, Team,
									  InstigatorCharacter->Team, DeadCause);
	}
}

void ATPSAICharacter::OnHealthChange_Implementation(APawn* InstigatorPawn, float Delta, float CurrentHealth,
                                                    const FVector& Force,
                                                    FName Bone)
{
	Super::OnHealthChange_Implementation(InstigatorPawn, Delta, CurrentHealth, Force, Bone);
	
	ATPSAIController* PController = Cast<ATPSAIController>(GetController());

	if (PController && InstigatorPawn != this)
	{
		PController->UpdateTargetActor(InstigatorPawn);
	}
}

void ATPSAICharacter::PostInitializeComponents()
{
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, TEXT("PostInitComps!"));
	Super::PostInitializeComponents();
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ATPSAICharacter::OnActorSensed);
}

void ATPSAICharacter::Rush()
{
	if (AnimStatus->bIsAimingHold || AnimStatus->bIsAimingTap)
	{
		AnimStatus->bIsAimingHold = false;
		AnimStatus->bIsAimingTap = false;
	}
	SetMovementMode(Rushing);
}

void ATPSAICharacter::RushRelease()
{
	if (AnimStatus->bIsAimingHold || AnimStatus->bIsAimingTap)
	{
		SetMovementMode(Walking);
	}
	else
	{
		SetMovementMode(Jogging);
	}
}

void ATPSAICharacter::Interact()
{
	ServerInteract();
}

void ATPSAICharacter::ServerInteract_Implementation()
{
	InteractComponent->AIInteract();
}

void ATPSAICharacter::Aim()
{
	if (bHasWeapon)
	{
		AnimStatus->bIsAimingHold = true;
		SetMovementMode(Walking);
	}
}

void ATPSAICharacter::AimRelease()
{
	if (bHasWeapon)
	{
		AnimStatus->bIsAimingHold = false;
		AnimStatus->bIsAimingTap = false;
		if (!AnimStatus->bIsCrouching)
		{
			SetMovementMode(Jogging);
		}
	}
}

void ATPSAICharacter::Fire()
{
	if(GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		return;
	}
	if (Gun && Gun->GetCurrentAmmo() >= Gun->GetConsumedAmmo())
	{
		// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, TEXT("I'm Gonna Fire!"));
		FireImpl();
		switch (Gun->GetFireMode())
		{
		case AGun::EFireMode::Auto:
			GetWorld()->GetTimerManager().SetTimer(Gun->AutoFireTimerHandle, this, &ATPSAICharacter::FireImpl,
			                                       Gun->GetAutoFireDelta(), true);
			GetWorld()->GetTimerManager().SetTimer(StopFire, this, &ATPSAICharacter::FireRelease,
			                                       AutoFireDuration + FMath::RandRange(
				                                       -AutoFireDurationOffsetMax, AutoFireDurationOffsetMax));
		case AGun::EFireMode::Single:
		default:
			break;
		}
	}
}

void ATPSAICharacter::FireRelease()
{
	if (Gun)
	{
		GetWorld()->GetTimerManager().ClearTimer(Gun->AutoFireTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(StopFire);
		Gun->ResetAutoFireCount();
	}
}

void ATPSAICharacter::GetGun(AGun* NewGun)
{
	Super::GetGun(NewGun);
	ATPSAIController* AIController = Cast<ATPSAIController>(GetController());
	if (AIController)
	{
		AIController->UpdateHasWeapon(true);
		if(NewGun->GetCurrentAmmo())
		{
			AIController->UpdateHasCurrAmmo(true);
			AIController->UpdateHasTotalAmmo(true);
		}
		else if(NewGun->GetBackupAmmo())
		{
			AIController->UpdateHasTotalAmmo(true);
		}
	}
}

void ATPSAICharacter::DiscardGun()
{
	Super::DiscardGun();
	ATPSAIController* AIController = Cast<ATPSAIController>(GetController());
	if (AIController)
	{
		AIController->UpdateHasWeapon(false);
	}
}
