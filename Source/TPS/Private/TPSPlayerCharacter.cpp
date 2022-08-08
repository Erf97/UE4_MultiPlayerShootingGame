// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerCharacter.h"

#include "Gun.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InteractComponent.h"
#include "AnimStatusComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "TPS/TPSGameModeBase.h"


ATPSPlayerCharacter::ATPSPlayerCharacter() : Super()
{
	APlayerState* PState = GetPlayerState();
	if (PState)
	{
		PState->SetIsABot(false);
	}

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");

	// OnDeadViewSpringArm = CreateDefaultSubobject<USpringArmComponent>("OnDeadViewSpringArm");
	// OnDeadViewCamera = CreateDefaultSubobject<UCameraComponent>("OnDeadViewCamera");

	Camera->SetupAttachment(SpringArmComponent);
	SpringArmComponent->SetupAttachment(RootComponent);

	// OnDeadViewCamera->SetupAttachment(OnDeadViewSpringArm);
	// OnDeadViewSpringArm->SetupAttachment(RootComponent);
}

void ATPSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("TurnX", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnY", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ATPSCharacter::Interact);
	
	PlayerInputComponent->BindAction("ReloadAmmo", IE_Pressed, this, &ATPSCharacter::ReloadAmmo);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ATPSCharacter::Aim);
	PlayerInputComponent->BindAction("AimRelease", IE_Released, this, &ATPSCharacter::AimRelease);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATPSCharacter::Fire);
	PlayerInputComponent->BindAction("FireRelease", IE_Released, this, &ATPSCharacter::FireRelease);

	PlayerInputComponent->BindAction("Rush", IE_Pressed, this, &ATPSCharacter::Rush);
	PlayerInputComponent->BindAction("RushRelease", IE_Released, this, &ATPSCharacter::RushRelease);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATPSCharacter::TPSCrouch);
	PlayerInputComponent->BindAction("CrouchRelease", IE_Released, this, &ATPSCharacter::CrouchRelease);
}

void ATPSPlayerCharacter::OnDead_Implementation(APawn* InstigatorPawn, const FVector& Force, FName Bone)
{
	Super::OnDead_Implementation(InstigatorPawn, Force, Bone);
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Child!"));
	if (AnimStatus->bIsAimingTap)
	{
		AimTapReleaseImpl();
	}
	APlayerController* PController = Cast<APlayerController>(GetController());
	if (PController)
	{
		DisableInput(PController);
		PController->UnPossess();
		ATPSGameModeBase* GameMode = Cast<ATPSGameModeBase>(UGameplayStatics::GetGameMode(this));
		if (GameMode)
		{
			ATPSCharacter* InstigatorCharacter = Cast<ATPSCharacter>(InstigatorPawn);
			int DeadCause = 0;
			if(Bone == FName("head"))
			{
				DeadCause = 1;
			}
			GameMode->HandleCharacterDead(PController, InstigatorPawn->GetController(), true, Team,
			                              InstigatorCharacter->Team, DeadCause);
		}
	}
}

void ATPSPlayerCharacter::GetGun(AGun* NewGun)
{
	Super::GetGun(NewGun);
	ClientOnGetGun(NewGun);
	ClientOnGetGun_Implementation(NewGun);
}

void ATPSPlayerCharacter::DiscardGun()
{
	if (AnimStatus->bIsAimingTap)
	{
		AimTapReleaseImpl();
	}
	if (CrossHairWidget)
	{
		CrossHairWidget->RemoveFromViewport();
		CrossHairWidget = nullptr;
	}
	ClientOnDiscardGun(Gun);
	Super::DiscardGun();
}

void ATPSPlayerCharacter::ClientOnGetGun_Implementation(AGun* NewGun)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, "ClientGetGun!");
	OnGetGun.Broadcast(NewGun);
	// NewGun->ClientOnAmmoChange_Implementation();
}

void ATPSPlayerCharacter::ClientOnDiscardGun_Implementation(AGun* OldGun)
{
	OnDiscardGun.Broadcast(OldGun);
}

void ATPSPlayerCharacter::ClientOnHitOtherCharacter_Implementation(bool bIsOtherCharacterKilled)
{
	OnHitOtherCharacter.Broadcast(bIsOtherCharacterKilled);
}

