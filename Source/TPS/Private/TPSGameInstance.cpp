// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGameInstance.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"



void UTPSGameInstance::InitHostConfig(int InModeIndex, int InMapIndex, int InMaxPlayerNum, int InMaxRounds,
                                      int InMaxScores, int InMaxTime, bool bInEnableBot, bool bInAllowTeamDamage)
{
	this->ModeIndex = InModeIndex;
	this->MapIndex = InMapIndex;
	this->MaxPlayerNum = InMaxPlayerNum;
	this->MaxRounds = InMaxRounds;
	this->MaxScores = InMaxScores;
	this->MaxTime = InMaxTime;
	this->bEnableBot = bInEnableBot;
	this->bAllowTeamDamage = bInAllowTeamDamage;
}
