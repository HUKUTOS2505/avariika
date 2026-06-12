# WORKLOG — модульная разработка «Аварийки»

**Зачем этот файл:** контекст переписки с Claude чистится; после очистки Claude читает этот файл и продолжает работу. Обновлять после каждого модуля.

---

## Правила работы (не нарушать)

- **Работаем по модулям**: один модуль за раз, доделываем до конца (код + сборка + смоук + размещение + доки + пуш), только потом следующий.
- **Редактор пользователя не открывать.** Сборка только при закрытом редакторе:
  `"C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="D:\unrealEngine\avariika\avariika.uproject" -WaitMutex`
- Ассеты/уровень — headless-скриптами: `UnrealEditor-Cmd.exe <uproject> -run=pythonscript -script=<py>` (скрипты в `Scripts/`, все идемпотентны).
- Смоук-тест после каждого блока: `UnrealEditor-Cmd.exe <uproject> -game -nullrhi -nosound -unattended -log` ~40 c, лог `Saved/Logs/avariika.log` на `Fatal|Ensure|: Error:`.
- Коммит + пуш (GitHub `HUKUTOS2505/avariika`, ветка `main`) после каждого рабочего куска.
- **Монстра-слухача НЕ делать** без явной отмашки пользователя (вся шумовая база уже готова).
- Фиолетовый цвет в UI запрещён, акцент — оранжевый.
- Перед «релизом» вернуть Health=100, Panic=0 в `VitalsComponent.cpp` (сейчас 50/50 для тестов).
- 3D-модели делает пользователь в **meshy.ai** по промптам Claude; анимации делает пользователь. Импорт моделей — `Scripts/` + `asset.import` или `set_static_mesh` по метке актора.

## Что уже сделано (стабильно, в main)

- **Ядро**: инвентарь (1 тяжёлый + 4 лёгких, правило сварочника), фонарь с батареей, шкалы HP/паника/выносливость/туалет, ранение→ползание→аптечка, огнетушитель, передача предметов ПКМ, присед.
- **Цикл забега**: ARepairable (щиток/труба/генератор), случайные поломки на старт (мин. 2), ARunState (фазы, таймер, статистика, спавнится `URunStateSubsystem` — BP GameMode не тронут), зона ГАЗели (AExitZone), победа = всё починено + все в зоне; поражение = все ранены; R на финальном экране = рестарт.
- **«Акт выполненных работ»**: статистика всех (починки/подъёмы/эвакуации/туалет/ранения/инциденты/паника), мемные звания, рубли, итог бригады.
- **Кооп**: перетаскивание раненого (E взял/E отпустил), совместный перекур (250 см), монитор оператора (Tab в зоне ГАЗели — нагрудные камеры SceneCapture 320×180 всех).
- **Хаос**: газовая труба травит газ → курение в облаке = взрыв (урон/паника/шум/прогресс сгорает); рация-тумблер шипит из кармана.
- **Ночь**: луна 0.6 lux, туман, PPV_Night (экспозиция 0.25–0.8), красные пульсирующие лампы на сломанном, лут по тёмным углам, миникарта на Canvas, тряска камеры (паника — бесконечный мягкий шейк; взрыв — мультикаст-толчок).
- **Шум под монстра**: бег, падение, распыление, починка, волочение, рация, туалет-промахи, взрыв, замыкание — всё через MakeNoise.

## ТЕКУЩИЙ МОДУЛЬ: «Туалет + Щиток» (мини-игры)

### Статус: код готов, собран, смоук чистый, ЗАПУШЕН. НЕ проверен пользователем в PIE.

Сделано в этом модуле:
- **Фикс**: дёрганье камеры вправо при беге — паник-шейк перезапускался каждые 0.9 с; теперь бесконечный, старт/стоп по входу/выходу из паники.
- **Туалет-мини-игра** (`AToilet`): E у куба-сиденья → телепорт НА куб, разворот на 180°, движение+камера заблокированы (`AAvaryoCharacter::SetInteractionLocked`, `ClientSetControlYaw`). Верхняя полоска — «содержимое» (уходит само ~2%/с), нижняя — курсор пинг-понг; зелёная зона (старт 20%) −30, жёлтая ОТДЕЛЬНАЯ зона (старт 70%) −12, промах −2 + громкий шум. Зоны переезжают (не пересекаясь), курсор ускоряется до ×1.7. Ноль = визит в «Акт». **Встать — G.**
- **Щиток-мини-игра** (`ARepairable`, `bMinigameRepair=true` только у Repairable_Breaker): нужен **Тестер** в руках (`RequiredTool=Tester`), E → фиксация на месте, хаотичная зелёная зона; попал = +1/4 прогресса; промах = ток −15 HP +8 паники; **3 промаха = дуга по всем в 350 см (−25 HP +15 паники) и «ЗАМКНУЛО» на 60 с для всех** (таймер на табличке и в подсказке). Выход — G. Параметры все EditAnywhere.
- **Тестер** — новый предмет (PickupItem, ToolTag=Tester), лежит у ГАЗели (240, 60).
- Скрипт модуля: `Scripts/setup_minigames.py`.

