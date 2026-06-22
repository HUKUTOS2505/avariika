# CODE_AUDIT3 — 3-й глубокий проход (perf / NaN / coop-race / regression)

> Автономный аудит 2026-06-22 (Workflow: 11 финдеров × 5 линз — perf/Tick · numeric/NaN/overflow · coop-race · validation · logic; затем верификация по живому коду).
> ⚠️ **Проход НЕПОЛНЫЙ:** фон-воркфлоу упёрся в session-limit (сброс 5:30) и убил verify-фазу + 3 финдера. Успели отработать **8 из 11** финдеров.
> **Верификацию 12 кандидатов сделал вручную по коду** (как делал бы скептик-агент): **все 12 подтверждены, 0 опровергнуто.**
> Сведено в **11 находок** (один дубль-перф схлопнут). **Код НЕ правил — на твою отмашку** (тот же порядок, что в AUDIT1/AUDIT2).

## Сводка

| # | Файл / строки | Severity | Тип | Суть |
|---|---|---|---|---|
| 1 | `VitalsComponent.cpp` 300-321, 444-472 | 🔴 high | logic | DoT/повторный урон на раненом мгновенно вырубает в «отруб» мимо 40-сек окна подъёма |
| 2 | `AvaryoCharacter.cpp` 2550 | 🔴 high | regression | бесконечный огнетушитель (`Charges==-1`) обнуляется и глохнет на 1-м кадре спрея (неполный AUDIT2 #9) |
| 3 | `AvaryoCharacter.cpp` 2188 | 🟠 med | logic | нельзя потушить сигарету при `Panic≤1` → soft-lock «куришь у газа → форс-взрыв» |
| 4 | `AvaryoHUD.cpp` 1119 | 🟠 med | logic | предупреждение «НЕ КУРИТЬ» по статик `GasRadius`, а взрыв растёт до 4× → нет варнинга в ~94% облака |
| 5 | `ARepairable.cpp` 666-684 | 🟠 med | perf | live-wire шок: неоттроттленный full-world `TActorIterator` каждый кадр, пока рядом нет жертвы |
| 6 | `AvaryoCharacter.cpp` 2742-2771 | 🟡 low | perf | `UpdateTrip` — два full-world скана (chars+pickups) каждый кадр спринта, без spatial-query/throttle |
| 7 | `VitalsComponent.cpp` 311 | 🟡 low | regression | burn-паника — единственная прямая запись `Panic` без гарда `Av.NoPanic`/`bSafeZone` |
| 8 | `ARunState.cpp` 624-636, 1020-1026 | 🟡 low | coop-race | `PlayerStats` не чистится при дисконнекте → `ShiftNet` суммирует ушедших + дабл-каунт на реконнект |
| 9 | `AvaryoHUD.cpp` 638-645 | 🟡 low | perf | миникарта сканирует статик `AExitZone`/`AToilet` полным `TActorIterator` каждый DrawHUD-кадр |
| 10 | `AvaryoCharacter.cpp` 865-873 | 🟡 low | logic | адреналин ×1.15 применяется ПОСЛЕ stumble-клампа → споткнувшийся на низком HP едет быстрее `TripSlowSpeed` |
| 11 | `ARunState.cpp` 432-436 | 🟡 low | perf | `RefreshCompanyMirror` строит 5×`FName(TEXT(...))`+lookup каждый серверный тик (2 Гц) |

---

## 🔴 High

### 1. DoT/повторный урон мгновенно «добивает» раненого мимо окна подъёма
`UVitalsComponent::ApplyDamage` (444-472) гейтит только `!IsVitalAuthority() || Amount<=0 || bInvulnerable` — НЕ `bWounded`. У раненого `Health` уже 0, поэтому **любой** следующий вызов `ApplyDamage` (даже на доли HP) проваливается в `Health<=0` → `else if (!bUnconscious){ bUnconscious=true; }` (467-469) и мгновенно переводит wounded→unconscious, минуя `WoundedBleedOut` (463) — 40-сек окно «подняться/подлечиться/дотащить» (кооп-revive). Комментарий на 310 «`ApplyDamage` сам гейтит wounded» — **неверен**.
Триггеры (все реальны): **горение** (burn-DoT тик, 300-321 — газ-взрыв `ExplodeGas` поджигает ВСЕХ рядом без фильтра `IsWounded`), live-wire шок, ловушка, био-снаряд. Раненый возле любой опасности теряет окно подъёма мгновенно.
**Фикс (нужно решение по геймдизайну):**
(а) корректность: эскалацию wounded→unconscious запускать только от «значимого» удара, а не от каждого DoT-тика при `Health==0` (порог `Amount`), **или**
(б) дизайн: не применять burn-DoT/тушить горящего при уходе в wounded (gate burn-блока на `!bWounded` + сброс `BurnRemaining` в `OnWounded`).
Минимум — поправить ложный комментарий на 310.

### 2. Бесконечный огнетушитель (`Charges==-1`) глохнет на 1-м кадре спрея (неполный AUDIT2 #9)
`TickSpray` (2534-2555): вход (2537) исправлен под `-1` (`Charges==0`), но **пост-дрейн (2550) остался `if (Item->Charges <= 0)`**. Для `-1`: while-цикл (2545, `Charges>0`) не крутится, `-1` остаётся → строка 2550 `-1<=0` TRUE → `Charges=0` (2552) + `StopSpraying()`. Со следующего кадра вход `Charges==0` тоже глушит. Итог: бесконечный баллон превращается в пустой и навсегда не пшикает. `APickupItem::Charges` по умолчанию `-1`, путь достижим для любого баллона на дефолте.
**Фикс:** строку 2550 заменить `<= 0` → `== 0` (while декрементит финитный ровно до 0; `-1` в цикл не входит и должен сохраняться). Никогда не перезаписывать отрицательный (бесконечный) `Charges` нулём.

---

## 🟠 Medium

### 3. Нельзя потушить сигарету при `Panic≤1` → soft-lock «куришь у газа»
`CanApplyEffect` Calm (2188) = `Panic>1`. `BeginUseHeldItem` (2403) рано выходит при `!CanApplyEffect`. `StopSmoking` (2245) — **единственный** инпут-путь потушить. Курение само снижает панику → закурил, паника спала ≤1 → use больше не доходит до `ApplyItemEffect` → потушить нельзя. `ARepairable.cpp:607` поджигает газ при `IsSmoking()`; коммент на 2245 «рядом газ — чтоб не рвануло» подтверждает, что тушение у газа — задуманное действие. Игрок залочен в курении и форсится во взрыв.
**Фикс:** `case EItemEffect::Calm: return VitalsComponent->IsSmoking() || VitalsComponent->GetPanic() > 1.f;`

### 4. Варнинг «НЕ КУРИТЬ» по статик радиусу, а взрыв-зона растёт до 4×
HUD (1119) сверяется с `Obj->GasRadius` (статик 150). Реальный поджиг (`ARepairable.cpp:593/607`) — с растущим `CurrentGasRadius = GasRadius*Min(1+GasSpreadPerSecond*Elapsed, GasSpreadMaxScale=4)` (до 600). Площадь ∝ r² → статик-радиус покрывает лишь ~1/16 выросшего облака: игрок на 150-600 см от утечки **в смертельной зоне без предупреждения** (закурил → взрыв). `CurrentGasRadius` реплицируется (`DOREPLIFETIME`), геттер есть (`ARepairable.h:143`).
**Фикс:** в HUD заменить `Obj->GasRadius` → `Obj->GetCurrentGasRadius()`.

### 5. Live-wire шок: full-world скан каждый кадр, пока нет жертвы
`ARepairable::Tick` 666-684: `LiveWireShockCooldown` ставится (679) **только** когда жертва найдена (внутри loop, перед `break`). Если объект сломан+под напряжением (`IsLiveWireHot()`), но рядом никого — loop завершается без установки кулдауна → `cooldown` остаётся 0 → `TActorIterator<AAvaryoCharacter>` по всему миру каждый кадр. Газ (585-589) и потоп (`FloodCheckAccum`) троттлятся аккумулятором 5 Гц — у live-wire аккумулятора нет (в хедере только `LiveWireShockCooldown`). Несколько обесточенных-в-аварии объектов = постоянный per-frame O(N) обход.
**Фикс:** добавить `LiveWireCheckAccum` (как `GasCheckAccum`): входить в скан раз в ~0.2 с и сбрасывать аккумулятор независимо от того, найдена жертва или нет.

---

## 🟡 Low

### 6. `UpdateTrip` — два full-world скана каждый кадр спринта
2742 (`TActorIterator<AAvaryoCharacter>`) + 2757 (`TActorIterator<APickupItem>`), оба полным обходом мира каждый тик во время спринта, плюс `GetSafeNormal()` на кандидата. На карте с кучей выроненного хлама (механика «споткнись о свой хлам» сама поощряет физ-пикапы на полу) — горячий per-frame скан без spatial-query.
**Фикс:** `OverlapMultiByObjectType` радиусом ~120 (уже используется в `TickSpray`) вместо двух итераторов, либо троттл-аккумулятор (бросок «споткнулся» не нуждается в per-frame свежести).

### 7. Burn-паника без гарда `Av.NoPanic`/`bSafeZone` (пропуск AUDIT2 #7)
Строка 311 — единственная прямая запись `Panic`, не обёрнутая в гард: incident (277) под `Av.NoPanic`, wet (295) под `!bSafeZone && Av.NoPanic`, pain (454) под `Av.NoPanic`. Механику «Горит» добавили после AUDIT2 — гард забыли. Горящий копит панику даже в тест-режиме (`Av.NoPanic=1`)/safe-zone.
**Фикс:** обернуть 311 как wet: `if (!bSafeZone && CVarAvNoPanic.GetValueOnGameThread()==0) { ... }`.

### 8. `PlayerStats` не чистится при дисконнекте
`FindOrAddStats` (624-636) только добавляет, ключ — уничтожаемый `Character`. Нет `EndPlay`/Logout-хука. Дисконнект → пешка уничтожена, `Character` в записи null, но запись висит в реплицируемом `PlayerStats` (398) до конца карты. `FinishRun` (1020-1026) суммирует `ComputePlayerBalance` по ВСЕМ записям → премии/штрафы ушедшего попадают в `ShiftNet`/карьеру; HUD «Акт» (466-489) рисует строку с null-персонажем; реконнект заводит НОВУЮ запись → вклад дважды.
**Фикс:** связывать `FPlayerRunStats` с `APlayerState` (стабильный per-человек id), а не с персонажем; либо в `FinishRun`/HUD пропускать `!IsValid(S.Character)`.

### 9. Миникарта сканирует статик-акторы каждый кадр
638-645: `TActorIterator<AExitZone>` + `TActorIterator<AToilet>` каждый DrawHUD-кадр. Это статично размещённые акторы, их число/позиции не меняются в рантайме — тот же per-frame O(N) паттерн, что AUDIT1 #12 убрал для `ARunState::Get`. (Итератор бригады на 661 кэшировать сложнее — спавн/деспавн.)
**Фикс:** кэшировать позиции `AExitZone`/`AToilet` один раз (BeginPlay/лениво), не сканировать 60+ раз/с.

### 10. Адреналин негасит stumble-клампа
`RefreshMoveSpeed`: stumble-кламп (867) `Speed=Min(Speed, TripSlowSpeed=150)` идёт ДО адреналина (870-873) `Speed*=AdrenalineSpeedMult=1.15` → споткнувшийся не-раненый на низком HP едет ~172.5 вместо 150, частично сводя стаггер. wounded-ветка (874) спасает только раненого.
**Фикс:** сделать stumble-кламп финальным шагом (после адреналина и exhaust-стана), либо применять адреналин до клампа.

### 11. `RefreshCompanyMirror` строит 5 FName/тик
432-436: `Ledger->GetEquipmentLevel(FName(TEXT("Welder")))` ×5 — каждый раз хеш строки + лок таблицы имён, в Tick @2 Гц. Имена известны на компиляции.
**Фикс:** `static const FName` на пять имён (хеш один раз), либо типизированные геттеры в `UCompanyLedgerSubsystem`.

---

## Рекомендация
**Чинить первыми:** #1 (revive-окно — ядро кооп-механики; видно и в соло у любой опасности рядом с раненым), #2 (бесконечный огнетушитель сломан — регрессия). Затем #3 (soft-lock у газа), #4 (честный варнинг), #5 (live-wire perf). Low — по ходу при следующем ребилде.
Почти все фиксы — 1-3 строки; #1 и #3 требуют твоего решения по геймдизайну (поведение огня на лежачем / тушение сигареты в покое). Готов править по отмашке (закрыть редактор → `Build.bat` → смоук → коммит).

## ⚠️ Покрытие неполное (session-limit)
Из 11 финдеров до лимита дошли 8; **не отработали:** `world-props` (Toilet/Trap/BioProjectile/PowerSwitch/Door/Floodlight/FoamPatch/ExitZone/CallBoard/ToolCase), `items+subsystems` (PickupItem/BioPickup/Flashlight/CompanyLedger/OnlineSubsystem/MenuGameMode/PlayerController/SaveGame), и часть спец-проходов (numeric/coop-race/regression — двое из них вернулись пустыми, один убит). **Следующий шаг по покрытию:** дочистить эти области (отдельным мелким проходом или resume воркфлоу после сброса лимита — `resumeFromRunId: wf_7f3c2039-ff5`, кэш 8 финдеров вернётся мгновенно).

_Связано: `CODE_AUDIT.md` (1-й, репликация), `CODE_AUDIT2.md` (2-й, game-logic), WORKLOG._
