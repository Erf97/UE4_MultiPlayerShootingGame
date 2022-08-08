// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AI/TPSAICharacter.h"
#include "TPSPlayerCharacter.h"
#include "TPSPlayerController.h"
#include "TPSGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API ATPSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:

	bool bIsGameInProcess = false;
	
	UPROPERTY(EditDefaultsOnly, Category=Character)
	TSubclassOf<ATPSAICharacter> SingleAICharacterClass;

	UPROPERTY(EditDefaultsOnly, Category=Character)
	TSubclassOf<ATPSAICharacter> TeamAAICharacterClass;

	UPROPERTY(EditDefaultsOnly, Category=Character)
	TSubclassOf<ATPSAICharacter> TeamBAICharacterClass;

	UPROPERTY(EditDefaultsOnly, Category=Character)
	TSubclassOf<ATPSPlayerCharacter> SinglePlayerCharacterClass;

	UPROPERTY(EditDefaultsOnly, Category=Character)
	TSubclassOf<ATPSPlayerCharacter> TeamAPlayerCharacterClass;

	UPROPERTY(EditDefaultsOnly, Category=Character)
	TSubclassOf<ATPSPlayerCharacter> TeamBPlayerCharacterClass;

	UPROPERTY(EditDefaultsOnly, Category=GamePlay)
	TSubclassOf<AGun> GunClass;

	UPROPERTY(EditDefaultsOnly, Category=GamePlay)
	TSubclassOf<AAIController> AIControllerClass;

	UPROPERTY()
	TArray<AActor*> TeamARespawnLocations;

	UPROPERTY()
	TArray<AActor*> TeamBRespawnLocations;

	UPROPERTY()
	TArray<ATPSAIController*> AIControllers;

	UPROPERTY()
	TArray<ATPSPlayerController*> PlayerControllers;

	UPROPERTY(EditDefaultsOnly, Category=MultiPlayer)
	bool bAllowTeamDamage = true;

	int MaxPlayerNum;

	int MaxAINum;

	UPROPERTY(EditDefaultsOnly, Category=MultiPlayer)
	float RespawnTime = 5.0f;

	FTimerHandle GameTimerHandle;

protected:
	virtual void RespawnAICharacter(AController* Controller);

	virtual void RespawnPlayerCharacter(AController* Controller);

	FVector GetRandomTeamSpawnLocation(const TArray<AActor*>& TeamSpawnLocations);
	// @Fixme: Perhaps need to move this to a child class;

	void TimerTick();

	void AddScore(int Team);

public:
	
	virtual void PostActorCreated() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
	                      FString& ErrorMessage) override;

	virtual void StartPlay() override;

	virtual void HandleCharacterDead(AController* DeadPawnController, AController* InstigatorController, bool bIsPlayer,
	                                 int DeadTeam, int InstigatorTeam, int DeathCause);

	virtual void RespawnCharacter(AController* Controller, bool bIsPlayer);

	UFUNCTION(BlueprintCallable)
	virtual void StartGame();
	
	virtual void EndGame(int WinningTeam);

	void StartTimer();

	void FillWithBots(int NumSlotsA, int NumSlotsB);

	void OnClientLoginFinished();

	bool IsAllowTeamDamage() const { return bAllowTeamDamage; }

	void MulticastScoreBoardUpdate();

	UFUNCTION(BlueprintCallable)
	void SetAllowTeamDamage(const bool bIsAllowTeamDamage)
	{
		bAllowTeamDamage = bIsAllowTeamDamage;
	}
};
