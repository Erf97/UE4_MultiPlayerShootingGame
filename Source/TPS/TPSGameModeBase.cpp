// Copyright Epic Games, Inc. All Rights Reserved.


#include "TPSGameModeBase.h"

#include "TPSGameStateBase.h"
#include "TPSPlayerState.h"
#include "TPSGameInstance.h"
#include "AI/TPSAIController.h"
#include "BrainComponent.h"
#include "Kismet/GameplayStatics.h"

void ATPSGameModeBase::RespawnAICharacter(AController* Controller)
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Respawn 1! " + GetNameSafe(Controller));
	ATPSAIController* AIController = Cast<ATPSAIController>(Controller);
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Respawn 2! " + GetNameSafe(AIController));
	if (AIController && bIsGameInProcess)
	{
		FActorSpawnParameters PawnSpawnParameters, GunSpawnParameters;
		PawnSpawnParameters.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		GunSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FVector SpawnLocation;
		UClass* SpawnClass;
		if (AIController->GetTeam() == 0)
		{
			SpawnLocation = GetRandomTeamSpawnLocation(TeamARespawnLocations);
			SpawnClass = TeamAAICharacterClass;
		}
		else
		{
			SpawnLocation = GetRandomTeamSpawnLocation(TeamBRespawnLocations);
			SpawnClass = TeamBAICharacterClass;
		}
		AActor* Actor = GetWorld()->SpawnActor(SpawnClass, &SpawnLocation, &FRotator::ZeroRotator, PawnSpawnParameters);
		ATPSAICharacter* Pawn = Cast<ATPSAICharacter>(Actor);
		AGun* Gun = GetWorld()->SpawnActor<AGun>(GunClass, FVector::ZeroVector, FRotator::ZeroRotator, GunSpawnParameters);
		Pawn->GetGun(Gun);
		AIController->Possess(Pawn);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Respawned!"));
	}
}

void ATPSGameModeBase::RespawnPlayerCharacter(AController* Controller)
{
	ATPSPlayerController* PC = Cast<ATPSPlayerController>(Controller);
	if (PC && bIsGameInProcess)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		FVector SpawnLocation;
		UClass* SpawnClass;
		if (PC->GetTeam() == 0)
		{
			SpawnLocation = GetRandomTeamSpawnLocation(TeamARespawnLocations);
			SpawnClass = TeamAPlayerCharacterClass;
		}
		else
		{
			SpawnLocation = GetRandomTeamSpawnLocation(TeamBRespawnLocations);
			SpawnClass = TeamBPlayerCharacterClass;
		}
		AActor* Actor = GetWorld()->SpawnActor(SpawnClass, &SpawnLocation, &FRotator::ZeroRotator, SpawnParameters);
		ATPSPlayerCharacter* Pawn = Cast<ATPSPlayerCharacter>(Actor);
		PC->Possess(Pawn);
		PC->OnRespawn(Pawn);
	}
}

FVector ATPSGameModeBase::GetRandomTeamSpawnLocation(const TArray<AActor*>& TeamSpawnLocations)
{
	const int Num = TeamSpawnLocations.Num();
	if (ensure(Num))
	{
		const int Index = FMath::RandRange(0, Num - 1);
		return TeamSpawnLocations[Index]->GetActorLocation();
	}
	UE_LOG(LogTemp, Warning, TEXT("The Array TeamSpawnLocations is Empty!"));
	return FVector::ZeroVector;
}

void ATPSGameModeBase::TimerTick()
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Tick!");
	ATPSGameStateBase* GState = GetGameState<ATPSGameStateBase>();
	if (GState && bIsGameInProcess)
	{
		GState->ServerTimerTick();
		for (auto PC : PlayerControllers)
		{
			if (PC)
			{
				PC->ClientOnTimerTick(GState->GetTimeSec());
			}
		}
		if (GState->GetTimeSec() == 0)
		{
			int WinningTeam = 2;
			if (GState->GetScoreA() > GState->GetScoreB())
			{
				WinningTeam = 0;
			}
			else if (GState->GetScoreA() < GState->GetScoreB())
			{
				WinningTeam = 1;
			}
			EndGame(WinningTeam);
		}
	}
}

