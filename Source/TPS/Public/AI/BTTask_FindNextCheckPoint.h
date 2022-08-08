// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "CoreMinimal.h"
#include "BTTask_FindNextCheckPoint.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API UBTTask_FindNextCheckPoint : public UBTTaskNode
{
	GENERATED_BODY()

	AActor* FindNearestLocation(const TArray<AActor*>& CheckPoints, const FVector& SelfLocation) const;

public:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
