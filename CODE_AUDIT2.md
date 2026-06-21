# CODE_AUDIT2 — game-logic баги (2-й проход, адверсариально)

> Автономный аудит 2026-06-21 (Workflow: 5 подсистем механик → находки → каждая проверена скептиком по коду).
> Фокус — ИГРОВАЯ ЛОГИКА (не репликация, тот пласт в `CODE_AUDIT.md`): soft-locks, застрявшие состояния, пороги, дюпы, баланс.
> **25 находок → 11 подтверждённых.** ✅ **10 исправлено и запечено ребилдом** (Build Succeeded, смоук чист); #5 отложен (PIE-тюнинг).

## Сводка

| # | Файл | Severity | Тип | Суть | Статус |
|---|---|---|---|---|---|
| 1 | `VitalsComponent.cpp` | 🔴 high | state-stuck | `bSoiled` никогда не сбрасывался + заморозка спада амбре → вечный «воняет»/кашель/аура паники | ✅ |
| 2 | `VitalsComponent.cpp` | 🟠 med | edge | санитарный инцидент срабатывает в хабе (safe-zone) — нет гарда на пузырь | ✅ |
| 3 | `ARunState.cpp` | 🟠 med | dupe | ре-ремонт перегружающегося щитка фармит +1500/раз (обход квоты диспетчера) | ✅ |
| 4 | `AvaryoCharacter.cpp` | 🟠 med | dupe | grab/release-спам на раненом фармит +500/захват | ✅ |
| 5 | `AvaryoCharacter.cpp` | 🟠 med | logic | E хватает не тот объект (большие зоны + приоритет игнорят прицел) | ⏸ отложен |
| 6 | `ARepairable.cpp` | 🟡 low | balance | окно рывка стартера схлопывается без min-floor (паника+дешёвый инструмент) | ✅ |
| 7 | `VitalsComponent.cpp` | 🟡 low | logic | прямые записи паники (wet/incident/damage) обходят Av.NoPanic/hub-гард | ✅ |
| 8 | `AvaryoCharacter.cpp` | 🟡 low | edge | каст предмета завершается после отруба → hands-free самоподъём | ✅ |
| 9 | `AvaryoCharacter.cpp` | 🟡 low | threshold | бесконечный огнетушитель (`Charges==-1`) не пшикал (`<=0` vs `!=0`) | ✅ |
| 10 | `AvaryoCharacter.cpp` | 🟡 low | state-stuck | ранение в туалете/минигейме не роняло тяжёлое (DropItem early-return) | ✅ |
| 11 | `APowerSwitch.cpp`/`ADoor.cpp` | 🟡 low | balance | E-спам двери/щитка: строб света/ре-электрификация воды/спам звука | ✅ |

## Как исправлено (кратко)

- **#1** `RelieveBladder` снимает `bSoiled` + сбивает амбре (визит в туалет = привёл себя в порядок); `DebugSetVital("smell")` тоже сбрасывает. Заморозка спада (line 278) снимается автоматически.
- **#2** Блок пузыря/инцидента обёрнут в `if (!bSafeZone)` — в хабе не копится.
- **#3** `TSet<TWeakObjectPtr<ARepairable>> CountedRepairs` — оплата (`Repairs`) кредитуется за РАЗНЫЕ объекты; сброс на старте забега.
- **#4** `AddDrag(Who, Victim)` + `TSet DragCreditedVictims` — каждого раненого засчитываем за драг раз/забег.
- **#6** `FMath::Max(Half, 0.04f)` в `GetEffectiveStarterWindow` (зеркало floor курсора).
- **#7** wet/incident/damage-паника под гардом `CVarAvNoPanic==0` (incident — и `!bSafeZone`).
- **#8** `TickUseCast` обрывается при `IsUnconscious()`.
- **#9** `TickSpray` early-out `Charges == 0` (−1 = бесконечный).
- **#10** wounded-handler роняет тяжёлое через `ReleaseHeldItem(false)` напрямую.
- **#11** `LastToggleTime` + дебаунс 0.4с в `ADoor::ToggleBy` и `APowerSwitch::ToggleBy`.

## ⏸ #5 — отложен (нужен PIE)
Find*-хелперы (CallBoard/ToolCase/PowerSwitch/Door) при промахе aim-свипа возвращают БЛИЖАЙШИЙ overlap-актор, игнорируя прицел;
+ фиксированный приоритет диспетча (фикстуры до Item/Repairable). Зоны большие (2.2–3.2 м). Симптомы: в хабе E принимает
наряд вместо взятия ящика; у щитка-рядом-с-потопом E переключает рубильник вместо ремонта; в проёме E дёргает дверь вместо лута.
**Фикс (с PIE):** гейтить overlap-fallback по `dot(viewDir, toActor)` facing-чеку И/ИЛИ выбирать актор, в который реально попал
aim-свип (сравнить дистанции попаданий по всем типам), а не по фиксированному приоритету. Порог прицела подобрать в PIE.

_Связано: `CODE_AUDIT.md` (1-й проход — репликация), WORKLOG._