### Осталось в модуле (СЛЕДУЮЩИЙ ШАГ):
1. **Ждём фидбек пользователя из PIE** (чек-лист в TESTING.md, раздел «мини-игры»). **Приоритет №1 — фиксы багов из его фидбека.**
2. Модели из meshy.ai — **ОТЛОЖЕНЫ пользователем** («позже сгенерирую»). Когда принесёт FBX в `RawAssets\` — гнать `Scripts/import_models.py`.
3. Анимации — после моделей.

⚠️ Если пользователь играет, его редактор ОТКРЫТ: сборка и headless-скрипты невозможны (Live Coding/конфликт записи). Перед сборкой проверять `Get-Process UnrealEditor*`.

## МОДУЛЬ «Труба + Генератор» (мини-игры) — КОД ГОТОВ 2026-06-12

### Статус: собран, setup-скрипт прогнан, смоук чистый. НЕ проверен пользователем в PIE.

- `ERepairMinigameType: None/Cursor/Valve/Starter` вместо bool `bMinigameRepair` (щиток=Cursor, труба=Valve, генератор=Starter). Выставлено на уровне скриптом `setup_minigames.py`, уровень сохранён.
- **Труба — «вентиль»** (без инструмента): E — фиксация; тык E = +12% (`ValveTurnAmount`); тык чаще ~0.7 с (`ValveMinInterval`) = «срыв резьбы»: −20%, MakeNoise 1.0. HUD: полоска ритма (красная→зелёная = можно тыкать). Газ травит пока не закрыто.
- **Генератор — «стартер»** (Welder остаётся): держать E — натяжение растёт (`StarterChargeTime` 1.6 с); отпустить в окне 70–90% = рывок (+1/3, нужно `StarterPullsToFix`=3); рано/перетянул до 100% — «обратный удар» −5 HP +5 паники шум 0.8, попытка не считается; отпустил при <15% (`StarterGraceTension`) — без наказания. HUD: натяжение + зелёное окно.
- Отпускание E теперь уходит в `ARepairable::TryReleaseBy` (нужно стартеру); вся логика завершения вынесена в `FinishRepair`. Все параметры EditAnywhere.
- Выход из любой мини-игры — G (предмет не бросается); ранение рвёт мини-игру (CanContinueRepair).

### Чек-лист теста ОБОИХ модулей — TESTING.md, раздел «НОВОЕ — мини-игры» (2026-06-12).

## МОДУЛЬ «Диспетчер» — КОД ГОТОВ 2026-06-12

### Статус: собран, смоук чистый. НЕ проверен пользователем в PIE.

Из концепта (промт.docx §18 «Матерящийся диспетчер») — пока пользователь не может тестить, взят модуль без ассетов и без изменения геймплея:
- Реплики живут в `ARunState`: пулы в `DispatcherLines::` (cpp), `DispatcherSay(Pool, Param, bImportant)` → `MulticastDispatcherSay` → клиентский буфер `GetDispatcherLines()` (макс 3).
- События: приветствие через 6 с после старта (число поломок), починка объекта (с названием), все починено → «в ГАЗель», взрыв газа (имя виновника, из `ExplodeGas`), замыкание щитка (из `ShortCircuit`), ранение, санитарный инцидент, визит в туалет (35% шанс), победа/поражение.
- Анти-спам: неважные реплики глотаются 6 с после любой; важные (взрыв/замыкание/инцидент/финал) идут всегда.
- HUD: плашки сверху по центру, оранжевая кромка + «ДИСПЕТЧЕР:», живут 9 с, последние 2 с тают.
- Токен `{X}` в репликах = имя/название/число. Имя из PlayerState («Монтёр» как фолбэк).
- **+ «Панические крики»** (тот же §18): паникующий монтёр кричит в тот же эфир («Имя (паника): …») — первый крик через 4–10 с паники, дальше раз в 12–25 с, успокоился — сброс; каждый крик MakeNoise 0.6. `FDispatcherLine` получил поле Speaker, мультикаст — параметр спикера.

### Модели meshy.ai — ПОЛНЫЙ список и пайплайн

**Пайплайн:** Text-to-3D → экспорт **FBX** с текстурами → положить файл в `D:\unrealEngine\avariika\RawAssets\<ИмяПапки>\` (имя папки строго из таблицы) → сказать Claude «импортируй модели» → он гоняет `Scripts/import_models.py` (закрытый редактор!): импорт в `/Game/Avariika/Meshes/`, автоназначение на акторы по меткам / в CDO BP-предметов, автомасштаб для объектов уровня.

**СЕТТИНГ (важно для промптов):** первая карта — **условно больница** (старое лечебное/техническое здание), НЕ подвал-завод. Туалет = **обычный керамический унитаз** в больничном санузле, НЕ уличная кабинка-биотуалет. Остальные «индустриальные» пропсы (щиток/труба/генератор) ок как старое больничное хозяйство/подвал — если что-то выбьется из больничного тона, скажи, перепишу промпт.

Промпты ниже финальные — копировать как есть (хвост `single object, centered, no ground plane, no scene` уже вшит).

| Папка | Что | Промпт |
|---|---|---|
| `SM_Toilet` | Унитаз (больничный санузел) | `White ceramic toilet with cistern tank and lid, hospital restroom style, slightly worn grimy porcelain, scuffed, low-poly game prop, PBR textures, game-ready, single object, centered, no ground plane, no scene` |
| `SM_Breaker` | Электрощиток | `Industrial wall-mounted electrical breaker panel, open rusty metal door revealing fuses, wires and big switches, gray-green soviet industrial style, scorch marks, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene` |
| `SM_Tester` | Тестер (инструмент щитка) | `Handheld digital multimeter, yellow rubber protective case, small LCD screen, rotary dial, red and black probe wires wrapped around body, worn used look, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene` |
| `SM_GasPipe` | Газовая труба | `Vertical industrial gas pipe section with large red shut-off valve wheel, pressure gauge, yellow pipe paint peeling with rust, visible crack leaking, soviet factory style, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene` |
| `SM_Generator` | Генератор | `Old diesel power generator unit on metal frame, exposed engine, fuel tank on top, control box with gauges, orange-red chipped paint, oil stains, soviet industrial, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene` |
| `SM_Gazelle` | ГАЗель (база/выход) | `Russian GAZelle cargo van, white boxy delivery van 90s style, roof rack with ladder, mud splashes, orange emergency stripe and rotating beacon on roof, slightly rusty, low-poly game vehicle, PBR, game-ready, single object, centered, no ground plane, no scene` |
| `SM_WeldingMachine` | Сварочник (тяжёлый) | `Portable arc welding machine, heavy metal box with carrying handle, cable with electrode holder wrapped around, dials and clamps, dark blue chipped paint, rust and burn marks, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene` |
| `SM_FireExtinguisher` | Огнетушитель | `Red fire extinguisher with black hose and nozzle, pressure gauge, worn scratched paint, metal cylinder, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene` |
| `SM_FirstAidKit` | Аптечка | `First aid kit, small white plastic case with red cross, clasp latch, scuffed and dirty, slightly open lid, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene` |
| `SM_Cigarettes` | Сигареты | `Crumpled cigarette pack, soviet style plain white-red paper pack, one cigarette sticking out, with a small matchbox, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene` |
| `SM_Radio` | Рация | `Handheld walkie-talkie radio, black rubber body with orange accents, stubby antenna, speaker grille, belt clip, volume knob, worn scratched, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene` |
| `SM_Battery` | Батарея фонаря | `Big chunky lantern battery, blue metal casing with paper label, two spring terminals on top, slightly corroded, retro soviet style, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene` |
| `SM_Fuse` | Предохранитель | `Ceramic electrical fuse plug, white ceramic body with metal cap and thread, small handle, vintage soviet fuse, low-poly game prop, PBR, game-ready, single object, centered, no ground plane, no scene` |

Приоритет генерации: сперва модуль «Туалет + Щиток» — `SM_Toilet`, `SM_Breaker`, `SM_Tester`; потом крупняк (`SM_Gazelle`, `SM_Generator`, `SM_GasPipe`), потом мелочёвка.

⚠️ После импорта унитаза (вместо высокого куба-«кабинки») в туалет-мини-игре, скорее всего, надо будет подправить точку посадки/разворота (`AToilet`/телепорт игрока на актор) и масштаб заглушки в `setup_items.py` (сейчас стоит `(0.9,0.9,2.2)` — под кабинку). Делаю это после того, как принесёшь модель и глянем в PIE.

**Анимации (пользователь):** сидение на туалете, «тык щупами» у щитка, удар током (стан), волочение раненого, перекур. Когда будут файлы — обсудим скелет (UE5 Manny уже в проекте: `Content/Characters/Mannequins`) и подключим.

## МОДУЛЬ «Косяки оборудования» — КОД ГОТОВ 2026-06-12

### Статус: собран, смоук чистый. НЕ проверен пользователем в PIE.

Из концепта §18 «Косяки оборудования» — модуль без ассетов, без правки уровня (всё на рантайме):
- **Per-run «дешёвый комплект»**: `ARunState::bCheapGear` роллится в BeginPlay (~40% шанс), реплицируется (`HasCheapGear()`). В дешёвом забеге диспетчер после приветствия добавляет реплику из `DispatcherLines::CheapGearGreeting`.
- **Фонарь-дешёвка**: `UFlashlightComponent` получил `bCheapUnit` (реплицируется), `CheapGlitchChancePerSecond` (0.12), `SetCheapUnit()`. В `UpdateFlicker` при нормальном заряде дешёвый фонарь иногда коротко (0.08–0.22 с) гаснет. Обычный фонарь при норме горит ровно (старое мерцание под низкий заряд не тронуто). `ARunState::ApplyCheapGear` выдаёт `bCheapUnit=true` фонарю каждого монтёра один раз (через `CheapGearApplied` set), только если выпал дешёвый комплект.
- **Рация ловит чужой голос**: `ARunState::TickRadioInterference` (зовётся в Tick) ищет включённую рацию (`ItemEffect==Radio && IsToggledOn()`); пока эфир жив — раз в 12–25 с (дешёвый) / 25–50 с (обычный) вбрасывает важную реплику из `DispatcherLines::RadioGhost` со спикером «···помехи···», шумит `MakeNoise(0.9)` от рации и добавляет +8 паники всем в радиусе 900 см. Выключил рацию — `NextRadioGhostTime` сбрасывается.
- HUD не трогали: лента рисует `Speaker` обобщённо, «···помехи···» отрисовывается с оранжевой кромкой.
- Генератор «с 3–5 попытки» (§18) уже закрыт стартер-мини-игрой — отдельно не делал.

### Чек-лист теста — TESTING.md, раздел «НОВОЕ — Косяки оборудования» (2026-06-12).

## МОДУЛЬ «Репутация, деньги и долг конторы» (§19) — КОД ГОТОВ 2026-06-12

### Статус: собран, смоук чистый. НЕ проверен пользователем в PIE.

Бухгалтерия из «Акта» теперь переживает смены (раньше CrewTotal считался в HUD и никуда не переносился — было написано «вычтем из следующей смены», но переноса не было):
- **`UCompanyLedgerSubsystem`** (`UGameInstanceSubsystem`, новые `Public/Game/CompanyLedgerSubsystem.{h,cpp}`): хранит `CompanyBalance`, `ShiftNumber` (с 1), `ReputationPoints` [-10;10]. GameInstance переживает `ProcessServerTravel("?restart")`, поэтому касса/смена/репутация копятся между забегами. `CommitShift(net, bWon)`: net к балансу, репутация +1 за победу / −2 за провал / ещё −1 если ушли в минус.
- **Формула денег вынесена в статик** `ARunState::ComputePlayerBalance(stats)` — сервер (на финише) и HUD считают одинаково (раньше формула жила только в HUD).
- **ARunState**: в BeginPlay читает леджер → реплицируемые `ShiftNumber`, `CompanyBalanceStart`, `Reputation`. На `FinishRun` суммирует `ShiftNet` по бригаде и коммитит в леджер. Все 4 поля реплицируются для «Акта».
- **Связка с §18**: шанс дешёвого комплекта теперь зависит от репутации — `clamp(0.4 - 0.03*Reputation, 0.1, 0.7)`. Хорошая контора реже получает рыночный хлам, плохая — чаще.
- **HUD «Акт»**: номер в шапке `№ %03d`, строки «Итог смены», «Касса конторы: было → стало», «Репутация: <статус>» (`ARunState::ReputationTitle`). Высота панели увеличена. Цвета в рамках правил (оранжевый акцент, без фиолетового).

### Чек-лист теста — TESTING.md, раздел «НОВОЕ — Репутация и касса конторы» (2026-06-12).

## МОДУЛЬ «Не тот инструмент» (§18) — КОД ГОТОВ 2026-06-12

### Статус: собран, смоук чистый. НЕ проверен пользователем в PIE.

Колхозный ремонт без нужного инструмента (концепт §18 «не тот инструмент») — поверх готовой `ARepairable`, без ассетов/правки уровня:
- **`ARepairable`**: новый флаг `bBotching` (реплицируется) + `bAllowBotch` (default true) и параметры колхоза (`BotchDurationMultiplier`=2.0, `BotchMishapChancePerSecond`=0.25, `BotchMishapProgressLoss`=0.15, `BotchMishapDamage`=6, `BotchMishapPanic`=6). `CanBotchBy()` = сломан + свободен + не ранен + `RequiredTool!=None` + в руках НЕ тот инструмент.
- `BeginRepairBy`: если правильного инструмента нет, но можно колхозить — стартует колхоз (held-E, БЕЗ мини-игры и без локов движения). `CanContinueRepair` колхоз-aware.
- Тик колхоза: прогресс ползёт медленно (×`BotchDurationMultiplier`), шанс косяка/сек → −прогресс, урон, паника, громкий шум; колхоз шумит сильнее (`MakeNoise(1.2)` каждые 0.8 с). Работает на любом объекте с инструментом (щиток без тестера, генератор без сварочника).
- `FinishRepair`: колхозный финиш = громкий шум + `ARunState::AddBotchedRepair` (диспетчер из пула `BotchRepair` + `++BotchedRepairs`). Объект засчитывается как починенный (обычный `OnRepairFinished`).
- **AvaryoCharacter**: маршрутизация E (`TryHitBy`/`TryReleaseBy`) теперь только для НЕ-колхозных мини-игр (`IsMinigameRepair() && !IsBotching()`); колхоз завершается отпусканием E как обычный held-ремонт.
- **HUD**: подсказка «[E] Чинить «X» на коленке (без инструмента — рискованно)» вместо «нужен инструмент», когда `CanBotchBy`; бар ремонта пишет «Колхоз: … (без инструмента)»; в «Акте» колонка «колхоз: N», звание «Народный умелец», штраф −800 ₽/колхоз в общей формуле `ComputePlayerBalance` (синергия с §19 — много колхоза = меньше касса/репутация).

### Чек-лист теста — TESTING.md, раздел «НОВОЕ — Не тот инструмент» (2026-06-12).

## МОДУЛЬ «Предметы-ловушки» (§18) — КОД ГОТОВ 2026-06-12

### Статус: собран, setup-скрипт прогнан, уровень сохранён, смоук чистый. НЕ проверен пользователем в PIE.

Ставимая растяжка-шумелка (концепт §18 «предметы-ловушки», «может сработать против команды»):
- **Новый актор `ATrap`** (`World/ATrap.{h,cpp}`): меш-куб + триггер-сфера + индикатор-лампа. После `ArmDelay` (2 с) взводится (`bArmed` реплицируется → лампа мигает оранжевым). Сервер перебором ищет любого `AAvaryoCharacter` в `TriggerRadius` (250 см) — при попадании срабатывает: `MakeNoise(1.5)`, +25 паники всем в радиусе ×1.5, `MulticastFlash` (белая вспышка + `UExplosionCameraShake`), реплика диспетчера, самоуничтожение через 0.3 с. Поставивший защищён `PlacerGraceTime` (3 с) после взвода — успевает отойти; дальше подрывает и своих.
- **Новый эффект предмета** `EItemEffect::DeployTrap`: в `ApplyItemEffect`/`CanApplyEffect` спавнит `ATrap` у ног чуть впереди (instigator = ставящий) и тратит заряд. Мгновенный (UseCastTime 0).
- **Предмет «Растяжка»** (`TrapKit`, лёгкий, `DeployTrap`, Charges=2) — на платформе спавна `(120,120,342)`, скрипт `Scripts/setup_traps.py` (идемпотентный, уровень сохранён).
- **HUD**: подсказка «[ЛКМ] Поставить растяжку (взведётся через пару секунд!)»; диспетчер язвит из пула `TrapTriggered` (`ARunState::NotifyTrapTriggered`).
- ⚠️ Headless-скрипты надо запускать с **абсолютным** путём к .py (`-script="D:\...\Scripts\x.py"`) — относительный резолвится от папки движка и не находится.

### Чек-лист теста — TESTING.md, раздел «НОВОЕ — Предметы-ловушки» (2026-06-12).

## МОДУЛЬ «Запах / Амбре» (§16-17) — КОД ГОТОВ 2026-06-12

### Статус: собран, смоук чистый. НЕ проверен пользователем в PIE.

Шкала запаха на `VitalsComponent` (концепт §16 «запах», §17 статус «воняет»), без ассетов и правки уровня:
- **`Smell` 0..100** (реплицируется) + параметры: `SmellThreshold`=50, `SmellDecayPerSecond`=1.5, `SmellSmokingPerSecond`=5, `SmellPanicPerSecond`=1, `SmellIncidentJump`=70, `SmellTeammatePanicPerSecond`=2, `SmellRadius`=350. API `AddSmell()`, `GetSmell()`, `IsSmelly()`.
- **Растёт от**: курева и пота-паники (в тике Vitals), санитарного инцидента (скачок +70, испачканный не выветривается сам), газового облака (`ARepairable` тик — всем в `GasRadius`), порошка огнетушителя (`AvaryoCharacter::TickSpray` — распыляющему). Спадает сам.
- **Эффект**: пока `IsSmelly()`, вонючий монтёр добавляет панику тиммейтам в радиусе (перебор в тике Vitals). «Запах создаёт кооп-ситуации» без монстра и без PP-материалов.
- **HUD**: новая витальная полоска «Амбре» (оливковая) под «Туалет»; статус-чип «Воняет».
- **«Акт»**: `FPlayerRunStats.SmellSeconds` (копится в `ARunState::Tick`, пока воняет), колонка «вонял: N с», звание «Амбре смены»; при появлении амбре — неважная реплика диспетчера из пула `SmellJab`.
- Денежного штрафа за запах нет (это не «косяк», а ситуация) — синергия только с паникой/диспетчером.

### Чек-лист теста — TESTING.md, раздел «НОВОЕ — Запах / Амбре» (2026-06-12).

## МОДУЛЬ «Биологический снаряд» (§15) — КОД ГОТОВ 2026-06-12

### Статус: собран, смоук чистый. НЕ проверен пользователем в PIE.

Метаемый «подозрительный комок» после санитарного инцидента (концепт §15), поверх запаха+предметов, без правки уровня:
- **`ABioPickup`** (`Items/ABioPickup`, подкласс `APickupItem`): лёгкий предмет «Подозрительный комок», `ItemEffect=ThrowBio`, `Charges=1`. Меш-сфера и параметры заданы в конструкторе (CDO) → спавненный в рантайме виден/подбираем у всех клиентов без репликации меша. `RunState` роняет его у ног в момент инцидента (там же, где `++Incidents`).
- **Поднял голыми руками → `AddSmell(40)`** (концепт: «без перчаток — риск испачкаться»). Хук в `AAvaryoCharacter::PickupItem` по `ItemEffect==ThrowBio`.
- **Новый эффект `EItemEffect::ThrowBio`**: ЛКМ метает `ABioProjectile` из камеры (через `CanApplyEffect`/`ApplyItemEffect`, заряд тратится). Передать комок тиммейту по ПКМ тоже можно — отдельный угар.
- **`ABioProjectile`** (`World/ABioProjectile`): физический снаряд (меш в CDO). Сервер в тике ловит прямое попадание в монтёра (радиус `HitRadius`) или приземление (низкая скорость); «шлёп» = `SplatSmell`/`SplatPanic` прямой жертве, в полсилы — по радиусу `SplatRadius`, `MakeNoise` (отвлекает будущего монстра «на запах»), реплика диспетчера; затем `LingerTime` (5 с) «амбре-зона» (запах + периодический шум), потом самоуничтожение.
- **Диспетчер**: пулы `BioHit` (попал в своего, с именем) и `BioMiss` (мимо). HUD: подсказка «[ЛКМ] Метнуть комок (осторожно — зацепишь своих!)».

### Чек-лист теста — TESTING.md, раздел «НОВОЕ — Биологический снаряд» (2026-06-12).

## МОДУЛЬ «Переносной прожектор» (§18) — КОД ГОТОВ 2026-06-12

### Статус: собран, setup-скрипт прогнан, уровень сохранён, смоук чистый. НЕ проверен пользователем в PIE.

Ставимый прожектор — полезная утилита с разменом (концепт §18 «прожектор»), завязан на ядро свет↔паника:
- **`AFloodlight`** (`World/AFloodlight.{h,cpp}`): куб-тренога + яркий тёплый `PointLight` (radius 1400). Сервер в тике (0.25 с): всем монтёрам в `CalmRadius` (700 см) снимает панику `CalmPerSecond` (4/с) — «свет успокаивает»; но раз в `NoiseInterval` (3 с) гудит `MakeNoise(0.5)` — выдаёт позицию (задел под монстра). Без батареи, живёт весь забег.
- **Эффект `EItemEffect::DeployLight`**: ЛКМ ставит прожектор у ног (через `CanApplyEffect`/`ApplyItemEffect`, заряд тратится) — тот же паттерн, что у растяжки.
- **Предмет «Прожектор»** (`LightKit`, лёгкий, `DeployLight`, Charges=1) — на платформе спавна `(120,180,342)`, добавлен в `Scripts/setup_traps.py` (теперь скрипт ставит и растяжку, и прожектор; прогнан, уровень сохранён).
- **HUD**: подсказка «[ЛКМ] Поставить прожектор (светит и успокаивает, но гудит)».

### Чек-лист теста — TESTING.md, раздел «НОВОЕ — Переносной прожектор» (2026-06-12).

## МОДУЛЬ «Скользкая пена» (§18 косяки/хаос) — КОД ГОТОВ 2026-06-13

### Статус: собран, автотесты зелёные (5/5), смоук чистый. НЕ проверен пользователем в PIE.

Огнетушитель теперь не только тушит/толкает, но и **заливает пол скользкой пеной** — кооп-комедия «кто залил, на том и проедутся»:
- **Новый актор `AFoamPatch`** (`World/AFoamPatch.{h,cpp}`): плоский диск-лужа (заглушка из Cylinder), `bReplicates`, `SlipRadius`=150 см, `Lifetime`=25 с (потом сам `Destroy`). Без коллизии — не мешает ходьбе.
- **Спавн**: в `AAvaryoCharacter::TickSpray` раз в 0.6 с трассой вниз находим пол перед монтёром и роняем `AFoamPatch` (instigator = распыляющий). За баллон (~8 с) ≈ 13 луж.
- **Скольжение**: `AAvaryoCharacter::UpdateFoamSlip()` (сервер, в Tick) перебором `AFoamPatch` проверяет, стоит ли монтёр в луже (радиус + проверка по Z, чтобы не цеплять другой этаж). Состояние `bSlipping` **реплицируется** (`OnRep_Slipping`) → и сервер, и клиент зовут `ApplySlipFriction`: `GroundFriction` 8→0.4, `BrakingDecelerationWalking` 2048→120 (почти лёд, тормозить нечем). Дефолты сохраняются один раз и возвращаются при сходе с пены. Клиент применяет то же трение, чтобы скольжение совпало с серверным.
- Синергия: распыляющий и так пропахивает химией (`AddSmell`), пена усиливает «косячный» хаос. Без новых ассетов и без правки уровня (нужен только огнетушитель в руках — уже есть).

### Чек-лист теста — TESTING.md, раздел «НОВОЕ — Скользкая пена» (2026-06-13).

## АВТОТЕСТЫ 2026-06-12 (headless-верификация логики)

`Source/Avaryo/Private/Tests/AvaryoTests.cpp` — 5 C++ Automation-тестов (под `WITH_DEV_AUTOMATION_TESTS`):
`Avariika.PlayerBalance` (формула денег), `Avariika.ReputationTitle`, `Avariika.CompanyLedger` (накопление баланса/репутации/смены, зажимы), `Avariika.Vitals` (зажимы запаха/паники, DebugSetVital), `Avariika.Deployables` (живой тест-мир: безопасный спавн/тик персонажа+прожектора+растяжки+объекта; логика колхоза CanBotchBy/BeginRepairBy/IsBotching и запрет при bAllowBotch=false). **Все 5 — Success, EXIT CODE: 0.**
Прогон headless: `UnrealEditor-Cmd <uproj> -ExecCmds="Automation RunTests Avariika; Quit" -unattended -nullrhi -nosound -abslog=<...>`.
Нюансы 5.7: маска контекста — `EAutomationTestFlags_ApplicationContextMask` (с подчёркиванием); `UCompanyLedgerSubsystem` (ClassWithin=GameInstance) создавать с `NewObject<UGameInstance>` овнером; **тики акторов в ручном `World->Tick` хедлесс-мира не диспатчатся надёжно** — поведение Tick прожектора/ловушки (гашение/срабатывание паники) проверяется только в PIE, в автотесте оставлен лишь «не падает».
Также фикс мультиплеера: `ABioProjectile` теперь симулирует физику только на сервере (в BeginPlay по HasAuthority), клиенты — по реплицированному движению (без дёрганья).

## ДЕВ-КОНСОЛЬ 2026-06-12 (ускорение PIE-теста)

Exec-команды на персонаже (`AvaryoCharacter`), чтобы тестировать модули мгновенно (без ожидания шкал). Список и применение — TESTING.md, раздел «Дев-команды». Команды: `AvVital <health|panic|stamina|bladder|smell> <0..100>`, `AvIncident`, `AvGiveBio`, `AvCheapGear`, `AvFinish <win|lose>` (показать «Акт»). Маршрутизируются на сервер (работают и у клиента, и у хоста). UFUNCTION(Exec) нельзя оборачивать в `#if` — поэтому читы скомпилированы всегда (проект не шипится). Опираются на `VitalsComponent::DebugSetVital`, `ARunState::DebugForceCheapGear`, `ARunState::DebugFinishRun`.

