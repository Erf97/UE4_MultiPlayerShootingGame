// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"

#include "EnvQueryTest_HasOwner.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API UEnvQueryTest_HasOwner : public UEnvQueryTest
{
	GENERATED_UCLASS_BODY()

protected:

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
};
