// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "DebugStatics.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API UDebugStatics : public UGameplayStatics
{
	GENERATED_BODY()

public:

	static void DebugMsg(FString Msg);
	
};