## РЕВЬЮ-ПРОХОД 2026-06-12 (фиксы по своему коду сессии)

- `ATrap`/`AFloodlight` спавнились без меша (были физически невидимы — виден только свет). Добавлены кубо-/цилиндро-заглушки в конструкторах.
- Строка статистики в «Акте» разрослась до 10 колонок и вылезала за панель — разбита на две строки (работа / беды+деньги), высота панели = 3 строки на монтёра.
- Прочее (репликация, спавн только на сервере, null-инстигаторы в MakeNoise, разведение колхоз/мини-игра) перечитано — проблем не найдено.

## РАБОЧИЙ ПАЙПЛАЙН МОДЕЛЕЙ ЧЕРЕЗ MESHY API (2026-06-12) — ИСПОЛЬЗОВАТЬ ЭТОТ

DCC-мост meshy (`Plugins/meshy`) **крашит** импорт на текстурах (ассерт `Layers.Num()`), плагин в репо НЕ коммичен, в `.uproject` локально — НЕ использовать «Send to Unreal».

**Рабочий путь (так импортированы унитаз и щиток):**
1. Генерация: `Scripts/meshy_generate.ps1 -Folder SM_X -Prompt "..." -Model meshy-5 -Polycount 10000` (ключ в `.meshy_key`). Качает FBX + **только base color** (нормаль meshy роняет UE — не качаем). `remove_lighting` только для meshy-6.
2. Импорт: открыть редактор → Claudius `level.load_level` Lvl_FirstPerson → `editor.run_python_script` `Scripts/meshy_import.py`. Он: полный импорт FBX (геометрия; mesh-only под Interchange ломает в 0x0x0), импорт base-color PNG, сборка чистого материала `M_SM_X` (base color → BaseColor, без нормали), перекрытие материала меша, назначение на актор в ОТКРЫТОМ уровне (без `load_level` в python!), масштаб под max-dim, `save_dirty_packages(True,True)` (OFPA).
3. Маппинг папка→актор+габарит — в `MAPPING` внутри `meshy_import.py` (сейчас SM_Breaker→Repairable_Breaker 160). Дописывать туда новые модели.

