#include "UI/AvaryoHUD.h"

#include "AvaryoCharacter.h"
#include "Components/UFlashlightComponent.h"
#include "Components/VitalsComponent.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/TextureRenderTarget2D.h"
#include "EngineUtils.h"
#include "Game/ARunState.h"
#include "GameFramework/PlayerState.h"
#include "Items/APickupItem.h"
#include "World/ARepairable.h"
#include "World/AToilet.h"

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

	// ---------- Монитор оператора (Tab в зоне ГАЗели) ----------
	if (Character->IsMonitorOpen())
	{
		DrawRect(FLinearColor(0.01f, 0.012f, 0.02f, 0.97f), 0.f, 0.f, SizeX, SizeY);
		DrawText(TEXT("КАМЕРЫ БРИГАДЫ — [Tab] закрыть"), AvaryoHUDStyle::Accent, 30.f, 24.f, Font, 1.5f);

		// Плитки 16:9 в две колонки
		const float TileW = FMath::Min(SizeX * 0.42f, 560.f);
		const float TileH = TileW * 9.f / 16.f;
		const float PadX = (SizeX - TileW * 2.f) / 3.f;
		float TileX = PadX;
		float TileY = 80.f;
		int32 Column = 0;

		for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
		{
			AAvaryoCharacter* Crew = *It;

			DrawRect(FLinearColor(0.f, 0.f, 0.f, 1.f), TileX - 2.f, TileY - 2.f, TileW + 4.f, TileH + 4.f);
			if (UTexture* CamImage = Crew->GetChestCamTarget())
			{
				DrawTexture(CamImage, TileX, TileY, TileW, TileH, 0.f, 0.f, 1.f, 1.f);
			}

			// Подпись: имя + состояние
			FString Name = Crew->GetPlayerState() ? Crew->GetPlayerState()->GetPlayerName() : TEXT("Монтёр");
			if (Crew == Character)
			{
				Name += TEXT(" (вы)");
			}
			if (Crew->VitalsComponent && Crew->VitalsComponent->IsWounded())
			{
				Name += TEXT(" — РАНЕН");
			}
			DrawText(Name, AvaryoHUDStyle::TextMain, TileX + 8.f, TileY + 6.f, Font, 1.0f);

			// Мини-шкалы HP и паники под плиткой
			if (UVitalsComponent* CrewVitals = Crew->VitalsComponent)
			{
				const float BarY = TileY + TileH + 6.f;
				DrawRect(AvaryoHUDStyle::BarBG, TileX, BarY, TileW * 0.48f, 8.f);
				DrawRect(FLinearColor(0.8f, 0.12f, 0.12f), TileX, BarY, TileW * 0.48f * CrewVitals->GetHealth() / 100.f, 8.f);
				DrawRect(AvaryoHUDStyle::BarBG, TileX + TileW * 0.52f, BarY, TileW * 0.48f, 8.f);
				DrawRect(FLinearColor(0.25f, 0.7f, 0.85f), TileX + TileW * 0.52f, BarY, TileW * 0.48f * CrewVitals->GetPanic() / 100.f, 8.f);
			}

			if (++Column % 2 == 0)
			{
				TileX = PadX;
				TileY += TileH + 44.f;
			}
			else
			{
				TileX += TileW + PadX;
			}
		}
		return; // монитор перекрывает обычный HUD
	}

	// Текст по центру (по X) на заданной высоте
	auto DrawCentered = [&](const FString& Text, const FLinearColor& Color, float Y, float Scale)
	{
		float W = 0.f, H = 0.f;
		GetTextSize(Text, W, H, Font, Scale);
		DrawText(Text, Color, (SizeX - W) * 0.5f, Y, Font, Scale);
		return H;
	};

	// Плашка-подсказка по центру с оранжевой кромкой ("[E] Поднять ...")
	auto DrawPromptBox = [&](const FString& Prompt)
	{
		float W = 0.f, H = 0.f;
		GetTextSize(Prompt, W, H, Font, 1.3f);

		const float BoxW = W + 36.f, BoxH = H + 16.f;
		const float BoxX = (SizeX - BoxW) * 0.5f;
		const float BoxY = SizeY * 0.58f;

		DrawRect(BoxBG, BoxX, BoxY, BoxW, BoxH);
		DrawRect(Accent, BoxX, BoxY, BoxW, 3.f); // оранжевая кромка сверху
		DrawText(Prompt, TextMain, BoxX + 18.f, BoxY + 8.f, Font, 1.3f);
	};

	// Полоса прогресса с подписью по центру экрана (применение, ремонт)
	auto DrawCastBar = [&](const FString& Label, float Progress, const FLinearColor& Fill)
	{
		const float BarW = 340.f, BarH = 20.f;
		const float BoxX = (SizeX - BarW) * 0.5f;
		const float BoxY = SizeY * 0.46f;

		float LabelW = 0.f, LabelH = 0.f;
		GetTextSize(Label, LabelW, LabelH, Font, 1.1f);

		DrawRect(BoxBG, BoxX - 12.f, BoxY - LabelH - 10.f, BarW + 24.f, LabelH + BarH + 22.f);
		DrawText(Label, TextMain, BoxX, BoxY - LabelH - 4.f, Font, 1.1f);
		DrawRect(BarBG, BoxX, BoxY + 2.f, BarW, BarH);
		DrawRect(Fill, BoxX, BoxY + 2.f, BarW * FMath::Clamp(Progress, 0.f, 1.f), BarH);
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

	// ---------- Забег: задачи, таймер, фаза (справа сверху) ----------
	if (ARunState* Run = ARunState::Get(GetWorld()))
	{
		const int32 Elapsed = FMath::FloorToInt(Run->GetElapsedSeconds());
		const FString Header = FString::Printf(TEXT("ЗАДАЧИ %d/%d   %02d:%02d"),
			Run->GetRepairedCount(), Run->GetTotalObjectives(), Elapsed / 60, Elapsed % 60);

		float HeaderW = 0.f, HeaderH = 0.f;
		GetTextSize(Header, HeaderW, HeaderH, Font, 1.15f);

		const float PanelW = 250.f;
		const float RowH = 20.f;
		const float ListX = SizeX - PanelW - 24.f;
		float ListY = 30.f;

		DrawRect(BoxBG, ListX - 12.f, ListY - 8.f, PanelW + 24.f, HeaderH + Run->GetObjectives().Num() * RowH + 24.f);
		DrawRect(Accent, ListX - 12.f, ListY - 8.f, PanelW + 24.f, 3.f);
		DrawText(Header, Accent, ListX, ListY, Font, 1.15f);
		ListY += HeaderH + 8.f;

		for (const ARepairable* Objective : Run->GetObjectives())
		{
			if (!Objective)
			{
				continue;
			}
			FString Row;
			FLinearColor RowColor;
			if (!Objective->IsBroken())
			{
				Row = FString::Printf(TEXT("[+] %s"), *Objective->DisplayName.ToString());
				RowColor = FLinearColor(0.3f, 0.8f, 0.3f);
			}
			else if (Objective->GetRepairProgress() > 0.f)
			{
				Row = FString::Printf(TEXT("[~] %s (%d%%)"), *Objective->DisplayName.ToString(),
					FMath::RoundToInt(Objective->GetRepairProgress() * 100.f));
				RowColor = Accent;
			}
			else
			{
				Row = FString::Printf(TEXT("[-] %s"), *Objective->DisplayName.ToString());
				RowColor = TextDim;
			}
			DrawText(Row, RowColor, ListX, ListY, Font, 0.95f);
			ListY += RowH;
		}

		// Финал: «Акт выполненных работ» — мемный итоговый отчёт
		if (Run->GetPhase() == ERunPhase::Won || Run->GetPhase() == ERunPhase::Lost)
		{
			const bool bWon = Run->GetPhase() == ERunPhase::Won;
			const TArray<FPlayerRunStats>& AllStats = Run->GetPlayerStats();

			// Максимумы для раздачи званий
			int32 MaxRepairs = 0, MaxWounded = 0, MaxRevives = 0, MaxDrags = 0;
			float MaxPanic = 0.f;
			for (const FPlayerRunStats& S : AllStats)
			{
				MaxRepairs = FMath::Max(MaxRepairs, S.Repairs);
				MaxWounded = FMath::Max(MaxWounded, S.TimesWounded);
				MaxRevives = FMath::Max(MaxRevives, S.Revives);
				MaxDrags   = FMath::Max(MaxDrags,   S.Drags);
				MaxPanic   = FMath::Max(MaxPanic,   S.PanicSeconds);
			}

			const float ReportW = FMath::Min(820.f, SizeX - 80.f);
			const float ReportRowH = 26.f;
			const float ReportH = 170.f + AllStats.Num() * ReportRowH * 2.f;
			const float PX = (SizeX - ReportW) * 0.5f;
			float PY = FMath::Max(40.f, (SizeY - ReportH) * 0.5f);

			DrawRect(PanelBG, PX, PY, ReportW, ReportH);
			DrawRect(Accent, PX, PY, ReportW, 4.f);

			float TY = PY + 18.f;
			TY += DrawCentered(TEXT("АКТ ВЫПОЛНЕННЫХ РАБОТ № 001"), Accent, TY, 1.6f) + 6.f;
			TY += DrawCentered(bWon
				? FString::Printf(TEXT("Объект сдан. Время: %02d:%02d. Заказчик недоволен, но подписал."), Elapsed / 60, Elapsed % 60)
				: TEXT("Объект НЕ сдан: вся бригада выведена из строя. Акт подписан задним числом."),
				bWon ? FLinearColor(0.3f, 0.9f, 0.3f) : FLinearColor(0.95f, 0.25f, 0.25f), TY, 1.1f) + 14.f;

			int32 CrewTotal = 0;
			int32 PlayerIndex = 0;
			for (const FPlayerRunStats& S : AllStats)
			{
				++PlayerIndex;
				FString Name = FString::Printf(TEXT("Монтёр №%d"), PlayerIndex);
				if (S.Character && S.Character->GetPlayerState())
				{
					Name = S.Character->GetPlayerState()->GetPlayerName();
				}

				// Звание: инциденты вне конкуренции, дальше — по лучшему показателю
				FString Title;
				if (S.Incidents > 0)                                   Title = TEXT("Биологическая угроза");
				else if (S.Repairs > 0 && S.Repairs == MaxRepairs)     Title = TEXT("Работник месяца");
				else if (S.Revives > 0 && S.Revives == MaxRevives)     Title = TEXT("Полевой медик");
				else if (S.Drags > 0 && S.Drags == MaxDrags)           Title = TEXT("Эвакуатор");
				else if (S.TimesWounded > 0 && S.TimesWounded == MaxWounded) Title = TEXT("Главный пострадавший");
				else if (S.PanicSeconds > 1.f && S.PanicSeconds >= MaxPanic) Title = TEXT("Паникёр смены");
				else                                                   Title = TEXT("Просто присутствовал");

				// Бухгалтерия: премии и штрафы
				const int32 Balance = S.Repairs * 1500 + S.Revives * 1000 + S.Drags * 500
					- S.TimesWounded * 1000 - S.Incidents * 2000 - FMath::RoundToInt(S.PanicSeconds) * 10;
				CrewTotal += Balance;

				const FString Row1 = FString::Printf(TEXT("%s — «%s»"), *Name, *Title);
				const FString Row2 = FString::Printf(TEXT("починки: %d   подъёмы: %d   эвакуации: %d   ранения: %d   инциденты: %d   паника: %d сек   итог: %s%d ₽"),
					S.Repairs, S.Revives, S.Drags, S.TimesWounded, S.Incidents,
					FMath::RoundToInt(S.PanicSeconds), Balance >= 0 ? TEXT("+") : TEXT(""), Balance);

				DrawText(Row1, TextMain, PX + 28.f, TY, Font, 1.1f);
				TY += ReportRowH;
				DrawText(Row2, Balance >= 0 ? TextDim : FLinearColor(0.95f, 0.45f, 0.3f), PX + 28.f, TY, Font, 0.92f);
				TY += ReportRowH;
			}

			TY += 8.f;
			const FString TotalLine = CrewTotal >= 0
				? FString::Printf(TEXT("Итого к выплате бригаде: +%d ₽"), CrewTotal)
				: FString::Printf(TEXT("Итого: %d ₽ — вычтем из следующей смены"), CrewTotal);
			DrawCentered(TotalLine, CrewTotal >= 0 ? FLinearColor(0.3f, 0.9f, 0.3f) : FLinearColor(0.95f, 0.45f, 0.3f), TY, 1.25f);
		}
		else if (Run->AreAllObjectivesComplete() && Run->GetTotalObjectives() > 0)
		{
			DrawCentered(TEXT("Всё починено — вся бригада к ГАЗели!"), Accent, SizeY * 0.34f, 1.3f);
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
		DrawCastBar(FString::Printf(TEXT("Применение: %s  %d%%"),
			Casting ? *Casting->DisplayName.ToString() : TEXT("..."), Percent),
			Character->GetUseProgress(), BarFill);
	}
	// ---------- Починка (держит E у объекта) ----------
	else if (Character->IsRepairing() && Character->GetCurrentRepairable())
	{
		ARepairable* Repairing = Character->GetCurrentRepairable();
		const int32 Percent = FMath::RoundToInt(Repairing->GetRepairProgress() * 100.f);
		DrawCastBar(FString::Printf(TEXT("Ремонт: %s  %d%%"), *Repairing->DisplayName.ToString(), Percent),
			Repairing->GetRepairProgress(), Accent);
	}
	// ---------- Подсказка подбора (плашка как в референсе) ----------
	else if (APickupItem* Focused = Character->GetFocusedItem())
	{
		DrawPromptBox(FString::Printf(TEXT("[E] Поднять %s"), *Focused->DisplayName.ToString()));
	}
	// ---------- Подсказка починки ----------
	else if (ARepairable* FocusedRep = Character->GetFocusedRepairable())
	{
		FString Prompt;
		if (FocusedRep->CanBeRepairedBy(Character))
		{
			Prompt = FocusedRep->GetRepairProgress() > 0.f
				? FString::Printf(TEXT("[E] Дочинить %s (%d%%)"), *FocusedRep->DisplayName.ToString(),
					FMath::RoundToInt(FocusedRep->GetRepairProgress() * 100.f))
				: FString::Printf(TEXT("[E] Чинить %s (держать)"), *FocusedRep->DisplayName.ToString());
		}
		else if (FocusedRep->IsBeingRepaired())
		{
			Prompt = FString::Printf(TEXT("%s уже чинят"), *FocusedRep->DisplayName.ToString());
		}
		else
		{
			Prompt = FString::Printf(TEXT("Для «%s» нужен инструмент: %s"),
				*FocusedRep->DisplayName.ToString(), *FocusedRep->RequiredTool.ToString());
		}
		DrawPromptBox(Prompt);
	}
	// ---------- Подсказка биотуалета ----------
	else if (AToilet* Toilet = Character->GetFocusedToilet())
	{
		DrawPromptBox(Toilet->CanUseBy(Character)
			? TEXT("[E] Облегчиться")
			: TEXT("Биотуалет: пока не хочется"));
	}
	// ---------- Подсказка драга раненого ----------
	else if (Character->GetFocusedWounded() && !Character->IsDragging())
	{
		DrawPromptBox(Character->IsCarryingHeavy()
			? TEXT("Поставьте тяжёлое (G), чтобы тащить раненого")
			: TEXT("[E] Тащить раненого"));
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
			else if (Held->ItemEffect == EItemEffect::Radio)
			{
				Hint = Held->IsToggledOn()
					? TEXT("[ЛКМ] Выключить рацию — она шумит!")
					: TEXT("[ЛКМ] Включить рацию  •  [ПКМ] Передать");
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
			if (Item->IsToggledOn())
			{
				ItemName += TEXT(" [ВКЛ]");
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
		if (Character->IsDragging())      Statuses.Add(TEXT("Тащит раненого ([E] отпустить)"));
		if (Character->GetDraggedBy())    Statuses.Add(TEXT("Вас тащат"));
		if (Vitals->IsIncidentSlowed())   Statuses.Add(TEXT("Санитарный инцидент!"));
		else if (Vitals->IsSoiled())      Statuses.Add(TEXT("Испачкан"));

		// В газовом облаке — не курить!
		for (TActorIterator<ARepairable> It(GetWorld()); It; ++It)
		{
			if (It->IsLeakingGas()
				&& FVector::DistSquared(Character->GetActorLocation(), It->GetActorLocation()) <= FMath::Square(It->GasRadius))
			{
				Statuses.Add(TEXT("ПАХНЕТ ГАЗОМ — НЕ КУРИТЬ!"));
				break;
			}
		}

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
