// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/TPSAIController.h"
#include "TPSCharacter.h"
#include "TPSPlayerState.h"
#include "AI/TPSAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

void ATPSAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ATPSAIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ATPSAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunBehaviorTree(BehaviorTree);
	ResetMemory();
}

void ATPSAIController::OnUnPossess()
{
	Super::OnUnPossess();
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("UnPosses!"));
}

void ATPSAIController::UpdateTargetLocation(const FVector& TargetLocation)
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsVector(TargetLocationName, TargetLocation);
	}
}

void ATPSAIController::UpdateTargetCharacter(AActor* Actor)
{
	ATPSCharacter* NewCharacter = Cast<ATPSCharacter>(Actor);
	ATPSCharacter* OldCharacter = Cast<ATPSCharacter>(GetTargetCharacter());
	if (NewCharacter)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "Add");
		NewCharacter->GetSensedAIs().AddUnique(this);
	}
	if (OldCharacter && OldCharacter != NewCharacter)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "Remove");
		OldCharacter->GetSensedAIs().Remove(this);
	}
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsObject(TargetCharacterName, Actor);
	}
}

void ATPSAIController::ResetTargetCharacter()
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsObject(TargetCharacterName, nullptr);
	}
}

void ATPSAIController::UpdateTargetActor(AActor* Actor)
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsObject(TargetActorName, Actor);
	}
}

void ATPSAIController::UpdateHasWeapon(bool bHasWeapon)
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsBool(HasWeaponName, bHasWeapon);
	}
}

void ATPSAIController::UpdateHasSight(bool bHasSight)
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsBool(HasSightName, bHasSight);
	}
}

void ATPSAIController::UpdateHasCurrAmmo(bool bHasCurrAmmo)
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsBool(HasCurrAmmoName, bHasCurrAmmo);
	}
}

void ATPSAIController::UpdateHasTotalAmmo(bool bHasTotalAmmo)
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsBool(HasTotalAmmoName, bHasTotalAmmo);
	}
}

void ATPSAIController::ResetMemory()
{
	UpdateHasSight(false);
	UpdateHasWeapon(false);
	UpdateTargetActor(nullptr);
	UpdateTargetCharacter(nullptr);
	UpdateTargetLocation(FVector::ZeroVector);
	UpdateHasCurrAmmo(false);
	UpdateHasTotalAmmo(false);
}

AActor* ATPSAIController::GetTargetActor()
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	AActor* Actor = nullptr;
	if (BB)
	{
		Actor = Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(TargetActorName));
	}
	return Actor;
}

ATPSCharacter* ATPSAIController::GetTargetCharacter()
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	ATPSCharacter* Chara = nullptr;
	if (BB)
	{
		Chara = Cast<ATPSCharacter>(GetBlackboardComponent()->GetValueAsObject(TargetCharacterName));
	}
	return Chara;
}

bool ATPSAIController::GetHasSight()
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	bool HasSight = false;
	if (BB)
	{
		HasSight = GetBlackboardComponent()->GetValueAsBool(HasSightName);
	}
	return HasSight;
}

int ATPSAIController::GetTeam() const
{
	ATPSPlayerState* PState = GetPlayerState<ATPSPlayerState>();
	if (ensure(PState))
	{
		return PState->GetTeam();
	}
	return -1;
}

void ATPSAIController::SetTeam(const int _Team)
{
	ATPSPlayerState* PState = GetPlayerState<ATPSPlayerState>();
	if (ensure(PState))
	{
		return PState->SetTeam(_Team);
	}
}