Орфанные авто-ассеты от FBX-импорта (`Image_*`, `normal`, `Material_001`, `texture_0`) НЕ используются (меш на `M_SM_X`) и не коммитятся.

## МОДЕЛИ — статус (2026-06-12)

**Импортирован первый ассет: унитаз (`SM_Toilet`)** — 10k тр, ~75 см, назначен на актор `Toilet` (scale 1.0), в `main`. Сейчас стоит с ПРОСТЫМ материалом (BasicShapeMaterial), без текстур — потому что meshy-нормаль импортируется битой и роняет рендер («Texture not valid! NormalMap»). **Следующий шаг по унитазу:** собрать чистый материал из base color (`Image_0`), без проблемной нормали, и назначить на меш.

**Тестер (`SM_Tester`) — импортирован 2026-06-13** (модель пользователя, 452k тр). Безопасный legacy-импорт (без Interchange) → 0 орфанных текстур/нормалей, чистый материал `M_SM_Tester` из base color, Nanite включён. Назначен на актор `Tester` (PickupItem) в `Lvl_FirstPerson`, scale 0.733 (→ ~22 см).

**Щиток (`SM_Breaker`) — импортирован 2026-06-13**, новая модель пользователя (775k тр). Чистый материал `M_SM_Breaker` (base color `SM_Breaker_BaseColor`, без нормали), **Nanite включён** (775k → GPU-децимация, дешёвый). Назначен на `Repairable_Breaker` в `Lvl_FirstPerson`, в `main`. Орфанные авто-ассеты от FBX (normal/Image_2/Image_3/Material_001/texture_0) удалены — meshy-нормаль (29 МБ) роняла build текстур (assert `Layers.Num()==NumLayers`) при открытии редактора. **Урок:** FBX-импорт с текстурами всё равно плодит битую нормаль; удалять её сразу после импорта, до открытия редактора.

