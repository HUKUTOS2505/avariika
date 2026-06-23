# LOCALIZATION — RU/EN (текущее состояние + как расширять)

> 2026-06-23. Цель: «язык» в настройках реально переключает текст. Реальность: почти весь UI — хардкод `TEXT("…")` на русском (FString, не FText), поэтому сделан прагматичный RU/EN-слой под Canvas-HUD.

## Как устроено (MVP)
**`FAvLoc::T(Ru, En)`** (`Source/Avaryo/Public/AvariikaLoc.h`) — возвращает EN, если текущий язык английский, иначе RU. Каждая строка несёт обе версии инлайн:
```cpp
DrawText(FAvLoc::T(TEXT("Продолжить"), TEXT("Resume")), ...);
```
Переключение — по `FInternationalization` текущему языку. Без ассетов/string-tables/.locres-пайплайна (под 2 языка и Canvas-HUD — достаточно; DrawText принимает FString).

## Что УЖЕ локализовано (RU/EN)
- **Главное меню** (`MenuHUD`): заголовок, подзаголовок, кнопки (Создать/Найти/Настройки/Выход), экран поиска игр, подсказки.
- **Пауза-меню** (`AvaryoHUD::DrawPauseMenu`): ПАУЗА, Продолжить, Настройки, Выйти в меню.

## Как проверить
Дев-команда: **`AvLang en`** → текст меню/паузы на английском; **`AvLang ru`** → обратно. (`AvLang` = `AAvaryoCharacter`, exec.)
EasyOptionsMenu-дропдаун языка может НЕ показывать EN, пока нет supported-cultures/.locres — поэтому для теста проще `AvLang`. Интеграцию дропдаута допилить отдельно (см. ниже).

## Как добавить перевод новой строки
Оборачивай в `#include "AvariikaLoc.h"` + `FAvLoc::T(TEXT("рус"), TEXT("eng"))`. Формат с числом — собирай конкатенацией (UE5.7 `FString::Printf` не берёт рантайм-формат): `FAvLoc::T(TEXT("Игра "),TEXT("Game ")) + FString::FromInt(n)`.

## Ещё НЕ локализовано (большие батчи — на потом)
- **Реплики диспетчера** (`ARunState` namespace DispatcherLines — ~35 пулов, флавор).
- **Статусы/подсказки HUD** (горит/мокрый/воняет, `[E] …`, газ-варнинг и т.д.).
- **«Акт»** (звания, строки отчёта, сводка).
- **EasyOptionsMenu** (UMG — там FText; его собственные тексты локализуются через его String Tables/.locres).

## Путь на ПОЛНУЮ локализацию (если масштаб/много языков)
Мигрировать на нативный UE: `NSLOCTEXT`/String Tables (FText) → Localization Dashboard → gather (`-run=GatherText`) → перевод → compile `.locres` → supported cultures в `DefaultGame.ini`. Тогда EasyOptionsMenu-дропдаун подхватит языки сам, и UMG локализуется. FAvLoc — временный мост для Canvas-HUD, при миграции заменяется на FText.

_Связано: внутриигровое Esc-меню (AvaryoHUD/AvaryoPlayerController), MenuHUD._
