// Fill out your copyright notice in the Description page of Project Settings.


#include "MapCheckPoint.h"

// Sets default values
AMapCheckPoint::AMapCheckPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultScene = CreateDefaultSubobject<USceneComponent>("Root");

}

AMapCheckPoint* AMapCheckPoint::GetNextCheckPoint() const
{
	if(ensure(NextCheckPoint))
	{
		return NextCheckPoint;
	}
	return nullptr;
}

AMapCheckPoint* AMapCheckPoint::GetRandomNearbyCheckPoint() const
{
	if(ensure(NearbyCheckPoints.Num() != 0))
	{
		const int Index = FMath::RandRange(0, NearbyCheckPoints.Num() - 1);
		return NearbyCheckPoints[Index];
	}
	return nullptr;
}

// Called when the game starts or when spawned
void AMapCheckPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMapCheckPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

