// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterUMGComponent.h"

#include "Blueprint/UserWidget.h"

// Sets default values for this component's properties
UCharacterUMGComponent::UCharacterUMGComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCharacterUMGComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UCharacterUMGComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// void UCharacterUMGComponent::AddWidgetToViewport(UUserWidget* Widget)
// {
// 	if(Widget)
// 	{
// 		Widget->AddToViewport();
// 	}
// }
