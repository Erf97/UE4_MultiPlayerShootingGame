// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Gun.h"
#include "TPSPlayerController.generated.h"

/**
 * 
 */
class ATPSPlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerNumChange, int, NewPlayerNum);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStart, int, GameModeIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerTick, int, NewTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreChange, int, Team, int, NewScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreBoardUpdate, const TArray<APlayerState*>&, PlayerStates);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoginFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerRespawn, APawn*, RespawnedPawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnKill, APlayerState*, DeadPawnState, APlayerState*,
											  InstigatorState,
											  int, DeadTeam, int, InstigatorTeam, int, DeathCause);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameEnd, int, WinningTeam);


UCLASS()
class TPS_API ATPSPlayerController : public APlayerController
{
	GENERATED_BODY()

private:

	UPROPERTY()
	UUserWidget* TitleUI;

	UPROPERTY()
	UUserWidget* PrepareUI;

	UPROPERTY()
	UUserWidget* HudUI;

	UPROPERTY()
	UUserWidget* HitResponseUI;

	UPROPERTY()
	UUserWidget* KillNotifyUI;

	UPROPERTY()
	UUserWidget* ScoreBoardUI;
	
	UPROPERTY()
	UUserWidget* GameEndUI;

	UPROPERTY()
	UUserWidget* SkipWaitingUI;

	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<UUserWidget> TitleUIClass;

	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<UUserWidget> PrepareUIClass;

	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<UUserWidget> HudUIClass;

	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<UUserWidget> HitResponseUIClass;

	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<UUserWidget> KillNotifyUIClass;

	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<UUserWidget> ScoreBoardUIClass;

	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<UUserWidget> GameEndUIClass;

	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<UUserWidget> SkipWaitingUIClass;
	
	UPROPERTY(EditDefaultsOnly, Category=UI, Replicated)
	TSubclassOf<AGun> GunClass;

	UPROPERTY(EditDefaultsOnly, Category=GamePlay)
	APawn* ObserverPawn;

protected:

	void AddWidget(UUserWidget*& UserWidget, UClass* WidgetClass, bool bShouldShow=false);
	
	void SetupUI();

	UFUNCTION(Server, Reliable)
	void ServerBroadcastOnLoginFinished(bool bIsUISet);

	UFUNCTION()
	void PlayerGetGun(ATPSPlayerCharacter* PlayerPawn, AGun* Gun);


public:

	ATPSPlayerController();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual bool InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad) override;
	
	int GetTeam() const;

	void SetTeam(const int _Team);

	UFUNCTION(BlueprintNativeEvent)
	void ShowScoreBoard();

	UFUNCTION(Client, Reliable)
	void ClientPostLogin();

	UFUNCTION(Client, Reliable)
	void ClientUpdatePlayerNumUI(int NewPlayerNum);

	UFUNCTION(Client, Reliable)
	void ClientOnGameStart(int ModeIndex);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerUpdateGunClass(UClass* NewGunClass);

	UFUNCTION(Server, Reliable)
	void ServerOnPlayerRespawn(APawn* RespawnPawn);

	UFUNCTION(Client, Reliable)
	void ClientOnPlayerRespawn(APawn* RespawnPawn);

	UFUNCTION(Client, Reliable)
	void ClientOnTimerTick(int NewTime);

	UFUNCTION(Client, Reliable)
	void ClientOnScoreChange(int Team, int NewScore);

	UFUNCTION(Client, Reliable)
	void ClientOnKill(APlayerState* DeadPawnState, APlayerState*
					  InstigatorState,
					  int DeadTeam, int InstigatorTeam, int DeathCause);

	UFUNCTION(Client, Reliable)
	void ClientOnScoreBoardUpdate(const TArray<APlayerState*>& PlayerStates);

	UFUNCTION(Client, Reliable)
	void ClientOnGameEnd(int WinningTeam);

	void OnRespawn(APawn* RespawnPawn);

	UFUNCTION(BlueprintCallable)
	UUserWidget* GetScoreBoardUI() const { return ScoreBoardUI; }

	UFUNCTION(BlueprintCallable)
	UUserWidget* GetSkipWaitingUI() const { return SkipWaitingUI; }
	
	UPROPERTY(BlueprintAssignable)
	FOnPlayerNumChange OnPlayerNumChange;

	UPROPERTY(BlueprintAssignable)
	FOnGameStart OnGameStart;

	UPROPERTY(BlueprintAssignable)
	FOnLoginFinished OnLoginFinished;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerRespawn OnPlayerRespawn;

	UPROPERTY(BlueprintAssignable)
	FOnTimerTick OnTimerTick;

	UPROPERTY(BlueprintAssignable)
	FOnScoreChange OnScoreChange;

	UPROPERTY(BlueprintAssignable)
	FOnKill OnKill;

	UPROPERTY(BlueprintAssignable)
	FOnScoreBoardUpdate OnScoreBoardUpdate;

	UPROPERTY(BlueprintAssignable)
	FOnGameEnd OnGameEnd;
	
};
