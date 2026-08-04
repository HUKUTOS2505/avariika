#include "Game/AvaryoPlayerController.h"

#include "UI/AvaryoHUD.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "InputKeyEventArgs.h"
#include "InputCoreTypes.h"
#include "TimerManager.h"

void AAvaryoPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ForceGameInput();
	if (IsLocalController() && GetWorld() && GetWorld()->URL.HasOption(TEXT("AvStartCustomize")))
	{
		NewGameCustomizationAttempts = 0;
		GetWorldTimerManager().SetTimer(
			NewGameCustomizationTimer, this,
			&AAvaryoPlayerController::OpenCustomizationForNewGame, 0.25f, false);
	}
}

void AAvaryoPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
	// После ServerTravel/повторного владения пешкой ещё раз закрепляем игровой ввод
	ForceGameInput();
}

void AAvaryoPlayerController::ForceGameInput()
{
	if (!IsLocalController())
	{
		return;
	}
	bShowMouseCursor = false;
	ResetIgnoreMoveInput();
	ResetIgnoreLookInput();
	SetInputMode(FInputModeGameOnly());
}

bool AAvaryoPlayerController::InputKey(const FInputKeyEventArgs& Params)
{
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	if (Params.Key == EKeys::F9 && Params.Event == IE_Pressed && bCustomizeOpen)
	{
		if (AAvaryoHUD* H = Cast<AAvaryoHUD>(GetHUD()))
		{
			return H->ToggleCustomizeAppearanceInspector();
		}
		return true;
	}
#endif
	if (Params.Key == EKeys::B && Params.Event == IE_Pressed)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AvCustomizeInput] B pressed in PlayerController InputKey: %s"), *GetNameSafe(this));
		OnCustomizePressed();
		return true;
	}
	if (Params.Key == EKeys::R && Params.Event == IE_Pressed && bCustomizeOpen)
	{
		if (AAvaryoHUD* H = Cast<AAvaryoHUD>(GetHUD()))
		{
			return H->TriggerCustomizeRandomAppearance();
		}
		return true;
	}

	return Super::InputKey(Params);
}

void AAvaryoPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// Esc открывает/закрывает внутриигровое меню. BindKey работает рядом с Enhanced Input (Esc не занят экшеном).
	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &AAvaryoPlayerController::OnEscapePressed);
		InputComponent->BindKey(EKeys::B, IE_Pressed, this, &AAvaryoPlayerController::OnCustomizePressed);
	}
}

void AAvaryoPlayerController::OnEscapePressed()
{
	if (bCustomizeOpen)
	{
		ToggleCustomize();
		return;
	}
	TogglePauseMenu();
}

void AAvaryoPlayerController::OnCustomizePressed()
{
	if (bPauseMenuOpen)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AvCustomizeInput] B ignored: pause menu is open"));
		return;
	}
	ToggleCustomize();
}

void AAvaryoPlayerController::OpenCustomizationForNewGame()
{
	if (!IsLocalController() || bCustomizeOpen)
	{
		return;
	}

	if (Cast<AAvaryoHUD>(GetHUD()))
	{
		UE_LOG(LogTemp, Log, TEXT("[AvMainMenu] New Game opened the existing character-customization flow."));
		ToggleCustomize();
		return;
	}

	++NewGameCustomizationAttempts;
	if (NewGameCustomizationAttempts < 10)
	{
		GetWorldTimerManager().SetTimer(
			NewGameCustomizationTimer, this,
			&AAvaryoPlayerController::OpenCustomizationForNewGame, 0.20f, false);
	}
	else
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AvMainMenu] New Game reached gameplay, but the existing customization HUD was unavailable."));
	}
}

void AAvaryoPlayerController::AvCustomize()
{
	UE_LOG(LogTemp, Warning, TEXT("[AvCustomizeInput] AvCustomize exec on PlayerController"));
	OnCustomizePressed();
}

void AAvaryoPlayerController::LookYaw(float Value)
{
	if (!IsLocalController() || bPauseMenuOpen || bCustomizeOpen || IsLookInputIgnored() || FMath::IsNearlyZero(Value))
	{
		return;
	}
	AddYawInput(Value);
}

void AAvaryoPlayerController::LookPitch(float Value)
{
	if (!IsLocalController() || bPauseMenuOpen || bCustomizeOpen || IsLookInputIgnored() || FMath::IsNearlyZero(Value))
	{
		return;
	}
	AddPitchInput(-Value);
}

void AAvaryoPlayerController::TogglePauseMenu()
{
	if (!IsLocalController())
	{
		return;
	}
	if (bCustomizeOpen)
	{
		ToggleCustomize();
	}
	bPauseMenuOpen = !bPauseMenuOpen;
	if (AAvaryoHUD* H = Cast<AAvaryoHUD>(GetHUD()))
	{
		H->SetPauseMenu(bPauseMenuOpen);
	}
	SetMenuInputMode(bPauseMenuOpen);
	// Одиночная игра — ставим реальную паузу; в кооп сервер не остановить, меню = оверлей.
	if (GetNetMode() == NM_Standalone)
	{
		SetPause(bPauseMenuOpen);
	}
}

void AAvaryoPlayerController::ClosePauseMenu()
{
	if (!IsLocalController() || !bPauseMenuOpen)
	{
		return;
	}
	bPauseMenuOpen = false;
	if (AAvaryoHUD* H = Cast<AAvaryoHUD>(GetHUD()))
	{
		H->SetPauseMenu(false);
	}
	SetMenuInputMode(false);
	if (GetNetMode() == NM_Standalone)
	{
		SetPause(false);
	}
}

void AAvaryoPlayerController::SetMenuInputMode(bool bMenu)
{
	if (!IsLocalController())
	{
		return;
	}
	if (bMenu)
	{
		bShowMouseCursor = true;
		bEnableClickEvents = true;
		bEnableMouseOverEvents = true;
		FInputModeGameAndUI Mode;
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		Mode.SetHideCursorDuringCapture(false);
		SetInputMode(Mode);
	}
	else
	{
		ForceGameInput();
	}
}

void AAvaryoPlayerController::ToggleCustomize()
{
	if (!IsLocalController()) { return; }
	bCustomizeOpen = !bCustomizeOpen;
	UE_LOG(LogTemp, Warning, TEXT("[AvCustomizeInput] ToggleCustomize -> %s, HUD=%s"),
		bCustomizeOpen ? TEXT("open") : TEXT("closed"),
		*GetNameSafe(GetHUD()));
	if (AAvaryoHUD* H = Cast<AAvaryoHUD>(GetHUD()))
	{
		H->SetCustomize(bCustomizeOpen);
	}
	SetMenuInputMode(bCustomizeOpen);
}
