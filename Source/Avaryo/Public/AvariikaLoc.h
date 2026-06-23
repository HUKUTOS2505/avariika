#pragma once

#include "CoreMinimal.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Culture.h"

/**
 * Лёгкая RU/EN-локализация для Canvas-HUD (DrawText принимает FString, не FText).
 * Прагматично: каждая строка несёт обе версии инлайн — FAvLoc::T(TEXT("Продолжить"), TEXT("Resume")).
 * Переключается по текущему языку (его ставит EasyOptionsMenu / SetCurrentLanguage). По умолчанию RU.
 *
 * Это MVP под 2 языка без gather/locres-пайплайна. Для масштаба (UMG, много языков) — миграция на
 * FText + String Tables + .locres (см. LOCALIZATION.md).
 */
struct FAvLoc
{
	/** true, если текущий язык — английский. */
	static bool IsEnglish()
	{
		return FInternationalization::Get().GetCurrentLanguage()->GetTwoLetterISOLanguageName() == TEXT("en");
	}

	/** Вернуть EN если язык английский, иначе RU. */
	static FString T(const FString& Ru, const FString& En)
	{
		return IsEnglish() ? En : Ru;
	}

	/**
	 * Локализованная строка из String Table "AvariikaUI" по ключу (исходник — Content/Avariika/Localization/AvariikaUI.csv).
	 * Возвращает текст текущей культуры (перевод из .locres) либо исходный русский (фоллбэк).
	 * Это путь на 13 языков — постепенно вытесняет FAvLoc::T (инлайн ru/en).
	 */
	static FString S(const TCHAR* Key)
	{
		return FText::FromStringTable(TEXT("AvariikaUI"), Key).ToString();
	}
};
