# Code Audits

This consolidated document preserves the source material listed below. Originals are archived under Docs/Archive/Originals/.

## Sources
- `CODE_AUDIT.md`
- `CODE_AUDIT2.md`
- `CODE_AUDIT3.md`

---

## Source: CODE_AUDIT.md

# CODE_AUDIT — баги C++ механик (read-only, адверсариально проверено)

> Автономный аудит 2026-06-21 (Workflow: 5 подсистем → находки → каждая проверена скептиком-агентом по коду).
> **23 находки → 12 подтверждённых** (ложные отсеяны). Код НЕ правил — на твою отмашку.
> ⚠️ Почти всё — **кооп-репликация/десинк**: в соло-PIE (как ты тестишь) НЕ видно, ценность для кооп. Линии могли сдвинуться.

> ✅ **СТАТУС 2026-06-21: ВСЕ 12 ИСПРАВЛЕНЫ** и запечены полным ребилдом (Build Succeeded 13с, смоук чист — Engine initialized).
> #1/#4 через Live Coding, #2/#3/#5/#6/#7/#8/#9/#10/#11/#12 — правки кода + полный ребилд. PIE-проверка кооп-эффектов за тобой.

## Сводка

| # | Файл | Severity | Тип | Суть |
|---|---|---|---|---|
| 1 | `APowerSwitch.cpp` | 🟠 medium | coop-desync | свет `PoweredLight` гасится только на сервере — клиенты не видят темноту |
| 2 | `ATrap.cpp` | 🟠 medium | coop-desync | вспышка ловушки затирается блинком у клиентов (`bTriggered` не реплик.) |
| 3 | `AvaryoCharacter.h` (SelfNoise) | 🟠 medium | replication | HUD-шкала шума мертва у не-хост клиентов (кашель/икота/спотык/бросок) |
| 4 | `AvaryoCharacter.cpp` (RefreshMoveSpeed) | 🟠 medium | logic | волокомого можно «ползти против драга» → джиттер (виден и в соло) |
| 5 | `AToilet.cpp` (минигейм) | 🟠 medium | coop-desync | тайминг судится по лагнутому курсору → несправедливые промахи у клиентов |
| 6 | `MenuHUD.cpp` (OpenSettings) | 🟠 medium | leak | повторные клики стакают виджеты настроек + хитбоксы под ними живы |
| 7 | `ARepairable.cpp` | 🟡 low | coop-desync | состояние ремонта не чистится при Repairer→null (дисконнект) — фантом-луп |
| 8 | `AvaryoCharacter.cpp` (chest-cam) | 🟡 low | coop-desync | поздно-присоединившиеся не начинают захват камеры → чёрный тайл монитора |
| 9 | `ABioProjectile.cpp` | 🟡 low | leak | нет max-lifetime — не-оседающий снаряд тикает вечно (катится по лестнице) |
| 10 | `AToilet.cpp` (lock) | 🟡 low | lifetime | туалет уничтожен в сессии → игрок залочен (soft-lock) |
| 11 | `AvariikaOnlineSubsystem.cpp` | 🟡 low | logic | HostGame destroy→create без await — ре-хост сломается на EOS/Steam (Null ок) |
| 12 | `AvaryoHUD.cpp` (DrawHUD) | 🟡 low | perf | полный скан мира каждый кадр (ARunState::Get ×4 + TActorIterator) |

---

## Medium (видны в кооп; #4 и #6 — и в соло)

### 1. PowerSwitch: свет гаснет только у хоста — главная механика «свет↔ток» сломана в кооп
`ApplyToFloods()` (`APowerSwitch.cpp:90-122`) гасит лампы с тегом `PoweredLight` через `SetVisibility(bPowerOn)`,
но зовётся только из `ToggleBy()`/`BeginPlay()` под `HasAuthority()`. Видимость `ULightComponent` НЕ реплицируется.
У клиентов на смену `bPowerOn` срабатывает только `OnRep_Power→RefreshVisual()`, который трогает лишь СОБСТВЕННУЮ
лампочку щитка. → Хост вырубил свет (темно у хоста), а у клиентов комната по-прежнему освещена — не платят «цену темноты».
Поздние клиенты тоже не инициализируются. (Опасность/ток реплик. корректно — десинкает только свет.)
**Фикс:** перенести цикл `PoweredLight` в `RefreshVisual()` (он идёт и на клиентах через OnRep, и на сервере) + звать на клиентах в `BeginPlay`.

