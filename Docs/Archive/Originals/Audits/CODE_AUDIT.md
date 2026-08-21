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
