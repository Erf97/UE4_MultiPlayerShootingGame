// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_EquipWeapon.h"
#include "AI/TPSAIController.h"
#include "AI/TPSAICharacter.h"
#include "Gun.h"

EBTNodeResult::Type UBTTask_EquipWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ATPSAIController* AIController = Cast<ATPSAIController>(OwnerComp.GetAIOwner());
	auto Result = EBTNodeResult::Failed;
	if (AIController)
	{
		ATPSAICharacter* AICharacter = Cast<ATPSAICharacter>(AIController->GetPawn());
		if (AICharacter)
		{
			AActor* TargetActor = AIController->GetTargetActor();
			if(TargetActor && TargetActor->GetClass()->IsChildOf(AGun::StaticClass()) && !TargetActor->GetOwner())
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,TEXT("Okay"));
				AICharacter->Interact();
				// AIController->UpdateHasWeapon(true);
				Result = EBTNodeResult::Succeeded;
			}
		}
	}
	return Result;
}
