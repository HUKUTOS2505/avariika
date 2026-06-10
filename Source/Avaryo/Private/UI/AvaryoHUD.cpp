#include "UI/AvaryoHUD.h"

#include "AvaryoCharacter.h"
#include "Components/UFlashlightComponent.h"
#include "Components/VitalsComponent.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Items/APickupItem.h"

namespace AvaryoHUDStyle
{
	const FLinearColor PanelBG(0.03f, 0.035f, 0.06f, 0.94f);   // тёмная панель
	const FLinearColor CellBG(0.07f, 0.08f, 0.12f, 0.95f);     // ячейка слота
	const FLinearColor Accent(0.95f, 0.45f, 0.04f, 1.f);       // аварийный оранжевый
	const FLinearColor AccentDim(0.6f, 0.27f, 0.02f, 1.f);     // акцентная кромка
	const FLinearColor BoxBG(0.04f, 0.05f, 0.09f, 0.88f);      // плашки по центру
	const FLinearColor TextMain(0.96f, 0.96f, 0.98f, 1.f);
	const FLinearColor TextDim(0.62f, 0.64f, 0.72f, 1.f);
	const FLinearColor BarFill(0.2f, 0.55f, 0.95f, 1.f);       // синий прогресс применения
	const FLinearColor BarBG(0.f, 0.f, 0.f, 0.55f);
}

