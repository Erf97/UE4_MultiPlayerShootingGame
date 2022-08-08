// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGameStateBase.h"

#include "TPSPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ATPSGameStateBase::BroadcastPlayerNumChange_Implementation(int NewPlayerNum)
{
	// OnPlayerNumChange.Broadcast(NewPlayerNum);
}

ATPSGameStateBase::ATPSGameStateBase()
{
	bReplicates = true;
}

void ATPSGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPSGameStateBase, CurrentPlayerNum);
	DOREPLIFETIME(ATPSGameStateBase, MaxPlayerNum);
	DOREPLIFETIME(ATPSGameStateBase, TimeSec);
	DOREPLIFETIME(ATPSGameStateBase, ScoreA);
	DOREPLIFETIME(ATPSGameStateBase, ScoreB);
	DOREPLIFETIME(ATPSGameStateBase, MaxScore);
	DOREPLIFETIME(ATPSGameStateBase, AliveNumA);
	DOREPLIFETIME(ATPSGameStateBase, AliveNumB);
	DOREPLIFETIME(ATPSGameStateBase, CurrentStates);
}

void ATPSGameStateBase::AddPlayerState(APlayerState* PlayerState)
{
	if (HasAuthority())
	{
		PlayerStates.AddUnique(PlayerState);
	}
}

void ATPSGameStateBase::ServerUpdatePlayerNum_Implementation(int NumDelta)
{
	CurrentPlayerNum += NumDelta;
	// BroadcastPlayerNumChange(CurrentPlayerNum);
}

void ATPSGameStateBase::UpdatePlayerNum(int NumDelta)
{
	ServerUpdatePlayerNum(NumDelta);
}

void ATPSGameStateBase::SetStates(int States)
{
	CurrentStates = States;
}

void ATPSGameStateBase::ServerAddScore_Implementation(int Team)
{
	if(Team == 0)
	{
		AddScoreA();
	}
	else
	{
		AddScoreB();
	}
}

void ATPSGameStateBase::ServerTimerTick_Implementation()
{
	--TimeSec; 
}