### Грабли импорта (важно для всех будущих моделей)
- **Headless-импорт FBX падает** (UE5.7 Interchange дёргает Content Browser → Slate assert в commandlet). Импортировать только в **открытом редакторе** через Claudius `editor.run_python_script` (Slate есть).
- **`les.load_level()` внутри открытого редактора** плодит отдельный мир — правки туда не сохраняются. Менять акторы надо в УЖЕ открытом уровне (см. `Scripts/assign_toilet.py`), без load_level.
- **OFPA/World Partition**: акторы — внешние пакеты; `save_current_level()` их НЕ пишет. Нужен `EditorLoadingAndSavingUtils.save_dirty_packages(True, True)` (вшито в `import_models.py`).
- **meshy normal map**: приходит битой/sRGB; рендер крашится. `import_models.py` пытается чинить (TC_NORMALMAP), но надёжнее строить материал из base color без нормали. Тяжёлые меши (>20k тр) импортёр сам ставит в Nanite.
- Тяжёлые исходники (`RawAssets/*.fbx/*.png`) в git НЕ коммитим (gitignore); коммитим только `/Game/Avariika/Meshes/*` + изменения уровня.

## КАРТА «Больница» — ОТМЕНЕНА пользователем 2026-06-13 (greybox не подошёл)