void ATPSGameModeBase::AddScore(int Team)
{
	ATPSGameStateBase* GState = GetGameState<ATPSGameStateBase>();
	if (GState && bIsGameInProcess)
	{
		GState->ServerAddScore(Team);
		const int NewScore = Team == 0 ? GState->GetScoreA() : GState->GetScoreB();
		for (auto PC : PlayerControllers)
		{
			PC->ClientOnScoreChange(Team, NewScore);
		}
		if (GState->GetScoreA() == GState->GetMaxScore())
		{
			EndGame(0);
		}
		else if (GState->GetScoreB() == GState->GetMaxScore())
		{
			EndGame(1);
		}
	}
}

void ATPSGameModeBase::PostActorCreated()
{
	Super::PostActorCreated();
}

void ATPSGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	if (HasAuthority())
	{
		// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "PostLogin");
		Super::PostLogin(NewPlayer);
		ATPSPlayerController* NP = Cast<ATPSPlayerController>(NewPlayer);
		PlayerControllers.AddUnique(NP);

		ATPSGameStateBase* TPSGameState = GetGameState<ATPSGameStateBase>();
		// GEngine->AddOnScreenDebugMessage(-1, 3.0, FColor::Green, TEXT("TPSGameModeBase.cpp:73, GameState is ") + GetNameSafe(TPSGameState));
		if (TPSGameState)
		{
			TPSGameState->AddPlayerState(NP->GetPlayerState<APlayerState>());
		}

		UTPSGameInstance* GInstance = GetGameInstance<UTPSGameInstance>();
		if (GInstance && NewPlayer->IsLocalPlayerController())
		{
			TPSGameState->InitGameState(GInstance->GetMaxPlayerNum(), GInstance->GetMaxTime(),
			                            GInstance->GetMaxScores());
		}

		if (NP)
		{
			ATPSPlayerState* PState = NP->GetPlayerState<ATPSPlayerState>();
			if (PState)
			{
				PState->SetName(FName("Player" + FString::FromInt(PlayerControllers.Num())));
			}
			NP->ClientPostLogin();
		}
	}
}

void ATPSGameModeBase::OnClientLoginFinished()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Client Login Finished");
	ATPSGameStateBase* TPSGameState = GetGameState<ATPSGameStateBase>();
	TPSGameState->UpdatePlayerNum(1);
	for (auto PC : PlayerControllers)
	{
		PC->ClientUpdatePlayerNumUI(TPSGameState->GetCurrentPlayerNum());
	}
	if (TPSGameState->GetCurrentPlayerNum() == TPSGameState->GetMaxPlayerNum())
	{
		// TODO: Change to timer;
		StartGame();
	}
}

void ATPSGameModeBase::MulticastScoreBoardUpdate()
{
	for (auto PC : PlayerControllers)
	{
		if (PC)
		{
			PC->ClientOnScoreBoardUpdate(GetGameState<ATPSGameStateBase>()->GetPlayerStates());
		}
	}
}

void ATPSGameModeBase::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueNetId,
                                FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueNetId, ErrorMessage);
}

void ATPSGameModeBase::StartPlay()
{
	Super::StartPlay();
	UGameplayStatics::GetAllActorsWithTag(this, "TeamARespawnLocation", TeamARespawnLocations);
	UGameplayStatics::GetAllActorsWithTag(this, "TeamBRespawnLocation", TeamBRespawnLocations);
}

void ATPSGameModeBase::HandleCharacterDead(AController* DeadPawnController, AController* InstigatorController,
                                           bool bIsPlayer, int DeadTeam, int InstigatorTeam, int DeathCause)
{
	if (ensure(DeadPawnController))
	{
		if (InstigatorController)
		{
			if (DeadTeam != InstigatorTeam)
			{
				AddScore(InstigatorTeam);
				ATPSPlayerState* InstigatorState = InstigatorController->GetPlayerState<ATPSPlayerState>();
				if (InstigatorState)
				{
					InstigatorState->UpdateKills();
				}
			}
		}
		ATPSPlayerState* PState = DeadPawnController->GetPlayerState<ATPSPlayerState>();
		if (PState)
		{
			PState->UpdateDeaths();
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Ready to Respawn!"));
			FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(
				this, &ATPSGameModeBase::RespawnCharacter, DeadPawnController, bIsPlayer);
			GetWorld()->GetTimerManager().SetTimer(PState->RespawnTimerHandle, RespawnDelegate, RespawnTime, false);
		}

		for (auto PC : PlayerControllers)
		{
			if (PC && InstigatorController && DeadPawnController)
			{
				PC->ClientOnKill(DeadPawnController->PlayerState, InstigatorController->PlayerState, DeadTeam,
				                 InstigatorTeam, DeathCause);
				// PC->ClientOnScoreBoardUpdate(GetGameState<ATPSGameStateBase>()->PlayerArray);
			}
		}
	}
}