ATPSPlayerController* ATPSPlayerCharacter::GetPlayerController() const
{
	return Cast<ATPSPlayerController>(GetController());
}

void ATPSPlayerCharacter::AimHoldImpl()
{
	ServerAimHoldImpl();
	ServerAimHoldImpl_Implementation();
}

void ATPSPlayerCharacter::ServerAimHoldImpl_Implementation()
{
	GetAnimStatus()->bIsAimingHold = true;
	SetMovementMode(Walking);
	AimModeChangeBroadcast();
}

void ATPSPlayerCharacter::AimTapImpl()
{
	ServerAimTapImpl();
	ServerAimTapImpl_Implementation();
}

void ATPSPlayerCharacter::ServerAimTapImpl_Implementation()
{
	GetAnimStatus()->bIsAimingTap = true;
	SetMovementMode(Walking);
	APlayerController* PC = Cast<APlayerController>(GetController());
	PC->SetViewTargetWithBlend(GetGunActor(), GetAnimStatus()->ViewChangeBlendTime);
	AimModeChangeBroadcast();
}

void ATPSPlayerCharacter::AimHoldReleaseImpl()
{
	ServerAimHoldReleaseImpl();
	ServerAimHoldReleaseImpl_Implementation();
}

void ATPSPlayerCharacter::ServerAimHoldReleaseImpl_Implementation()
{
	GetAnimStatus()->bIsAimingHold = false;
	if (!AnimStatus->bIsCrouching)
	{
		SetMovementMode(Jogging);
	}
	AimModeChangeBroadcast();
}

void ATPSPlayerCharacter::AimTapReleaseImpl()
{
	ServerAimTapReleaseImpl();
	ServerAimTapReleaseImpl_Implementation();
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->SetViewTargetWithBlend(this, .1);
	}
}

void ATPSPlayerCharacter::ServerAimTapReleaseImpl_Implementation()
{
	GetAnimStatus()->bIsAimingTap = false;
	if (!AnimStatus->bIsCrouching)
	{
		SetMovementMode(Jogging);
	}
	AimModeChangeBroadcast();
}

void ATPSPlayerCharacter::SetMovementMode(int Mode)
{
	ServerSetMovementMode_Implementation(Mode);
	ServerSetMovementMode(Mode);
}

void ATPSPlayerCharacter::ServerSetMovementMode_Implementation(int Mode)
{
	switch (Mode)
	{
	case Jogging:
		Enter8DirMode();
		GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
		break;
	case Walking:
		Enter8DirMode();
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		break;
	case Rushing:
		Exit8DirMode();
		GetCharacterMovement()->MaxWalkSpeed = RushSpeed;
		break;
	default:
		break;
	}
}