Greybox `L_Hospital` (каркас+комнаты+квест-акторы) построили по `TZ_Hospital_Map_UE5.md`, но пользователь забраковал: «прямоугольная коробка без изюминки, нет нормального входа в вестибюль, на больницу не похоже». **Карту удалили** (`/Game/Hospital` снесён, `delete_hospital.py`). Старые сборочные скрипты `build_hospital_greybox/rooms/quest.py` удалены из репо. Пользователь напишет **новый промт/ТЗ для больницы** — ждём его, делать заново будем по нему (нужен интересный вход/вестибюль, архитектурная подача, не голая коробка).

### Что осталось полезного из этого захода (НЕ терять)
- **Сборка уровня EDITOR-FREE:** `spawn_actor_from_object(mesh)` **падает headless** (EXCEPTION_ACCESS_VIOLATION в EditorFramework); замена — `spawn_actor_from_class(StaticMeshActor) → static_mesh_component.set_static_mesh(CUBE)`, работает в commandlet (`-run=pythonscript`). `load_level`+spawn(from_class)+`save_current_level` headless проходят на обычном (не-WP) уровне. Дневной свет, который НЕ даёт чёрный экран: Movable DirectionalLight **100000 lux** + `atmosphere_sun_light=True` + realtime SkyLight + SkyAtmosphere (movable-свет не печётся → PIE сразу светлый).
- **TZ_Hospital_Map_UE5.md** оставлен как справочник по комнатам/пропсам/маршруту (§3-5) — переиспользуем при новом дизайне, если подойдёт.

