// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerState.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TPS/TPSGameModeBase.h"

ATPSPlayerState::ATPSPlayerState()
{
	bReplicates = true;
}

void ATPSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPSPlayerState, Team);
	DOREPLIFETIME(ATPSPlayerState, Kills);
	DOREPLIFETIME(ATPSPlayerState, Deaths);
	DOREPLIFETIME(ATPSPlayerState, PlayerName);
}

void ATPSPlayerState::SetName(const FName& S)
{
	PlayerName = S;
}

void ATPSPlayerState::UpdateKills()
{
	++Kills;
	FTimerHandle T;
	GetWorld()->GetTimerManager().SetTimer(T, this, &ATPSPlayerState::OnRep_ServerUpdateScoreBoard_Implementation, 0.5);
}

void ATPSPlayerState::UpdateDeaths()
{
	++Deaths;
	FTimerHandle T;
	GetWorld()->GetTimerManager().SetTimer(T, this, &ATPSPlayerState::OnRep_ServerUpdateScoreBoard_Implementation, 0.5);
}

void ATPSPlayerState::OnRep_ServerUpdateScoreBoard_Implementation()
{
	if (HasAuthority())
	{
		ATPSGameModeBase* GameModeBase = Cast<ATPSGameModeBase>(UGameplayStatics::GetGameMode(this));
		if(GameModeBase)
		{
			GameModeBase->MulticastScoreBoardUpdate();
		}
	}
}


