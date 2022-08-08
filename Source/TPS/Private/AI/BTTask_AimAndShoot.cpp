// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_AimAndShoot.h"
#include "AI/TPSAIController.h"
#include "AI/TPSAICharacter.h"
#include "Gun.h"

EBTNodeResult::Type UBTTask_AimAndShoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ATPSAIController* AIController = Cast<ATPSAIController>(OwnerComp.GetAIOwner());
	auto Result = EBTNodeResult::Failed;
	if (AIController)
	{
		ATPSAICharacter* AICharacter = Cast<ATPSAICharacter>(AIController->GetPawn());
		if (AICharacter)
		{
			ATPSCharacter* TargetCharacter = AIController->GetTargetCharacter();
			if(TargetCharacter)
			{
				AICharacter->Aim();
				AICharacter->Fire();
				Result = EBTNodeResult::Succeeded;
				
			}
		}
	}
	return Result;
}