void ATPSGameModeBase::RespawnCharacter(AController* Controller, bool bIsPlayer)
{
	if (ensure(Controller) && bIsGameInProcess)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Go Respawn!"));
		APawn* OldPawn = Controller->GetPawn();
		if(OldPawn)
		{
			OldPawn->Destroy();
		}
		ATPSPlayerState* PState = Controller->GetPlayerState<ATPSPlayerState>();
		if (PState)
		{
			GetWorld()->GetTimerManager().ClearTimer(PState->RespawnTimerHandle);
		}
		if (bIsPlayer)
		{
			RespawnPlayerCharacter(Controller);
		}
		else
		{
			RespawnAICharacter(Controller);
		}
	}
}

void ATPSGameModeBase::StartGame()
{
	bIsGameInProcess = true;
	const UTPSGameInstance* GInstance = Cast<UTPSGameInstance>(GetGameInstance());
	SetAllowTeamDamage(GInstance->IsAllowTeamDamage());

	int Slots[2] = {GInstance->GetMaxPlayerNum() / 2, GInstance->GetMaxPlayerNum() / 2};
	for (int i = 0; i < PlayerControllers.Num(); ++i)
	{
		PlayerControllers[i]->SetTeam(i % 2);
		--Slots[i % 2];
		RespawnCharacter(PlayerControllers[i], true);

		PlayerControllers[i]->ClientOnGameStart(0);
	}

	const bool bEnableBot = GInstance->IsEnableBot();
	if (bEnableBot && HasAuthority())
	{
		FillWithBots(Slots[0], Slots[1]);
	}

	StartTimer();
}

void ATPSGameModeBase::EndGame(int WinningTeam)
{
	bIsGameInProcess = false;
	for (auto PC : PlayerControllers)
	{
		if (PC)
		{
			PC->ClientOnGameEnd(WinningTeam);
		}
	}

	// TODO: Disable AI Controllers;
	for(auto AIC : AIControllers)
	{
		AIC->GetBrainComponent()->StopLogic("Killed");
	}
}

void ATPSGameModeBase::StartTimer()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(GameTimerHandle, this, &ATPSGameModeBase::TimerTick, 1, true);
	}
}

void ATPSGameModeBase::FillWithBots(int NumSlotsA, int NumSlotsB)
{
	for (int _ = 0; _ < NumSlotsA; ++_)
	{
		ATPSAIController* NewAIC = Cast<ATPSAIController>(GetWorld()->SpawnActor(AIControllerClass));
		AIControllers.AddUnique(NewAIC);
		NewAIC->InitPlayerState();
		Cast<ATPSPlayerState>(NewAIC->PlayerState)->SetName(FName("Bot" + FString::FromInt(AIControllers.Num())));
		NewAIC->SetTeam(0);
		ATPSGameStateBase* GState = GetGameState<ATPSGameStateBase>();
		if(GState)
		{
			GState->AddPlayerState(NewAIC->PlayerState);
		}
		
		RespawnCharacter(NewAIC, false);
	}
	for (int _ = 0; _ < NumSlotsB; ++_)
	{
		ATPSAIController* NewAIC = Cast<ATPSAIController>(GetWorld()->SpawnActor(AIControllerClass));
		AIControllers.AddUnique(NewAIC);
		NewAIC->InitPlayerState();
		Cast<ATPSPlayerState>(NewAIC->PlayerState)->SetName(FName("Bot" + FString::FromInt(AIControllers.Num())));
		NewAIC->SetTeam(1);
		ATPSGameStateBase* GState = GetGameState<ATPSGameStateBase>();
		if(GState)
		{
			GState->AddPlayerState(NewAIC->PlayerState);
		}
		RespawnCharacter(NewAIC, false);
	}
}
