# Аудит проекта «Аварийка» (автономный, ночь)

Прогон по пяти осям: кооп/репликация, геймплей-баги, пред-релизная отладка, перформанс, плейсхолдеры/лицензии. Все находки ниже подтверждены чтением кода (47 сырых → 39 подтверждённых, состязательная перепроверка каждой). Серьёзность отсортирована high→low внутри каждого раздела. Главное: перед сборкой обязательно вернуть отладочные витали и почистить чит-консоль; критичный класс багов — россыпь незащищённых `GetWorld()` в Tick-пути (потенциальные краши); перформанс просаживают пер-фреймовые обходы всего мира; релиз блокируют нелицензированные паки.

> Статус правок: ✅ `bTeamInside` — пофикшено этой ночью (кооп). Остальное — НЕ трогал автономно: перф меняет тайминги (нужна проверка в игре), отладочные команды/витали нужны СЕЙЧАС для тестов (это пред-релиз), лицензии — бизнес-решение.

## Кооп / репликация

| Severity | Что | Где (file:line) | Фикс |
|---|---|---|---|
| medium | ✅ ИСПРАВЛЕНО: `bTeamInside` не реплицировался → на клиентах статус выхода всегда `false` | `Source/Avaryo/Public/World/AExitZone.h` | Добавлен `UPROPERTY(Replicated)` + `DOREPLIFETIME` |
| low | `IsTeamInside()` `BlueprintPure` для клиентского HUD (закрыто фиксом выше) | `AExitZone.h:32-34` | Решается репликацией bTeamInside |
| low | OK: спрей-конус `APickupItem` синхронизирован (`bSpraying` + `OnRep_Spraying`) | `Items/APickupItem.cpp:56-71` | — |
| low | OK: вся виталика реплицируется, серверный расчёт под `HasAuthority` | `Components/VitalsComponent.cpp:68-89` | — |
| low | OK: общие эффекты через `NetMulticast, Unreliable` с сервера | `World/ARepairable.cpp:1087+`, `ATrap.cpp:159` | — |

## Геймплей-баги

Единый класс: незащищённый `GetWorld()` в коде Tick-пути (Tick→Find*) и в `BeginPlay`/спавнах. В тех же файлах есть правильный паттерн (строки 1448/1458/1465) — это несогласованность. Реальный риск краша низкий (у живого актора в Tick мир есть), но гигиенически стоит привести к защищённому виду перед релизом.

| Severity | Что | Где (file:line) | Фикс |
|---|---|---|---|
| high | `GetWorld()->SweepSingleByChannel()` без null-чека — `FindFocusedItem` | `AvaryoCharacter.cpp:727` | `if (UWorld* W = GetWorld()) {...}` |
| high | То же — `FindFocusedRepairable` | `AvaryoCharacter.cpp:797` | null-чек |
| high | То же — `FindFocusedToilet` | `AvaryoCharacter.cpp:1200` | null-чек |
| high | То же — `FindFocusedWoundedTeammate` (+1229) | `AvaryoCharacter.cpp:1219` | null-чек |
| high | `SpawnActor` без null-чека (Bio/Trap/BioProjectile/Floodlight) | `AvaryoCharacter.cpp:987,1733,1751,1767` | `if (UWorld* W = GetWorld())` |
| high | `LineTraceSingleByChannel()` без null-чека (+1995,2028,2034) | `AvaryoCharacter.cpp:2022` | null-чек |
| high | `GetGameState()` без null-чека в `BeginPlay` | `Game/ARunState.cpp:272` | тернарник |
| high | `GetGameInstance()` без null-чека в `BeginPlay` | `Game/ARunState.cpp:278` | тернарник |
| medium | `TActorIterator<T>(GetWorld())` без null-чека (много мест) | `AvaryoCharacter.cpp:288,1229`; `VitalsComponent.cpp:222,269`; `ARunState.cpp:243,467,689,916,951` | защищённый паттерн |

## Пред-релиз / отладка

Все пункты — high. **Сейчас нужны для тестов; убрать/гейтить ПЕРЕД сборкой релиза.**

| Severity | Что | Где (file:line) | Фикс |
|---|---|---|---|
| high | Отладочный старт `Health=50`, `Panic=50` (TODO «вернуть 100/0») | `Components/VitalsComponent.cpp:17-18` | `Health=100; Panic=0` перед релизом |
| high | `AvVital` — задать любую виталь | `AvaryoCharacter.h:719-720` | `#if !UE_BUILD_SHIPPING` |
| high | `AvIncident` — мгновенный инцидент | `AvaryoCharacter.h:723-724` | гейт |
| high | `AvGiveBio` — биооружие в руки | `AvaryoCharacter.h:727-728` | гейт |
| high | `AvCheapGear` — форс дешёвого комплекта | `AvaryoCharacter.h:731-732` | гейт |
| high | `AvFinish win/lose` — завершить забег | `AvaryoCharacter.h:735-736` | гейт |
| high | `AvQuota` — выставить квоту | `AvaryoCharacter.h:739-740` | гейт |
| high | `AvShop` — открыть магазин локально | `AvaryoCharacter.h:747-748` | гейт |
| high | `AvGod` — бессмертие + рефилл | `AvaryoCharacter.h:751-752` | гейт |

## Перформанс (важно — у тебя лагает)

