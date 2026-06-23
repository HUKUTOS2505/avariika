// Регистрация String Table для локализуемого текста UI (меню/HUD).
// Исходные строки (русский) живут в редактируемом CSV — Content/Avariika/Localization/AvariikaUI.csv.
// Переводы на остальные культуры идут через стандартный .locres-конвейер (GatherText → archive → locres).
// Использование в коде: FText::FromStringTable(TEXT("AvariikaUI"), TEXT("ключ")).ToString().
//
// CSV правится в таблице/редакторе без ребилда (меняется только текст). Ребилд нужен лишь при
// добавлении/переименовании КЛЮЧЕЙ (т.к. на них ссылается код).
//
// GatherText находит таблицу парсингом этого исходника (макрос LOCTABLE_FROMFILE_GAME),
// а в рантайме её регистрирует отложенный хелпер ниже.

#include "Internationalization/StringTableRegistry.h"
#include "Misc/DelayedAutoRegister.h"

static const FDelayedAutoRegisterHelper GAvariikaStringTableRegister(
	EDelayedRegisterRunPhase::EndOfEngineInit,
	[]()
	{
		// Путь — относительно Content/. ID и namespace = "AvariikaUI".
		LOCTABLE_FROMFILE_GAME("AvariikaUI", "AvariikaUI", "Avariika/Localization/AvariikaUI.csv");
	});
