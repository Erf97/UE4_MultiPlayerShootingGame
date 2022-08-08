// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimStatusComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Windows/AllowWindowsPlatformTypes.h"

// Sets default values for this component's properties
UAnimStatusComponent::UAnimStatusComponent() : bIsAiming(false), bIsAimingHold(false), bIsAimingTap(false),
                                               bIsCrouching(false), bIsRushing(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	State = Idle;

	SetIsReplicatedByDefault(true);
}


void UAnimStatusComponent::ServerPlayMontage_Implementation(UAnimMontage* AnimMontage)
{
	MulticastPlayMontage(AnimMontage);
}

void UAnimStatusComponent::MulticastPlayMontage_Implementation(UAnimMontage* AnimMontage)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if(Character && AnimMontage)
	{
		Character->PlayAnimMontage(AnimMontage);
	}
}

void UAnimStatusComponent::PlayMontage(UAnimMontage* AnimMontage)
{
	ServerPlayMontage(AnimMontage);
}

// Called when the game starts
void UAnimStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UAnimStatusComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAnimStatusComponent, bIsAiming);
	DOREPLIFETIME(UAnimStatusComponent, bIsAimingHold);
	DOREPLIFETIME(UAnimStatusComponent, bIsAimingTap);
	DOREPLIFETIME(UAnimStatusComponent, bIsCrouching);
	DOREPLIFETIME(UAnimStatusComponent, bIsRushing);
	DOREPLIFETIME(UAnimStatusComponent, PlayerPitch);
	DOREPLIFETIME(UAnimStatusComponent, PlayerSpeedX);
	DOREPLIFETIME(UAnimStatusComponent, PlayerSpeedY);
}

// Called every frame
void UAnimStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	const ACharacter* Chara = Cast<ACharacter>(GetOwner());
	if (Chara)
	{
		const AController* Controller = Chara->GetController();
		if (Controller)
		{
			PlayerPitch = Controller->GetControlRotation().Pitch;
			PlayerPitch = (PlayerPitch <= 90) ? 90.0f - PlayerPitch : 450.0f - PlayerPitch;
			bIsRushing = (Chara->GetVelocity().Size() > 600.0f);
		}
	}
}

UAnimMontage* UAnimStatusComponent::GetFireAnim()
{
	if (bIsAimingHold || bIsAimingTap)
	{
		return FireAnimAim;
	}
	else
	{
		return FireAnimHip;
	}
}