Болезнь — обход всего мира (`TActorIterator`/`TObjectIterator`) в Tick/HUD без троттлинга. У части акторов троттлинг уже есть (`AFloodlight=0.25s`, `AFoamPatch=0.5s`, `ATrap=0.1s`) — привести остальных к тому же. ⚠️ `ARepairable` НЕ троттлить наивно: курсор-мини-игра требует 60 Гц (иначе курсор дёрганый) — там нужен точечный фикс (троттлить только газ-петлю, а не весь Tick).

| Severity | Что | Где (file:line) | Фикс |
|---|---|---|---|
| high | `IsLitByNearbyLight()` — `TObjectIterator<UPointLightComponent>` по ВСЕМ лампам мира, из Tick раз в 0.3с | `Components/VitalsComponent.cpp:297-314` | кэш света / spatial / реестр в subsystem |
| high | `Vitals::TickComponent` — до 4 `TActorIterator<AAvaryoCharacter>` | `VitalsComponent.cpp:121,147,222,270` | `SetComponentTickInterval(0.2f)` + кэш соседей |
| high | `ABioProjectile::Tick` — O(n) обходы каждый кадр | `World/ABioProjectile.cpp:74-105` | `TickInterval=0.1f` + `GetOverlappingActors` |
| high | `ARepairable::Tick` — `TActorIterator` каждый кадр при утечке газа | `World/ARepairable.cpp:467-481` | троттлить ТОЛЬКО газ-петлю (аккум-таймер), не весь Tick (курсор!) |
| high | `AAvaryoCharacter::Tick` — `TActorIterator` ради чест-камер каждый кадр | `AvaryoCharacter.cpp:288-294` | перенести в `ToggleMonitor()`, апдейт на переходе |
| medium | `ABioProjectile` без `TickInterval` (60+ Гц) | `World/ABioProjectile.cpp:13` | `PrimaryActorTick.TickInterval=0.1f` |
| medium | `AvaryoHUD::DrawHUD` — 5 `TActorIterator` каждый рендер-кадр | `UI/AvaryoHUD.cpp:172-213,561,565,584,990-998` | кэш в `UpdateHUD()` раз в 0.5с |

## Плейсхолдеры / лицензии (пред-релизный блокер)

| Severity | Что | Где | Фикс |
|---|---|---|---|
| high | `Survival_SFX` — 13 хардкод-ссылок (движение/крафт/предметы/UI/ловушки) | `AvaryoCharacter.cpp:142-180`, `ATrap.cpp`, `ARepairable.cpp`, `UFlashlightComponent.cpp` | лицензировать или заменить |
| high | Ghosthack Garage — 7+ ремонтных луп-звуков (кор) | `World/ARepairable.cpp:91,108-130` | лицензировать/заменить |
| high | Ghosthack Explosives + Magic Lightning — хазард-звуки | `ARepairable.cpp:72,132`, `AFloodlight.cpp` | лицензировать/заменить |
| high | `PostApocalypticHouse` (4.8GB) — прототип дома | `Content/PostApocalypticHouse/Maps/` | лицензировать или пересобрать |
| high | `City Modular Hospital v.2` (3.1GB) — флагман-локация | `Content/Hospital/Maps/` | лицензировать или cherry-pick+пересборка |
| high | `HospitalCombo` (VOL 1-6, 1.2GB) — мебель госпиталя | `Content/HospitalCombo/` | лицензировать/заменить |
| high | `Construction_Pit` (2.1GB) — задел мультилокации | `Content/Construction_Pit/` | отложить/заменить до фичи |
| medium | `EasyOptionsMenu` — экран настроек | `UI/MenuHUD.cpp:195-196` | лицензировать (~$40-80) или свой UMG |
| medium | `NiagaraExamples` (взрыв/искры/дым) — бесплатно, но плейсхолдер-качество | `ARepairable.cpp:80-86` | по желанию апгрейд VFX-пака |

## ТОП-5 на починку первыми

1. **Вернуть боевые витали** перед релизом — `Health=50→100`, `Panic=50→0` (`VitalsComponent.cpp:17-18`).
2. **Зачистить чит-консоль** — 9 `Exec`-команд обернуть в `#if !UE_BUILD_SHIPPING` (`AvaryoCharacter.h:719-752`).
3. **Защитить `GetWorld()`** в Find*/Spawn/трейсах + `ARunState::BeginPlay` (гигиена против крашей).
4. **Снять пер-фреймовые обходы мира** — `IsLitByNearbyLight`, `Vitals::Tick`, чест-камеры, HUD; точечно газ-петля `ARepairable`. Главный просад FPS.
5. **Развязать релиз с нелицензированными паками** — Hospital(+Combo), House, Survival_SFX, Ghosthack-аудио.

## Пред-релизный чек-лист

- [ ] `VitalsComponent.cpp:17-18`: `Health=100`, `Panic=0`.
- [ ] Гейт `#if !UE_BUILD_SHIPPING` на 9 `Exec`-команд (+ их серверные RPC).
- [ ] null-чек `GetWorld()` во всех Find*/SpawnActor/трейсах + `ARunState::BeginPlay`.
- [ ] Перф: TickInterval/кэш для Vitals, света, чест-камер, HUD; точечно газ-петля `ARepairable`; `TickInterval` на `ABioProjectile`.
- [x] Реплицировать `AExitZone::bTeamInside` — **сделано**.
- [ ] Лицензировать/заменить: `Survival_SFX`, Ghosthack (ремонт+хазард), `EasyOptionsMenu`.
- [ ] Лицензировать/пересобрать окружения: Hospital(+Combo), PostApocalypticHouse; `Construction_Pit` — отложить.
- [ ] Опц.: апгрейд VFX с `NiagaraExamples`.
