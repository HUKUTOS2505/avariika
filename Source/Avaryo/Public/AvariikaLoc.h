#pragma once

#include "CoreMinimal.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Culture.h"

/** Canonical project localization accessors backed by the gathered AvariikaUI String Table. */
struct FAvLoc
{
	/** true, если текущий язык — английский. */
	static bool IsEnglish()
	{
		return FInternationalization::Get().GetCurrentLanguage()->GetTwoLetterISOLanguageName() == TEXT("en");
	}

	/** Legacy two-language helper. New UI must use Text/S so pseudo cultures and future locales work. */
	static FString T(const FString& Ru, const FString& En)
	{
		return IsEnglish() ? En : Ru;
	}

	/** Culture-aware FText retaining its String Table identity across live culture changes. */
	static FText Text(const TCHAR* Key)
	{
		return FText::FromStringTable(TEXT("AvariikaUI"), Key);
	}

	static FText Text(const FString& Key)
	{
		return FText::FromStringTable(TEXT("AvariikaUI"), Key);
	}

	/** FString bridge for APIs such as Canvas DrawText. Avoid for persistent UMG labels. */
	static FString S(const TCHAR* Key)
	{
		return Text(Key).ToString();
	}

	static FString S(const FString& Key)
	{
		return Text(Key).ToString();
	}
};
