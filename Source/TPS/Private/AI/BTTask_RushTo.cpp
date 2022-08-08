// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_RushTo.h"
#include "AI/TPSAIController.h"
#include "AI/TPSAICharacter.h"


EBTNodeResult::Type UBTTask_RushTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ATPSAIController* AIController = Cast<ATPSAIController>(OwnerComp.GetAIOwner());
	if (AIController)
	{
		ATPSAICharacter* AICharacter = Cast<ATPSAICharacter>(AIController->GetPawn());
		if(AICharacter)
		{
			AICharacter->Rush();
		}
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
