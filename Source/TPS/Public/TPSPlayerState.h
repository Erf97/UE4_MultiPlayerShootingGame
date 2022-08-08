// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TPSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API ATPSPlayerState : public APlayerState
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing=OnRep_ServerUpdateScoreBoard)
	int Team;

	UPROPERTY(ReplicatedUsing=OnRep_ServerUpdateScoreBoard)
	int Kills;

	UPROPERTY(ReplicatedUsing=OnRep_ServerUpdateScoreBoard)
	int Deaths;

	UPROPERTY(Replicated)
	FName PlayerName;

public:

	ATPSPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	int GetTeam() const { return Team; }

	void SetTeam(int _Team) { Team = _Team; }

	void SetName(const FName& S);

	UFUNCTION(BlueprintCallable)
	FName GetDefaultName() const { return PlayerName; }

	void UpdateKills();

	void UpdateDeaths();

	UFUNCTION(Server, Unreliable)
	void OnRep_ServerUpdateScoreBoard();

	UFUNCTION(BlueprintCallable)
	int GetKills() const { return Kills; }

	UFUNCTION(BlueprintCallable)
	int GetDeaths() const { return Deaths; }

	FTimerHandle RespawnTimerHandle;
	
};
