// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "TPSGameInstance.generated.h"

/**
 * 
 */

class IOnlineSubsystem;

UCLASS()
class TPS_API UTPSGameInstance : public UGameInstance
{
	GENERATED_BODY()

	/*-------- UI --------*/

private:
protected:
	/*-------- UI End --------*/

	/*-------- Host Configs --------*/

private:
	int ModeIndex;

	int MapIndex;

	int MaxRounds;

	int MaxScores;

	int MaxTime; // Minutes

	int MaxPlayerNum;

	bool bEnableBot;

	bool bAllowTeamDamage;

	FName ServerName;

	UPROPERTY(EditDefaultsOnly, Category=Map)
	TArray<FName> TeamDeadMatchMapNames;

	UPROPERTY(EditDefaultsOnly, Category=Map)
	TArray<FName> TeamMatchMapNames;

public:
	UFUNCTION(BlueprintCallable)
	void InitHostConfig(int InModeIndex, int InMapIndex, int InMaxPlayerNum, int InMaxRounds, int InMaxScores,
	                    int InMaxTime, bool bInEnableBot, bool bInAllowTeamDamage);

	UFUNCTION(BlueprintCallable)
	int GetModeIndex() const { return ModeIndex; }

	UFUNCTION(BlueprintCallable)
	int GetMapIndex() const { return MapIndex; }

	UFUNCTION(BlueprintCallable)
	int GetMaxRounds() const { return MaxRounds; }

	UFUNCTION(BlueprintCallable)
	int GetMaxScores() const { return MaxScores; }

	UFUNCTION(BlueprintCallable)
	int GetMaxTime() const { return MaxTime; }

	UFUNCTION(BlueprintCallable)
	int GetMaxPlayerNum() const { return MaxPlayerNum; }

	UFUNCTION(BlueprintCallable)
	bool IsEnableBot() const { return bEnableBot; }

	UFUNCTION(BlueprintCallable)
	bool IsAllowTeamDamage() const { return bAllowTeamDamage; }

	UFUNCTION(BlueprintCallable)
	FName GetMapName(const int Index) const
	{
		FName MapName;
		switch (ModeIndex)
		{
		case 0:
			MapName = TeamDeadMatchMapNames[Index];
			break;
		case 1:
			MapName = TeamMatchMapNames[Index];
			break;
		default:
			MapName = "None";
			break;
		}
		return MapName;
	}

	/*-------- Host Configs End --------*/

	/*-------- Game State --------*/

private:
public:
	/*-------- Game State End --------*/

	/*-------- Game Session --------*/

public:
	/*-------- Game Session End --------*/
};
