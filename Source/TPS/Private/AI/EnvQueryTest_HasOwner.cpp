// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnvQueryTest_HasOwner.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h"

UEnvQueryTest_HasOwner::UEnvQueryTest_HasOwner(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Cost = EEnvTestCost::Low;
	SetWorkOnFloatValues(false);
	ValidItemType = UEnvQueryItemType_ActorBase::StaticClass();
}

void UEnvQueryTest_HasOwner::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* QueryOwner = QueryInstance.Owner.Get();
	if (QueryOwner == nullptr)
	{
		return;
	}

	BoolValue.BindData(QueryOwner, QueryInstance.QueryID);
	bool bWantsValid = BoolValue.GetValue();
	
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		AActor* TestedActor = GetItemActor(QueryInstance, It.GetIndex());
		bool bHasOwner = TestedActor->GetOwner() ? true : false;
		It.SetScore(TestPurpose, FilterType, !bHasOwner, bWantsValid);
	}
}
