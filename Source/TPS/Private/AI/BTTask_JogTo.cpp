// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_JogTo.h"
#include "AI/TPSAIController.h"
#include "AI/TPSAICharacter.h"

EBTNodeResult::Type UBTTask_JogTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ATPSAIController* AIController = Cast<ATPSAIController>(OwnerComp.GetAIOwner());
	if (AIController)
	{
		ATPSAICharacter* AICharacter = Cast<ATPSAICharacter>(AIController->GetPawn());
		if(AICharacter)
		{
			AICharacter->RushRelease();
			AICharacter->AimRelease();
		}
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