### 2. Trap: драматичная вспышка затирается блинком у клиентов
`bTriggered` — обычный bool (`ATrap.h:78`, не реплик.), ставится true только на сервере. `MulticastFlash()` зажигает
индикатор на 8000 на всех, но `Tick` (каждые 0.1с) делает `if (bArmed && !bTriggered) SetIntensity(200+1200*Blink)`.
У клиентов `bTriggered` всегда false → следующий тик затирает вспышку тусклым блинком. Вспышка видна только хосту.
(WORKLOG уже отмечал это как «косметика пропущена».)
**Фикс:** локальный `bFlashing` в `MulticastFlash_Implementation()`, гейтить блинк по нему (чище, чем реплицировать — гонка с 0.3с destroy).

### 3. SelfNoise: HUD-шкала «шума» мертва у не-хост клиентов
`SelfNoiseLevel/SelfNoiseTime` — обычные члены (не UPROPERTY/реплик.). Все `RegisterSelfNoise()` — серверные
(VitalsComponent winded/cough/hiccup/incident, ReleaseHeldItem, TriggerStumble, FlashlightDeadBattery). А `AvaryoHUD:345`
читает `GetSelfNoise01()` на ЛОКАЛЬНОМ пешке. → У присоединившегося клиента шкала шума стоит на 0 для всех событий.
**Фикс:** Client/owner-RPC из `RegisterSelfNoise`, или `DOREPLIFETIME_CONDITION COND_OwnerOnly` + клиентский decay.

### 4. Драг напарника: волокомого можно «ползти против движения» (видно и в соло)
В `RefreshMoveSpeed`: `if (DraggedBy) Speed=0` затирается ниже безусловным `if (IsWounded()) Speed=CrawlSpeed` (120).
Тащить можно только раненого → строка `DraggedBy=0` всегда перезаписана → волокомый-в-сознании инпутит ползанье против
`TickDrag VInterpTo` → джиттер/перетягивание. Гард `DraggedBy` — мёртвый код.
**Фикс:** переместить проверки `DraggedBy`/`Unconscious` ПОСЛЕ `IsWounded`-ветки, или финальный `if (DraggedBy || IsUnconscious()) Speed=0` последним.

### 5. Toilet-минигейм: несправедливые промахи у клиентов
Курсор едет на сервере и реплицируется голым float (30Гц, без интерполяции). HUD клиента рисует устаревший курсор,
а `TryHitBy` судит по ТЕКУЩЕМУ серверному. При CursorSpeed 0.8×SpeedMult 1.7 ~100мс лага = дрейф ~0.13-0.14 ≈ вся
зелёная зона (0.14). Клиент целится в центр зелёного, что видит → на сервере промах. Хост не страдает.
**Фикс:** лаг-компенсация (допуск по пингу) или детерминированный курсор от реплицированного start-time (все считают фазу локально).

### 6. MenuHUD: утечка виджетов настроек + клики сквозь оверлей (видно в соло-меню)
`OpenSettings()` (`MenuHUD.cpp:187-206`) делает `CreateWidget+AddToViewport(100)` без guard/ссылки/RemoveFromParent.
N кликов = N стакнутых `EasyOptionsMenu`. + `DrawHUD` каждый кадр перерисовывает хитбоксы кнопок меню под оверлеем
(`bEnableClickEvents=true`, `FInputModeGameAndUI`) → клики по прозрачным зонам бьют по меню за оверлеем.
**Фикс:** `TWeakObjectPtr<UUserWidget> SettingsWidget` + early-return если валиден; UI-only input или `bSettingsOpen` гасит DrawButton; RemoveFromParent на закрытии.

---

## Low (робастность / поздний кооп / перф)

### 7. ARepairable: состояние ремонта не чистится при `Repairer→null`
Если чинящий пешка уничтожен посреди (дисконнект/смерть-через-destroy), реплик. `Repairer` нулится; серверный Tick
скипает обработку через `if (HasAuthority() && Repairer)`, но флаги (`bStarterPulling`/`bDoingPrereqHold`/`bBotching`...)
остаются true и реплицируются → фантомный аудио-луп + «чинится» состояние. Самолечится при следующем `BeginRepairBy`/`SetBroken`.
**Фикс:** в серверном Tick при null/invalid `Repairer` + любой активный флаг — сбросить флаги + стоп `FillAudioComp`.

