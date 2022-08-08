// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_FindNextCheckPoint.h"

#include "MapCheckPoint.h"
#include "AI/TPSAICharacter.h"
#include "AI/TPSAIController.h"
#include "Kismet/GameplayStatics.h"


AActor* UBTTask_FindNextCheckPoint::FindNearestLocation(const TArray<AActor*>& CheckPoints,
                                                        const FVector& SelfLocation) const
{
	AActor* Ret = nullptr;
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, Ret.ToString());
	if (ensure(CheckPoints.Num() != 0))
	{
		Ret = CheckPoints[0];
		float MinDist = FVector::Distance(SelfLocation, CheckPoints[0]->GetActorLocation());
		// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::SanitizeFloat(MinDist));
		for (const auto CheckPoint : CheckPoints)
		{
			const auto& Location = CheckPoint->GetActorLocation();
			const auto Dist = FVector::Distance(SelfLocation, Location);
			// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString::SanitizeFloat(Dist));
			if (Dist < MinDist)
			{
				Ret = CheckPoint;
				MinDist = Dist;
			}
		}
	}
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, Ret.ToString());
	return Ret;
}

EBTNodeResult::Type UBTTask_FindNextCheckPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("I'm running!"));
	ATPSAIController* AIController = Cast<ATPSAIController>(OwnerComp.GetAIOwner());
	if (AIController)
	{
		ATPSAICharacter* AICharacter = Cast<ATPSAICharacter>(AIController->GetPawn());
		if (AICharacter)
		{
			AMapCheckPoint* CurrentCheckPoint = AICharacter->GetCurrentCheckPoint();
			if (CurrentCheckPoint)
			{
				AMapCheckPoint* NextCheckPoint = CurrentCheckPoint->GetRandomNearbyCheckPoint();
				AICharacter->SetCurrentCheckPoint(NextCheckPoint);
				AIController->UpdateTargetLocation(NextCheckPoint->GetActorLocation());
				return EBTNodeResult::Succeeded;
			}
			else
			{
				TArray<AActor*> AllCheckPoints;
				UGameplayStatics::GetAllActorsWithTag(AICharacter, "CheckPoint", AllCheckPoints);
				AMapCheckPoint* FirstCheckPoint = Cast<AMapCheckPoint>(FindNearestLocation(AllCheckPoints, AICharacter->GetActorLocation()));
				if (FirstCheckPoint)
				{
					AICharacter->SetCurrentCheckPoint(FirstCheckPoint);
					AIController->UpdateTargetLocation(FirstCheckPoint->GetActorLocation());
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}
	return EBTNodeResult::Failed;
}
