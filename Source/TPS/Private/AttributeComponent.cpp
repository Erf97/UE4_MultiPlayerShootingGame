// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeComponent.h"

#include "TPSCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TPS/TPSGameModeBase.h"

bool UAttributeComponent::CanApplyHealthChange(APawn* InstigatorPawn, float Delta) const
{
	bool CanChange = true;
	CanChange &= CheckDead(Delta);
	CanChange &= CheckTeamDamage(InstigatorPawn, Delta);
	CanChange &= CheckInVincible(Delta);

	return CanChange;
}

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAttributeComponent, Health);
}


bool UAttributeComponent::CheckInVincible(float Delta) const
{
	if (Delta > 0) return false;
	return !bIsInvincible;
}

bool UAttributeComponent::CheckTeamDamage(APawn* InstigatorPawn, float Delta) const
{
	if (!InstigatorPawn) return true;
	ATPSCharacter* InstigatorCharacter = Cast<ATPSCharacter>(InstigatorPawn);
	if (!InstigatorCharacter) return true;
	ATPSCharacter* ThisCharacter = Cast<ATPSCharacter>(GetOwner());
	if (!ThisCharacter) return true;
	if (Delta > 0) return true;
	ATPSGameModeBase* GameMode = Cast<ATPSGameModeBase>(UGameplayStatics::GetGameMode(this));
	return InstigatorCharacter->Team != ThisCharacter->Team || GameMode->IsAllowTeamDamage();
}

// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAttributeComponent::ApplyHealthChange_Implementation(float Delta, APawn* InstigatorPawn,
	const FVector& Force, FName Bone)
{
	if (!CanApplyHealthChange(InstigatorPawn, Delta)) return;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "1 " + Bone.ToString());
	ATPSCharacter* Character = Cast<ATPSCharacter>(GetOwner());
	
	if(Character && Delta < 0)
	{
		Delta = Character->CalcDamageByBone(Delta, Bone);
	}
	
	float OldHealth = Health;
	Health += Delta;
	if (Health <= 0)
	{
		Health = 0;
		bIsDead = true;
		OnActorDead(InstigatorPawn, Force, Bone);
	}
	else if (Health >= HealthMax) Health = HealthMax;
	float ActualDelta = Health - OldHealth;
	OnActorHealthChange(InstigatorPawn, ActualDelta, Health, Force, Bone);
	return;
}

// bool UAttributeComponent::ApplyHealthChange(float Delta, bool& bIsActorDead, APawn* InstigatorPawn,
//                                             const FVector& Force, FName Bone)
// {
// 	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Damage!");
// 	if (!CanApplyHealthChange(InstigatorPawn, Delta)) return false;
// 	float OldHealth = Health;
// 	Health += Delta;
// 	if (Health <= 0)
// 	{
// 		Health = 0;
// 		bIsDead = true;
// 		OnActorDead(InstigatorPawn, Force, Bone);
// 	}
// 	else if (Health >= HealthMax) Health = HealthMax;
// 	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::SanitizeFloat(Health));
// 	bIsActorDead = bIsDead;
// 	float ActualDelta = Health - OldHealth;
// 	OnActorHealthChange(InstigatorPawn, ActualDelta, Health, Force, Bone);
// 	return true;
// 	// bool bIsSucceed = false;
// 	// ServerApplyHealthChange(Delta, bIsActorDead, bIsSucceed, InstigatorPawn, Force, Bone);
// 	// return bIsSucceed;
// }

void UAttributeComponent::OnActorDead(APawn* InstigatorPawn, const FVector& Force, FName Bone)
{
	MulticastOnActorDead(InstigatorPawn, Force, Bone);
}

void UAttributeComponent::MulticastOnActorDead_Implementation(APawn* InstigatorPawn, const FVector& Force, FName Bone)
{
	OnDead.Broadcast(InstigatorPawn, Force, Bone);
}

void UAttributeComponent::OnActorHealthChange(APawn* InstigatorPawn, float Delta, float CurrentHealth,
                                              const FVector& Force, FName Bone)
{
	MulticastOnActorHealthChange(InstigatorPawn, Delta, CurrentHealth, Force, Bone);
}

void UAttributeComponent::MulticastOnActorHealthChange_Implementation(APawn* InstigatorPawn, float Delta,
	float CurrentHealth, const FVector& Force, FName Bone)
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "HealthChange 1! : " + FString::SanitizeFloat(CurrentHealth));
	OnHealthChange.Broadcast(InstigatorPawn, Delta, CurrentHealth, Force, Bone);
}
