// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TPSGameStateBase.generated.h"

/**
 * 
 */

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerNumChange, int, NewPlayerNum);

UCLASS()
class TPS_API ATPSGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

private:
	UPROPERTY(Replicated)
	int CurrentPlayerNum;

	UPROPERTY(Replicated)
	int MaxPlayerNum;

	UPROPERTY(Replicated)
	int TimeSec;

	UPROPERTY(Replicated)
	int ScoreA;

	UPROPERTY(Replicated)
	int ScoreB;

	UPROPERTY(Replicated)
	int MaxScore;

	UPROPERTY(Replicated)
	int AliveNumA;

	UPROPERTY(Replicated)
	int AliveNumB;

	UPROPERTY(Replicated)
	int CurrentStates;

	UPROPERTY(Replicated)
	TArray<APlayerState*> PlayerStates;

	UFUNCTION(NetMulticast, Reliable)
	void BroadcastPlayerNumChange(int NewPlayerNum);

protected:
	UFUNCTION()
	void AddScoreA() { ++ScoreA; }

	UFUNCTION()
	void AddScoreB() { ++ScoreB; }

public:
	enum EGameStates
	{
		None,
		Prepare,
		InGame,
		End
	};

	ATPSGameStateBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void AddPlayerState(APlayerState* PlayerState) override;

	UFUNCTION(Server, Reliable)
	void ServerUpdatePlayerNum(int NumDelta);

	void UpdatePlayerNum(int NumDelta);

	void SetStates(int States);

	UFUNCTION(BlueprintCallable)
	void InitGameState(int InMaxPlayerNum, int InTimeSec, int InMaxScore);

	UFUNCTION(BlueprintCallable)
	int GetMaxPlayerNum() const { return MaxPlayerNum; }

	UFUNCTION(BlueprintCallable)
	int GetCurrentPlayerNum() const { return CurrentPlayerNum; }

	UFUNCTION(BlueprintCallable)
	int GetTimeSec() const { return TimeSec; }

	UFUNCTION(Server, Reliable)
	void ServerTimerTick();

	UFUNCTION(BlueprintCallable)
	int GetScoreA() const { return ScoreA; }

	UFUNCTION(BlueprintCallable)
	int GetScoreB() const { return ScoreB; }

	UFUNCTION(Server, Reliable)
	void ServerAddScore(int Team);

	UFUNCTION(BlueprintCallable)
	int GetMaxScore() const { return MaxScore; }

	UFUNCTION(BlueprintCallable)
	const TArray<APlayerState*>& GetPlayerStates() const { return PlayerStates; }
};

inline void ATPSGameStateBase::InitGameState(int InMaxPlayerNum, int InTimeSec, int InMaxScore)
{
	CurrentPlayerNum = 0;
	ScoreA = ScoreB = 0;

	MaxPlayerNum = InMaxPlayerNum;
	TimeSec = InTimeSec;
	MaxScore = InMaxScore;
}