### Безопасный импорт meshy-FBX (новый канон, без битой нормали)
`Scripts/import_tester.py`: отключаем Interchange-FBX (`Interchange.FeatureFlags.Import.FBX 0`) → работает legacy-импортёр, который уважает `import_materials=False/import_textures=False` → **меш без авто-текстур и без проблемной meshy-нормали** (раньше mesh-only ломался в 0x0x0 именно из-за Interchange). Материал собираем из base color сами. Тестер так импортнулся: **0 орфанов** (против кучи Image_*/normal/Material_001 при полном импорте). Этот путь надо вшить в `meshy_import.py` для будущих моделей.

## Очередь модулей (дальше)

1. **Фиксы по фидбеку пользователя** из PIE-теста модулей (всё перечисленное ниже + прожектор).
2. **Модели и вид**: импорт meshy-моделей на все заглушки (когда пользователь сгенерит), материалы.
3. **Анимации** (пользователь даёт файлы — подключаем AnimBP headless).
4. Остатки асет-фри (всё мельче): §18 ещё ловушки (датчик движения — без монстра толку мало, лужа пены — скользкая физика, подпорка под дверь — дверей нет); грязь/следы (нужны декали).
5. **Монстр-слухач** — ТОЛЬКО по отмашке.

## Техсправка