void ATPSPlayerCharacter::Enter8DirMode()
{
	// GetController()->SetControlRotation(GetActorRotation());
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void ATPSPlayerCharacter::Exit8DirMode()
{
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ATPSPlayerCharacter::FireImpl()
{
	if(GetCurrentMontage() == AnimStatus->GetReloadAnim())
	{
		return;
	}
	UAnimMontage* FireAnim = GetAnimStatus()->GetFireAnim();
	if (FireAnim && Gun && Gun->GetCurrentAmmo() >= Gun->GetConsumedAmmo())
	{
		if (GetAnimStatus()->bIsRushing)
		{
			SetMovementMode(Jogging);
		}
		PlayAnimMontage(FireAnim);
		int AimingStatus;
		if (GetAnimStatus()->bIsAimingHold) AimingStatus = 1;
		else if (GetAnimStatus()->bIsAimingTap) AimingStatus = 2;
		else AimingStatus = 0;
		GetGunActor()->OnPlayerFire(AimingStatus);
		GetGunActor()->AddAutoFireCount();
	}
}

void ATPSPlayerCharacter::AimModeChangeBroadcast()
{
	int AimState = GetAimState();
	OnAimStateChanged.Broadcast(AimState);
}

void ATPSPlayerCharacter::FireChangeBroadCast(bool BIsStart)
{
	int AimState = GetAimState();
	OnFireChanged.Broadcast(AimState, BIsStart);
}

int ATPSPlayerCharacter::GetAimState()
{
	int AimState;
	if (GetAnimStatus()->bIsAimingTap) AimState = 2;
	else if (GetAnimStatus()->bIsAimingHold) AimState = 1;
	else AimState = 0;
	return AimState;
}

void ATPSPlayerCharacter::OnRep_SetGunStatus()
{
	Super::OnRep_SetGunStatus();

	if (Gun && IsLocallyControlled())
	{
		if (!CrossHairWidget)
		{
			CrossHairWidget = CreateWidget(GetPlayerController(), Gun->CrossHairWidgetClass);
		}
		if (CrossHairWidget)
		{
			CrossHairWidget->AddToViewport();
		}
	}
}
void ATPSPlayerCharacter::Interact()
{
	Super::Interact();
}

void ATPSPlayerCharacter::ReloadAmmo()
{
	Super::ReloadAmmo();
}

void ATPSPlayerCharacter::Aim()
{
	if (bHasWeapon)
	{
		bDontAim = false;
		if (AnimStatus->bIsRushing)
		{
			SetMovementMode(Jogging);
		}
		if (AnimStatus->bIsAimingHold)
		{
			// ...
		}
		else if (AnimStatus->bIsAimingTap)
		{
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(AnimStatus->AimHoldCheckTimerHandle);
			GetWorld()->GetTimerManager().SetTimer(AnimStatus->AimHoldCheckTimerHandle, this,
			                                       &ATPSPlayerCharacter::AimHoldImpl, 0.2, false);
		}
	}
}

void ATPSPlayerCharacter::AimRelease()
{
	if (bHasWeapon)
	{
		GetWorld()->GetTimerManager().ClearTimer(AnimStatus->AimHoldCheckTimerHandle);
		if (AnimStatus->bIsAimingHold)
		{
			AimHoldReleaseImpl();
		}
		else if (AnimStatus->bIsAimingTap)
		{
			AimTapReleaseImpl();
		}
		else
		{
			if (!bDontAim)
				AimTapImpl();
		}
	}
}

void ATPSPlayerCharacter::Fire()
{
	if(GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		return;
	}
	if (Gun && Gun->GetCurrentAmmo() >= Gun->GetConsumedAmmo())
	{
		FireImpl();
		switch (Gun->GetFireMode())
		{
		case AGun::EFireMode::Auto:
			GetWorld()->GetTimerManager().SetTimer(Gun->AutoFireTimerHandle, this, &ATPSPlayerCharacter::FireImpl,
			                                       Gun->GetAutoFireDelta(), true);
		case AGun::EFireMode::Single:
		default:
			break;
		}
		FireChangeBroadCast(true);
	}
}

void ATPSPlayerCharacter::FireRelease()
{
	if (Gun)
	{
		GetWorld()->GetTimerManager().ClearTimer(Gun->AutoFireTimerHandle);
		Gun->ResetAutoFireCount();
		FireChangeBroadCast(false);
	}
}

void ATPSPlayerCharacter::Rush()
{
	if(GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Rush"));
	if (AnimStatus->bIsAimingTap)
	{
		AimTapReleaseImpl();
	}
	if (AnimStatus->bIsAimingHold)
	{
		AimHoldReleaseImpl();
		bDontAim = true;
	}
	SetMovementMode(Rushing);
}

void ATPSPlayerCharacter::RushRelease()
{
	if (AnimStatus->bIsAimingHold || AnimStatus->bIsAimingTap)
	{
		SetMovementMode(Walking);
	}
	else
	{
		SetMovementMode(Jogging);
	}
	AimModeChangeBroadcast();
}

void ATPSPlayerCharacter::TPSCrouch()
{
	ServerTPSCrouch();
	ServerTPSCrouch_Implementation();
}

void ATPSPlayerCharacter::ServerTPSCrouch_Implementation()
{
	AnimStatus->bIsCrouching = true;
	SetMovementMode(Walking);
}

void ATPSPlayerCharacter::CrouchRelease()
{
	ServerCrouchRelease();
	ServerCrouchRelease_Implementation();
}

void ATPSPlayerCharacter::ServerCrouchRelease_Implementation()
{
	AnimStatus->bIsCrouching = false;
	if (AnimStatus->bIsAimingHold || AnimStatus->bIsAimingTap)
	{
		SetMovementMode(Walking);
	}
	else
	{
		SetMovementMode(Jogging);
	}
}
