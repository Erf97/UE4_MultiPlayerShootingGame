// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapCheckPoint.generated.h"

UCLASS()
class TPS_API AMapCheckPoint : public AActor
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category=CheckPoint)
	USceneComponent* DefaultScene;

	UPROPERTY(EditAnywhere, Category=CheckPoint)
	AMapCheckPoint* NextCheckPoint;

	UPROPERTY(EditAnywhere, Category=CheckPoint)
	TArray<AMapCheckPoint*> NearbyCheckPoints;
	
public:	
	// Sets default values for this actor's properties
	AMapCheckPoint();

	AMapCheckPoint* GetNextCheckPoint() const;

	AMapCheckPoint* GetRandomNearbyCheckPoint() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
