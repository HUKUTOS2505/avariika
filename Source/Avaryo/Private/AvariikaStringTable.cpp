// Регистрация основной String Table для локализуемого текста UI, включая кастомизацию.
// Исходные строки (русский) живут в редактируемом CSV — Content/Avariika/Localization/AvariikaUI.csv.
// Переводы на остальные культуры идут через стандартный .locres-конвейер (GatherText → archive → locres).
// Использование в коде: FText::FromStringTable(TEXT("AvariikaUI"), TEXT("ключ")).ToString().
//
// CSV правится в таблице/редакторе без ребилда (меняется только текст). Ребилд нужен лишь при
// добавлении/переименовании ключей (на них ссылается код).
//
// GatherText находит таблицу парсингом этого исходника (макрос LOCTABLE_FROMFILE_GAME),
// а в рантайме её явно регистрирует игровой модуль.

#include "Internationalization/StringTableRegistry.h"

void RegisterAvariikaStringTable()
{
	// StartupModule may run again after a Live Coding/module reload.
	FStringTableRegistry::Get().UnregisterStringTable(TEXT("AvariikaUI"));
	LOCTABLE_FROMFILE_GAME("AvariikaUI", "AvariikaUI", "Avariika/Localization/AvariikaUI.csv");
}

void UnregisterAvariikaStringTable()
{
	FStringTableRegistry::Get().UnregisterStringTable(TEXT("AvariikaUI"));
}