- Движок: UE 5.7, проект `D:\unrealEngine\avariika\avariika.uproject`, модуль C++ `Avaryo` (+ зависимость EngineCameras).
- Структура: `Source/Avaryo/{AvaryoCharacter, Components/{Vitals,UFlashlight}, Items/APickupItem, World/{ARepairable,AExitZone,AToilet}, Game/ARunState, UI/{AvaryoHUD,AvaryoCameraShakes}}`.
- Уровень `Content/FirstPerson/Lvl_FirstPerson`: PlayerStart на платформе (0,0,~300), пол z=-15 (±2000), объекты по меткам: Repairable_Breaker (1200,-1320), Repairable_GasPipe (1700,150), Repairable_Generator (-1200,1250), ExitZone_Gazelle (-250,0), Toilet (-1650,-1550), Radio/Tester/лут.
- Скрипты: place_run_objects, setup_items, setup_minigames, setup_traps (растяжка), night_atmosphere, scatter_loot, import_models (модели из RawAssets/), inspect_level (дамп акторов в Saved/level_actors.txt). Результаты скриптов пишутся в Saved/*.txt. **Запускать с абсолютным путём:** `-script="D:\unrealEngine\avariika\Scripts\<имя>.py"`.
- Полные механики и управление — README.md; тест-чек-листы — TESTING.md.