### 8. Operator-монитор: поздно-присоединившиеся не захватываются
Цикл `bCaptureEveryFrame=bMonitorOpen` идёт только на смене `bMonitorOpen != bChestCaptureApplied`. Чар, заспавнившийся
пока монитор открыт, остаётся с `bCaptureEveryFrame=false` → его тайл чёрный, пока оператор не закроет/откроет монитор.
**Фикс:** пока монитор открыт — ре-применять флаг при спавне чара (или каждый кадр).

### 9. ABioProjectile: нет предохранителя времени жизни
Уничтожается только через linger-таймаут ПОСЛЕ `Splat()`. Splat — при прямом попадании или оседании (<40 скорость 0.4с).
Не-оседающий снаряд (катится по склону/лестнице, дрожит в углу) тикает вечно, каждый кадр итерируя всех `AAvaryoCharacter`.
**Фикс:** `SetLifeSpan()` — форс-сплат/destroy через пару секунд.

### 10. AToilet: не освобождает interaction-lock при teardown
`BeginUseBy` ставит `SetInteractionLocked(true)`; снимается только в `EndUseBy`/`FinishSession`. Нет `EndPlay`/`Destroyed`.
Туалет уничтожен/выстримлен посреди сессии → игрок навсегда залочен (движение+обзор off, реплик.) — soft-lock.
**Фикс:** override `EndPlay`/`Destroyed` → `EndUseBy(Occupant)`.

### 11. AvariikaOnlineSubsystem: HostGame destroy→create синхронно
`HostGame` зовёт `DestroySession` затем сразу `CreateSession`. На EOS/Steam destroy асинхронен → create с тем же именем
падает → `ServerTravel` молча не происходит. На Null работает случайно (destroy мгновенен). Ломает ре-хост на реальном бэкенде.
**Фикс:** если сессия есть — `CreateSession` из коллбэка `OnDestroySessionComplete`; иначе напрямую.

### 12. AvaryoHUD::DrawHUD: полный скан мира каждый кадр
`ARunState::Get(World)` = `TActorIterator`-скан, зовётся 4 раза за один DrawHUD; монитор+миникарта ещё гоняют свои
`TActorIterator<AAvaryoCharacter>`, миникарта — все `AExitZone`/`AToilet`. Несколько O(N) обходов мира на кадр.
**Фикс:** резолвить `ARunState` один раз вверху DrawHUD и переиспользовать; кешировать списки на RunState.

---

## Рекомендация
Все 12 — **не краши**. Приоритет на починку (когда дойдём до кооп-полировки, закрыв редактор для ребилда):
**#1 (свет в кооп)** и **#4 (драг-джиттер, виден в соло)** — первыми; затем #3/#5 (HUD/минигейм честность в кооп), #6 (меню).
Low — по ходу. Большинство фиксов — мелкие (реплик-флаг / порядок строк / EndPlay-хук). Готов править по отмашке.
_Связано: WORKLOG (прошлый аудит нашёл 4 бага репликации — паттерн повторяется), `live-pie-gotchas` (в соло не видно)._

---

## Source: CODE_AUDIT2.md

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

---

## Source: CODE_AUDIT3.md

# CODE_AUDIT3 — 3-й глубокий проход (perf / NaN / coop-race / regression)

