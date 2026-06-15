#include "UI/AvaryoHUD.h"

#include "AvaryoCharacter.h"
#include "Components/UFlashlightComponent.h"
#include "Components/VitalsComponent.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/GameInstance.h"
#include "Engine/TextureRenderTarget2D.h"
#include "EngineUtils.h"
#include "Game/ARunState.h"
#include "Game/CompanyLedgerSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "Items/APickupItem.h"
#include "World/ACallBoard.h"
#include "World/AExitZone.h"
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

void AAvaryoHUD::DrawShop()
{
	using namespace AvaryoHUDStyle;
	if (!Canvas)
	{
		return;
	}
	UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
	if (!Font)
	{
		return;
	}

	// Читаем из ARunState (реплицируется), а не из host-only леджера — иначе у клиента баланс/уровни 0
	const ARunState* Run = ARunState::Get(GetWorld());
	const int32 Balance = Run ? Run->GetCompanyBalanceLive() : 0;

	const float SizeX = Canvas->SizeX;
	const float SizeY = Canvas->SizeY;
	const float W = FMath::Min(660.f, SizeX - 80.f);
	const float H = 360.f;
	const float PX = (SizeX - W) * 0.5f;
	float TY = FMath::Max(40.f, (SizeY - H) * 0.5f);

	DrawRect(PanelBG, PX, TY, W, H);
	DrawRect(Accent, PX, TY, W, 4.f);
	TY += 16.f;

	DrawText(TEXT("СНАРЯЖЕНИЕ — склад бригады"), Accent, PX + 22.f, TY, Font, 1.3f);
	TY += 36.f;
	DrawText(FString::Printf(TEXT("Касса конторы: %d ₽"), Balance), TextMain, PX + 22.f, TY, Font, 1.0f);
	TY += 32.f;

	struct FShopRow { const TCHAR* Cat; const TCHAR* Name; int32 MaxLevel; };
	static const FShopRow Rows[] = {
		{ TEXT("Welder"),       TEXT("Сварочник"),    3 },
		{ TEXT("Tester"),       TEXT("Тестер"),       3 },
		{ TEXT("Flashlight"),   TEXT("Фонарь"),       4 },
		{ TEXT("Extinguisher"), TEXT("Огнетушитель"), 3 },
		{ TEXT("Radio"),        TEXT("Рация"),        3 },
	};
	for (const FShopRow& R : Rows)
	{
		const int32 Lvl = Run ? Run->GetEquipmentLevelRep(FName(R.Cat)) : 1;
		FString Line;
		FLinearColor Col;
		if (Lvl >= R.MaxLevel)
		{
			Line = FString::Printf(TEXT("%s — ур. %d  (МАКС)"), R.Name, Lvl);
			Col = TextDim;
		}
		else
		{
			const int32 Price = 3000 * Lvl;
			Line = FString::Printf(TEXT("%s — ур. %d → %d:  %d ₽   [AvUpgrade %s]"), R.Name, Lvl, Lvl + 1, Price, R.Cat);
			Col = (Balance >= Price) ? TextMain : FLinearColor(0.85f, 0.42f, 0.3f);
		}
		DrawText(Line, Col, PX + 26.f, TY, Font, 0.95f);
		TY += 28.f;
	}
	TY += 12.f;
	DrawText(TEXT("Покупка: AvUpgrade <англ. название>.   Закрыть: AvShop"), TextDim, PX + 22.f, TY, Font, 0.85f);
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

	// Модальный экран магазина (AvShop) — рисуем его и больше ничего
	if (bShopOpen)
	{
		DrawShop();
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

	// ---------- Виньетка паники (хоррор-атмосфера, пульсирует как сердцебиение) ----------
	if (Vitals && !Character->IsMonitorOpen())
	{
		const float P = Vitals->GetPanic();
		if (P > 45.f)
		{
			const float Intensity = FMath::GetMappedRangeValueClamped(FVector2D(45.f, 100.f), FVector2D(0.f, 1.f), P);
			const float Time = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
			const float Beat = 0.6f + 0.4f * FMath::Sin(Time * (4.f + 6.f * Intensity)); // чем паничнее — тем чаще
			const float A = 0.5f * Intensity * Beat;
			const FLinearColor Edge(0.35f, 0.f, 0.f, A); // тёмно-красный (без фиолетового)
			const float BandX = SizeX * 0.16f;
			const float BandY = SizeY * 0.16f;
			DrawRect(Edge, 0.f, 0.f, SizeX, BandY);           // верх
			DrawRect(Edge, 0.f, SizeY - BandY, SizeX, BandY); // низ
			DrawRect(Edge, 0.f, 0.f, BandX, SizeY);           // лево
			DrawRect(Edge, SizeX - BandX, 0.f, BandX, SizeY); // право
		}
	}

	// ---------- Зарядка броска (удержание G): полоска силы у центра ----------
	if (Character->IsChargingThrow())
	{
		const float A = Character->GetThrowChargeAlpha();
		const float BW = 200.f, BH = 12.f;
		const float BX = (SizeX - BW) * 0.5f, BY = SizeY * 0.60f;
		DrawRect(BarBG, BX, BY, BW, BH);
		DrawRect(A >= 1.f ? FLinearColor(0.95f, 0.85f, 0.2f) : Accent, BX, BY, BW * A, BH);
		DrawText(A >= 1.f ? TEXT("БРОСОК заряжен — отпусти G") : TEXT("Зарядка броска... (отпусти G)"),
			TextMain, BX, BY - 18.f, Font, 0.85f);
	}

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
				DrawRect(FLinearColor(0.8f, 0.12f, 0.12f), TileX, BarY, TileW * 0.48f * FMath::Clamp(CrewVitals->GetHealth(), 0.f, 100.f) / 100.f, 8.f);
				DrawRect(AvaryoHUDStyle::BarBG, TileX + TileW * 0.52f, BarY, TileW * 0.48f, 8.f);
				DrawRect(FLinearColor(0.25f, 0.7f, 0.85f), TileX + TileW * 0.52f, BarY, TileW * 0.48f * FMath::Clamp(CrewVitals->GetPanic(), 0.f, 100.f) / 100.f, 8.f);
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
		DrawBar(TEXT("Туалет"),       Vitals->GetBladder(), FLinearColor(0.8f, 0.6f, 0.12f),  Y); Y += 22.f;
		DrawBar(TEXT("Амбре"),        Vitals->GetSmell(),   FLinearColor(0.5f, 0.45f, 0.12f), Y); Y += 22.f;

		// Шум: насколько ты сейчас слышен — задел под монстра-слухача.
		// Движение (бег/присед) + события MakeNoise (кашель, икота, отдышка, бросок, починка…).
		float Noise01 = FMath::Clamp(Character->GetVelocity().Size2D() / 750.f, 0.f, 1.f);
		if (Character->bIsCrouched)
		{
			Noise01 *= 0.4f;
		}
		if (APickupItem* HeldNoisy = Character->GetHeldItem())
		{
			if (HeldNoisy->IsSpraying())
			{
				Noise01 = 1.f;
			}
		}
		Noise01 = FMath::Max(Noise01, Character->GetSelfNoise01()); // всплески от событий шума
		DrawBar(TEXT("Шум"), Noise01 * 100.f, FLinearColor(1.f, 0.55f, 0.2f), Y);

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

	// ---------- Рация диспетчера (сверху по центру, плашки гаснут сами) ----------
	if (ARunState* Run = ARunState::Get(GetWorld()))
	{
		const float Now = GetWorld()->GetTimeSeconds();

		float LineY = 16.f;
		for (const FDispatcherLine& Line : Run->GetDispatcherLines())
		{
			const float Age = Now - Line.ReceivedAt;
			if (Age > 9.f)
			{
				continue; // отговорил — плашка погасла
			}
			const float Alpha = Age > 7.f ? 1.f - (Age - 7.f) / 2.f : 1.f; // последние 2 с тают

			const FString Prefix = Line.Speaker + TEXT(": ");
			float PrefixW = 0.f, PrefixH = 0.f;
			GetTextSize(Prefix, PrefixW, PrefixH, Font, 1.05f);
			float TextW = 0.f, TextH = 0.f;
			GetTextSize(Line.Text, TextW, TextH, Font, 1.05f);
			const float BoxW = PrefixW + TextW + 28.f;
			const float BoxX = (SizeX - BoxW) * 0.5f;

			FLinearColor BG = BoxBG;        BG.A *= Alpha;
			FLinearColor Edge = Accent;     Edge.A *= Alpha;
			FLinearColor NameC = Accent;    NameC.A *= Alpha;
			FLinearColor TextC = TextMain;  TextC.A *= Alpha;

			DrawRect(BG, BoxX, LineY, BoxW, TextH + 14.f);
			DrawRect(Edge, BoxX, LineY, 4.f, TextH + 14.f); // оранжевая кромка слева — «эфир»
			DrawText(Prefix, NameC, BoxX + 14.f, LineY + 7.f, Font, 1.05f);
			DrawText(Line.Text, TextC, BoxX + 14.f + PrefixW, LineY + 7.f, Font, 1.05f);
			LineY += TextH + 20.f;
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
			int32 MaxRepairs = 0, MaxWounded = 0, MaxRevives = 0, MaxDrags = 0, MaxBotched = 0;
			int32 MaxShoved = 0, MaxTripped = 0;
			int32 MaxExplosions = 0, MaxShorts = 0, MaxThrows = 0, MaxCoffees = 0;
			float MaxPanic = 0.f, MaxSmell = 0.f;
			for (const FPlayerRunStats& S : AllStats)
			{
				MaxRepairs = FMath::Max(MaxRepairs, S.Repairs);
				MaxWounded = FMath::Max(MaxWounded, S.TimesWounded);
				MaxRevives = FMath::Max(MaxRevives, S.Revives);
				MaxDrags   = FMath::Max(MaxDrags,   S.Drags);
				MaxBotched = FMath::Max(MaxBotched, S.BotchedRepairs);
				MaxShoved  = FMath::Max(MaxShoved,  S.ShovedOthers);
				MaxTripped = FMath::Max(MaxTripped, S.TimesTripped);
				MaxExplosions = FMath::Max(MaxExplosions, S.ExplosionsCaused);
				MaxShorts  = FMath::Max(MaxShorts,  S.ShortsCaused);
				MaxThrows  = FMath::Max(MaxThrows,  S.Throws);
				MaxCoffees = FMath::Max(MaxCoffees, S.Coffees);
				MaxPanic   = FMath::Max(MaxPanic,   S.PanicSeconds);
				MaxSmell   = FMath::Max(MaxSmell,   S.SmellSeconds);
			}

			const float ReportW = FMath::Min(820.f, SizeX - 80.f);
			const float ReportRowH = 26.f;
			const float ReportH = 260.f + AllStats.Num() * ReportRowH * 4.f;
			const float PX = (SizeX - ReportW) * 0.5f;
			float PY = FMath::Max(40.f, (SizeY - ReportH) * 0.5f);

			DrawRect(PanelBG, PX, PY, ReportW, ReportH);
			DrawRect(Accent, PX, PY, ReportW, 4.f);

			float TY = PY + 18.f;
			TY += DrawCentered(FString::Printf(TEXT("АКТ ВЫПОЛНЕННЫХ РАБОТ № %03d"), Run->GetShiftNumber()),
				Accent, TY, 1.6f) + 6.f;
			TY += DrawCentered(bWon
				? FString::Printf(TEXT("Объект сдан. Время: %02d:%02d. Заказчик недоволен, но подписал."), Elapsed / 60, Elapsed % 60)
				: TEXT("Объект НЕ сдан: вся бригада выведена из строя. Акт подписан задним числом."),
				bWon ? FLinearColor(0.3f, 0.9f, 0.3f) : FLinearColor(0.95f, 0.25f, 0.25f), TY, 1.1f) + 14.f;

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
				else if (S.ExplosionsCaused > 0 && S.ExplosionsCaused == MaxExplosions) Title = TEXT("Пожарный наоборот");
				else if (S.ShortsCaused > 0 && S.ShortsCaused == MaxShorts) Title = TEXT("Электрик Эдисон");
				else if (S.BotchedRepairs > 0 && S.BotchedRepairs == MaxBotched) Title = TEXT("Народный умелец");
				else if (S.SmellSeconds > 5.f && S.SmellSeconds >= MaxSmell) Title = TEXT("Амбре смены");
				else if (S.ToiletVisits >= 2)                          Title = TEXT("Дисциплинированный мочевой пузырь");
				else if (S.Repairs > 0 && S.Repairs == MaxRepairs)     Title = TEXT("Работник месяца");
				else if (S.Revives > 0 && S.Revives == MaxRevives)     Title = TEXT("Полевой медик");
				else if (S.Drags > 0 && S.Drags == MaxDrags)           Title = TEXT("Эвакуатор");
				else if (S.ShovedOthers > 0 && S.ShovedOthers == MaxShoved) Title = TEXT("Гроза коллектива");
				else if (S.Throws > 1 && S.Throws == MaxThrows)        Title = TEXT("Метатель снарядов");
				else if (S.TimesTripped > 1 && S.TimesTripped == MaxTripped) Title = TEXT("Спотыкач смены");
				else if (S.TimesWounded > 0 && S.TimesWounded == MaxWounded) Title = TEXT("Главный пострадавший");
				else if (S.PanicSeconds > 1.f && S.PanicSeconds >= MaxPanic) Title = TEXT("Паникёр смены");
				else if (S.Coffees >= 2 && S.Coffees == MaxCoffees)    Title = TEXT("Кофеман смены");
				else                                                   Title = TEXT("Просто присутствовал");

				// Бухгалтерия: премии и штрафы (единая формула с сервером)
				const int32 Balance = ARunState::ComputePlayerBalance(S);

				const FString Row1 = FString::Printf(TEXT("%s — «%s»"), *Name, *Title);
				// Длинную статистику делим на две строки, чтобы не вылезала за панель
				const FString Row2 = FString::Printf(TEXT("починки: %d   колхоз: %d   подъёмы: %d   эвакуации: %d   туалет: %d"),
					S.Repairs, S.BotchedRepairs, S.Revives, S.Drags, S.ToiletVisits);
				const FString Row3 = FString::Printf(TEXT("ранения: %d   инциденты: %d   паника: %d с   вонял: %d с   итог: %s%d ₽"),
					S.TimesWounded, S.Incidents, FMath::RoundToInt(S.PanicSeconds), FMath::RoundToInt(S.SmellSeconds),
					Balance >= 0 ? TEXT("+") : TEXT(""), Balance);

				const FString Row4 = FString::Printf(TEXT("толкнул: %d   споткнулся: %d   катался по пене: %d с"),
					S.ShovedOthers, S.TimesTripped, FMath::RoundToInt(S.SlipSeconds));

				DrawText(Row1, TextMain, PX + 28.f, TY, Font, 1.1f);
				TY += ReportRowH;
				DrawText(Row2, TextDim, PX + 28.f, TY, Font, 0.92f);
				TY += ReportRowH;
				DrawText(Row3, Balance >= 0 ? TextDim : FLinearColor(0.95f, 0.45f, 0.3f), PX + 28.f, TY, Font, 0.92f);
				TY += ReportRowH;
				DrawText(Row4, TextDim, PX + 28.f, TY, Font, 0.92f);
				TY += ReportRowH;
			}

			TY += 8.f;
			const int32 ShiftNet = Run->GetShiftNet();
			const FLinearColor PosC(0.3f, 0.9f, 0.3f);
			const FLinearColor NegC(0.95f, 0.45f, 0.3f);

			const FString TotalLine = ShiftNet >= 0
				? FString::Printf(TEXT("Итог смены: +%d ₽"), ShiftNet)
				: FString::Printf(TEXT("Итог смены: %d ₽ (в минус)"), ShiftNet);
			TY += DrawCentered(TotalLine, ShiftNet >= 0 ? PosC : NegC, TY, 1.25f) + 6.f;

			// Баланс конторы: было → стало (§19, копится между сменами)
			const int32 BalBefore = Run->GetCompanyBalanceStart();
			const int32 BalAfter = BalBefore + ShiftNet;
			const FString BalLine = FString::Printf(TEXT("Касса конторы: %d ₽  →  %d ₽"), BalBefore, BalAfter);
			TY += DrawCentered(BalLine, BalAfter >= 0 ? TextMain : NegC, TY, 1.1f) + 4.f;

			// Репутация конторы — влияет на качество выдаваемого комплекта в следующих сменах
			const FString RepLine = FString::Printf(TEXT("Репутация: %s"), *ARunState::ReputationTitle(Run->GetReputation()));
			TY += DrawCentered(RepLine, TextDim, TY, 1.0f) + 6.f;

			// Карьера конторы за всё время (реплицируется из ARunState — у клиента тоже верно)
			const FString CareerLine = FString::Printf(TEXT("Карьера: починок всего %d · домов спалили %d · инцидентов %d"),
				Run->GetCareerRepairs(), Run->GetCareerBlownUp(), Run->GetCareerIncidents());
			TY += DrawCentered(CareerLine, TextDim, TY, 0.85f) + 8.f;

			// Квота диспетчера (game-over крючок) — показываем только когда включена
			if (Run->IsQuotaFailed())
			{
				TY += DrawCentered(TEXT("КВОТА ПРОВАЛЕНА — контора закрыта"), NegC, TY, 1.15f) + 4.f;
				DrawCentered(TEXT("[R] Начать карьеру заново"), TextDim, TY, 1.05f);
			}
			else if (Run->IsQuotaActive())
			{
				const FString QLine = FString::Printf(TEXT("Квота: %d / %d ₽   срок — смена №%d"),
					Run->GetQuotaPaid(), Run->GetQuotaTarget(), Run->GetQuotaDeadlineShift());
				const bool bMet = Run->GetQuotaPaid() >= Run->GetQuotaTarget();
				TY += DrawCentered(QLine, bMet ? PosC : TextMain, TY, 1.05f) + 8.f;
				DrawCentered(TEXT("[R] Следующая смена"), TextDim, TY, 1.05f);
			}
			else
			{
				DrawCentered(TEXT("[R] Следующая смена"), TextDim, TY, 1.05f);
			}
		}
		else if (Run->AreAllObjectivesComplete() && Run->GetTotalObjectives() > 0)
		{
			DrawCentered(TEXT("Всё починено — вся бригада к ГАЗели!"), Accent, SizeY * 0.34f, 1.3f);
		}
	}

	// ---------- Миникарта (слева, под шкалами) ----------
	{
		const float MapSize = 150.f;
		const float MapX = 30.f, MapY = 175.f;
		const float WorldHalf = 2100.f; // пол карты ±2000 + запас

		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.55f), MapX, MapY, MapSize, MapSize);
		DrawRect(AccentDim, MapX, MapY, MapSize, 2.f);
		DrawRect(AccentDim, MapX, MapY + MapSize - 2.f, MapSize, 2.f);
		DrawRect(AccentDim, MapX, MapY, 2.f, MapSize);
		DrawRect(AccentDim, MapX + MapSize - 2.f, MapY, 2.f, MapSize);

		// Мир → миникарта: север (X+) сверху, восток (Y+) справа
		auto WorldToMap = [&](const FVector& W, float& OutX, float& OutY)
		{
			OutX = MapX + FMath::Clamp((W.Y + WorldHalf) / (2.f * WorldHalf), 0.02f, 0.98f) * MapSize;
			OutY = MapY + FMath::Clamp((WorldHalf - W.X) / (2.f * WorldHalf), 0.02f, 0.98f) * MapSize;
		};
		auto DrawDot = [&](const FVector& W, const FLinearColor& Color, float Half)
		{
			float X = 0.f, Y = 0.f;
			WorldToMap(W, X, Y);
			DrawRect(Color, X - Half, Y - Half, Half * 2.f, Half * 2.f);
		};

		// Зона ГАЗели и биотуалеты
		for (TActorIterator<AExitZone> It(GetWorld()); It; ++It)
		{
			DrawDot(It->GetActorLocation(), Accent, 5.f);
		}
		for (TActorIterator<AToilet> It(GetWorld()); It; ++It)
		{
			DrawDot(It->GetActorLocation(), FLinearColor(0.45f, 0.75f, 1.f), 3.f);
		}

		// Задачи: сломанные красным, починенные зелёным
		if (ARunState* Run = ARunState::Get(GetWorld()))
		{
			for (const ARepairable* Objective : Run->GetObjectives())
			{
				if (Objective)
				{
					DrawDot(Objective->GetActorLocation(),
						Objective->IsBroken() ? FLinearColor(0.95f, 0.2f, 0.2f) : FLinearColor(0.3f, 0.85f, 0.3f), 3.f);
				}
			}
		}

		// Бригада: тиммейты белым (раненые красным), я — оранжевый с направлением взгляда
		for (TActorIterator<AAvaryoCharacter> It(GetWorld()); It; ++It)
		{
			if (*It == Character)
			{
				continue;
			}
			const bool bCrewWounded = It->VitalsComponent && It->VitalsComponent->IsWounded();
			DrawDot(It->GetActorLocation(), bCrewWounded ? FLinearColor(1.f, 0.3f, 0.3f) : TextMain, 2.5f);
		}

		float SelfX = 0.f, SelfY = 0.f;
		WorldToMap(Character->GetActorLocation(), SelfX, SelfY);
		DrawRect(Accent, SelfX - 3.f, SelfY - 3.f, 6.f, 6.f);
		const FVector Forward = Character->GetControlRotation().Vector();
		Draw2DLine(FMath::RoundToInt(SelfX), FMath::RoundToInt(SelfY),
			FMath::RoundToInt(SelfX + Forward.Y * 10.f), FMath::RoundToInt(SelfY - Forward.X * 10.f),
			Accent.ToFColor(true));
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
	// ---------- Мини-игра биотуалета ----------
	else if (Character->IsUsingToilet() && Character->GetCurrentToilet() && Vitals)
	{
		AToilet* T = Character->GetCurrentToilet();
		const float BarW = 340.f, BarH = 18.f;
		const float BoxX = (SizeX - BarW) * 0.5f;
		float BoxY = SizeY * 0.42f;

		const FString Label = FString::Printf(TEXT("Процесс... осталось %d%%"), FMath::RoundToInt(Vitals->GetBladder()));
		float LabelW = 0.f, LabelH = 0.f;
		GetTextSize(Label, LabelW, LabelH, Font, 1.1f);

		// Плашка на обе полоски и подсказку
		DrawRect(BoxBG, BoxX - 12.f, BoxY - LabelH - 10.f, BarW + 24.f, LabelH + BarH * 2.f + 56.f);
		DrawText(Label, TextMain, BoxX, BoxY - LabelH - 4.f, Font, 1.1f);

		// Верхняя полоска: сколько «содержимого» осталось
		DrawRect(BarBG, BoxX, BoxY + 2.f, BarW, BarH);
		DrawRect(FLinearColor(0.65f, 0.45f, 0.15f), BoxX, BoxY + 2.f, BarW * FMath::Clamp(Vitals->GetBladder(), 0.f, 100.f) / 100.f, BarH);

		// Нижняя полоска: мини-игра — красный фон, отдельные жёлтая и зелёная зоны, белый курсор
		BoxY += BarH + 12.f;
		DrawRect(FLinearColor(0.55f, 0.12f, 0.1f), BoxX, BoxY + 2.f, BarW, BarH);
		const float Yellow = T->GetYellowCenter();
		const float YellowL = FMath::Clamp(Yellow - T->YellowHalfWidth, 0.f, 1.f);
		const float YellowR = FMath::Clamp(Yellow + T->YellowHalfWidth, 0.f, 1.f);
		DrawRect(FLinearColor(0.8f, 0.7f, 0.15f), BoxX + YellowL * BarW, BoxY + 2.f, (YellowR - YellowL) * BarW, BarH);
		const float Green = T->GetGreenCenter();
		const float GreenL = FMath::Clamp(Green - T->GreenHalfWidth, 0.f, 1.f);
		const float GreenR = FMath::Clamp(Green + T->GreenHalfWidth, 0.f, 1.f);
		DrawRect(FLinearColor(0.25f, 0.8f, 0.25f), BoxX + GreenL * BarW, BoxY + 2.f, (GreenR - GreenL) * BarW, BarH);
		DrawRect(TextMain, BoxX + T->GetCursorPos() * BarW - 2.f, BoxY - 2.f, 4.f, BarH + 8.f);

		DrawText(TEXT("[E] — жми в зелёной (или жёлтой) зоне!  [G] — встать"), TextDim, BoxX, BoxY + BarH + 8.f, Font, 0.9f);
	}
	// ---------- Мини-игры починки (щиток/вентиль/стартер) — только когда все подготовительные этапы пройдены ----------
	else if (Character->IsRepairing() && Character->GetCurrentRepairable()
		&& Character->GetCurrentRepairable()->IsMinigameRepair() && Character->GetCurrentRepairable()->ArePrereqsDone()
		&& !Character->GetCurrentRepairable()->IsBotching())
	{
		ARepairable* R = Character->GetCurrentRepairable();
		const float BarW = 340.f, BarH = 18.f;
		const float BoxX = (SizeX - BarW) * 0.5f;
		float BoxY = SizeY * 0.42f;

		FString Label;
		switch (R->GetMinigameType())
		{
		case ERepairMinigameType::Valve:
			Label = FString::Printf(TEXT("Вентиль: %s  %d%%"),
				*R->DisplayName.ToString(), FMath::RoundToInt(R->GetRepairProgress() * 100.f));
			break;
		case ERepairMinigameType::Starter:
			Label = FString::Printf(TEXT("Стартер: %s  %d%%"),
				*R->DisplayName.ToString(), FMath::RoundToInt(R->GetRepairProgress() * 100.f));
			break;
		default:
			Label = FString::Printf(TEXT("Ремонт: %s  %d%%   промахи %d/%d"),
				*R->DisplayName.ToString(), FMath::RoundToInt(R->GetRepairProgress() * 100.f),
				R->GetMissCount(), R->MissesBeforeLockout);
			break;
		}
		float LabelW = 0.f, LabelH = 0.f;
		GetTextSize(Label, LabelW, LabelH, Font, 1.1f);

		DrawRect(BoxBG, BoxX - 12.f, BoxY - LabelH - 10.f, BarW + 24.f, LabelH + BarH * 2.f + 56.f);
		DrawText(Label, TextMain, BoxX, BoxY - LabelH - 4.f, Font, 1.1f);

		// Верхняя полоска: прогресс починки
		DrawRect(BarBG, BoxX, BoxY + 2.f, BarW, BarH);
		DrawRect(Accent, BoxX, BoxY + 2.f, BarW * R->GetRepairProgress(), BarH);

		// Нижняя полоска — своя на каждый режим
		BoxY += BarH + 12.f;
		if (R->GetMinigameType() == ERepairMinigameType::Valve)
		{
			// Ритм: полоска наполняется к «безопасно тыкать»; тык до зелёного = срыв резьбы
			const float Ready = 1.f - FMath::Clamp(R->GetValveCooldown() / FMath::Max(R->ValveMinInterval, 0.01f), 0.f, 1.f);
			DrawRect(BarBG, BoxX, BoxY + 2.f, BarW, BarH);
			DrawRect(Ready >= 1.f ? FLinearColor(0.25f, 0.8f, 0.25f) : FLinearColor(0.55f, 0.12f, 0.1f),
				BoxX, BoxY + 2.f, BarW * Ready, BarH);
			DrawText(TEXT("[E] — докручивай РАЗМЕРЕННО: зелёная полоска = можно. Частить = срыв резьбы!  [G] — отойти"),
				TextDim, BoxX, BoxY + BarH + 8.f, Font, 0.9f);
		}
		else if (R->GetMinigameType() == ERepairMinigameType::Starter)
		{
			// Натяжение шнура: зелёное окно, белая полоса растёт пока держишь E
			DrawRect(BarBG, BoxX, BoxY + 2.f, BarW, BarH);
			float WStart, WEnd;
			R->GetEffectiveStarterWindow(WStart, WEnd); // окно как у сервера (паника+инструмент)
			const float WinL = FMath::Clamp(WStart, 0.f, 1.f);
			const float WinR = FMath::Clamp(WEnd, 0.f, 1.f);
			DrawRect(FLinearColor(0.25f, 0.8f, 0.25f), BoxX + WinL * BarW, BoxY + 2.f, (WinR - WinL) * BarW, BarH);
			const float Tension = R->GetStarterTension();
			DrawRect(FLinearColor(0.9f, 0.9f, 0.9f), BoxX, BoxY + 4.f, BarW * Tension, BarH - 4.f);
			DrawRect(TextMain, BoxX + Tension * BarW - 2.f, BoxY - 2.f, 4.f, BarH + 8.f);
			DrawText(R->IsStarterPulling()
				? TEXT("Тяни шнур... отпусти E в ЗЕЛЁНОМ окне! Перетянешь — ударит.  [G] — отойти")
				: TEXT("[E] (держать) — дёрнуть стартер. Нужно 3 удачных рывка.  [G] — отойти"),
				TextDim, BoxX, BoxY + BarH + 8.f, Font, 0.9f);
		}
		else
		{
			// Щиток: красный фон, хаотичная зелёная зона, белый курсор
			DrawRect(FLinearColor(0.55f, 0.12f, 0.1f), BoxX, BoxY + 2.f, BarW, BarH);
			const float Green = R->GetGreenCenter();
			const float HalfW = R->GetEffectiveGreenHalf(); // та же зона, что принимает сервер (паника+инструмент)
			const float GreenL = FMath::Clamp(Green - HalfW, 0.f, 1.f);
			const float GreenR = FMath::Clamp(Green + HalfW, 0.f, 1.f);
			DrawRect(FLinearColor(0.25f, 0.8f, 0.25f), BoxX + GreenL * BarW, BoxY + 2.f, (GreenR - GreenL) * BarW, BarH);
			DrawRect(TextMain, BoxX + R->GetCursorPos() * BarW - 2.f, BoxY - 2.f, 4.f, BarH + 8.f);
			DrawText(TEXT("[E] — жми в зелёной! Промах бьёт током, 3 промаха — замыкание.  [G] — отойти"), TextDim, BoxX, BoxY + BarH + 8.f, Font, 0.9f);
		}
	}
	// ---------- Подготовительная мини-игра (заварка / починка руками — курсор с откатом) ----------
	else if (Character->IsRepairing() && Character->GetCurrentRepairable()
		&& Character->GetCurrentRepairable()->IsDoingPrereqMinigame())
	{
		ARepairable* R = Character->GetCurrentRepairable();
		FRepairStage St;
		R->GetCurrentStage(St);
		const FString Label = FString::Printf(TEXT("%s  %d%%"),
			St.Label.IsEmpty() ? TEXT("Этап") : *St.Label.ToString(), FMath::RoundToInt(R->GetPrereqProgress() * 100.f));
		const float BarW = 340.f, BarH = 18.f;
		const float BoxX = (SizeX - BarW) * 0.5f;
		float BoxY = SizeY * 0.42f;
		float LW = 0.f, LH = 0.f;
		GetTextSize(Label, LW, LH, Font, 1.1f);
		DrawRect(BoxBG, BoxX - 12.f, BoxY - LH - 10.f, BarW + 24.f, LH + BarH * 2.f + 56.f);
		DrawText(Label, TextMain, BoxX, BoxY - LH - 4.f, Font, 1.1f);
		DrawRect(BarBG, BoxX, BoxY + 2.f, BarW, BarH);
		DrawRect(Accent, BoxX, BoxY + 2.f, BarW * R->GetPrereqProgress(), BarH);
		BoxY += BarH + 12.f;
		DrawRect(FLinearColor(0.55f, 0.12f, 0.1f), BoxX, BoxY + 2.f, BarW, BarH);
		const float Green = R->GetGreenCenter();
		const float HalfW = R->GetEffectiveGreenHalf(); // совпадает с серверной зоной (паника+инструмент)
		const float GreenL = FMath::Clamp(Green - HalfW, 0.f, 1.f);
		const float GreenR = FMath::Clamp(Green + HalfW, 0.f, 1.f);
		DrawRect(FLinearColor(0.25f, 0.8f, 0.25f), BoxX + GreenL * BarW, BoxY + 2.f, (GreenR - GreenL) * BarW, BarH);
		DrawRect(TextMain, BoxX + R->GetCursorPos() * BarW - 2.f, BoxY - 2.f, 4.f, BarH + 8.f);
		DrawText(TEXT("[E] — жми в зелёной! Промах — откат прогресса."), TextDim, BoxX, BoxY + BarH + 8.f, Font, 0.9f);
	}
	// ---------- Подготовительный этап: держать E (починка руками) или AutoFill (установка кабеля) ----------
	else if (Character->IsRepairing() && Character->GetCurrentRepairable()
		&& !Character->GetCurrentRepairable()->ArePrereqsDone())
	{
		ARepairable* Rep = Character->GetCurrentRepairable();
		FRepairStage St;
		const FString StepName = (Rep->GetCurrentStage(St) && !St.Label.IsEmpty()) ? St.Label.ToString() : TEXT("Подготовка");
		DrawCastBar(Rep->IsAutoFilling() ? FString::Printf(TEXT("%s (установка...)"), *StepName) : StepName,
			Rep->GetPrereqProgress(), Accent);
	}
	// ---------- Основная починка (держит E у объекта) ----------
	else if (Character->IsRepairing() && Character->GetCurrentRepairable())
	{
		ARepairable* Repairing = Character->GetCurrentRepairable();
		const int32 Percent = FMath::RoundToInt(Repairing->GetRepairProgress() * 100.f);
		DrawCastBar(FString::Printf(TEXT("Ремонт: %s  %d%%"), *Repairing->DisplayName.ToString(), Percent),
			Repairing->GetRepairProgress(), Accent);
	}
	// ---------- Подсказка подбора (плашка как в референсе) ----------
	else if (ACallBoard* Board = Character->GetFocusedCallBoard())
		{
			const TArray<FCallListing>& BC = Board->GetCalls();
			const int32 Sel = Board->GetSelectedIndex();
			const FString Title = BC.IsValidIndex(Sel) ? BC[Sel].Title : FString(TEXT("заявка"));
			DrawPromptBox(FString::Printf(TEXT("[E] Взять заявку: %s"), *Title));
		}
		else if (APickupItem* Focused = Character->GetFocusedItem())
	{
		DrawPromptBox(FString::Printf(TEXT("[E] Поднять %s"), *Focused->DisplayName.ToString()));
	}
	// ---------- Подсказка починки ----------
	else if (ARepairable* FocusedRep = Character->GetFocusedRepairable())
	{
		FString Prompt;
		if (FocusedRep->GetLockoutRemaining() > 0.f)
		{
			Prompt = FString::Printf(TEXT("%s замкнуло — подождите %d с"),
				*FocusedRep->DisplayName.ToString(), FMath::CeilToInt(FocusedRep->GetLockoutRemaining()));
		}
		else if (!FocusedRep->ArePrereqsDone())
		{
			// Подготовительный этап: подсказываем, что делать (заварить / подключить кабель / вставить...)
			FRepairStage St;
			FocusedRep->GetCurrentStage(St);
			const FString Step = St.Label.IsEmpty() ? TEXT("Подготовка") : St.Label.ToString();
			const APickupItem* Held = Character->GetHeldItem();
			const bool bNeedItem = (St.Kind == ERepairStageKind::HoldTool
				|| St.Kind == ERepairStageKind::InsertItem
				|| St.Kind == ERepairStageKind::AutoFill
				|| (St.Kind == ERepairStageKind::Minigame && !St.ItemTag.IsNone()));
			const bool bHasItem = Held && Held->ToolTag == St.ItemTag;
			if (bNeedItem && !bHasItem)
			{
				Prompt = FString::Printf(TEXT("%s — нужен в руках: %s"), *Step, *St.ItemTag.ToString());
			}
			else if (St.Kind == ERepairStageKind::Minigame)
			{
				Prompt = FString::Printf(TEXT("[E] %s (мини-игра)"), *Step);
			}
			else if (St.Kind == ERepairStageKind::HoldHand || St.Kind == ERepairStageKind::HoldTool)
			{
				Prompt = FString::Printf(TEXT("[E] %s (держать)"), *Step);
			}
			else
			{
				Prompt = FString::Printf(TEXT("[E] %s"), *Step); // InsertItem / AutoFill
			}
		}
		else if (FocusedRep->CanBeRepairedBy(Character))
		{
			if (FocusedRep->GetMinigameType() == ERepairMinigameType::Valve)
			{
				Prompt = FString::Printf(TEXT("[E] Закрутить вентиль: %s (жми размеренно!)"), *FocusedRep->DisplayName.ToString());
			}
			else if (FocusedRep->GetMinigameType() == ERepairMinigameType::Starter)
			{
				Prompt = FString::Printf(TEXT("[E] Заводить %s (держать и отпустить вовремя)"), *FocusedRep->DisplayName.ToString());
			}
			else if (FocusedRep->IsMinigameRepair())
			{
				Prompt = FString::Printf(TEXT("[E] Чинить %s (мини-игра, нужен тестер)"), *FocusedRep->DisplayName.ToString());
			}
			else
			{
				Prompt = FocusedRep->GetRepairProgress() > 0.f
					? FString::Printf(TEXT("[E] Дочинить %s (%d%%)"), *FocusedRep->DisplayName.ToString(),
						FMath::RoundToInt(FocusedRep->GetRepairProgress() * 100.f))
					: FString::Printf(TEXT("[E] Чинить %s (держать)"), *FocusedRep->DisplayName.ToString());
			}
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
		FString ToiletPrompt;
		if (Toilet->IsOccupied() && Toilet->GetOccupant() != Character)
		{
			ToiletPrompt = TEXT("Биотуалет: ЗАНЯТО");
		}
		else if (Toilet->CanUseBy(Character))
		{
			ToiletPrompt = TEXT("[E] Облегчиться (мини-игра, не двигаться)");
		}
		else
		{
			ToiletPrompt = TEXT("Биотуалет: пока не хочется");
		}
		DrawPromptBox(ToiletPrompt);
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
			else if (Held->ItemEffect == EItemEffect::DeployTrap)
			{
				Hint = TEXT("[ЛКМ] Поставить растяжку (взведётся через пару секунд!)  •  [ПКМ] Передать");
			}
			else if (Held->ItemEffect == EItemEffect::ThrowBio)
			{
				Hint = TEXT("[ЛКМ] Метнуть комок (осторожно — зацепишь своих!)  •  [ПКМ] Передать");
			}
			else if (Held->ItemEffect == EItemEffect::DeployLight)
			{
				Hint = TEXT("[ЛКМ] Поставить прожектор (светит и успокаивает, но гудит)  •  [ПКМ] Передать");
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
		if (Vitals->IsSmelly())           Statuses.Add(TEXT("Воняет"));

		// В газовом облаке — не курить! (берём из кэша задач RunState, без обхода всех акторов каждый кадр)
		if (ARunState* GasRun = ARunState::Get(GetWorld()))
		{
			for (const ARepairable* Obj : GasRun->GetObjectives())
			{
				if (Obj && Obj->IsLeakingGas()
					&& FVector::DistSquared(Character->GetActorLocation(), Obj->GetActorLocation()) <= FMath::Square(Obj->GasRadius))
				{
					Statuses.Add(TEXT("ПАХНЕТ ГАЗОМ — НЕ КУРИТЬ!"));
					break;
				}
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
