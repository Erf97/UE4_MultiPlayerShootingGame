// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTTask_FindWeapon.h"
#include "AI/TPSAIController.h"
#include "AI/TPSAICharacter.h"
#include "Gun.h"


EBTNodeResult::Type UBTTask_FindWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ATPSAIController* AIController = Cast<ATPSAIController>(OwnerComp.GetAIOwner());
	auto Result = EBTNodeResult::Failed;
	if (AIController)
	{
		ATPSAICharacter* AICharacter = Cast<ATPSAICharacter>(AIController->GetPawn());
		if (AICharacter)
		{
			AActor* TargetActor = AIController->GetTargetActor();
			if (TargetActor && !TargetActor->GetOwner())
			{
				AIController->UpdateTargetLocation(TargetActor->GetActorLocation());
				Result = EBTNodeResult::Succeeded;
			}
		}
	}
	return Result;
}