void AAvaryoHUD::DrawHUD()
{
	Super::DrawHUD();

	using namespace AvaryoHUDStyle;

	AAvaryoCharacter* Character = Cast<AAvaryoCharacter>(GetOwningPawn());
	if (!Character || !Canvas)
	{
		return;
	}

	UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
	if (!Font)
	{
		return;
	}

	UVitalsComponent* Vitals = Character->VitalsComponent;
	const float SizeX = Canvas->SizeX;
	const float SizeY = Canvas->SizeY;

	// Текст по центру (по X) на заданной высоте
	auto DrawCentered = [&](const FString& Text, const FLinearColor& Color, float Y, float Scale)
	{
		float W = 0.f, H = 0.f;
		GetTextSize(Text, W, H, Font, Scale);
		DrawText(Text, Color, (SizeX - W) * 0.5f, Y, Font, Scale);
		return H;
	};

	// ---------- Шкалы (слева сверху) ----------
	if (Vitals)
	{
		auto DrawBar = [&](const TCHAR* Label, float Value, const FLinearColor& Color, float Y)
		{
			const float X = 30.f, W = 180.f, H = 14.f;
			DrawRect(BarBG, X, Y, W, H);
			DrawRect(Color, X, Y, W * FMath::Clamp(Value, 0.f, 100.f) / 100.f, H);
			DrawText(Label, TextMain, X + W + 8.f, Y - 2.f, Font, 0.95f);
		};

		float Y = 30.f;
		DrawBar(TEXT("Здоровье"),     Vitals->GetHealth(),  FLinearColor(0.8f, 0.12f, 0.12f), Y); Y += 22.f;
		DrawBar(TEXT("Выносливость"), Vitals->GetStamina(), FLinearColor(0.2f, 0.7f, 0.25f),  Y); Y += 22.f;
		DrawBar(TEXT("Паника"),       Vitals->GetPanic(),   FLinearColor(0.25f, 0.7f, 0.85f), Y); Y += 22.f;
		DrawBar(TEXT("Туалет"),       Vitals->GetBladder(), FLinearColor(0.8f, 0.6f, 0.12f),  Y);

		// Батарея налобного фонаря: ярко-жёлтая когда включён, тусклая когда выключен
		if (UFlashlightComponent* Flashlight = Character->FlashlightComponent)
		{
			Y += 22.f;
			const bool bOn = Flashlight->IsOn();
			DrawBar(bOn ? TEXT("Фонарь [F] вкл") : TEXT("Фонарь [F]"),
				Flashlight->GetBatteryLevel(),
				bOn ? FLinearColor(0.95f, 0.85f, 0.25f) : FLinearColor(0.45f, 0.42f, 0.2f), Y);
		}
	}

	// ---------- Баннер ранения ----------
	if (Vitals && Vitals->IsWounded())
	{
		DrawCentered(TEXT("ВЫ РАНЕНЫ — ползите к команде, вас поднимет аптечка"), FLinearColor::Red, SizeY * 0.4f, 1.6f);
	}

	// ---------- Применение предмета (по центру, как в референсе) ----------
	if (Character->IsUsingItem())
	{
		APickupItem* Casting = Character->GetHeldItem();
		const int32 Percent = FMath::RoundToInt(Character->GetUseProgress() * 100.f);
		const FString Label = FString::Printf(TEXT("Применение: %s  %d%%"),
			Casting ? *Casting->DisplayName.ToString() : TEXT("..."), Percent);

		const float BarW = 340.f, BarH = 20.f;
		const float BoxX = (SizeX - BarW) * 0.5f;
		const float BoxY = SizeY * 0.46f;

		float LabelW = 0.f, LabelH = 0.f;
		GetTextSize(Label, LabelW, LabelH, Font, 1.1f);

		// плашка под текст и полосу
		DrawRect(BoxBG, BoxX - 12.f, BoxY - LabelH - 10.f, BarW + 24.f, LabelH + BarH + 22.f);
		DrawText(Label, TextMain, BoxX, BoxY - LabelH - 4.f, Font, 1.1f);
		DrawRect(BarBG, BoxX, BoxY + 2.f, BarW, BarH);
		DrawRect(BarFill, BoxX, BoxY + 2.f, BarW * Character->GetUseProgress(), BarH);
	}
	// ---------- Подсказка подбора (плашка как в референсе) ----------
	else if (APickupItem* Focused = Character->GetFocusedItem())
	{
		const FString Prompt = FString::Printf(TEXT("[E] Поднять %s"), *Focused->DisplayName.ToString());
		float W = 0.f, H = 0.f;
		GetTextSize(Prompt, W, H, Font, 1.3f);

		const float BoxW = W + 36.f, BoxH = H + 16.f;
		const float BoxX = (SizeX - BoxW) * 0.5f;
		const float BoxY = SizeY * 0.58f;

		DrawRect(BoxBG, BoxX, BoxY, BoxW, BoxH);
		DrawRect(Accent, BoxX, BoxY, BoxW, 3.f); // оранжевая кромка сверху
		DrawText(Prompt, TextMain, BoxX + 18.f, BoxY + 8.f, Font, 1.3f);
	}

	// ---------- Подсказки использования / передачи ----------
	if (!Character->IsUsingItem())
	{
		if (Character->IsOffering())
		{
			APickupItem* Held = Character->GetHeldItem();
			const FString OfferText = FString::Printf(TEXT("Передаёте: %s — тиммейт может забрать [E]"),
				Held ? *Held->DisplayName.ToString() : TEXT("..."));
			DrawCentered(OfferText, Accent, SizeY * 0.62f, 1.15f);
		}
		else if (APickupItem* Held = Character->GetHeldItem())
		{
			FString Hint;
			if (Held->ItemEffect == EItemEffect::Extinguish)
			{
				Hint = TEXT("[ЛКМ] Распылять (держать)  •  [ПКМ] Передать");
			}
			else if (Held->ItemEffect != EItemEffect::None)
			{
				Hint = TEXT("[ЛКМ] Применить (держать)  •  [ПКМ] Передать");
			}
			else
			{
				Hint = TEXT("[ПКМ] Передать (держать)");
			}
			DrawCentered(Hint, TextDim, SizeY * 0.66f, 0.95f);
		}
	}

	// ---------- Нижняя панель инвентаря (как в референсе) ----------
	const float PanelH = 50.f;
	const float PanelY = SizeY - PanelH;
	DrawRect(PanelBG, 0.f, PanelY, SizeX, PanelH);
	DrawRect(AccentDim, 0.f, PanelY, SizeX, 2.f); // тонкая акцентная кромка

	const float CellW = SizeX / AAvaryoCharacter::NumSlots;
	for (int32 SlotIndex = 0; SlotIndex < AAvaryoCharacter::NumSlots; ++SlotIndex)
	{
		APickupItem* Item = Character->GetItemInSlot(SlotIndex);
		const bool bActive = Character->GetActiveSlot() == SlotIndex;
		const float CellX = SlotIndex * CellW;

		if (bActive)
		{
			DrawRect(Accent, CellX + 5.f, PanelY + 5.f, CellW - 10.f, PanelH - 10.f);
		}

		FString ItemName;
		if (Item)
		{
			ItemName = Item->DisplayName.ToString();
			if (Item->Charges >= 0)
			{
				ItemName += FString::Printf(TEXT(" (%d)"), Item->Charges);
			}
		}
		else
		{
			ItemName = SlotIndex == 0 ? TEXT("Нет") : TEXT("Пусто");
		}

		const FString Label = SlotIndex == 0
			? FString::Printf(TEXT("[1] Тяж: %s"), *ItemName)
			: FString::Printf(TEXT("[%d] %s"), SlotIndex + 1, *ItemName);

		float W = 0.f, H = 0.f;
		const float Scale = 1.0f;
		GetTextSize(Label, W, H, Font, Scale);
		DrawText(Label, bActive ? TextMain : TextDim, CellX + (CellW - W) * 0.5f, PanelY + (PanelH - H) * 0.5f, Font, Scale);
	}

	// ---------- Статусы (справа, над панелью) ----------
	if (Vitals)
	{
		TArray<FString> Statuses;
		if (Vitals->IsWounded())          Statuses.Add(TEXT("Ранен"));
		if (Vitals->IsSmoking())          Statuses.Add(TEXT("Курит"));
		if (Vitals->IsPanicking())        Statuses.Add(TEXT("Паникует"));
		if (Vitals->GetBladder() > 70.f)  Statuses.Add(TEXT("Хочет в туалет"));
		if (Vitals->GetStamina() < 20.f)  Statuses.Add(TEXT("Устал"));
		if (Character->IsCarryingHeavy()) Statuses.Add(TEXT("Несёт тяжёлое"));
		if (Vitals->IsIncidentSlowed())   Statuses.Add(TEXT("Санитарный инцидент!"));
		else if (Vitals->IsSoiled())      Statuses.Add(TEXT("Испачкан"));

		float StatusY = PanelY - Statuses.Num() * 22.f - 12.f;
		for (const FString& Status : Statuses)
		{
			float W = 0.f, H = 0.f;
			GetTextSize(Status, W, H, Font, 1.0f);
			DrawText(Status, FLinearColor(1.f, 0.55f, 0.2f), SizeX - W - 30.f, StatusY, Font, 1.0f);
			StatusY += 22.f;
		}
	}
}