> Автономный аудит 2026-06-22 (Workflow: финдеры × 5 линз — perf/Tick · numeric/NaN/overflow · coop-race · validation · logic; затем верификация по живому коду).
> Два раунда воркфлоу (оба упёрлись в session-limit на фан-ауте → verify доделан вручную по коду).
>
> ✅ **СТАТУС 2026-06-22: ВСЁ ИСПРАВЛЕНО** двумя коммитами + ребилдами (Build Succeeded, смоук чист).
> Раунд 1 (коммит `b4f55ac5`): 9 фиксов (#1-#11 ниже, #9 отложен осознанно). Раунд 2 (коммит ниже): покрытие + ре-ревью.

## Раунд 2 — покрытие непройденных файлов + ре-ревью своих фиксов (2026-06-22)

Второй воркфлоу (5 финдеров `effort:high`: world-props, items/subsystems, numeric, coop-race + **адверсариальный ре-ревью 9 фиксов раунда 1**). 18 сырых находок; верификация по коду вручную (лимит снова убил verify-фазу).
**Ключевой результат ре-ревью: из 9 моих фиксов 8 признаны корректными, найдена 1 регрессия (#R1).** Применено ещё **7 фиксов**, остальные отсеяны как ложные/маргинальные.

| # | Файл / строки | Sev | Тип | Суть | Статус |
|---|---|---|---|---|---|
| R1 | `AvaryoHUD.cpp` ~509 | 🟡 low | regression | мой фикс #8 неполон: HUD «Акт» рисовал ghost-строку дисконнекта без `IsValid`, строки не сходились с `ShiftNet` | ✅ гард `IsValid(S.Character)` |
| C1 | `CompanyLedgerSubsystem.cpp` 168 | 🟡 low | logic | `BuyUpgrade` неизвестного инструмента («Cameras»/опечатка) списывал→рефандил (2 сейва, дёрг баланса) | ✅ валидация имени до `TrySpend` |
| C2 | `CompanyLedgerSubsystem.cpp` 113 | 🟡 low | logic | `ResetCompany` не сбрасывал `QuotaWindowShifts` (свежая карьера наследовала окно прошлой квоты) | ✅ сброс в 0 |
| C3 | `ATrap.h` 86 | 🟡 low | coop-race | `MulticastFlash` = Unreliable: одношот-фидбэк ловушки (вспышка/тряска/звук) мог дропнуться, актор гибнет 0.3с | ✅ → Reliable |
| C4 | `AvariikaOnlineSubsystem` 32-59 | 🟡 low | coop-race | дабл-клик «Хост» до завершения destroy перезатирал `DestroyHandle` → двойной `CreateSession` | ✅ guard `bSessionTransition` |
| C5 | `UFlashlightComponent.cpp` 211 | 🟡 low | perf | `UpdateFlicker` жёг RNG/Sin/SetIntensity на выделенном сервере (луч не рендерится) | ✅ early-out `NM_DedicatedServer` |
| C6 | `AFoamPatch.cpp` 32 | 🟡 low | perf | `SetRelativeScale3D` дёргал transform каждый тик константой | ✅ применять только при изменении (`Equals`) |

**Отсеяно при ручной верификации (ложные/не достижимы/приемлемо):** AToilet seat-teleport (movement-репликация+`ClientSetControlYaw` уже покрывают); ACallBoard `SelectedIndex` (детерминированный `BeginPlay`-init на всех машинах, рантайм-сеттера нет); AExitZone re-fire ReadySound (ре-нотификация при повторной готовности — разумно); ABioProjectile splat-collision (клиенты кинематичны+replicated movement, overlap-only, гибнет за LingerTime); Flashlight intensity `-1` (`OnIntensity` зовётся только при non-null `AttachedLight` → `DefaultIntensity` уже реальный); APowerSwitch `RefreshVisual` O(N) (следствие фикса AUDIT1 #1, скан только на toggle/BeginPlay, кэш рискнул бы застейлить); BuyUpgrade двойной Save (второй персистит Equipment — нужен); AToilet billboard per-frame (маргинально, требует accumulator+.h ради копеек).

---

## Раунд 1 — 11 находок (9 исправлено в `b4f55ac5`)

> Раунд 1: 8 из 11 финдеров отработали до лимита; 12 кандидатов верифицированы вручную (все подтверждены). **Код раунда 1 исправлен** в `b4f55ac5`.

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

## Итог (раунд 1 + раунд 2)
**Раунд 1** — 9 фиксов в `b4f55ac5` (#1-#11, #9 миникапа отложена осознанно). Решения юзера по геймдизайну: огонь на лежачем НЕ добивает (сохраняем revive-окно), сигарету тушить можно в покое.
**Раунд 2** — 7 фиксов покрытия + 1 регрессия моего же фикса #8 (#R1). Все собраны (Build Succeeded) + смоук чист.
Осталось отложенным: **#9** (кэш миникарты — микро-выигрыш, риск устаревания) и **#5 из AUDIT2** (E-aim-gate — нужен PIE-тюнинг). Оба — низкий приоритет, требуют PIE/решения.

## Покрытие
Раунд 2 закрыл непройденные в раунде 1 области: world-props (Toilet/Trap/BioProjectile/PowerSwitch/Door/Floodlight/FoamPatch/ExitZone/CallBoard/ToolCase), items+subsystems (PickupItem/BioPickup/Flashlight/CompanyLedger/OnlineSubsystem), numeric+coop-race спец-проходы, и ре-ревью всех 9 фиксов раунда 1. **Тонко покрыто/не трогали:** ADoor, MenuGameMode, AvaryoPlayerController, AvariikaSaveGame, AvaryoCameraShakes (мелкие, низкий риск). Кооп-эффекты всех трёх аудитов — за PIE-проверкой юзера (в соло не видны).

_Связано: `CODE_AUDIT.md` (1-й, репликация), `CODE_AUDIT2.md` (2-й, game-logic), WORKLOG._

