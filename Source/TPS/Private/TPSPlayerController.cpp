// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerController.h"

#include "TPSGameStateBase.h"
#include "TPSPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TPS/TPSGameModeBase.h"

void ATPSPlayerController::AddWidget(UUserWidget*& UserWidget, UClass* WidgetClass, bool bShouldShow)
{
	UserWidget = CreateWidget(this, WidgetClass);
	if (UserWidget)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Orange, GetNameSafe(UserWidget));
		if(!bShouldShow)
		{
			UserWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		UserWidget->AddToViewport();
	}
}

void ATPSPlayerController::SetupUI()
{
	if (IsLocalPlayerController())
	{
		AddWidget(TitleUI, TitleUIClass, true);
		AddWidget(PrepareUI, PrepareUIClass, false);
		AddWidget(HudUI, HudUIClass, false);
		AddWidget(HitResponseUI, HitResponseUIClass, false);
		AddWidget(KillNotifyUI, KillNotifyUIClass, true);
		AddWidget(ScoreBoardUI, ScoreBoardUIClass, false);
		AddWidget(GameEndUI, GameEndUIClass, false);
		if(HasAuthority())
		{
			AddWidget(SkipWaitingUI, SkipWaitingUIClass, true);
		}
	}
}

void ATPSPlayerController::ServerBroadcastOnLoginFinished_Implementation(bool bIsUISet)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Client Call Login Finished 1");
	if (HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Client Call Login Finished 2");
		ATPSGameModeBase* GMode = Cast<ATPSGameModeBase>(UGameplayStatics::GetGameMode(this));
		if (GMode && bIsUISet)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Client Call Login Finished 3");
			GMode->OnClientLoginFinished();
		}
	}
}

void ATPSPlayerController::PlayerGetGun(ATPSPlayerCharacter* PlayerPawn, AGun* Gun)
{
	PlayerPawn->GetGun(Gun);
}

ATPSPlayerController::ATPSPlayerController()
{
	bReplicates = true;
}

void ATPSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATPSPlayerController, GunClass, COND_OwnerOnly);
}

bool ATPSPlayerController::InputKey(FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad)
{
	return Super::InputKey(Key, Event, AmountDepressed, bGamepad);
}

int ATPSPlayerController::GetTeam() const
{
	ATPSPlayerState* PState = GetPlayerState<ATPSPlayerState>();
	if (ensure(PState))
	{
		return PState->GetTeam();
	}
	return -1;
}

void ATPSPlayerController::SetTeam(const int _Team)
{
	ATPSPlayerState* PState = GetPlayerState<ATPSPlayerState>();
	if (ensure(PState))
	{
		return PState->SetTeam(_Team);
	}
}

void ATPSPlayerController::ClientOnGameStart_Implementation(int ModeIndex)
{
	OnGameStart.Broadcast(ModeIndex);
}

void ATPSPlayerController::ClientOnTimerTick_Implementation(int NewTime)
{
	OnTimerTick.Broadcast(NewTime);
}

void ATPSPlayerController::ClientOnScoreChange_Implementation(int Team, int NewScore)
{
	OnScoreChange.Broadcast(Team, NewScore);
}

void ATPSPlayerController::ClientOnKill_Implementation(APlayerState* DeadPawnState,
	APlayerState* InstigatorState, int DeadTeam, int InstigatorTeam, int DeathCause)
{
	OnKill.Broadcast(DeadPawnState, InstigatorState, DeadTeam, InstigatorTeam, DeathCause);
}

void ATPSPlayerController::ClientOnScoreBoardUpdate_Implementation(const TArray<APlayerState*>& PlayerStates)
{
	OnScoreBoardUpdate.Broadcast(PlayerStates);
}

void ATPSPlayerController::ClientOnGameEnd_Implementation(int WinningTeam)
{
	// DisableInput(this);
	OnGameEnd.Broadcast(WinningTeam);
}

void ATPSPlayerController::OnRespawn(APawn* RespawnPawn)
{
	ServerOnPlayerRespawn(RespawnPawn);
	ClientOnPlayerRespawn(RespawnPawn);
}

void ATPSPlayerController::ClientOnPlayerRespawn_Implementation(APawn* RespawnPawn)
{
	OnPlayerRespawn.Broadcast(RespawnPawn);
	const FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
	HudUI->SetVisibility(ESlateVisibility::Visible);
}

void ATPSPlayerController::ServerOnPlayerRespawn_Implementation(APawn* RespawnPawn)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AGun* Gun = World->SpawnActor<AGun>(GunClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
		ATPSPlayerCharacter* PlayerPawn = GetPawn<ATPSPlayerCharacter>();
		if (PlayerPawn && Gun)
		{
			FTimerHandle TempHandle;
			FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ATPSPlayerController::PlayerGetGun, PlayerPawn, Gun);
			GetWorld()->GetTimerManager().SetTimer(TempHandle, Delegate, 0.5, false);
			// PlayerPawn->GetGun(Gun);
		}
	}
}

void ATPSPlayerController::ServerUpdateGunClass_Implementation(UClass* NewGunClass)
{
	GunClass = NewGunClass;
}

void ATPSPlayerController::ClientUpdatePlayerNumUI_Implementation(int NewPlayerNum)
{
	OnPlayerNumChange.Broadcast(NewPlayerNum);
}

void ATPSPlayerController::ClientPostLogin_Implementation()
{
	SetupUI();

	ServerBroadcastOnLoginFinished((TitleUI != nullptr));
}

void ATPSPlayerController::ShowScoreBoard_Implementation()
{
}