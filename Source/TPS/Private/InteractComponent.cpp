// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractComponent.h"

#include "DrawDebugHelpers.h"
#include "InteractInterface.h"
#include "TPSPlayerCharacter.h"
#include "AI/TPSAICharacter.h"
#include "AI/TPSAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UInteractComponent::UInteractComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInteractComponent::Interact()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	AActor* Owner = GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;
	Owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	// const FVector Start = EyeLocation;
	// const FVector End = EyeLocation + (EyeRotation.Vector() * InteractRange);

	ATPSPlayerCharacter* PlayerCharacter = Cast<ATPSPlayerCharacter>(Owner);

	if (PlayerCharacter && PlayerCharacter->GetCamera())
	{
		const FVector Start = PlayerCharacter->GetCamera()->GetComponentLocation();
		const FVector End = Start + PlayerCharacter->GetCamera()->GetForwardVector() * InteractRange;

		FCollisionShape Shape;
		Shape.SetSphere(30.f);
	
		FHitResult Hit;
		// GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectQueryParams);
		GetWorld()->SweepSingleByObjectType(Hit, Start, End, FQuat::Identity, ObjectQueryParams, Shape);

		AActor* HitActor = Hit.GetActor();
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0, 0, 1);
		if (HitActor)
		{
			DrawDebugSphere(GetWorld(), HitActor->GetActorLocation(), 5, 1, FColor::Red, false, 2.0, 0, 1);
			if (HitActor->Implements<UInteractInterface>())
			{
				APawn* OwnerPawn = Cast<APawn>(Owner);
				IInteractInterface::Execute_Interact(HitActor, OwnerPawn);
			}
		}
	}
}

void UInteractComponent::AIInteract()
{
	AActor* Owner = GetOwner();
	ATPSAICharacter* AICharacter = Cast<ATPSAICharacter>(Owner);
	if(AICharacter)
	{
		ATPSAIController* AIController = Cast<ATPSAIController>(AICharacter->GetController());
		if(AIController)
		{
			AActor* InteractActor = AIController->GetTargetActor();
			if(InteractActor && InteractActor->Implements<UInteractInterface>())
			{
				APawn* OwnerPawn = Cast<APawn>(Owner);
				IInteractInterface::Execute_Interact(InteractActor, OwnerPawn);
			}
		}
	}
}
