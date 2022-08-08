// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ReloadAmmo.h"

#include "Gun.h"
#include "AI/TPSAICharacter.h"
#include "AI/TPSAIController.h"

EBTNodeResult::Type UBTTask_ReloadAmmo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ATPSAIController* AIController = Cast<ATPSAIController>(OwnerComp.GetAIOwner());
	auto Result = EBTNodeResult::Failed;
	if (AIController)
	{
		ATPSAICharacter* AICharacter = Cast<ATPSAICharacter>(AIController->GetPawn());
		AGun* Gun = AICharacter->GetGunActor();
		if (AICharacter && Gun && Gun->GetBackupAmmo() > 0)
		{
			AICharacter->ReloadAmmo();
			Result = EBTNodeResult::Succeeded;
		}
		else if (Gun && Gun->GetBackupAmmo() <= 0)
		{
			AIController->UpdateHasTotalAmmo(false);
		}
	}
	return Result;
}
