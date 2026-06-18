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

## СЕССИЯ 2026-06-18 (ДЕНЬ) — git-прибор: закоммитил/запушил накопленное за прошлые сессии

Юзер вернулся (удалёнка), попросил отчёт за ночь + продолжить. Редактор был **ЗАКРЫТ** (плагин не отвечал; у юзера машина виснет с открытым → при удалёнке логично работать с закрытым). Выбор юзера: сперва **закоммитить+запушить**, потом «прибрать аккуратно». Дерево было ГРЯЗНОЕ (**1495 файлов**, копилось много сессий). Разобрал по категориям и прибрал в чистые коммиты — всё в `origin/main`:

- `f384aa7` **Исходники** (14 ф., +663): дверь-фикс (вращаем полотно `DoorMesh`+`Zone`, не корень — поворот размещения больше не затирается); кооп-репликация ×3 (`bFloodElectrified`/`CurrentGasRadius`/`CurrentFloodRadius` + `AToolCase::bLoaded` через OnRep); HUD-полировка (рамки шкал + рамочный хотбар); live-wire/потоп; модульные голова/кисти; газ-сварка; пути карты. ⚠️ В коде сейчас: реакции-монтажи в `MulticastPlayMontage` **ЗАКОММЕНТИРОВАНЫ** (доводится локомоция — вернуть = раскомментить 1 строку); паника off (`Av.NoPanic=1`).
- `f452cb4` Удаление 2 пиратских паков (`HospitalCombo` 826 + `Construction_VOL2` 183; нет ссылок в коде; `Content/Hospital` = ДРУГАЯ папка, цела).
- `0adb703` Гигиена: `.gitignore` += поздно-импортированные паки (AtmosphericHouse/ResidentialHouses/Warehouse/фабрики/CitizenNPC/вода/небо — тот же паттерн «локально, re-copy из RawAssets»), бэкапы, рабочие png/jpg; перестал трекать пер-юзер `DefaultEditorPerProjectUserSettings.ini`; снёс устаревшие план-картинки.
- `79ca33d` Завершил **переезд карты** `FirstPerson` → `Avariika/Maps` (старые OFPA-акторы удалены, новые добавлены — было закоммичено наполовину, свежий клон терял акторов карты).
- `bf744da` Контент Avariika: 74 файла `Anim` (7 монтажей — на них ссылается код!), модульный `BP_AvaryoCharacter`, `L_Dom`/`L_Hub`, песочница `blueprinsTest`+`Test`, `DefaultScalability.ini`.
- (этот коммит) Доки (`HOUSE_PLAN`/`HOUSE_STRUCTURE`/`PROJECT_STRUCTURE`/`PACKS_REFERENCE`/`AVARIIKA_MEHANIKI`/`PROJECT_BRIEF_FOR_WEB`) + dev-скрипты `Scripts/` + `.mcp.json` + `capture-window.ps1` + WORKLOG.

**Дерево 1495 → почти чисто.** НЕ трогал (на решение юзера): `Config/DefaultEngine.ini` (внутри **EOS Client Secret**), `Config/DefaultEditor.ini` (мелочь-лэйаут), `test.txt` (твои заметки по поиску ассетов), запутанный `ResourcePack` (154 удал.+30 измен., частично на диске — нужен вердикт «дроп или оставить»). **CitizenNPC (1.1ГБ)** гитигнорен как остальные паки, хотя память отмечала «трекать» — для бэкапа на GitHub нужен Git LFS / выборочный трекинг (спросить).

**Дальше (выбор юзера):** стройка дома `L_Dom` Фаза A (лестница/потолки/двери/подвал — нужен ОТКРЫТЫЙ редактор) ИЛИ отложенные механики C++ (монтажи M_Fix-луп/M_Knocked + 4-й баг репликации `ATrap` — без редактора).

**ИТОГ ПРИБОРА (дерево 1495 → 0, чисто; 11 коммитов запушено):**
- ⚠️ **ВАЖНЫЙ УРОК:** сперва принял пак `HospitalCombo` за пиратку и закоммитил удаление — юзер поправил: **по ТЗ 3 карты — Дом / Завод / Больница**, паки этих карт НЕ мусор. Восстановил `HospitalCombo` (826) + `Construction_VOL2` (183) из истории `bd76ef7` на диск, gitignore-локально (записано в память `three-maps-plan`). **Перед удалением любого пака — сверяться со списком 3 карт.**
- Все импортированные паки теперь в `.gitignore` (стратегия «локально, re-copy из RawAssets»; у Hospital/Construction источник = ТОЛЬКО git-история, в RawAssets их нет). `CitizenNPC` тоже gitignore — память хотела «трекать», для бэкапа на GitHub нужен Git LFS (спросить юзера).
- Удалил 5 мёртвых `.md` (ANIMBP_PLAN/SETUP про удалённого Оператора, MECHANICS_PLAN, TEST_CHECKLIST, STATUS_ROADMAP). Все доки про 3 карты/ассеты/концепт ОСТАВЛЕНЫ.
- `Config/DefaultEngine.ini` закоммичен с EOS-кредами (репо приватный; **ротировать ClientSecret если откроют репо**) + пути новой карты + PoolSize=2500.
- Завод: на диске `IndustrialFactory` (только диск, без бэкапа) + `Warehouse` (диск+RawAssets); `AbandonedFactory` был 3-м лишним — его нет.

**МЕХАНИКИ (день, после прибора; юзер открывал/закрывал редактор по запросу). Дизайн-vs-код разбор подагентом → 3 новые механики, все собраны, смоук чист, запушены. PIE-проверка за юзером:**
- **Газ «дышит» + противогаз** (`82e9ba9`, через Live Coding): без `ToolTag=GasMask` в облаке — запах + паника (~10/с) + токсичный урон (~5 HP/с); противогаз фильтрует воздух, полностью спасает. Предмет — скрипт `Scripts/setup_gas_mask.py` (`7607b6f`), размещает ЮЗЕР.
- **«Промок/Wet»** (`89d62ed`, ребилд): стоял в потопе → скорость ×0.82 + лёгкая паника, держится `WetDuration`=6с после выхода; HUD «Промок»; сапоги от мокроты НЕ спасают (они от тока). Зеркало `IncidentSlow` (реплик. `WetRemaining` в VitalsComponent).
- **Генератор коротит при живом щитке** (`89d62ed`): флаг `bGeneratorShortsIfPanelLive` (ВЫКЛ по умолч. → ноль влияния) + запитанный `APowerSwitch` в `GeneratorPanelScanRadius`=15м → КЗ вместо запуска («наказание в момент успеха»). Включать на генераторе, рядом ставить рубильник.
- **PIE-тесты:** газ — встать в облако у `Repairable_GasPipe`; промок — пройти разлив у `Repairable_WaterPipe`; генератор — выставить флаг + живой щиток рядом, завести.
- ⚠️ **ГОЧА Live Coding:** тянет тела функций + новые НЕ-UFUNCTION методы, но НЕ новые `UPROPERTY`/члены классов (меняется layout/`.generated`) → новые поля = полный ребилд при ЗАКРЫТОМ редакторе. Газ-маска прошла Live Coding (plain-метод); Wet/генератор потребовали ребилд.
- **Разделение труда (юзер, 2026-06-18):** стройку уровней (геометрия/расстановка) делает ТОЛЬКО юзер; Claude — механики/звук/анимации/эффекты/HUD/фиксы. Размещение игровых объектов для механик — описывать юзеру или давать скрипт, не лезть в его карту.
- **Дальше по gap-карте (C++, editor-closed):** оглушение у генератора без наушников, урон глазам без свармаски, пробки выбивает при невыключенной нагрузке, статус «Горит», запрос диспетчеру/подсказка. Звук + Niagara-эффекты — нужен ОТКРЫТЫЙ редактор.

## СЕССИЯ 2026-06-18 (НОЧЬ) — АВТОНОМ: баги механик + HUD-полировка + запекание (юзер спит)

Юзер: «занимайся автономно — механики/звуки/эффекты; баги, задумки, красивый HUD/инвентарь». Не трогал ручную стройку L_Dom (геометрию), монстра не делал. Редактор был открыт → закрыл для чистого ребилда, открыл обратно.

**АУДИТ C++-механик (подагент, read-only) → 4 бага репликации в кооп (на клиентах HUD/визуал застывали). Исправил 3 (4-й косметика, пропущен):**
- `ARepairable::bFloodElectrified` → `UPROPERTY(Replicated)` + DOREPLIFETIME (клиентский HUD «зона под током»).
- `ARepairable::CurrentGasRadius` / `CurrentFloodRadius` → `UPROPERTY(Replicated)` + DOREPLIFETIME (рост облака/разлива для HUD и газодетектора у клиента).
- `AToolCase::bLoaded` → `UPROPERTY(ReplicatedUsing=OnRep_Loaded)` + `GetLifetimeReplicatedProps` + `OnRep_Loaded()→RefreshLabel()` (у клиента подпись «ЯЩИК СОБРАН»). Добавлен `#include "Net/UnrealNetwork.h"`.
- ПРОПУЩЕН (косметика): `ATrap::bTriggered` — мигание индикатора 0.3с, маскируется MulticastFlash.
- ⚠️ Все 4 — клиентские HUD/визуал; геймплей серверно-авторитетен (не было десинка). В одиночном PIE юзер их не видит — ценность для будущего кооп.

**БАГ ДВЕРИ (ADoor) — ИСПРАВЛЕН (важно):** петля `Hinge` была КОРНЕМ, и `BeginPlay`/`Tick` крутили корень → затирался поворот размещения актора (повёрнутая дверь в игре «доворачивалась»). Фикс: вращаем `DoorMesh`+`Zone` (полотно), корень/размещение не трогаем. Для полотен с пивотом-на-петле (`SM_InsideDoor`/`SM_EntranceDoor`, как мы ставим) распах правильный. ⚠️ дефолтный куб-плейсхолдер теперь крутился бы в центре — но мы его не используем.

**HUD-ПОЛИРОВКА (`AvaryoHUD.cpp`):** HUD и так добротный (стиль-неймспейс, оранж-акцент, виньетка паники, рация, задачи, миникарта, акт работ). Две правки «красивее»: (1) шкалы слева — тёмный трек + тонкая рамка `AccentDim` (читаемо на тёмной сцене); (2) инвентарь-хотбар — активная ячейка теперь **оранжевой РАМКОЙ** + фон каждой ячейки `CellBG` (раньше активная = сплошная оранжевая заливка, грубо).

**СБОРКА/ПРОВЕРКА:** закрыл редактор (taskkill PID 10468, L_Dom сохранён) → **полный ребилд `Build.bat avariikaEditor` → Succeeded, exit 0** (запеклись: дверь + репликация×4 + HUD + ранее голова/кисти+монтажи; только варнинги-депрекейшн). **Смоук** `-game -nullrhi`: загрузил `L_MainMenu`, «Engine is initialized», мир поднялся — **краша нет** (единств. Error = handled-ensure про депрекейтнутый CVar `r.TranslucencyLightingVolumeDim`, движковый, не наш). Редактор открыт обратно.

**НЕ ЗАКОММИЧЕНО (намеренно):** репо полон некоммиченных правок прошлых сессий (`ADoor`/`APowerSwitch` вообще untracked; M в AvaryoCharacter/VitalsComponent/UFlashlight/ACallBoard/AvariikaOnlineSubsystem — не мои этой сессией). Чтобы не смешать своё с чужой незакоммиченной «грязью» — оставил на ревью юзера. **Мои файлы этой сессии:** `World/ADoor.cpp`, `World/ARepairable.{h,cpp}`, `World/AToolCase.{h,cpp}`, `UI/AvaryoHUD.cpp`. Всё запечено в DLL и на диске — работает без коммита.

**ЗАДУМКИ НА БУДУЩЕЕ (HUD/инвентарь/механики, для юзера — обсудить):**
- HUD: перейти с Canvas-`AHUD` на **UMG-виджеты** (WBP_HUD) — иконки предметов в слотах вместо текста, анимированные шкалы, нормальный шрифт (текущий — движковый LargeFont). Иконки слотов = рендер-таргеты предметов или 2D-спрайты.
- Инвентарь: иконка+счётчик зарядов в ячейке; подсветка «активно/нельзя» цветом рамки; колесо выбора (radial) на удержании.
- Газодетектор/HUD-индикаторы аварий: компас-стрелка к ближайшей аварии, пульс по близости газа.
- Эффекты (Niagara, отдельная фаза, нужен открытый редактор — headless крашит на Niagara CDO): брызги/струя у `Repairable_WaterPipe`, газ-дымка у газовой, искры/дуга у щитка, взрыв газа (Next Gen Destruction рушит стену — gated по EffectsQuality).
- Звуки: привязать к механикам (дверь уже с OpenSound=FlashClick-плейсхолдер; щиток/вентиль/струя/рация — заменить плейсхолдеры на курированные WAV из звуковой базы).

## СЕССИЯ 2026-06-17 (ВЕЧЕР, ч.2) — СТРОЙКА ДОМА L_Dom (модульно по чертежам, Фаза A начата)

**План утверждён** (`~/.claude/plans/peppy-crunching-dawn.md`): 1 этаж особняка модульно ПО ЧЕРТЕЖАМ (не House01), сразу с мебелью. Решения юзера: модульно по чертежам / только 1 этаж / сразу с мебелью.

**1 этаж — 7 зон (one_itaj.png + HOUSE_PLAN.md):** Гараж 28м²(💧⚡,вход в подвал) слева+ворота · Кухня 14м²(💧🔥) верх-центр · Столовая 20м² верх-право · Гостиная 28м²(⚡,камин) право · Санузел 6м²(💧) центр-лево · Лестница 12м²(дверь в подвал под ней) центр · Кладовая 7м² низ-центр · Главный вход снизу-центр.

**Сделано (Фаза A старт):**
- L_Dom очищен от House01: удалено 599 акторов (House01_C + 597 разобранных мешей House01 + 2 двери), оставлено 6 (Ground-плита 50×50м, DirectionalLight, SkyLight, SkyAtmosphere, ExpFog, PlayerStart). World-guard на L_Dom.
- **Пол уложен:** 154 тайла `SM_Floor_1x1m` сеткой 14×11м (центры тайлов 100·i+50, 100·j+50), папка HOUSE/Floor1. L_Dom **СОХРАНЁН**.

**ДАННЫЕ МОДУЛЕЙ для стройки (ResidentialHouses, проверено):**
- Стены внешние `/Game/ResidentialHouses/Meshes/Architecture/Walls/SM_Outside_1m/2m/3m` (ширина 100/200/300, толщина ~4, **высота 350**); углы `SM_Outside_Corner_*`; проёмы — у House01 были `SM_Outside_Window_3_3m`, `SM_Outside_Door_2_3m`.
- Стены внутренние `SM_Inside_1m/2m/3m` (толщина 15, **высота 315**), `SM_Inside_Corner_*`, `SM_InsideDoor`.
- Пол `SM_Floor_1x1m` (1×1, пивот: спавн(0,0,0)→bounds origin(−50,50)), `SM_Floor_2m_a` (3×2м).
- Потолок `SM_Ceiling_2m_a` (3×2). Лестница `SM_Staircase_Straight` (187×606×366), `SM_Staircase_U`. Ворота `/Game/ResidentialHouses/Meshes/GarageDoor/SM_GarageDoor_Closed`. Щиток `/Game/ResidentialHouses/Meshes/ElectricBox/SM_ElectricBox`.
- **Пивот стены `SM_Outside_3m`:** спавн(px,py,0) rot0 → занимает X∈[px−300,px], Y∈[py−3.8,py] (толщина к −Y), Z∈[0,350]. Стены вдоль Y — rot yaw90.

**ИСПРАВЛЕНО (по чистому плану new.png — раньше работал по ОБРЕЗАННОМУ one_itaj.png и наврал):**
- Дом **Г-ОБРАЗНЫЙ**, не прямоугольник. ЮЗ-угол (под гаражом) = ДВОР, ВНЕ дома. Гараж только в СЗ-углу.
- Раскладка (X запад→восток 0–1400, Y юг→север 0–1100, север сверху, вход юг-центр):
  - ГАРАЖ X[0,400] Y[500,1100] (СЗ; ворота запад, проём Y[700,1000])
  - ДВОР (вне дома) X[0,400] Y[0,500] — вырезано 20 тайлов пола
  - КУХНЯ X[400,700] Y[750,1100] · САНУЗЕЛ X[400,700] Y[550,750] · КЛАДОВАЯ X[400,700] Y[300,550]
  - ЛЕСТНИЦА X[700,1000] Y[300,750] · КОРИДОР X[700,1000] Y[750,1100] · ХОЛЛ/вход X[400,1000] Y[0,300]
  - ГОСТИНАЯ X[1000,1400] Y[0,750] (28м²) · СТОЛОВАЯ X[1000,1400] Y[750,1100]
- Проход совпал с правкой юзера: вошёл→лестница впереди, слева кладовая(ближе)→санузел, справа гостиная.
- **Калибровка пивотов стен (проверено bounds):** Outside_3m h350 тол.~4; Inside h315 тол.15. rot0: X[px-L,px],Y[py-t,py]. yaw90: X[px,px+t],Y[py-L,py]. yaw180: X[px,px+L],Y[py,py+t]. yaw270: X[px-t,px],Y[py,py+L].
- Сделано: периметр Г-формы (19 внешн., вход юг X[800,900], ворота запад Y[700,1000], стены выреза), перегородки (17 внутр.) с дверными проёмами — все 7 комнат достижимы от входа. Пол 134 тайла. PlayerStart→холл (850,150). **L_Dom СОХРАНЁН.**
- Гаражные ворота из пака (SM_GarageDoor_Closed) ≈8м — не лезут в 3м проём, отложены (декор-фаза, масштаб/замена).
- Верификация без скринов: ASCII-карта занятости из bounds (clip клетки 50см), не точечный сэмпл (тонкие стены 15см проскакивают между узлами).

**ОСТАЛОСЬ (Фаза A):** лестница (меш в комнате ЛЕСТНИЦА), потолки, ADoor на проёмах, проём+дверь в подвал под лестницей (заглушка). Потом B (мебель), C (аварии), D (свет+PIE). Живой редактор, world-guard.

## СЕССИЯ 2026-06-17 (ВЕЧЕР) — верификация аним-монтажей CitizenNPC + захват окон (юзер отъехал, авторежим)

Авторежим, редактор ОТКРЫТ (MCP `unreal` жив, 5.7.4). Решение юзера: персонажей-паков докупать ПОТОМ, сейчас остаёмся на **UE4 / CitizenNPC**. PIE не использовал (без фокуса окна дохнет за ~2с — юзер away).

- **Захват окон решён (новый инструмент).** screen-capture-mcp юзер поставил, но он СЛОМАН на PS 5.1 (шлёт буквальные `\n`). Сделал свой `.claude/scripts/capture-window.ps1` (WinAPI `PrintWindow(hwnd,hdc,2)` = PW_RENDERFULLCONTENT) — снимает окно по заголовку, **даже когда оно перекрыто/не в фокусе/на 2-м мониторе** → юзер может alt-tab. Без `-Title` = оба монитора (VirtualScreen). Окна редакторов ассетов (AnimBP) — дочерние top-level, нужен `EnumWindows` (не MainWindowTitle). Проверено: снял и главный редактор, и окно AnimGraph. См. [[screen-window-capture]].
- **АНИМ-МОНТАЖИ CitizenNPC — ЦЕПОЧКА ГОТОВА (блокер из памяти оказался УСТАРЕВШИМ).** Прежняя заметка «у `ABP_CitizenNPC_male` нет montage-слота» НЕВЕРНА. Проверил (read-only, без правок):
  - 7 монтажей `/Game/Avariika/Anim/Montages/` (M_Fix 14с/M_Hit 4.2/M_Death 1.1/M_Knocked 0.9/M_Revive 2.5/M_Bandage 0.4/M_Drink 1.3) — все `AnimMontage`, скелет `UE4_Mannequin_Skeleton`, группа `DefaultGroup`.
  - `SKEL_CitizenNPC_ma.compatible_skeletons = [UE4_Mannequin_Skeleton]` — связка ЖИВА → UE4-mann монтажи играют на CitizenNPC напрямую.
  - **Визуально подтверждён AnimGraph** (скрин окна): `Default (State Machine)` → **`Slot 'DefaultSlot'` (Group 'DefaultGroup')** → `Output Pose`. Слот вставлен и подключён. `get_anim_blueprint_info` → `montage_slot_groups:["DefaultGroup"]`.
  - ⚠️ Гочи: `mcp__unreal__execute_python` НЕ возвращает stdout (только Output Log) → писать в файл + читать; AnimGraph-графы не отдаются через Python-свойства (нет MCP-инструмента на slot-узел — но он уже и не нужен).
- **НЕ делал (риск без присмотра):** правок AnimGraph (он untracked в git, нет безопасного отката — только мой файл-бэкап `Saved/_abp_backup/`). Оказалось и не требовалось.

**ЧИСТКА АНИМ-БИБЛИОТЕКИ (юзер: «удаляем SK_Operator_Skeleton, SKM_MCUE5v2, кота; собаку и StorageUnitsSet оставляем»):**
- ✅ Удалены: `/Game/Characters/Operator` (194 файла, БЫЛ в git → откатываемо), корневые `/Game/Op_*` (~21), `/Game/_Packs/MC_Sample` (MCUE5v2/Mixamo), кот `/Game/AnimX/_Common/CatMannequin` (110). До удаления: убраны 2 актора-превью оператора (`PREVIEW_RIG_GOOD_UE4/BAD_UE5`) с `Lvl_FirstPerson` (сохранён), удалена тест-карта `L_AnimTest`.
- ✅ Сохранены: собака `/Game/AnimX/Dogs` (97), грид-материал `MI_Grid_Gray` (его держит твоя `Test`), `StorageUnitsSet`, CitizenNPC (639). 7 монтажей не зависели от удалённого (проверено).
- ⚠️ **ГОЧА: удаление ассетов через редактор СМЕРТЕЛЬНО медленное** (EAL.delete_asset/delete_directory дёргает revision-control на каждый ассет, ~1с; на ~470 ассетах редактор ЗАВИС в дедлоке, MCP-таймауты). Файловое `rm` при ОТКРЫТОМ редакторе невозможно (.uasset замаплены → «Device or resource busy»). **РЕШЕНИЕ: закрыть редактор → `rm -rf` папок (untracked паки сносятся за секунды).** Юзер разрешил taskkill. Редактор закрыт принудительно (PID убит), уровень был сохранён → потерь нет. Закоммитить удаления (git видит `D`).
- ⚠️ После принудительного закрытия — при следующем старте редактор пере-сканит asset registry (норма); проверить, что `Lvl_FirstPerson` грузится без спама missing-ref.

**A — триггеры монтажей: СДЕЛАНО (Build Succeeded, exit 0).** В `AAvaryoCharacter` (h+cpp):
- 7 `UAnimMontage*` UPROPERTY (`Avaryo|Anim`), грузятся в BeginPlay через `LoadObject` из `/Game/Avariika/Anim/Montages/` (не перезатирают BP-дефолты).
- `MulticastPlayMontage(UAnimMontage*)` (NetMulticast Unreliable) → `GetMesh()->GetAnimInstance()->Montage_Play` у всех (кооп, тело видят напарники).
- **Привязано 5 реакций (событийные, single-shot):** M_Hit ← `TakeDamage` (урон прошёл, не ранен); M_Death ← `VitalsComponent->OnWounded` (HP→0); M_Revive ← `OnRevived`; M_Bandage ← `ApplyItemEffect` Heal (на лекаре); M_Drink ← `ApplyItemEffect` Drink. Делегаты привязаны на сервере в BeginPlay.
- ⚠️ **M_Fix (луп ремонта) и M_Knocked — ОТЛОЖЕНЫ** (нужна loop/edge-state логика, безопаснее с PIE-фидбеком; монтажи уже грузятся, осталось только вызвать).
- **PIE-ПРОВЕРКА за юзером:** получить урон → Hit; HP в 0 → Death (валится); поднять напарника аптечкой → у него Revive, у лекаря Bandage; выпить кофе/термос → Drink. Монтаж играет на ТЕЛЕ (видно в 3-м лице / напарникам; владельцу в 1-м лице тело скрыто — это норма).
2. (фон) докупка модульного воркер-пака (UE5 Manny) когда юзер решит — тогда консолидация скелета на Manny + триггеры заиграют на купленном теле.

**ГОЛОВА/КИСТИ модульного тела — ПОЧИНЕНО (Live Coding succeeded):** PIE-тест монтажей: Hit/Death/Revive ✅; Drink играет «в конце» + Bandage не видно (M_Bandage всего 0.4с — блип; refine later). Голова/кисти НЕ показывались — корень: компоненты `SK_WorkerHead`/`SK_WorkerHands` в BP_AvaryoCharacter ЕСТЬ, но **меш = null + нет Leader Pose** (PASS-2 их добавил, но меш не сохранился — CDO-set гоча). Фикс в `AvaryoCharacter::BeginPlay`: найти части по имени Worker Head/Hand → назначить `SK_ma_head_06`/`SK_ma_hands_average` (LoadObject, если null) → AttachToComponent(Body) identity → `SetLeaderPoseComponent(Body)` (общий скелет, кости 1:1). Видимость в 1/3 лице за них уже делает ApplyCameraView (цикл по модульным частям). **ЮЗЕР: рестартни PIE (Stop→Play) — BeginPlay переотработает, голова/кисти появятся в 3-м лице (V).** Не закоммичено (Live Coding patch; запечь полным ребилдом при закрытом редакторе).

**ПЛАГИНЫ/ТУЛКИТЫ + UDS + железо (вечер, продолжение):**
- **TOT (Tick Optimization Toolkit) + Imperfecter — УСТАНОВЛЕНЫ, СОБРАНЫ, ВКЛЮЧЕНЫ** (оба 5.7-нативные, в `Plugins/` → gitignore, в uproject enabled). TOT — под наши Tick-перф-проблемы; Imperfecter — пост-процесс атмосфера. Ещё не применены к контенту (TOT надо натравить на Repairable/HUD-итераторы; Imperfecter — настроить пресет).
- **UDS уже стоял** (`Content/UltraDynamicSky` 597M, импорт 06-17) + EasyFog/FluidNinja/FluidFlux. **НО развернуть UDS headless НЕЛЬЗЯ:** спавн через Python создаёт полный риг (своё небо/2 DirLight солнце+луна/SkyAtmosphere/VolumetricCloud/SkyLight/фог/PP), но **construction script не рендерит небо** + editor-viewport не перерисовывается без realtime → визуально не проверить; плюс старые ручные DirectionalLight/SkyAtmosphere/VolumetricCloud/SkyLight конфликтуют (задвоение). **UDS = ИНТЕРАКТИВНАЯ задача:** в редакторе перетащить `Ultra_Dynamic_Sky` в уровень (диалог сам предложит убрать существующие sky-акторы), выставить ночь + облака. Я заспавнил→проверил→ОТКАТИЛ (Lvl_FirstPerson не сохранён, 96 акторов как было).
- **Чистка тулкитов в RawAssets** (вердикты): брать — TOT/Imperfecter(5.7); под геймплей — Hyper Spline (кабели/трубы по сплайну = ядро, BP-проект миграция), Next Gen Destruction (взрыв газа рушит стену/пол, BP-проект, нужен фрактуринг мешей в Geometry Collection), Flashlight (фонарь-в-руке стартом → каска-апгрейд), Luoss VFX, DrCG level-tools, InteractionToolkit(референс); монстр Boss AI+слухач — gated; Multiplayer FPS (5.0, ребилд); TranslationToolkit (4.27, это editor-translate, НЕ рантайм-локализация). Скопом не ставить (диск/контекст).
- **ЖЕЛЕЗО:** Ryzen 7 9800X3D (элита) + 32ГБ + **RTX 5060 8ГБ VRAM** ← слабое звено. Зависания редактора = переполнение 8ГБ VRAM (Lumen+Nanite+тяжёлые паки). Менять только GPU, ради VRAM: цель ≥16ГБ (5060Ti 16GB / 5070Ti). CPU/RAM не трогать. Релизу не обязательно (TSR+Scalability масштабируют). Красивый стек (UDS-облака/вода/destruction) гейтить через EffectsQuality + TOT.
- **КОММИТ bd76ef7** (main, НЕ запушен): монтажи (h+cpp) + удаление Operator(194)/L_AnimTest + uproject(плагины). Пред-существующая грязь (1272 удаления + 47 M + untracked) НЕ трогалась — на ревью юзера. Покупки на будущее: **Modular Workers Bundle $50** (280 модулей, UE5, MetaHuman — лучше пака за $60; распродажа до конца июня; сперва FREE-сэмпл QUANTUM проверить) > Workers Modular $60; Xandra CC — отложить (overkill, лица проще через MetaHuman); Road Creator Pro/Waterline Pro — только под конкретику (дорога/затоп подвала), проверить пересечение с WorldBLD/FluidFlux.

## СЕССИЯ 2026-06-17 (ДЕНЬ) — WorldBLD собран, чистка диска, апскейл/Scalability (ФСР-база), паки фабрики

Автономно, редактор был ЗАКРЫТ (MCP не отвечал → идеальный момент для билд-батча). Engine 5.7.4.

- **WorldBLD (`worldbld.com`, окружение: город/дорога/«год») — УСТАНОВЛЕН + СОБРАН.** Скопирован в `Plugins/WorldBLD` (3 C++-модуля Bootstrap/Runtime/Editor, EngineVersion 5.7.0), добавлен в `avariika.uproject`. **Билд сначала упал** (`Build.bat` exit 6): unity/jumbo-сборка склеила `SWorldBLDConfirmPurchaseModal.cpp` + `SWorldBLDAssetDetailPanel.cpp` в один TU, а у обоих в анонимном namespace был `static FSlateFontInfo GetCreditsValueFont()` → ODR-коллизия «function already has a body» (C2084) + каскад `[`-ошибок. **Фикс:** переименовал копию в модалке покупки → `GetModalCreditsValueFont()` (1 определение + 1 вызов, стр.29/154). Ребилд **Succeeded, exit 0**. ⚠️ Плагин платный/контентный, `Plugins/` в gitignore → ЛОКАЛЬНЫЙ, на свежем клоне re-copy из RawAssets + ребилд; правка ODR — в `SWorldBLDConfirmPurchaseModal.cpp`.
- **Полный ребилд `UnrealEditor-Avaryo.dll` (clean, не Live Coding) — ЗАПЕК предыдущие правки навсегда:** `IsWelding()` + газ-петля «сварка=взрыв» + модульные части в `ApplyCameraView` (FirstPersonPrimitiveType-тогл) теперь в базовой DLL, а не в patch-DLL Live Coding. Warning про packaging от UFUNCTION ушёл.
- **Апскейл / Scalability («DLC ФСР»):** встроенный апскейлер UE 5.7 = **TSR** (Temporal Super Resolution) — наш бесплатный аналог FSR/DLSS, уже дефолт. **AMD FSR3 / NVIDIA DLSS в движок НЕ входят** (поиск по Engine/Plugins пуст) — это отдельные плагины FAB/NVIDIA, докинуть позже опцией в меню. Создал **`Config/DefaultScalability.ini`**: `ResolutionQuality` (= r.ScreenPercentage, коэф. апскейла) по уровням 50/67/80/**100**(Epic)/100, `EffectsQuality` гейтит тяжёлые партиклы/воду. Ничего не форсит → на сильной машине (Epic) картинка не меняется; слабая машина авто-бенчмарком падает на Low и TSR дотягивает. Вода FluidFlux — показывать при EffectsQuality≥2 (логика в актёре, ориентир записан). `r.AntiAliasingMethod` — TSR (дефолт, не трогал).
- **Чистка диска (было 40ГБ → стало ~63ГБ свободно):** удалены дубликаты RawAssets уже импортированного (EOSCore 21ГБ, Residential Vol.2, AtmosphericHouse, CitizenNPC, NiagaraExplosion01, 3 plugin-source), Paragon-боёвка 9ГБ (не наш жанр), Explosions Builder 1.5ГБ. Импортированы в Content: UltraDynamicSky, EasyFog, FluidNinja (`/Game/FluidNinjaLive`), FluidFlux (`/Game/FluidFlux`, 858 ассетов). См. [[water-tech-decision]].
- **ПАКИ ФАБРИКИ — ЗАГРУЖЕНЫ ВСЕ (юзер: «грузи все 3, потом сделаем скан, проверим что оставляем»):**
  - `Content/IndustrialFactory` (22ГБ, 2587 uasset, 15 карт) → `/Game/IndustrialFactory/`. Огромный модульный кит (бочки/ящики/кирпич/кабели/бульдозер/...) + демо-сцена **FactoryDocks** с сублевелами (Env/Lighting Day-Overcast/Sounds/Effects/Canal/Outdoor). MOVE из RawAssets (rename, 0 диска).
  - `Content/Warehouse` (17ГБ, 2523 uasset, 9 карт) → `/Game/Warehouse/`. **«Warehouse - Abandoned Factory District»** (Scans Factory, фотограмметрия): демо Warehouse_01_P + сублевелы день/ночь/улица/EngineHouse/камеры/аудио. MOVE из RawAssets (rename, 0 диска). **СНАЧАЛА ПРОПУСТИЛ** этот пак (спутал с AbandonedFactory) — юзер заметил «что то не так», догрузил.
  - `Content/AbandonedFactory` (3.7ГБ, 603 uasset, 10 карт) → `/Game/AbandonedFactory/`. **«Abandoned Factory Buildings - Day Night Scene»** (3-й, отдельный Scans-пак), демо тоже названы Warehouse_01/02/03. Взята UE5.0-версия (распаковал split-RAR через 7z; 1 файл `t_WallA_01_1004_n` упал по CRC → перезалил чистой копией из UE4.27-версии).
  - **ИТОГО 3 пака Scans Factory** (юзер ждал именно Warehouse + Industrial Abandoned): `/Game/IndustrialFactory` (Industrial Abandoned), `/Game/Warehouse` (Warehouse District), `/Game/AbandonedFactory` (Abandoned Factory Buildings).
  - **Корни /Game проверены** grep'ом по бинарю uasset (каждый пак ↔ своя папка) — коллизий с нашими папками нет. **Нужен рестарт редактора** для пере-скана asset registry.
  - **СКАН/ВЫБОР — отдельный шаг (с юзером, нужен открытый редактор для визуалки):** 3 фабричных окружения, держать все избыточно (~43ГБ). Кандидат на Workflow-сравнение (качество/перекрытие/пригодность под наш ремонт-хоррор). RawAssets-источники: Industrial перемещён (нет), Warehouse перемещён (нет), Abandoned Factory Buildings 14ГБ ещё в RawAssets (фоллбэк).

**ОЖИДАЕТ (нужен открытый редактор или решение юзера):**
1. **Импорт звуков/анимаций** (нужен ОТКРЫТЫЙ редактор; headless-pythonscript у нас крашит на Niagara CDO): `звуки` (rope creak — 1 WAV распакован, остальное в `.rar`); `еще анимации` = **1436 FBX** в 5 паках (Getting Up ×2, Motifect Injured/Exhausted, Motifect Emotes/Social, GASP-retargeted-to-UE5-Mannequin 2.3ГБ). Курировать, не лить всё.
2. **Выбор пака фабрики** (A IndustrialFactory vs B Abandoned Factory Scans) → импорт + удаление второго.
3. GASP 2.3ГБ — оставить как апгрейд локомоции или удалить (наши скелеты — CitizenNPC/Mixamo, не UE5-маннекен → ретаргет нужен).
4. Editor-restart чтобы подхватить WorldBLD + UltraDynamicSky/EasyFog/FluidNinja/FluidFlux.



Полностью автономно (юзер: «делай что хочешь, открывай/закрывай редактор, иди по WORKLOG»). MCP `unreal` (StraySpark) ЖИВ — основной канал, Engine 5.7.4.

- **EOSCore (eelDev, кооп-бэкенд) + UnrealMonsterAssetInstaller — УСТАНОВЛЕНЫ и СОБРАНЫ** (`Build.bat avariikaEditor … -WaitMutex` → Succeeded, exit 0). Из пака `RawAssets/EOSCore v1.9.6.5 5.5/` взят вариант под 5.7 — **`EOSCoreE942549feccaaV16` (uplugin EngineVersion 5.7.0)** + полный Source + EOS SDK (в паке 5 вариантов: 5.0/5.4/5.5/5.6/5.7). ⚠️ **`Plugins/` в .gitignore → плагины ЛОКАЛЬНЫЕ, НЕ в репо** (на свежем клоне re-add из RawAssets + ребилд). EOS-ключи в `Config/DefaultEngine.ini` → `[/Script/OnlineSubsystemEOSCore.EOSCoreSettings]` (формат — стандартный UE-struct-text). `DefaultPlatformService=Null` (LAN/PIE работает); онлайн EOS = флипнуть на `EOSCore` + раскомментить NetDriver-блок + плейтест. ⚠️ ClientSecret в git-файле → **ротировать если репо публичный**. См. [[eos-plugins-batch]].
- **Бригада CitizenNPC импортирована** (`Content/CitizenNPC`, 639 ассетов, 1.1ГБ, НЕ gitignored). Самодостаточная **модульная UE4-Mannequin система**: своя локомоция (`ABP_CitizenNPC_male/female`: idle/walk/run/jump + BlendSpace), **Leader-Pose «одежда отдельно»** (`BP_master_character`: SK_Hat/Hair/Head/Chest/Hand-Glove/Eyeglasses + SM_RightHand/LeftHand). Скелеты `SKEL_CitizenNPC_ma/fe`. Типажи: construction_worker ×3, firefighter, police ×5, waste_driver, worker.
- **Сварочник заменён** (Lvl_FirstPerson, актор `WeldingMachine`): meshy `SM_WeldingMachine` → `Garage_Tools_Props/Meshes/07_Welding_Machine/SM_Welding_Machine`, scale 1.0 (~49см), материал `MI_07_Welding_Machine` форс-назначен (был синий override). Уровень сохранён. ⚠️ **Кабели/торч НЕ добавлены** — пак-BP `BP_Welding_Machine` = 6 мешей (тело+торч+зажим+электрод+коннекторы), нужен визуальный заход.
- **Тело игрока — Option A (юзер: «а» = бригада = тело игрока). PASS-2 СДЕЛАН:** `BP_AvaryoCharacter` CharacterMesh0 (унаследованный нативный Mesh) hazmat → `SK_ma_construction_worker_01_c` + `ABP_CitizenNPC_male` (скелет `SKEL_CitizenNPC_ma`). **+ добавлены компоненты `SK_WorkerHead` (`SK_ma_head_06`) и `SK_WorkerHands` (`SK_ma_hands_average`)** на ТОМ ЖЕ ABP+скелете, оффсет тела (0,0,-90 / yaw-90), `bOwnerNoSee=true` (скрыты от владельца, как тело). Части синхронятся одним аниматором без Leader-Pose-графа. BP компилируется + сохранён. ⚠️ **НУЖЕН PIE-EYEBALL:** выравнивание головы/кистей на шее/запястьях (если съехало — править RelativeLocation Z; стандартный ACharacter оффсет -90 мог отличаться в кастомном AvaryoCharacter). Откат тела на hazmat = 1 однострочник (mesh→`/Game/hazmat/Mesh/hazmat`, anim→`/Game/Hospital/Free_Content_Epic_Games/Mannequin/Animations/ThirdPerson_AnimBP`; голову/кисти удалить из BP).
- **СКЕЛЕТЫ (важно для ретаргета):** hazmat = `UE4_Mannequin_Skeleton`; CitizenNPC = свой UE4-Mann `SKEL_CitizenNPC_ma`; `SK_Operator` (196 `Op_` анимов) = **Mixamo** (24 кости Hips/Spine — НЕ маннекен) → `Op_` на бригаду = **Mixamo→UE4Mann IK-ретаргет**.
- ⚠️ **Готчи сессии:** (1) MCP `execute_python` надёжно гонит только ОДНОСТРОЧНЫЙ код (многострочный с def/try не выполняется → писать в файл и читать, либо однострочники с `;`); (2) скрин редакторского вьюпорта в автономе НЕ обновляется (старый кадр) → визуалка только через PIE; (3) **в MCP НЕТ IK-ретаргет-инструмента**.

- **+ МЕХАНИКА (утро 06-17): сварка рядом с утечкой газа = ВЗРЫВ.** Новый `AAvaryoCharacter::IsWelding()` (чинит Hold-этапом + в руках `ToolTag==Welder`, открытая дуга); газовая петля в `ARepairable::Tick` теперь поджигает облако при `It->VitalsComponent->IsSmoking() || It->IsWelding()` (было только курение). → «перекрой газ (вентиль) ПЕРЕД заваркой» — один из 4 газ-способов. Пена по-прежнему гасит (контрплей). **Live Coding: succeeded** (⚠️ warning про packaging от новой UFUNCTION — перед кук-билдом сделать полный ребилд). PIE-тест: утечка газа + варить рядом → бабах; перекрыть вентилем / запенить → безопасно.

**ОЖИДАЕТ (верифицируемая сессия с PIE/юзером):**
1. Достроить тело игрока: модульные голова/кисти/каска/одежда (Leader Pose) + FP-меш под монтёра. Хирургия ядрового BP — только с визуальной проверкой.
2. Ретаргет `Op_`(Mixamo)→`SKEL_CitizenNPC`(UE4Mann) через IK Retargeter (`IK_UE4Mann` — половина есть) → геймплейный AnimBP бригаде поверх локомоции.
3. Дверь-анима `anim_OpenDoor` (Free_Interaction_Animation) → в `ADoor` (играть по E).
4. Сварочник: кабели/торч (6 мешей пак-BP).
5. Стройка дома (слайс подвал+1эт) — план ч.4 ниже, нужны планы/глаза юзера.

**НЕ закоммичено** (грязное дерево 1495 файлов + секрет в ini + плагины в gitignore) — всё на диске сохранено, коммит по ревью юзера. См. [[operator-character-anim-system]].

## СЕССИЯ 2026-06-16 (НОЧЬ, ч.4) — решения по MCP-каналу + план стройки (юзер: «делай сам»)

Сравнили MCP-варианты (Claudius / StraySpark 2.0.2 / ClaudusBridge / Rekall / Claude Unreal). Итоги:
- **StraySpark 2.0.2 — основной канал** (нативный MCP, 304 тулзы, AnimGraph-стейт-машины ПОДТВЕРЖДЕНЫ). ⚠️ Это НЕ v4: нет `run_tool_script`/catalog/background. Обновить юзер не может — живём на 2.0.2 (Full preset, 1M контекст тянет).
- **`.mcp.json` → только `unreal`** (NWIRO убран из моего конфига: дублирует + бьёт контекст; его editor-UI у юзера остаётся).
- **ClaudusBridge** — единственный с «зрением» (WebRTC /preview + watcher), но их доки НЕ подтверждают работу при свёрнутом/нефокусном окне (наш кейс). Юзер НЕ купил. Отложено; вопрос подан в их Discord-тикет (ответ 48-72ч). Зрение пока — скрины юзера.
- **Способ стройки дома: 🅱️-1** — я строю по координатам, юзер раз в этаж кидает скрин для сверки (его скрины я ВИЖУ; мои самозахваты — нет). Начинаем с вертикального слайса **подвал + 1 этаж** (план 1-го этажа уже есть — `one_itaj.png`).

**КРИТ для следующей сессии (нужно от юзера):**
1. **Рестарт Claude Code** (в этой папке) + **подтвердить доверие** MCP `unreal` → тогда у меня 304 нативных тулзы StraySpark.
2. **Редактор держать открытым** (MCP-сервер живёт внутри него; порт 13579).
3. **Прислать планы**: подвал + 2/3 этаж + чердак (примерные размеры комнат, высота этажа).
4. Дальше: я собираю слайс (подвал+1эт) из модулей Residential + подвал AtmosphericHouse, юзер шлёт скрины для сверки; параллельно — AnimBP оператора (StraySpark умеет стейт-машины).

## СЕССИЯ 2026-06-16 (НОЧЬ, ч.3) — установлены MCP-плагины (UnrealMCPServer + NWIRO)

Юзер закинул 2 плагина в `RawAssets/` → поставил оба в `Plugins/` проекта:
- **`Plugins/UnrealMCPServer57/`** (StraySpark, v2.0.2) — MCP-сервер UE: **304 инструмента**, `run_tool_script` (батч-транзакции — ключ для стройки дома!), catalog-режим, фоновые задачи. Слушает **`http://localhost:13579/mcp`** (Streamable HTTP, без токена, localhost-only). Собрался из исходников (Build Succeeded).
- **`Plugins/NwiroIntegrationKit/`** (Leartes, v1.0.3) — тоже MCP-сервер на **порту 5353 (209 инструментов)** + in-editor AI-UI. Подхватил готовый бинарь.
- **`.mcp.json`** в корне: NWIRO прописал себя сам (5353), я дописал `unreal` (13579). Оба `type:http`.
- **Проверено:** оба сервера стартуют на запуске редактора (порты слушают, лог `LogUnrealMCP: MCP server ready with 304 tools`).
- ⚠️ **АКТИВАЦИЯ:** Claude Code подхватит MCP-серверы только после **рестарта Claude Code** + **подтверждения доверия** проектным MCP (он спросит). До этого работаю через старый Claudius (file-mode). MCP-серверы живут ВНУТРИ редактора → редактор должен быть открыт.
- **После активации:** перейти на UnrealMCPServer как основной канал (чище/быстрее Claudius + `run_tool_script` для стройки). NWIRO можно отключить из `.mcp.json`, если 304+209 тулзов раздувают контекст (его in-editor UI всё равно останется юзеру).

## СЕССИЯ 2026-06-16 (НОЧЬ, ч.2) — МЕХАНИКА «живой провод» (электро-цепочка из плана)

Юзер: «делай механики» (планировку дома пришлёт позже). Сделал электро-аварию по плану «⚡ выключить рубильник → починить проводку».
- **Аддитивно в `ARepairable`** (новые флаги по умолчанию ВЫКЛ → существующие объекты не затронуты): `bLiveWireWhenBroken` + `LiveWireShockDamage/Interval/Panic` (конфиг), `bElectricallyPowered` (Replicated, рантайм-питание), геттеры `IsLiveWire()`/`IsLiveWireHot()`/`IsPowered()`, метод `SetPowered(bool)` (ставит и `bElectricallyPowered`, и `bFloodElectrified`).
- **Поведение:** пока сломан + питание подано → провод «горячий»: Tick бьёт током всех в радиусе `RepairRange*1.15` (сухой контакт — **сапоги НЕ спасают**, в отличие от потопа), и `CanBeRepairedBy`/`CanBotchBy`/`CanContinueRepair` возвращают false (чинить нельзя). Срубить рубильник → не горячий → чинить (изолента = RequiredTool/этап).
- **`APowerSwitch.ApplyToFloods`** теперь зовёт `SetPowered` вместо `SetFloodElectrified` → один рубильник рулит и потопом, и живым проводом.
- **Сборка Succeeded** (15с, только пред-существующие deprecation-варнинги). **Верификация** (`Scripts/verify_livewire.py`): спавн сломанного живого провода под питанием → `IsLiveWireHot()=true` ✅. Power-off-кейс скриптом не проверить (флаг реплик-онли, методы рубильника не-UFUNCTION) — логика тривиальна (`&& bElectricallyPowered`) + вызовы проверены по коду.
- **Готчи:** UE Python снимает `b`-префикс у bool-UPROPERTY (`bBroken`→`broken`, `bLiveWireWhenBroken`→`live_wire_when_broken`); реплик-онли проперти (без EditAnywhere) не ставятся `set_editor_property`.
- **+ HUD-подсказка на объекте** (`RefreshStatusVisual`, код состояния 3000): «{имя} — ПОД НАПРЯЖЕНИЕМ! сними рубильник» электро-жёлтым, пока горячий. Live Coding.
- **+ МЕХАНИКА «свет на домовом питании»** (`APowerSwitch.ApplyToFloods`): рубильник гасит/зажигает лампы у акторов с тегом **`PoweredLight`** (`GetComponents<ULightComponent>` → SetVisibility(bPowerOn)). **Размен питание↔темнота:** ВКЛ = светло, но провод/вода под током; ВЫКЛ = безопасно чинить, но темно → фонарь (кооп-хоррор-напряжение). Тег-гейтед → существующие карты не затронуты. Live Coding.
- **Итоговая пересборка Succeeded** (запекла подсказку + свет в бинарь, чтобы не откатились при рестарте). Редактор закрыт.
- **Рубильник теперь рулит тремя вещами:** электрификация потопа + живой провод + свет дома (тег `PoweredLight`). Цельный «электро/питание» слайс.
- **Ожидает:** размещение в доме (live-wire щиток `SM_ElectricBox` + рубильник + изолента + лампы с тегом `PoweredLight`) — когда будет планировка.

## СЕССИЯ 2026-06-16 (НОЧЬ) — смена ассет-пака дома: удалён PostApoc, импорт Atmospheric + Residential

**Решение по дому (после сравнения 4 паков):** старый `PostApocalypticHouse` (постапок-руина, пиратский плейсхолдер) — НЕ наш сеттинг. Берём **реалистичные современные дома**.
- **Камера-фикс ПОДТВЕРЖДЁН пользователем** («работает хорошо») — 1-е лицо больше не в стене, фонарь по центру. См. ниже ч.2 + [[character-camera-crouch]].
- **`PostApocalypticHouse` УДАЛЁН** (4.8 ГБ + 21 МБ external actors, gitignored → чисто дисковое, на git ноль). Редактор закрылся чисто перед удалением. ⚠️ Осиротел `Content/Avariika/Maps/L_Dom.umap` (git-трекается, ссылался на пак) — НЕ удалял, ждёт решения пользователя. `LV_Main_moy` был внутри пака → удалён вместе с ним.
- **Импортируем 3 пака (юзер закинул в `RawAssets/`):**
  - `AtmosphericHouse` — **целиком** (1633 ассета, 9.3 ГБ; корень `/Game/AtmosphericHouse/`; есть подвал-модули + свапер clean↔worn + демо-карты House_clean_day/worn_night/Showcase). robocopy → `Content/AtmosphericHouse/`.
  - `Residential Houses Vol.2` (RAR x3, ~12 ГБ) + `Residential Vol.1` (качается) — **дедуп между ними** (один дев, сильное пересечение), импорт не-дублей. Vol.1 = меблированные дома (на скринах обставлено), Vol.2 = больше коробок (на витрине пусто, но мебель в пакете). Residential = реалистичный пригород США (наш сеттинг).
- **Метод импорта:** робокопия дерева в `Content/<Pack>/` (редактор закрыт → подхватит при старте). Тяжёлые текстуры → следить за Streaming Pool (уже 2500).
- **Дальше (автономно, разрешил юзер):** дедуп+импорт Residential, разбор структуры, сборка 1-го этажа по `HOUSE_PLAN.md`, перенос систем (двери/вода/газ/электрика — пак-независимы).

**Сборка L_Dom (автономно, юзер отъехал, разрешил открывать редактор):**
- Удалены старые карты `L_Dom`+`L_Dom_Phase1` (1016 файлов; в коде `ACallBoard` были только комментарии → ничего не сломалось).
- Импорт подтверждён: редактор видит ResidentialHouses (House01-12, Garage01) в реестре.
- **Создана `/Game/Avariika/Maps/L_Dom`** (`Scripts/build_house_v1.py` + `build_house_v2_fix.py`, оба с гардом на world==L_Dom): готовый дом **House01** (599 мешей, габарит **22.8×27.8×9 м** — ~3 этажа), земля-плоскость, DirectionalLight+SkyLight+SkyAtmosphere+Fog, PlayerStart по центру (floor+100), **GameMode override = BP_AvaryoGameMode** (проектный дефолт = тестовый blueprinsTest/BP_Gamemode, поэтому override обязателен). Сохранено.
- ⚠️ **Готчи:** House01 = ~600 child-акторов → габариты считать по всем акторам, не по родителю BP (родитель давал 2.4м); `world.get_world_settings()` ставит GameMode (get_all_level_actors WorldSettings НЕ возвращает); мои `viewport.take_screenshot` не создают файл (нужен фокус окна).
- **Ожидает юзера (визуал):** PIE-тест L_Dom (не в стене ли PlayerStart? как House01 на вид?). Дальше — подвал из AtmosphericHouse + аварии (вода/электрика/газ/двери) по комнатам — нужны глаза/выбор дома.

## СЕССИЯ 2026-06-16 (ПОЗДНИЙ ВЕЧЕР, ч.2) — КОРЕНЬ бага «в стене»/коллизии: FP-камера уехала вбок

**Симптом** (юзер, скрины three.png/fo.png): в 1-м лице игрок «внутри стены», фонарь светит СПРАВА от персонажа, не входит в комнаты; в 3-м лице всё ок. Двери/паника были ни при чём.
**Диагностика** (`Scripts/diagnose_fp_offset.py` — спавн BP в редакт. мир, дамп world_off всех компонентов): капсула radius=34, halfheight=96. **FirstPersonCamera world_off от центра капсулы = (X −16, Y +80, Z +61).** Камера висела на сокете кости `head` тела `CharacterMesh0` (yaw −90, rel −20/0/−96); FP-меш без корректирующего трансформа (rel 0,0,0) → голова уходит на +80 см вбок, далеко за радиус капсулы 34. Фонарь+предмет в руках — дети камеры, ехали с ней.
**Фикс** (`AvaryoCharacter.cpp`, Live Coding, без новых членов → шла с открытым редактором):
- `BeginPlay`: FP-камеру (`ViewCamera`) намертво перецепляем к `GetCapsuleComponent()` по центру (rel (0,0,64), `bUsePawnControlRotation=true`). Геометрия костей больше не влияет. **Нужен `#include "Components/CapsuleComponent.h"`** — иначе `UCapsuleComponent` неполный тип → C2446/overload fail (споткнулся на этом, добавил инклюд).
- `ApplyCameraView`: FP-«меш» (полное тело, не руки) прячем от владельца всегда (`SetOwnerNoSee(true)`) — иначе клиппит центрированную камеру. Для других тело рисует `CharacterMesh0` (WorldSpaceRepresentation).
- `UpdateCrouchEye`: при центрированной камере (родитель == капсула) — ранний выход: присед опускает камеру сам (движок ужимает капсулу ~46 см). Старый способ (двигать FP-меш по Z) оставлен фолбэком.
**Ожидает плейтеста:** рестарт PIE (не редактора) → 1-е лицо по центру, фонарь прямо, вход в комнаты, присед опускает обзор.

## СЕССИЯ 2026-06-16 (ПОЗДНИЙ ВЕЧЕР) — карта дома LV_Main_moy: двери, паника, фонарь, каталог пака

**Карта `LV_Main_moy`** (рабочая копия дома из `PostApocalypticHouse/Maps/`, World Partition). Фиксы по фидбеку:
- **Открывающиеся двери.** Новый C++-класс `ADoor` (`Source/Avaryo/World/ADoor.{h,cpp}`): Hinge(root)+DoorMesh(static, ECR_Block)+Zone(box); распахивается вокруг петли (OpenAngle=95°, OpenSpeed=4.5), `bOpen` реплицируется, звук. Роутинг E в `AvaryoCharacter` (`FindFocusedDoor`+`ToggleBy`) добавлен после рубильника. `Scripts/place_openable_doors.py` заменил **7 статичных дверей** (`SM_Door_01a`×6 + `SM_Door_02a`×1) на `ADoor` в их трансформах (пивот меша на петле X=0 → меш с offset 0 стоит на месте). Гаражную/фасадную не трогали. Карта сохранена. **Ожидает плейтеста: E у двери → распахнуть → войти в комнату** (это и был баг «упираюсь в текстуры в 1-м лице» — закрытый лист двери блокировал проход).
  - ⚠️ Готча: `dm.set_relative_location(...)` на компоненте требует аргументы `sweep`/`teleport` → использовать `set_editor_property("relative_location", Vector)`.
- **Паника отключена для тестов** (`VitalsComponent.cpp`): cvar `Av.NoPanic` default=1 (AddPanic игнорится, паника утекает к 0). **Перед релизом вернуть 0.**
- **Фонарь приглушён** (Headlamp intensity 8000→900) + клампы автоэкспозиции в `DefaultEngine.ini` (Min 0.5 / Max 4.0) — лечило «белый экран». `r.Streaming.PoolSize=2500` (было «лимит» Texture Streaming Pool Over Budget на three.png).

**Док для стройки:** `HOUSE_STRUCTURE.md` переписан в ПОЛНЫЙ каталог — все 71 кусок `Mesh\Structure` (фундамент/стены/фасад/проёмы/полы/лестницы/перила/крыльцо/крыша/дымоход) + 102 `Mesh\Props` по группам + `Blockout` + порядок стройки по шагам. Отправлен пользователю.

## СЕССИЯ 2026-06-16 (ВЕЧЕР) — реорганизация папок + сцена «Вода 2.1» на карте

**Реорг папок (по просьбе «всё по папкам»):** 29 чужих/неиспользуемых паков → **`/Game/_Packs/`** (корень ~40→~26). Игровые/код-залоченные папки (`Audio`,`Avariika`,`hazmat`,`Characters`,`Hospital`,`Niagara*`,`Survival_SFX`,`Input`,`EasyOptionsMenu`,`LevelPrototyping`) ОСТАВЛЕНЫ в корне — вшиты в C++/карты, по сути уже разложены по типам. Игра ЦЕЛА (аудит зависимостей: перенесённое не в графе игры; лог чист; 97 акторов на Lvl_FirstPerson). `PROJECT_STRUCTURE.md` — карта «что за что».
- **Метод:** `EditorAssetLibrary.rename_directory("/Game/X","/Game/_Packs/X")` — надёжно для обычных паков. Дубликаты общего UE-манекена между аним-паками = unreferenced, чистил `delete_directory`/filesystem.
- **🟡 Застряли (entangled, unused, оставлены в корне):** `GoreAndHorrorMegapack` (split), `JKMotion_HitReaction`, `ResourcePack` (кросс-зависит с `_Packs/FootstepSystem`). Дочистить ручным перетаскиванием в CB.
- **⚠️ Дом-переименование (PostApocalypticHouse→moduleHouse) НЕ удалось скриптом:** `rename_directory` для WP-папки (5ГБ, 6 карт) вернул false и НИЧЕГО не сделал (а `AssetTools.rename_assets` ранее крашил). PostApocalypticHouse ЦЕЛ (667 ассетов). **РЕШЕНИЕ: переименовать вручную в Content Browser** (ПКМ по папке → Rename → moduleHouse; диалог корректно тащит WP). Пустой остаток `/Game/moduleHouse` снёс.

**Вода 2.1 — слайс собран и РАЗМЕЩЁН на Lvl_FirstPerson** (`Scripts/setup_water_cascade.py`): `Repairable_WaterPipe` (bFloodsWhenBroken, разлив+ток), `APowerSwitch` «WaterPowerSwitch» (рубильник снимает ток), `RubberBoots` (диэлектрик). C++ собран (slice 1a flood + slice 1b APowerSwitch+routing). **Ожидает плейтеста:** зайти в разлив без сапог при питании ВКЛ = бьёт током; рубильник ВЫКЛ или сапоги = безопасно → чинить трубу.

## СЕССИЯ 2026-06-16 (ДЕНЬ) — фиксы камеры/приседа, перенос карты, песочница Test, старт «Вода 2.1»

**Текущий модуль: 🚰 Прорыв воды 2.1** (флагман). Дизайн + статус всего проекта зафиксированы в `AVARIIKA_MEHANIKI.md` §12 (что есть) и §13 (что понадобится). Пользователь дал мне выбирать направление — выбрал воду (единственный из 4 базовых каскадов, кого нет; содержит всё ядро: цепочка + порядок-наказание + дефицит снаряжения). План слайса 1: потоп (мирроринг газового облака в `ARepairable`) + электрифицированная зона (вода+питание=ток) + рубильник (снять ток) + резиновые сапоги (закрыть уязвимость) + добить существующими этапами (вентиль+сварка).

**Фиксы по тесту пользователя (всё собрано, в `main`-коде локально, PIE-подтверждено):**
- **Присед не опускал камеру в 1-м лице.** Корень: FP-камера висит на кости `head` FP-меша (шаблон UE5.5 true-FPS), а поиск FP-меша по имени `Contains("FirstPerson")` молча давал NULL → `UpdateCrouchEye` не работал. Фикс: брать «единственный скелетный меш кроме GetMesh()» + ленивое самолечение; `UpdateCrouchEye` (Tick) двигает FP-меш по Z на −46. Замер: FP cam Z 366.9→317.3 при присяде. ✅
- **В 3-м лице «только тень, нет тела».** Корень — НЕ `owner_no_see` (часы потрачены зря на флаги): тело `CharacterMesh0` помечено `FirstPersonPrimitiveType=WorldSpaceRepresentation` (UE5.5 first-person рендер прячет его ОТ владельца, тень оставляет). Фикс: `ApplyCameraView` переключает `None` (3-е лицо) ↔ `WorldSpaceRepresentation` (1-е). ✅ скрин подтвердил оператора.
- **Ctrl в прыжке дёргал камеру** → `StartCrouchInput` игнорит присед в воздухе (`IsFalling`). ✅

**Карта Test = песочница пользователя** (учит UE по урокам): World Settings → GameMode Override = `BP_GameMode` (его `BP_Pawn` + `BP_PlayerController` из `Content/Avariika/blueprinsTest/`). Avaryo-логику на Test НЕ навязывать.

**🚚 Главная карта ПЕРЕНЕСЕНА:** `/Game/FirstPerson/Lvl_FirstPerson` → **`/Game/Avariika/Maps/Lvl_FirstPerson`** (имя сохранено — гарды скриптов по подстроке работают). 94 актора + GameMode override целы. Обновлены `DefaultEngine.ini` (EditorStartupMap), C++ `ACallBoard.cpp`/`AvariikaOnlineSubsystem.h`, пересобрано. ⚠️ **Готча:** перенос WP/OFPA-уровня — `rename_asset`=no-op, `AssetTools.rename_assets` КРАШИТ редактор (но физически перенос успевает пройти). Бэкап лежал в `_mapmove_backup`. Старые dev-`Scripts/*.py` и доки (README/ASSETS) ещё ссылаются на старый путь — на игру не влияет.

**🔑 Новый факт по верификации:** снять ИГРОВОЙ PIE-кадр (то, что видит игрок) — `unreal.AutomationLibrary.take_high_res_screenshot(w,h,name)`; а `viewport.take_screenshot` отдаёт РЕДАКТОРСКИЙ вьюпорт (не игру). PIE сам закрывается за ~2с, если окно редактора не в фокусе.

## СЕССИЯ 2026-06-16 (НОЧЬ, полный автономный доступ) — фиксы по фидбеку + PIE-верификация

Пользователь ушёл спать, дал ПОЛНЫЙ доступ: «открываешь/выключаешь редактор когда удобно, дорабатываешь механики, ведёшь свой список задач, тестишь, звуки/эффекты». Задачи — TaskList #14-19.

**🔑 ГЛАВНОЕ ОТКРЫТИЕ (отменяет прежнюю заметку): скриншоты РАБОТАЮТ, пока активен PIE.** Раньше думал «свои скрины не рендерятся» — но это только в фоне БЕЗ PIE. С `console.start_pie` редактор рисует кадры → `viewport.take_screenshot` пишет файл → я его читаю и РЕАЛЬНО ВИЖУ игру. Плюс можно телепортировать игрока/крутить камеру скриптом (`GameplayStatics.get_player_pawn/controller` в `get_game_world()`) и читать рантайм-состояние. → теперь могу верифицировать вслепую-фиксы по-настоящему. См. `Scripts/inspect_pie.py`, `teleport_to_breaker.py`, `pose_stand.py`/`pose_crouch.py`.

**Фидбек пользователя (тест) → разобрано:**
- **«присяда нету»** → корень: `bCanCrouch` из C++-конструктора ПЕРЕБИВАЛСЯ дефолтом компонента в Blueprint (классика). Фикс: форсю `bCanCrouch/CrouchedHalfHeight=50/MaxWalkSpeedCrouched` в `BeginPlay` (рантайм). **PIE-подтверждено:** `can_crouch=true`, и камера РЕАЛЬНО опускается (прицел упал с Термоса z=342 стоя → Огнетушитель z=217 присев).
- **«смотрю на щиток, а пишет поднять аптечку/кабель»** → предмет у щитка (оверлап из-под ног) перебивал подсказку ремонта. Фикс: `FindFocusedItem` не отдаёт оверлап-предмет, если стоишь в радиусе ремонта сломанного объекта (хочешь предмет — наведись прямо). **PIE-подтверждено:** перебоя больше нет.
- **«всё починил → сразу Акт, нет точки эвакуации»** → `HasExitZone()` **PIE=true**: зона ВЫХОДА (ExitZone_Gazelle у фургона) детектится, победа ТРЕБУЕТ прийти в неё. Было незаметно (нет подсветки) → добавлен зелёный маяк (прошлая пачка) + по просьбе **скриптовый облом**: всё починили → щиток выбивает СНОВА (искры+звук), эвакуация отменяется, диспетчер «Чёрт, щиток снова выбило! За работу, бездари» → дочинить заново.
- **камера 1↔3 лицо (V)** — добавлена (TP на пружине, локальный вид); **присед опускает камеру** — подтверждено. **V проверить пользователю** (нажатие клавиши скриптом не сэмулировать).
- **газ-редизайн (перекрыть→убрать→заварить=бабах→открыть, 4 способа)** — БОЛЬШОЙ, отдельной итерацией с PIE-тестом (стейт-машина трубы, вслепую рискованно). Пока сделан способ №2: **пена огнетушителя развеивает облако**.

**Все фиксы в `main`, билды зелёные, PIE-смоук чист.** Коммиты сессии: hub-loop → audio/idle → tool-case → soundscape/kit-rib/impatience → surge BZZT → surge-flashlight-blackout → gitignore → temp-map(Дом→Lvl_FirstPerson)+exit-beacon → foam-disperses-gas → crouch+retrip → prompt-fix+exit-getter.

**ОЖИДАЕТ ПОЛЬЗОВАТЕЛЯ (перезапусти редактор — DLL обновилась!):** пройти петлю L_Hub→ящик→доска→выезд→починка→облом-щитка→дочинить→зелёный маяк→к ГАЗели→Акт→R→база; нажать V (камера); потыкать присед (Ctrl/C). Что не так — фидбек, чиню.

## СЕССИЯ 2026-06-15 — МОДУЛЬ «Хаб + Доска заявок» (мета-петля), автономно

Пользователь ушёл учить редактор по урокам **MakeYourGame!** (30 уроков), дал отмашку автономно делать всё, что НЕ требует его глаз: хаб, окружение, «стену с вызовами», диспетчера, звуки, эффекты, механики по `CONCEPT.md`.

**Ключевой факт сессии:** скриншоты, которые я триггерю через плагин, в фоне НЕ рендерятся (редактор не рисует кадр без фокуса/PIE) → свои скрины снять не могу. Но скрины, которые снимает ПОЛЬЗОВАТЕЛЬ, я читаю нормально (вопрос был в пути к файлу). Вывод: красивый арт/визуал — за пользователем (+гайд `DOM_BUILD_GUIDE.md`), за мной — код/гейплей/аудио/координаты. Greybox-«красоту» не строю (отвергнуто ранее).

### МОДУЛЬ «Хаб + Доска заявок» — КОД ГОТОВ, СБОРКА/L_Hub/смоук — ОЖИДАЮТ ЗАКРЫТИЯ РЕДАКТОРА
Спина мета-петли из `CONCEPT.md` (ХАБ → заявка с доски → выезд → объект → «Акт» → возврат на базу):
- **`UDispatchSubsystem`** (GameInstanceSubsystem, переживает ServerTravel как леджер): помнит `HomeHubMap` (куда вернуться) + активную заявку (`ActiveCallId/Title`). `BeginJob()` / `ClearActiveCall()`.
- **`ACallBoard`** (`World/ACallBoard.{h,cpp}`) — «стена с вызовами». Компоненты: Zone (box) + Board (cube-меш, заменит арт) + Label. `TArray<FCallListing>` (Id/Title/Brief/ObjectMap/bAvailable). Дефолт: Дом доступен (`/Game/Avariika/Maps/L_Dom`), Завод/Больница «скоро». `AcceptBy()` (сервер): пишет в DispatchSubsystem хаб+заявку → диспетчер брифинг → `ServerTravel(ObjectMap?listen)` через таймер `TravelDelay`. `HubMapOverride` (явный путь хаба, без PIE-префикса).
- **`AAvaryoCharacter`**: `FindFocusedCallBoard()` (свип из камеры + оверлап-фолбэк, как у предметов), маршрут E → `Board->AcceptBy(this)`, `FocusedCallBoard` + геттер для HUD.
- **`ARunState`**: `bHubMode` = есть ли `ACallBoard` на карте. В хабе — НЕ забег: BeginPlay не ломает объекты и `return`, Tick `return` (иначе 0 задач = мгновенная «победа»!), своё приветствие `HubWelcome`. `AnnounceCallAccepted()` (пул `CallBriefing`). `RequestRestart()`: если `DispatchSubsystem->HasHomeHub()` → `ServerTravel(Hub?listen)` (возврат на базу после «Акта»), иначе старый `?restart`.
- **HUD**: подсказка «[E] Взять заявку: <Title>» когда смотришь на доску.
- **`Scripts/build_hub.py`** (идемпотентный, headless): создаёт `L_Hub` — функциональный каркас (база 16×11м + гаражный бокс, MOVABLE-свет, PlayerStart, `ACallBoard` с `HubMapOverride`, фургон Hilux/Gazelle, верстак, GameMode Avaryo). Это РАБОЧИЙ каркас, не финальный арт — красоту наводит пользователь/Fab.

**СДЕЛАНО (3 чанка, всё в main, билды зелёные, headless-смоук L_Hub чистый):**
- Чанк 1 (8bdcb42): модуль выше. Грабли: `build_hub.py` сперва заспавнил хаб в Lvl_FirstPerson (new_level в живом редакторе не переключает мир, тем более если ассет уже есть). Рабочий уровень вычищен (`clean_hub_from_firstperson.py` + git checkout .umap и external actors). Добавлен **ГАРД**: скрипт проверяет `get_editor_world()` содержит "L_Hub", иначе abort — загрязнить рабочий уровень больше нельзя. **L_Hub создаётся headless НЕЛЬЗЯ** (`-run=pythonscript` крашит на CDO `ARepairable` FObjectFinder<UNiagaraSystem> — Niagara не инициализирован в commandlet); строим через ЖИВОЙ редактор + плагин (`editor.run_python_script`).
- Чанк 2 (c197e6e): **аудио+жизнь хаба** — `ACallBoard` дефолтные звуки (RadioComm бип + EngineStart «поехали»), `TravelDelay`=2.2с; `ARunState` фоновая болтовня на базе (`HubIdle`, таймер 35с, гаснет при приёме заявки) + брифинг прибытия (`ArrivedAtCall`); `build_hub.py` фикс пути фургона (`/Game/Avariika/Meshes/SM_Gazelle.SM_Gazelle`) + 3D-эмбиент гула лампы.
- Чанк 3: **ящик инструмента** (`AToolCase`) — шаг петли «собери инструмент в ящик». E у ящика → `UDispatchSubsystem::SetKitLoaded` + звук + реплика диспетчера (`KitLoaded`); сбрасывается на новую смену. Подсказка HUD «[E] Собрать ящик инструмента». В `L_Hub` (23 актёра: доска, фургон, ящик, гараж, свет, эмбиент).
- Чанк 4 (звук+механики, по очереди пользователя «звук→механики→хаб»): **саундскейп** — `ARunState::TickAmbient` теперь РЕАЛЬНО играет скрипы/стуки (раньше был только noise-эвент+текст): пулы `CreakPool` (creak_struct+door_impact) и `DreadPool` (jumpscare) грузятся в BeginPlay через `UObjectLibrary` из `/Game/Audio/Lib/*`, играются 3D у всех через `MulticastAmbientSound`, dread-звук чаще при несломанных задачах. **Механики:** (а) ящик теперь *значимый* — взял заявку без собранного ящика → диспетчер ребёт (`NoKitWarning`, `AnnounceCallAccepted(title, bKitLoaded)`, `ACallBoard` читает `IsKitLoaded`); (б) **нетерпение диспетчера** — `TickImpatience` язвит на 4/7/10 минутах забега (`Impatience` пул). Билд зелёный, смоук Lvl_FirstPerson чист. ⚠️ для пакеджа добавить `/Game/Audio/Lib` в Additional Asset Directories to Cook (UObjectLibrary грузит динамически — в редакторе/PIE ок, в куке без этого пусто). Хазард-VFX (газ/искры/взрыв) уже систематизированы на `ARepairable`; средовой Niagara (пыль/туман) — глазами пользователя.

**ОСТАЛОСЬ — за пользователем в PIE:** проверить выезд между картами (ServerTravel headless не тестируется; в PIE возможен префикс `UEDPIE_` — потому `HubMapOverride` задан явным путём) и общий цикл хаб→заявка→объект→«Акт»→возврат. **L_Hub — серый функциональный каркас:** дом-командный-центр (арт) наводит пользователь/Fab-пак, я держу геймплей/свет/структуру.
**Грабли (важно):** редактор после запуска отвечает на ping за ~10с, но мир ещё грузится — перед `build_hub.py` ЖДАТЬ, пока `get_info` вернёт стабильную карту (иначе new_level в транзитном старте). Пре-существующий битый ассет `Loot_Anim_Set/.../Paired_Loot_FlipOverCorpse_GrabItem_Vic2.uasset` (Invalid PACKAGE_FILE_TAG) — в логе Error, не наш, к крашу не ведёт.

## СЕССИЯ 2026-06-13 (вечер, продолжение) — реалистичные модели предметов + фикс материалов

Пользователь: «грузим модели, ставим реалистичные модели». Сделано (всё в `main`):

**1. Импорт 4 новых предметов** (`import_items_realistic.py`, редактор открыт, Claudius :8080):
`SM_Radio`, `SM_Battery`, `SM_FireExtinguisher`, `SM_FirstAidKit` — legacy FBX (без Interchange-битой нормали), полный PBR из meshy-карт. Размеры: Radio 20см, Battery 18см, огнетушитель 55см, аптечка 28см. Привязка: Radio → актор `Radio`; остальные → CDO BP + **per-instance override на ВСЕ размещённые инстансы** (Battery×3, FireExt×1, FirstAid×2). Подбор аттачит тот же актор → в руках меш тот же.
**2. ⚠️ Фикс materials без metallic:** meshy-карты `_metallic.png` ЗАВЫШЕНЫ — крашеный металл/пластик становился белым зеркалом (красный огнетушитель рендерился белым). Пересобрал `M_SM_*` как base color + normal + roughness, **metallic=0** (`rebuild_item_mats.py`). Проверено в PIE: огнетушитель красный, рация тёмная, батарея светлая. → правило в [[ue-python-scripting-gotchas]].
**3. Скан мешей (`scan_item_meshes.py`) нашёл откат:** `Repairable_GasPipe/Generator/Breaker` и `Tester` снова стали кубами (CDO-меш не сериализовался; вероятно revert газели ee8ea7d откатил и их override в external-акторах). Переназначил per-instance (`assign_repairables.py`) — теперь в OFPA-пакетах, устойчиво.
**4. Тряска паники ВЫКЛ** (флаг `bPanicCameraEffects=false` в AvaryoCharacter, по просьбе на время тестов; true — вернуть).

**Остались ЗАГЛУШКИ (нужны модели от пользователя):** Cigarettes (Cube), Fuse (Cylinder), Thermos (Cylinder), TrapKit (Cube), LightKit (Cube), MotionSensor (Cone). Газель — всё ещё серая (см. ниже).

### Доимпорт (вечер 2): Fuse + Cigarettes + Hilux + аудит-воркфлоу
Пользователь догрузил Fuse, сигарету, **Toyota Hilux** (вместо газели) и сказал «современный формат, не советский». Сделано (всё в `main`):
- **Fuse** — meshy «Modern cylindrical», PBR без metallic, на BP_Fuse + 2 инстанса (~12см). ✅
- **Cigarettes** — скачанный `cig.fbx` (пачка + 20 сигарет = 70 слотов). Первый импорт вывалил ~120 ассетов в корень `Meshes/`; переимпортировал в подпапку `Meshes/Cigarettes/`, корень вычистил. ✅
- **Hilux** — скачанный 38МБ мульти-материал (61 секция, 982k тр, Nanite). В подпапку `Meshes/Hilux/` (120 ассетов), реальные габариты 208×533×189см, на актор `Gazelle_Mesh` у выхода (loc -300,0,210, yaw90, scale~1.0). Проверен в PIE — белый текстурированный пикап. ✅
- ⚠️ **Грабли:** строгая чистка корня (keep SM_*/M_SM_*/Image_0) случайно снесла `M_Toilet` (материал унитаза без префикса M_SM_) — восстановил из git, переназначил на SM_Toilet. На будущее: keep-фильтр должен учитывать не-SM_ имена.
- **Воркфлоу-аудит (18 агентов)** → `MESHY_PROMPTS.md`: статус-таблица 22 объектов + современный арт-бриф + 14 готовых англо-промтов. Промты «убрать soviet», материалы матовые (избегать metallic-пересвета).

**ИТОГ по моделям — заглушки остались только у 4 размещённых:** Thermos, TrapKit, LightKit, MotionSensor (+ рантайм-спавн: BioBlob, Floodlight, Trap; FoamPatch — делать декалью). Промты для них в `MESHY_PROMPTS.md`. Остальные 12 + Hilux — с мешами.

### Вечер 2 (доп.): дневной свет + стекло Hilux + размеры
- **Стартовый мир СВЕТЛЫЙ (день):** DirectionalLight 0.6→9 (белое солнце, use_temperature off, pitch −58), SkyLight 0.15→2 (recapture), PPV_Night → дневная histogram-авто-экспозиция (bias 0.5). **Откат в ночь:** DirLight 0.6 / SkyLight 0.15 / убрать override экспозиции. Скрипты `brighten_and_glass.py`, `daylight_tune.py`.
- **Стёкла Hilux:** окна (слоты 38/46/54 `glass/glass_001/glass_002`) были непрозрачны снаружи (односторонний). Сделал `M_HiluxGlass` (Translucent, **two_sided**, opacity 0.18, rough 0.06) — видно сквозь кабину с обеих сторон. Фары/поворотники не трогал.
- **Реальные размеры заглушек (= целевые для импорта):** Thermos 25, TrapKit 18, LightKit 28, MotionSensor 16 см (`restore_and_sizes.py`).
- ⚠️ Грабли: `StaticMeshComponent.get_static_mesh()` нет в 5.7 Python → `get_editor_property('static_mesh')`.

### Вечер 2 (доп.2): импорт 4 предметов + аудит размеров
Пользователь догрузил Thermos/LightKit/MotionSensor/TrapKit (meshy «Modern_*», по моим промтам). Импортировал пакетно (`import_meshy_batch.py`, без metallic), назначил на акторы-заглушки в реальном размере: Thermos 25, LightKit 28, TrapKit 18, MotionSensor 16 см. **Заглушек среди размещённых предметов больше НЕТ.**
**Аудит размеров (`audit_sizes.py`)** всех моделей: все реалистичны, КРОМЕ сварочника — был 14см и с НЕРАВНОМЕРНЫМ масштабом (искажён); починил на равномерный ~45см (`fix_welder_size.py`). Breaker 160 / Generator 200 — крупные, но в реальных пределах для промышленных (оставил).
**Без моделей остались только рантайм-спавн объекты:** AFloodlight (развёрнутый прожектор), ATrap (развёрнутая растяжка), BioBlob (метаемый комок); FoamPatch — декаль. Промты в `MESHY_PROMPTS.md`.

### Вечер 3: ⚠️ КРИТ — модели были ОДНОЦВЕТНЫЕ из-за моего материала
Пользователь: «в игре хрень, в meshy норм». Симптом — модели **одноцветные** (огнетушитель весь красный, аптечка одноцветная), а должны быть многоцветными. **Корень:** мой самодельный материал (`M_SM_*`, base+normal+roughness) рендерил меш одним тоном. A/B-тест (мой материал vs нативный импорт meshy на одном меше под одним светом) — нативный даёт ВЕРНЫЙ многоцвет (красный+чёрный огнетушитель, бело-красная аптечка), мой — серо-тускло.
**ПРАВИЛО НА БУДУЩЕЕ:** meshy-FBX импортировать с `import_materials=True, import_textures=True` (родной материал meshy = base+normal, UV 1:1) в ОТДЕЛЬНУЮ подпапку `Meshes/<Name>/` (каждый создаёт Image_0/Material_001 — в корне коллизия). НЕ строить материал руками. Унитаз так и был (M_Toilet+Image_0) — потому и не глючил.
Переимпортировал все 14 предметов нативно, переназначил на акторы/BP CDO, удалил 64 осиротевших root-ассета. Проверено в PIE — многоцветно.
**Рантайм-модели (Floodlight/Trap/BioBlob):** импортнул нативно + C++-конструкторы (AFloodlight/ATrap/ABioProjectile/ABioPickup) переведены с движковых примитивов на эти меши (FObjectFinder на `/Game/Avariika/Meshes/SM_*/SM_*` + равномерный scale). Собрано (Result: Succeeded), spawn-check подтвердил меши.
**Сигарета:** модель скачанная (пачка+20 сигарет = не то), нужна ПРОСТАЯ пачка — промт v2 в `MESHY_PROMPTS.md`, ждём от пользователя.
⚠️ Грабли: спавн игрока в PIE прыгает между y=0 и y=-134 (видимо 2 PlayerStart) → кадрирование скринов плавало. `StaticMeshComponent.get_editor_property('static_mesh')` (не `get_static_mesh()`).

## СЕССИЯ 2026-06-13 (день, автономно, доступ дал пользователь) — аудит + импорт 4 моделей + PIE

Пользователь уехал, попросил прогнать механики, импортировать добавленные модели, проверить в PIE. Сделано (всё в `main`):

**1. Аудит 24 ночных механик** (воркфлоу из 20 агентов, адверсариал-проверка). Реальных геймплейных багов НЕ найдено — код ночной пачки чистый (серверные guard'ы, репликация, масштаб по DeltaTime везде). Применён безопасный субсет находок:
- `VitalsComponent`: добавлен `IsVitalAuthority()` + guard на 9 серверных мутаторов (ApplyDamage/Heal/AddPanic/ReducePanic/RestoreStamina/AddSmell/Relieve|DrainBladder/StartSmoking) — кооп-харднинг от клиентского рассинхрона/чита. **SetSprinting НЕ трогал** (там намеренное клиентское предсказание), DebugSetVital тоже (гейтится на call-site). Паттерн через овнера — юнит-тесты без овнера проходят.
- HUD: клампы полосок crew-монитора (HP/паника) и туалета в [0,100] (как у главных).
- `AvaryoCharacter`: сброс аккумуляторов шума (шаги/распыление/пена) при остановке — ровная каденция.
- `ARepairable::GetCurrentGasRadius()` геттер.
- Автотесты: новый `Avariika.GasSpread` (зажим роста газа ×2 прямым Tick) + проверки RestoreStamina/ReducePanic. **Сборка зелёная, автотесты 6/6, headless-смоук (`-game`) чистый: карта грузится, "Bringing up level for play", 0 Fatal/Ensure/Error.**

**2. Импорт 4 моделей** (редактор поднимал сам через `UnrealEditor.exe`, Claudius на :8080; `import_models_safe.py` дополнен целями `actor:`/`spawn:`/`bp:`):
- `SM_GasPipe` → `Repairable_GasPipe` (~220 см, мат `M_SM_GasPipe`, 10k тр). FBX импортнулся крошечным (≈12 см) → авто-скейл ×18.3.
- `SM_Generator` → `Repairable_Generator` (~200 см, `M_SM_Generator`, 20k тр, Nanite).
- `SM_WeldingMachine` → BP_WeldingMachine + размещённый пикап `WeldingMachine` (`M_SM_WeldingMachine`, 5.4k тр). ⚠️ **Грабли:** set на нативном компоненте CDO НЕ сериализуется — пришлось ставить per-instance override на размещённый актор (`fix_welder.py`). Учесть для всех BP-предметов с нативным MeshComponent.
- `SM_Gazelle` → декор `Gazelle_Mesh` у выхода (поворот починен на плоский). ⚠️ **Без текстуры** (в `RawAssets/SM_Gazelle/` нет PNG → WorldGridMaterial) + **вырожденные полигоны** в под-частях Nissan-FBX (часть секций пропущена, 121k тр). Нужна чистая модель/текстура от пользователя.

**3. PIE-проверка:** PIE стартует чисто («Сервер вошёл в систему»), дев-читы (`AvIncident/AvCheapGear/AvFinish/ShowFlag`) выполняются без ошибок и краша (в логе ни Error/Ensure/«not recognized»). HUD рисуется корректно (шкалы вкл. новый «Шум», слоты, задачи, миникарта, плашка диспетчера), фиолетового нет.

⚠️ **Грабли инструментов (важно для будущего):** редактор, запущенный из неинтерактивной сессии, рендерит кадры для скриншота **только пока активен PIE** (его игровой вьюпорт качает рендер); редакторный вьюпорт в фоне хайрес-скриншот не дорисовывает. Ночной сеттинг + нет управления камерой PIE → чистые освещённые close-up моделей headless не снять. **Аккуратный визуальный осмотр моделей (текстуры/пропорции) и раскладку — пользователю в своём редакторе.**

**Открытые вопросы пользователю:** (а) `SM_Gazelle` — дай чистую модель пикапа с текстурой (текущая серая и кривая); (б) Repairable_Breaker/GasPipe/Generator скучены у спавна (x=200, z~290) — это тест-раскладка или развести по карте? (в) глянь модели вблизи в PIE.

## РЕШЕНИЕ 2026-06-13: greybox больницы ОТМЕНЁН → mechanics-first + реальная карта на Fab-ассетах

Greybox всего здания (388 акторов, `build_hospital.py`) собрали, но пользователь забраковал (как и v1): кубовый блокаут не похож на «больницу из Resident Evil» — он и не может, это блокаут, а не арт. **Вывод (важно на будущее): кодом из кубов AAA-окружение не сделать.** Путь к качеству — **готовый ассет-пак окружения с Fab** (пользователь добавляет из своего Epic-аккаунта, Claude строит на его кусках уровень + геймплей).

**Решение пользователя:** сначала ДОВОДИМ МЕХАНИКИ на рабочей `Lvl_FirstPerson`, красивую карту — позже на Fab-ассетах. **`L_Hospital` УДАЛЁН** (в git-истории до ea365e1; `build_hospital.py` оставлен как референс координат).

### Правки механик по фидбеку (в main, собрано, автотесты 6/6):
- **Фонарь**: «бесполезный» — артефакт дневного greybox (на солнце смывается; настройки норм 8000/30м/32°). Добавил `UFlashlightComponent::BeamIntensity` (C++, дефолт 14000, EditAnywhere) — перекрывает интенсивность из BP во всех путях. Без хрупкой правки BP.
- **Паника «не уходит на свету» → СДЕЛАНО (свет убирает панику в общем виде).** Пользователь подтвердил «да свет должен убирать панику». В VitalsComponent добавлен троттл-сканер (0.3с) по `UPointLightComponent` (ловит и точечные, и spot): если рядом видимый яркий (≥`CalmLightMinIntensity`=1000) не-красный источник в радиусе затухания — паника спадает (как при включённом фонаре). Ловит фонарь, прожектор и любой свет будущей карты; красные тревожные лампы отсечены по цвету. `IsLitByNearbyLight()`. Собрано, 6/6.
- **Открытый дизайн-вопрос:** «свет шире успокаивает» (починил электрику → здание освещено → паника спадает, фазы §8 ТЗ) — фича под будущую карту с переключаемым светом.

### ⚠️ ГРАБЛИ инструментов:
- **`level.load_level` WP-карты ВНУТРИ открытого редактора (неинтерактивная сессия) ВЕШАЕТ редактор** (деадлок, кадр замирает на минуты). Смена карты — перезапуск редактора свежим (откроет EditorStartupMap чисто) ИЛИ headless. Зависший убил (несохранённые правки BP потерялись → фонарь сделал в C++).
- **ГАЗель — ИТОГ:** `.glb` лежит в `RawAssets/SM_Gazelle/`. **Текстуры+материалы импортируются идеально** через Interchange БЕЗ destination_name (38 материалов + 16 текстур). НО модель — **111 узлов**, и в один меш скриптовым Interchange НЕ собирается: `mesh_pipeline.combine_static_meshes=True` + `common_meshes_properties.bake_meshes=True` (передавать пайплайн как СОЗДАННЫЙ ассет в `ImportAssetParameters.override_pipelines` через `SoftObjectPath`, не объектом!) дают меш с НУЛЕВЫМИ габаритами (трансформы не запеклись). **Решение цветного грузовика: ручной драг `.glb` в Content Browser (интерактивный Interchange собирает многоузловой glTF), либо scene-import, либо одномешевая модель.** Сейчас стоит видимый СЕРЫЙ FBX-грузовик (`SM_Gazelle`, реальная геометрия, scale ~96). Скрипты: `gazelle_combine2.py` (combine+bake, референс), `gazelle_restore.py` (вернуть серый).

---

## (архив, УДАЛЁН) БОЛЬНИЦА v2 — greybox Этапы 1-2

Пользователь дал отмашку строить больницу по `TZ_Hospital_Map_UE5_v2.md` и просил продолжать автономно. Сделан greybox **всего здания**: Этап 1 (вертикальный срез — атриум+двор+зап.подвал) + Этап 2 (вост.крыло/часовня/операционная/солярий, этажи 2-3, остальной подвал/морг, крыша). **388 акторов.** Так как билд скриптовый/идемпотентный — фидбек пользователя по пропорциям переприменяется одним прогоном, поэтому Этап 2 строить блайнд было низко-рисково.

- **Уровень:** `/Game/Hospital/L_Hospital` (НЕ-WP, всё в .umap), 191 актор. Скрипт `Scripts/build_hospital.py` — идемпотентный (каждый прогон пересоздаёт уровень свежим `new_level` → можно править и перезапускать), куб-greybox по координатам ТЗ (UU, угол здания в 0,0,0). Хелперы `box/wall_x/wall_y/slab/stair_y/column`.
- **Построено:** центральный корпус (оболочка X0-5600/Y0-1400, 3 перекрытия с пустотой атриума), **АТРИУМ** (3 света, парадная лестница марш→площадка→2 марша, балконы-галереи 2/3 эт. + балюстрады, 6 колонн, открытый световой фонарь, стойка регистратуры, гл.вход), наземная плита портика; **ДВОР** (ограда+ворота, фонтан, сторожка с ключом ПОДВАЛ, траншея); **ЗАП.КРЫЛО 1 эт.** (кухня+газовый стояк, столовая, ЛК-З с лестницей в подвал); **ЗАП.ПОДВАЛ** (газовый узел/GasPipe, котельная+канистра, генераторная+приямок, ГРЩ с гл.Breaker за стеклом, мастерская). PlayerStart + ГАЗель под портиком, ExitZone, 3 Repairable (Generator/STARTER, Breaker/CURSOR, GasPipe/VALVE).
- **Статус:** PIE грузит чисто (диспетчер «объектов 3», RunState нашёл 3 цели, 0 Fatal/Ensure/Error). **В `main` (46ffa6d).**
- **Свет ПОЧИНЕН:** сцена рендерится, атриум читается (колоннада + солнечные пятна из фонаря). ⚠️ **ГЛАВНАЯ ГРАБЛЯ:** `unreal.Rotator(a,b,c)` позиционно — это **(roll, pitch, yaw)**, НЕ (pitch,yaw,roll)! Из-за этого солнце было под горизонтом (чёрный экран), а PlayerStart/ГАЗель криво повёрнуты. Чинить — keyword-аргументами: `unreal.Rotator(pitch=..., yaw=..., roll=...)`. Экспозиция: PostProcessVolume с AEM_HISTOGRAM, override-флаги БЕЗ `b_` префикса (`override_auto_exposure_*`). Скриншоты сняты через активный PIE (см. грабли скриншотов в дневной сессии выше).
- **Скриншоты отправлены пользователю** (атриум-колоннада, аэро-вид, вид полного здания сверху).

### СЛЕДУЮЩИЙ ШАГ: ждём прогон/фидбек пользователя по greybox всего здания (массинг атриума, петли, маршрут §6, читаемость крыльев/часовни/операционной). По «ок» — Этапы 3-5: модульный кит (замена кубов), пропсы §7, двери/замки §5 + BP_QuestManager (фазы света), ночь/дождь/экспозиция §8 (сейчас дневной свет для осмотра), цвет крыльев (зап.зелёный/вост.охра §1.2). Эти этапы — после фидбека/ассетов.

**Отклонения от ТЗ (для §9.7):** (1) пустота атриума упрощена до X2350-3250/Y250-1150, лестница — greybox-ступени. (2) световой фонарь — открытый проём + рамка (стекло — Этап 3). (3) цвет крыльев (зап.зелёный/вост.охра) не делал — greybox серый. (4) экспозиция/ночь не настроены (Этап 5). Связность и ключевые пространства §3 соблюдены. Грабли: имя override-свойства экспозиции в FPostProcessSettings подобрать не удалось (`b_override_auto_exposure_min_brightness` не найдено в 5.7) — для Этапа 5 искать правильное.


## НОЧНАЯ ОЧЕРЕДЬ 2026-06-13 (авторежим, редактор закрыт, модели — завтра пачкой)

Пользователь ушёл спать, просил сделать максимум механик. Делаю по одной: код → сборка → автотесты 5/5 → коммit+пуш. Модели НЕ трогаю (FBX-импорт требует открытого редактора — завтра пачкой через `import_models_safe.py`; `SM_GasPipe` готов к импорту).

1. [x] Диспетчер реагирует на пену/толчок/спотыкание (пулы SlipFoam/Shoved/Tripped + Notify*).
2. [x] «Акт»: счётчики толкнул/споткнулся/скользил + звания «Гроза коллектива»/«Спотыкач смены» (4-я строка).
3. [x] Групповая паника (паникёр накручивает соседей: FearContagionRadius/PerSecond в VitalsComponent).
4. [x] Спотыкание роняет активный лёгкий предмет (TripFumbleChance).
5. [x] Толчок/взрыв выбивает тяжёлый предмет (FumbleHeavy из ServerShove и ExplodeGas).
6. [x] Перегрузка сети — TickOverload в ARunState (выбивает починенный щиток, пересчёт RepairedCount, без софтлока, bElectricalOverload).
7. [x] Бросок предмета — клавиша T, ReleaseHeldItem(bThrown), баллистика по прицелу.
8. [x] Паника трясёт мини-игры (PanicHardenScale: курсор быстрее+зона уже, окно стартера уже).

**ВСЕ 8 СДЕЛАНЫ, собраны, автотесты 5/5, в main.** Управление: Q — толчок, T — бросок (README обновлён).

**Доп. механики сверх плана (тоже собраны, 5/5, в main):**
9. [x] Кофе/термос — `EItemEffect::Drink` (восстанавливает выносливость + чуть успокаивает), термос на платформе спавна (`place_thermos.py`), `VitalsComponent::RestoreStamina`.
10. [x] Виньетка паники — края экрана пульсируют тёмно-красным как сердцебиение при панике (AvaryoHUD, чем паничнее — тем чаще).
11. [x] Пена тушит газ — струя огнетушителя в облако утечки = `SuppressGas` (≈2 с поджечь нельзя).
12. [x] Удар роняет несомого — `FumbleHeavy` теперь и выпускает раненого, которого тащишь (толчок/взрыв).

Итого первый батч: 12 механик. Все чисто собираются, автотесты 5/5, запушены.

### БАТЧ 2 (ещё 12 механик, тоже собраны 5/5, в main)
13. Пена: попал на неё — мелкий испуг (паника) + на быстром скольжении можно навернуться (стан).
14. Споткнуться о ЛЕЖАЩЕГО раненого товарища (влетел на бегу — упал).
15. Усталость (низкая выносливость) резко повышает шанс споткнуться.
16. Бросок (T) при панике уходит с разбросом (трясущиеся руки).
17. Кашель: провонявшись газом/химией, монтёр кашляет — шум + капля паники (задел под монстра).
18. Фумбл тяжёлого может уронить сварочник себе на ногу (урон + «ой»).
19. Фоновая жуть: ARunState изредка «скрипит» (тихий шум + реплика «показалось?»).
20. Газовое облако РАСТЁТ, пока не перекрыли (до ×2 радиуса, взрыв больше); перекрыл — опало.
21. Тряска прицела при панике (дрожащие руки, у владельца).
22. HUD «Шум» — насколько ты сейчас слышен (бег/распыление громко, присед тихо).
23. Адреналин: на низком HP (не ранен) рывок скорости ценой растущей паники.
24. Диспетчер язвит на бросок и кофе-брейк; удар (толчок/взрыв) роняет и несомого раненого.

**Итого за сессию: 24 механики.** Все в `main`, автотесты 5/5, НЕ проверены в PIE (чек-листы в TESTING.md). Параметры всех — EditAnywhere (легко крутить балант).

Сделанное за прошлый заход (в main): скользкая пена, толчок (Q), споткнуться. Лечение/подъём раненого уже было.

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
| `SM_Gazelle` | SM_ServiceTruck | `American pickup truck with white service utility bed, toolboxes on sides, ladder rack, amber rotating beacon, worn municipal maintenance livery, mud and rust, low-poly game vehicle, PBR, game-ready, single object, centered, no ground plane, no scene` |
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

## МОДУЛЬ «Споткнуться» (§18 хаос/хоррор) — КОД ГОТОВ 2026-06-13

### Статус: собран, автотесты 5/5, смоук чистый. НЕ проверен пользователем в PIE.

Бег вслепую опасен — можно споткнуться (хоррор-тензия + комедия):
- `AAvaryoCharacter::UpdateTrip(dt)` (сервер, в Tick): только на бегу (`IsSprinting`) по земле (`IsMovingOnGround`, скорость >300) и не раненым. Шанс/сек = `TripChancePerSecond`(0.03) × (фонарь выключен ? `TripDarkMultiplier`=2.5 : 1) × (1 + паника/100 × `TripPanicMultiplier`=1.5). `FMath::FRand()` ролл.
- Споткнулся → `bStumbling=true` (реплицируется) на `TripRecoverTime`=0.8 с, `MakeNoise(0.7)`. `RefreshMoveSpeed` при `bStumbling` зажимает скорость до `TripSlowSpeed`=150 (ранение/ползание перекрывает). Т.к. RefreshMoveSpeed крутится и на владельце-клиенте, а `bStumbling` реплицируется — проседание без рывка-коррекции.
- Стимул держать фонарь включённым (но свет потом будет приманкой для монстра — будущая тензия). Все параметры EditAnywhere (`Avaryo|Trip`).

### Чек-лист теста — TESTING.md, раздел «НОВОЕ — Споткнуться» (2026-06-13).

## МОДУЛЬ «Толчок/пинок» (§18 кооп-хаос) — КОД ГОТОВ 2026-06-13

### Статус: собран, автотесты 5/5, смоук чистый. НЕ проверен пользователем в PIE.

Можно толкнуть товарища — связка со всем хаосом (газ/пена/край/растяжка):
- **Клавиша Q** (`BindKey` в `SetupPlayerInputComponent`) → `Shove()` → `ServerShove()` (Server/Reliable, как остальной ввод).
- **Сервер**: перезарядка `ShoveCooldownTime`=1.2 с; нельзя пока сам ранен (`VitalsComponent->IsWounded()`) или залочен в мини-игре (`bInteractionLocked`). Ищет ближайшего `AAvaryoCharacter` в конусе ~70° перед собой в радиусе `ShoveRange`=220 см; `LaunchCharacter(FlatDir*ShoveForce + Up*ShoveUp, true, true)` (750/280), толкнутому `AddPanic(ShovePanic=10)`. `MakeNoise(0.5)` при каждом толчке (возня — задел под монстра). Все параметры EditAnywhere (`Avaryo|Shove`).
- `LaunchCharacter` на сервере реплицирует скорость владельцу — отлёт виден у всех. Без ассетов и правки уровня. Дискаверабилити: Q добавлен в README (Управление); отдельной HUD-подсказки нет (глобальное действие).

### Чек-лист теста — TESTING.md, раздел «НОВОЕ — Толчок / пинок» (2026-06-13).

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
`Scripts/import_tester.py`: отключаем Interchange-FBX (`Interchange.FeatureFlags.Import.FBX 0`) → работает legacy-импортёр, который уважает `import_materials=False/import_textures=False` → **меш без авто-текстур и без проблемной meshy-нормали** (раньше mesh-only ломался в 0x0x0 именно из-за Interchange). Материал собираем из base color сами. Тестер так импортнулся: **0 орфанов** (против кучи Image_*/normal/Material_001 при полном импорте).

**Канонный импортёр: `Scripts/import_models_safe.py`** — `MAPPING` (папка → метка актора/None + max-dim), импортирует все модели из RawAssets, у которых есть .fbx и которых ещё нет в Meshes; legacy-путь, чистый материал, Nanite >20k, чистка орфанов, назначение на актор уровня по метке.
- ⚠️ **FBX-импорт ТОЛЬКО в открытом редакторе** (через Claudius `editor.run_python_script`). Headless падает на Slate-assert `CurrentApplication.IsValid()` — проверено, даже legacy без Interchange. Спавн/правка/сейв акторов headless работают (`from_class`), а вот импорт FBX — нет.
- **Стратегия:** C++-механики делаю с ЗАКРЫТым редактором (сборка), модели импортирую пачкой в открытом. Не дёргать редактор ради одной модели.

### Модели — очередь импорта (обновлено 2026-06-13 день)
**Импортированы и в `main`:** Toilet, Breaker, Tester (ранее) + GasPipe, Generator, WeldingMachine, Gazelle (эта сессия). Gazelle — без текстуры/кривой FBX, ждёт замены.
**Ждут генерации (нет .fbx):** FireExtinguisher, FirstAidKit, Battery, Cigarettes, Radio, Fuse. Импорт — `import_models_safe.py` в открытом редакторе (MAPPING уже содержит цели `actor:`/`spawn:`/`bp:` для всех).

## СЕССИЯ 2026-06-13 (вечер 3) — ассеты `общее`, экономика/магазин, пачка механик, само-ревью

Пользователь уехал, дал **полную автономию** («работаешь один, скажешь что потестить»). Тестить сегодня не будет; кооп тестить не с кем (второй ноут не тянет, VM не вариант — нет GPU). Всё в `main`.

**Ассеты:** канистра — полоска заправки (AutoFill). Импортирован пак **AbandonedHospital** (331 ассет). Из `RawAssets/общее` (21 пак, каталогизированы воркфлоу) запушены: **EasyOptionsMenu, Construction_VOL2 (инструменты), FootstepSystem+ResourcePack**. Локально (не в git, ждут git-стратегии из-за лимита **~2ГБ/пуш** GitHub): **Construction_Pit, Decal_Forge, AmericanDrive**. Concrete/DeadBodies отложены. Карту НЕ строим (ждём остальные ассеты юзера). См. память [[obshchee-asset-catalog]], [[asset-import-method]].

**Экономика/магазин (новое, спека `SPEC_Shop_Progression.md`):**
- `UCompanyLedgerSubsystem` теперь **пишется на диск** (`UAvariikaSaveGame`) — прогресс не теряется при выходе. `TrySpend/AddBalance/ResetCompany`.
- **Квоты диспетчера** (game-over): эскалация ×1.6/провал→ресет, реплика на «Акт», `AvQuota <сумма>` (дормантна по умолчанию).
- **Одиночная петля прогрессии:** `BuyUpgrade`/`GetEquipmentLevel`; апгрейд инструмента → `RepairerToolQuality` → мини-игра легче (по тегу этапа/типу), персистит. Команды `AvUpgrade <Welder|Tester|Flashlight|Extinguisher|Radio>`, экран `AvShop` (модальный Canvas-оверлей). ⬜ уровни не реплицируются клиенту (кооп) — поправить позже.

**Пачка механик (solo, on by default, тюнинг в EditAnywhere):** 4 звания «Акта» (взрыв/замыкание/метание/кофе), кофе-диуретик, облегчение при починке/туалете, икота при полном пузыре, отдышка после спринта, споткнуться о брошенный предмет, качество инструмента во ВСЕХ мини-играх, испуг при севшей батарее.

**⚠️ Внёс и исправил баг:** продублировал страх темноты и заразную панику (уже были в `VitalsComponent`, единая `PanicDelta`) → двойной счёт; откатил дубли из ARunState. Фикс 1.3: раненый не получает прирост от темноты.

**Само-ревью (adversarial-воркфлоу):** критических нет; исправил **рассинхрон HUD↔сервер** зон мини-игр (рисовались сырыми, сервер ужимал по панике/качеству) → единые геттеры `GetEffectiveGreenHalf`/`GetEffectiveStarterWindow` (WYSIWYG) + пропущенный множитель щитка + утечка качества в этап «руками».

**Доки:** `KONTSEPT_Avariika.md` (питч), `MECHANICS_PLAN.md`, `TEST_CHECKLIST.md` (что тестить, с консольными командами), `SPEC_Shop_Progression.md`. Все билды `Succeeded`.

**Дальше:** (1) юзер тестит solo по `TEST_CHECKLIST.md`; (2) баланс 1.4 (кэп паники) по фидбеку; (3) UI-кнопки магазина (сейчас покупка из консоли); (4) кооп-механики (PIE-2, «напоследок»); (5) карта-слайс (ждёт ассеты).

### Продолжение сессии 3 (тест юзером + автономка)
- **Тест-проход 15/15 пройден юзером** (пометки в `TEST_CHECKLIST.md`). Баги в процессе: #8/#9 (икота/отдышка) «не работали» — оказалось, шумомер был только от скорости движения; **сделал шумомер реальным** (RegisterSelfNoise/GetSelfNoise01 — отражает MakeNoise-события). Испуг от батареи — добавил шум.
- **Паника↔предметы** (фидбек к #6): паника удлиняет применение предметов (`ItemUsePanicScale`) и замедляет AutoFill/залив бензина.
- **Метание переехало на одну кнопку G** с зарядкой по удержанию (тап=слабо, держать=сильнее ×3, полоска на HUD); T убран. ⚠️ новые клавиши требуют рестарта PIE.
- **Дев-команды для теста:** `AvGod` (бессмертие), `AvGive <предмет>`, `AvToolQ <0.25..3>`, `AvMoney`, `AvSetGear <tool> <lvl>`, `AvBattery <pct>`, `AvShop`, `AvUpgrade`, `AvQuota` + кооп `AvHost`/`AvFind`/`AvJoin`/`AvLeave`.
- **АССЕТЫ:** импортированы паки из `RawAssets/общее` + `общее2` (по каталогам-воркфлоу), затем юзер удалил обе папки + весь `RawAssets` (всё в `Content/`). Новое в Content: **Modern_Hospital_Leartes** (та самая Leartes), Madrid_Street, FoggyStreet (VFX туман/дождь/пар), LightHouse, Shoothouse, Backrooms_TheLobby, YI_Luggage, oldBenchPack, FencesVOL2, **Adventure_Pack/Characters = механик Sarah** (модель рабочего!). ⚠️ Эти + Construction_Pit/Decal/AmericanDrive (~35ГБ) лежат ЛОКАЛЬНО, не в git (лимит 2ГБ/пуш) — git-стратегия (LFS/чанки) ещё не решена.
- **КООП Фаза 0:** `UAvariikaOnlineSubsystem` (host/find/join через UE OnlineSubsystem, NULL/LAN сейчас, EOS/Steam через конфиг). Тест: 2 инстанса / PIE-2 (`AvHost` на одном, `AvFind`+`AvJoin 0` на другом).
- **Рабочий процесс:** при ОТКРЫТОМ редакторе компилю Live Coding'ом (`console.trigger_live_coding`), при смене модулей — закрытый Build.bat. См. память [[live-coding-workflow]].

**Ещё НЕ хватает (нужны ассеты юзера):** звук (полигра!), VFX-паки (взрыв/огонь/искры/пена — хуки готовы), плагин EOS/Steam (для онлайн-коопа поверх готовой сабсистемы); + решение git-стратегии и сборка слайса (Modern Hospital + Sarah).

### СЕССИЯ 2026-06-14 — импорт паков из `RawAssets/новое` + плагины (EOSCore, AudioToolkit)
Пользователь догрузил папку `RawAssets/новое` и список «что брать». Разобрал и интегрировал (собрано Build.bat при закрытом редакторе, `Result: Succeeded`, редактор перезапущен, реестр просканирован):

**Контент-паки (импорт robocopy дерева, FBX исключены `/XF *.fbx`):**
- **City MODULAR HOSPITAL v.2** (`новое/ModularHospital` — это целый проект): `Content/Hospital` → наш `Content/Hospital`, **1387 ассетов**, `/Game/Hospital/`. Карты: Demonstration, Demonstration_2, Overview. Флагман-окружение больницы (стены/лестницы/мебель/мусор).
- **Modular Haunted House** (`новое/Modular Haunted House (...)`): `PostApocalypticHouse` → `Content/PostApocalypticHouse`, **667 ассетов**, `/Game/PostApocalypticHouse/`. Карты: **LV_Horror_Light, LV_House, LV_Main**, LV_Cinematic, LV_Sunset_Light, L_Asset_Overview. Под прототип 0.1. ⚠️ Копировал только `__ExternalActors__/PostApocalypticHouse` — НЕ трогал FirstPerson/ThirdPerson/AncientRuins (иначе затёр бы OFPA нашего `Lvl_FirstPerson`).

**Код-плагины (в `Plugins/`, собраны UBT, gitignore'нуты):**
- **EOSCore v1.9.8.2 (5.7)** (eelDev) — фундамент коопа. Распакован из `новое/EOSCore/EOSCore v1.9.8.2 5.7.rar`, ~2 ГБ (со своим EOS SDK + precompiled Binaries). Модули: EOSCore, **OnlineSubsystemEOSCore** (своя реализация OnlineSubsystem!), EOSCoreWeb, EOSCoreShared. Скомпилился чисто, смонтировался (`Using EOSCore Version: 1.9.8.2`).
- **Audio Toolkit Pro 1.2 (5.7)** (UMonster) — редактор-инструмент обработки звука. ⚠️ **НЕ компилился** (VS2026 + unity-сборка: в нескольких .cpp одноимённые функции в anonymous namespace `ColorFromHex/PrimaryText/...` → при unity-слиянии C2572/C2084 + каскад Slate-ошибок). **ФИКС:** `bUseUnity = false;` в `Plugins/AudioToolkitPro/Source/AudioToolkitPro/AudioToolkitPro.Build.cs` → собралось. ⚠️ Фикс живёт ТОЛЬКО локально (Plugins/ в gitignore) — при переносе проекта повторить.

**Конфиг:**
- `avariika.uproject`: включены EOSCore (+ MarketplaceURL, чтоб не нудел «project requires update»), AudioToolkitPro (TargetAllowList Editor), OnlineSubsystemUtils.
- `DefaultEngine.ini`: `[EOSSDK] bHasProjectBinary=true` (снял варн EOSShared). DefaultPlatformService **оставлен Null** (LAN/PIE-кооп работает). Онлайн за NAT — когда пользователь заведёт продукт в Epic Dev Portal и даст ProductId/SandboxId/DeploymentId/ClientId/Secret → переключить на EOS/EOSCore + вписать креды.

**Уже было / пропущено:** `Easy Options Menu` — уже в `Content/EasyOptionsMenu` (не реимпортил).

**Доимпорт (тот же день, по отмашке «давай делаем»):**
- **OG Main Menu System** (проект YJW57 в zip) → `Content/OGMainMenu` (611 ассетов, `/Game/OGMainMenu/`) + `Content/Movies` (6.7М, фоны) + `Content/Localization` + только свои external-actors/objects (`OGMainMenu`). Карты: **MainMenu, LobbyMap**, Dungeon/Prison/Temple (демо), TransitionMap. Виджеты: **AWBP_CreateMatch / AWBP_GameMode / AWBP_GameKeys** и т.д. — меню заточено под мультиплеер-сессии, ляжет на EOSCore. ⚠️ ещё НЕ wired в игровой флоу (нужно решить: делать ли MainMenu стартовой картой + связать CreateMatch с нашими сессиями).
- **Citizens_Pack** → `Content/Citizens_Pack` (115 ассетов, `/Game/Citizens_Pack/`, карта Citizens_Pack_Map) — NPC-горожане под пациентов/персонал больницы.

**Доимпорт 2 (по просьбе):**
- **HyperDynamicWeatherSky 5.6** (проект, `/Game/Hyper/`) → `Content/Hyper` (1240 ассетов, 6.3 ГБ — целиком: weather-пресеты DA_Blizzard/Clear/Cloudy/Foggy ссылаются на ResourcePack/Locomotion, 78+11 рефов, cherry-pick сломал бы рефы). ⚠️ Система **компонентная на GameState/PlayerController** (`AC_Biome_WeatherManager_Abstract`, `AC_PlayerController_WeatherManager_Abstract`) — НЕ drop-in, требует врезки в наш фреймворк или их демо-карту. Локальный (gitignore).

**⚠️ ВАЖНОЕ ОТКРЫТИЕ — «меню» это целый фреймворк.** OG Main Menu — НЕ просто экран, а полный мульти-плеер game-framework (свои `BP_GameInstance`, `GM_Base/Game/Lobby/MainMenu`, `GS_*`, `HUD_*`, `BP_PlayerController`, CommonUI-стек `UI/CoreUI`, кастомизация/чат/квесты). Конфликтует с нашим (`BP_AvaryoGameMode`, `AAvaryoCharacter`, `ARunState`, `AvaryoHUD`). Цвета централизованы НЕ в одном дата-ассете, а в CommonUI-стилях (`CTS_*_Color` текст, `Button_Main`, `Border_BlackWithColorOutline`). Headless-перекраска через Python хрупкая (generated_class() у этих BP отдаёт None). **Вывод:** мерж двух фреймворков вслепую — высокий риск; направление выбирает пользователь (см. варианты в чате 2026-06-14). НЕ начинал wiring/перекраску до решения.

**Осталось в `новое`:** только `pos_fbx` (POS-терминал, FBX) — импорт в фазе звуков/магазина (пользователь отложил). Остальное из `новое` можно удалять.

**НЕ в `новое`, ждём от пользователя:** Hospital COMBO Prop Pack VOL 1-6, Modern Hospital Environment (229 — уже есть Leartes), Hyper Dynamic Weather & Sky.

⚠️ **Git:** Hospital (3 ГБ, BuiltData 558 МБ >100 МБ) и PostApocalypticHouse (4.8 ГБ) → gitignore'нуты (локальные). Plugins/ уже был в gitignore. Коммитятся только `.uproject`/`.ini`/`.gitignore`/доки.

### ГЛАВНОЕ МЕНЮ — своё, в нашем стиле (2026-06-14, по решению пользователя «путь 1»)
OG Main Menu забраковали как фронт (целый чужой фреймворк, headless-перекраска CommonUI капризная — нет смысла). Сделал **своё лёгкое меню на C++/Canvas** (как `AvaryoHUD`, без UMG/BP-возни), завязанное на нашу готовую кооп-подсистему:
- **`AMenuHUD`** (`UI/MenuHUD.h/.cpp`): рисует на Canvas тёмный фон + оранжевый акцент, заголовок «АВАРИЙКА», кнопки. Кнопки — нативные хитбоксы AHUD (`AddHitBox`/`NotifyHitBoxClick` + hover через `NotifyHitBoxBeginCursorOver`). Экраны Main/Browse. «Создать игру»→`HostGame()`, «Найти игру»→`FindGames()`+список найденных→`JoinGameByIndex`, «Настройки»→грузит `WBP_EasyOptionsMenuMain` (Easy Options) в вьюпорт, «Выход»→QuitGame.
- **`AMenuGameMode` + `AMenuPlayerController`** (`Game/MenuGameMode.h/.cpp`): контроллер включает курсор + `bEnableClickEvents`/`bEnableMouseOverEvents` + GameAndUI; гейммод без пешки (HUDClass=AMenuHUD).
- **Карта `/Game/Avariika/Maps/L_MainMenu`** (`Scripts/make_menu_level.py`): пустой уровень, WorldSettings GameMode=MenuGameMode, PlayerStart. `DefaultEngine.ini` GameDefaultMap→L_MainMenu (EditorStartupMap оставил Lvl_FirstPerson — редактор открывается на рабочем уровне). `HostGame` уезжает на Lvl_FirstPerson (там BP_AvaryoGameMode по GlobalDefaultGameMode).
- **+UMG** в `Avaryo.Build.cs` (для CreateWidget настроек). Собрано закрытым Build.bat (новые UCLASS), редактор перезапущен, **проверено PIE-скрином — меню рисуется, оранжевое, без фиолетового** (скрин отправлен).
- ⚠️ Тест пользователю: открыть L_MainMenu → Play → клик по кнопкам (Создать игру должно увезти в игру; Найти — поиск LAN; Настройки — открыть Easy Options). OG Main Menu пак остаётся как склад шрифтов/иконок/звуков.

### ОЗВУЧКА — стартовый набор (2026-06-14)
Первый проход звука из библиотек `RawAssets/звуки` (52 ГБ — НЕ импортим пачкой; берём точечно). Импортировал 5 WAV в `Content/Audio/SFX/` (Claudius `asset.import_asset`), завязал в C++ через `ConstructorHelpers::FObjectFinder` (пути захардкожены, но `EditAnywhere`/`BlueprintReadWrite` — можно переопределить в BP):
- **Взрыв газа** (`Explosion`) → в `ARepairable::MulticastExplosionShake` (слышат все, мультикаст уже был).
- **Починка готова** (`RepairDone`) → в `ARepairable::FinishRepair` (на листен-сервере слышит хост; клиентам — позже мультикастом).
- **Шум рации** (`RadioBlip`) → в `ARunState::MulticastDispatcherSay` (бип на всех, когда приходит реплика диспетчера).
- **Эмбиент уровня** (`Ambient_Boiler`, луп, из Abandoned Asylum) → `ARunState::BeginPlay` через `SpawnSound2D` ДО проверки авторитета (играет локально у каждого клиента).
- **Сердцебиение паники** (`Heartbeat`, луп) → `UAudioComponent` на персонаже (`bAllowSpatialization=false`), в Tick только для `IsLocallyControlled` + `IsPanicking`: громкость/питч растут с паникой → личный звук, слышит только сам игрок.
- ⚠️ **ГРАБЛЯ:** `asset.import_asset` НЕ сохраняет .uasset на диск сам → при следующем старте `FObjectFinder` не находит (`CDO Constructor: Failed to find`). Нужно ПОСЛЕ импорта `EditorAssetLibrary.save_asset` каждому, иначе звук теряется при закрытии редактора. (Луп ставится `looping=True` на USoundWave.)
- Собрано закрытым Build.bat (новые UPROPERTY/компонент), редактор перезапущен, FObjectFinder-ошибок нет — все 5 подхватились. Слушать пользователю в PIE.
- Очередь звука дальше (точечно по событию): шаги (есть FootstepSystem/звук), гудёж газа (луп на трубе), щёлк фонаря, мини-игры (тык/срыв), джампскейр (под монстра).

### УБОРКА АССЕТОВ + EOSCore→бесплатный EOS (2026-06-14)
Пользователь: освободить диск + минимизировать будущие лицензии. Закрыл редактор, удалил (все локальные/untracked, перекачиваемы):
- Не наш сеттинг: AmericanDrive (8.4G), LightHouse (3.3G), Madrid_Street (2.3G), Shoothouse (2.1G), Backrooms_TheLobby (1.0G), YI_Luggage (0.33G), FencesVOL2 (0.15G).
- По решению юзера: Adventure_Pack/Sarah (3.0G — генерим своих операторов в meshy), Citizens_Pack (0.77G).
- **EOSCore (платный eelDev, 3.0G) УБРАН** — юзер: использовать бесплатный встроенный EOS. Удалил `Plugins/EOSCore`, убрал из `.uproject`, почистил `[EOSSDK]` из DefaultEngine.ini. Онлайн-кооп позже через движковые `OnlineSubsystemEOS`+`EOSShared` (бесплатные, есть в движке) — включить + креды Epic Dev Portal при интернет-коопе. Сейчас Null/LAN. Мой код на `IOnlineSubsystem` — менять не пришлось. **Сборка зелёная** после удаления.
- **Итого освобождено ~28 ГБ.** Оставлены: Hospital(City Modular)+HospitalCombo, PostApocalypticHouse, Hyper (обрезать мусор внутри), Decal_Forge, **FoggyStreet** (ради тумана/пара под VFX), Fire_EXP, EasyOptionsMenu, AudioToolkitPro, Survival_SFX, Construction_Pit (мультилокация), ResourcePack, oldBench, OGMainMenu, NiagaraExamples.
- ⏳ **Ждёт решения юзера:** 3 лишних больничных кита (Leartes/Horror/Abandoned, ~6.7G) — рекомендую оставить ОДИН City Modular. Вопрос локаций: больница-только vs мультилокация (больница+дом+стройка). Я склоняюсь к **мультилокации** (диспетчер шлёт бригаду на разные объекты = реиграбельность) → Construction оставлен; «Abandoned Factory» от автора City Hospital — в бэклог.

### Звук, проход 2 + ⚠️ Survival запаролен (2026-06-14)
- Юзер: звук диспетчера (глитч-стэтик) не подошёл → заменил на «эфир рации» (`RadioComm` = Ghosthack Old Radio). Взрыв оставил.
- Добавил **щелчок фонаря** (`FlashClick`, в `ApplyLightState` с гард-флагом от первого вызова) и **звук подбора** (`PickupSound` в `PickupItem`).
- ⚠️ **Survival SFX (SurvSound.rar) ЗАПАРОЛЕН** (`Encrypted = +` на файлах данных; UnRAR/7z висли на запросе пароля — отсюда все «зависания» распаковки). В нём чистые UI/craft/foley-звуки (Open_menu, Button_press, Anvil_hit, Metal_item_pick_up) — идеальны под диспетчера/починку/подбор. **Нужен пароль от юзера** (или непаролёная копия). Пока: диспетчер/фонарь — из Ghosthack (extracted), **подбор — без звука** (`PickupSound` null, код no-op; ждёт Metal_item_pick_up из Survival).
- ⚠️ Грабли распаковки: запароленный rar в headless висит на вводе пароля. Проверять `7z l -slt ... | grep Encrypted` перед распаковкой.
- Звуки в `Content/Audio/SFX` (RadioComm, FlashClick + прежние). Собрано, перезапущено, FObjectFinder-ошибок нет.

### Больницы→1 флагман + Survival разлочен + звук проход 3 (2026-06-14)
Юзер согласовал: мультилокация (диспетчер шлёт на разные объекты) + оставить ОДИН больничный кит.
- **Удалил 3 лишних больницы:** Modern_Hospital_Leartes (rm, untracked), Horror_Hospital + AbandonedHospital (`git rm`, были в репо — 581 файл). Остался флагман **City Modular Hospital + Combo**. −6.7 ГБ, −3 лицензии.
- **Survival SFX:** юзер прислал распакованную (непаролёную) копию в `Downloads` → robocopy в `Content/Survival_SFX` (481 ассета, gitignore — локально).
- **Звук на нормальные Survival-сэмплы:** фонарь → `Button_press_1` (чистый клик вместо sci-fi), подбор → `Metal_item_pick_up` (тот самый идеальный), **+ тычок мини-игры → `Anvil_hit_1`** (металл-клац на каждый E в починке, vol 0.5). Диспетчер оставил «эфир рации» (RadioComm). Собрано, перезапущено, FObjectFinder-ошибок нет.
- Очередь звука дальше: гудёж газа (луп на трубе), шаги, кашель/икота/отдышка, джампскейр (под монстра).

### Уборка 2: удаление Foggy/Decal/OGMainMenu + чистка Hyper (2026-06-14)
Юзер: удалить FoggyStreet, Decal_Forge, OGMainMenu; в Hyper вычистить мусор.
- **Удалены целиком:** FoggyStreet (3.6G), Decal_Forge (2.8G), OGMainMenu (462M — своё меню есть). (Туман/декали возьмём из Hyper-погоды / VFX юзера.)
- **Hyper обрезан 6.1 ГБ → 106 МБ** через **dependency-closure** (`Scripts/cleanup_packs.py`): keep-roots = погодные NiagaraSystem (`/Effects/Weather`) + небо (SkySphere/M_SkySphere/T_Moon/T_Stars/MI_SimpleVolumetricClouds) + Icons/Weather_States + Post_Process + SpaceSkyboxes; closure по `/Game/Hyper`; удалены папки без keep-ассета (Environments 3.1G, Tileable_Materials 2.3G, Locomotion, Core/WeatherSystem/TimeManager/UI фреймворк). Проверено — погодные VFX + небо целы, битых ссылок НЕТ.
- ⚠️ Грабли: `EditorAssetLibrary.delete_directory` по тысячам ассетов МЕДЛЕННО (редактор не отвечает на ping, пока грызёт) + Claudius-таймаут 300с вернулся раньше конца. OGMainMenu реестром не дочистился — добил `rm` на диске (закрыл редактор). Пустые папки Hyper — `find -empty`.
- **Content: ~29 → 15 ГБ** (за обе уборки суммарно −~47 ГБ). Удалённые паки были untracked/gitignore — git rm не нужен; почистил stale-строки в `.gitignore`.

### VFX — подключены из бесплатных (2026-06-14)
Вердикт: бесплатных VFX ХВАТАЕТ (NiagaraExamples + Fire_EXP + Hyper-погода). Докупать к релизу опц.: Niagara Realistic Starter VFX **2**, опц. All Explosion Pack, Blood (если гор). НЕ брать: Sci-Fi/Rocket/Plane/Water/Cascade-версии. (Подробно в ASSETS.md.)
- Модуль **Niagara** в `Avaryo.Build.cs`. Подключил 3 хука в `ARepairable` (FObjectFinder, EditAnywhere — переопределяемо в BP):
  - **Взрыв газа** → `NS_Explosion` (NiagaraExamples) в `MulticastExplosionShake` — у всех.
  - **Замыкание щитка** → `NS_Spark_Burst` (NiagaraExamples) в `ShortCircuit` — на сервере/хосте (клиентам мультикастом позже).
  - **Утечка газа** → зацикленный `NS_Smoke_Plume` (NiagaraExamples), `SpawnSystemAttached` на трубу, лайфсайкл в `RefreshStatusVisual` (broken+leaks → spawn, fixed → deactivate+destroy). Работает на всех машинах (через OnRep_Broken).
- Foam огнетушителя — отдельный пак не нужен (подделать белой частицей позже).
- Собрано (Niagara dep → закрытый Build.bat), перезапущено, FObjectFinder-ошибок нет.
- Очередь VFX дальше: огонь на горящей трубе/генераторе (Fire_EXP циклы), обломки при взрыве (Fire_EXP debris), foam-струя.

### Правки по фидбеку: VFX-тюнинг + звуки действий (2026-06-14)
Фидбек юзера по тесту: дым высоко, лаги, нет звука у починки щитка/установки кабеля/«всех действий», искры — мелкие ок.
- **Газовое облако ниже+меньше:** `GasFXOffset` (деф (0,0,10)) + `GasFXScale` (деф 0.35) — EditAnywhere, крутятся в редакторе живьём. Меньше масштаб = ниже и дешевле (меньше лагов). _Если лаги останутся — частично это шейдер-компиляция при первом проигрывании VFX (разовый хитч); либо ужать `GasFXScale` ещё._
- **Искры мельче:** `SparkScale` (деф 0.6) в `SpawnSystemAtLocation`.
- **Звуки действий (из Survival):**
  - Установка кабеля/расходника (`TryInsertBy`) → `Building_item_remove_1` (клац-установлено).
  - **Заливка/прокладка (AutoFill)** → зацикленный `Crafting_wood_item_1` через `UAudioComponent` на объекте, гоняется в Tick по реплицируемому `bPrereqAutoFilling` (слышат все). Это и есть «сопровождение починки щитка/кабеля».
  - Применение предмета (`ApplyItemEffect`: аптечка/кофе/сигарета/прожектор) → `Crafting_cloth_item_1`.
  - (Тычок мини-игры `Anvil_hit` уже был.)
- Собрано, перезапущено, ошибок нет. ⚠️ звуки серверные (PlaySoundAtLocation на authority) — на листен-сервере слышит хост; кооп-мультикаст звуков — задел на потом.
- Очередь звука: шаги, бросок, туалет, кашель/икота; и распределить «все действия» по списку.

### Звук, проход 4 — шаги + грубый foley (2026-06-14, на доверии юзера)
Бесплатные Ghosthack слишком sci-fi для grounded-foley → взял grounded из **Survival** (Movement + Survival категории):
- **Шаги** (главное): `Walk_stone`/`Run_stone`, каденция в `AAvaryoCharacter::Tick` по скорости (бег чаще/громче, присед тише, ранен — нет). На всех машинах для каждого перса по его скорости (косметика, не реплицируется — каждый клиент сам). По камню (больница); позже surface-detect/другие поверхности (Walk_wood и т.д. уже в паке).
- **Толчок (Q)** → `Punch_1` (глухой удар).
- **Аптечка** (Heal) → `First_aid_1`, **сигарета** (Calm) → `Lighter_1`; общий `UseSound` (Crafting_cloth) для остального (кофе/прожектор). Выбор по `ItemEffect` в `ApplyItemEffect`.
- Собрано, перезапущено, ошибок нет. Звуки серверные (хост слышит); кооп-мультикаст — потом.
- В Survival ещё есть под наши механики: `Trap_place/Trap_snap` (растяжка), `Fire_ignite` (поджиг газа), `Matches`, `Gun_*`/`Knife_*` (не наше). Доберу по очереди.

### Кооп-корректность звука/VFX: мультикаст (2026-06-14, автономно)
Серверные звуки/VFX 2-й игрок не слышал/не видел. Добавил `MulticastSound(USoundBase*, Loc, Vol)` (NetMulticast Unreliable) на `ARepairable` и `AAvaryoCharacter` + `ARepairable::MulticastSparkFX`. Перевёл на мультикаст: починка-готово, установка кабеля, тычок мини-игры, искры замыкания (ARepairable); толчок, подбор, применение предмета (персонаж). Теперь сервер зовёт → слышат/видят ВСЕ.
- Уже были кооп-корректны (не трогал): взрыв (MulticastExplosionShake), газовое облако + луп заливки (по реплицируемым флагам, Tick на всех), эмбиент (BeginPlay на всех), шаги (per-character Tick по реплиц. скорости), щелчок фонаря (ApplyLightState через OnRep). Сердцебиение — намеренно личное (только локальный).
- Новые RPC → закрытый Build.bat (как репликация). Собрано, перезапущено, PIE чисто. ⚠️ Бенин-енсюр `EngineElementsLibrary.cpp:35 (Registry)` на старте PIE — движковый, handled, не наш (был и будет).

### Правки звука/VFX по тест-фидбеку №2 (2026-06-14)
Юзер потестил, дал детальный фидбэк. Исправил, что можно ХОРОШО из имеющихся (grounded) звуков:
- **Диспетчер**: звук реплики УБРАН (раздражал).
- **Бег**: `Run_stone` (ужасный) → `Jog_stone`, громкость шагов ниже (бег 0.55/шаг 0.45/присед 0.3).
- **Аптечка/применение**: звук теперь В НАЧАЛЕ каста (синхронно с действием), а не в конце. Хелпер `ItemUseSoundFor` (Heal→First_aid, Calm→Lighter, иначе общий). У мгновенных — в `ApplyItemEffect`, у кастовых — на старте.
- **Рация (предмет)**: тумблер ЛКМ теперь даёт звук вкл/выкл (`RadioComm`, бывший «эфир»).
- **Установка кабеля**: «клац как уронил» (`Building_item_remove`) → `Metal_item_pick_up` (металлический клик «вставлено»).
- **Утечка газа**: густой дым «как горит» → лёгкая струйка `NS_SmokePuffLight` + **зацикленное шипение** (`GasHiss` = пневматика, grounded). Теперь читается как утечка.
- **Стартер**: убрал неуместный «тык» в начале (для типа Starter).
- Всё через мультикаст (кооп). Собрано, перезапущено, ошибок нет.

**⚠️ Не озвучено — НЕТ grounded-звуков в текущих паках (sci-fi/horror/survival/dogs):**
заведённый генератор (двигатель), сварка (дуга), замыкание щитка (электро-зап), кофе/термос (глоток). Нужен индустриальный/electrical/foley SFX-пак (или meshy/запись). Поставил-плейсхолдеры НЕ ставил (юзер забракует sci-fi). Список — в отчёте юзеру.

### Продолжение (автономка, редактор открыт, Live Coding)
- **Кооп-харднинг добит закрытой сборкой** (`b4a80bc`): репликация новых зеркальных полей `ARunState` (касса/уровни апгрейдов/карьера хосту→клиенту) ЗАРЕГИСТРИРОВАНА чистым Build.bat (Live Coding репликацию не регистрирует), редактор перезапущен. `RepairerToolQuality`/`DrawShop`/«Акт» читают из `ARunState`, а не из host-only леджера. `APickupItem::ToolQualityScale` теперь Replicated.
- **Шумомер: пик-холд** (`6b37f6f`): `GetSelfNoise01` раньше затухал линейно за 0.8 с → одиночный блип (икота 0.3, севшая батарея 0.5) мелькал <1 с, юзер не ловил (#8/#9/#12 «не работало»). Теперь пик держится `SelfNoiseHoldTime`=0.45 с, потом спад `SelfNoiseDecayTime`=1.4 с (оба EditAnywhere). `DebugSetBattery(0)` теперь сам триггерит испуг (паника+шум) → `AvBattery 0` показывает мгновенно (фонарь должен быть ВКЛ). Логика испуга вынесена в `TriggerDeadBatteryFright` (общая для разряда и дев-команды).
- **Доки:** в `TEST_CHECKLIST.md` — кооп-тест двумя сценариями (A: PIE-2 репликация кассы/качества/«Акта»; B: LAN-сессии `AvHost`/`AvFind`/`AvJoin`) + правки #8/#9/#12. Всё в `main`, Live Coding succeeded.

### Аудио: полный аудит папок + курированный импорт (2026-06-14, автономно)
Юзер: «полное ТЗ звуков/эффектов» → `SOUND_VFX_TZ.md` (`e984233`). Потом: «пройти по всем папкам, шаги оставляем, проверить что есть/чего не хватает».
- **Аудит:** мульти-агентный разбор ВСЕХ паков в `RawAssets/звуки` (Ghosthack×BOOM bundle, Sonomar Abandoned Asylum, FootstepSoundComponent, Survival, Rope/Earthquake/Hazmat, монстр-паки) → `AUDIO_INVENTORY.md` (`a67905a`): по каждому событию ТЗ статус ✅/🟡/❌ + источник + шоппинг-лист дыр + план импорта.
- **Вывод:** ~40 событий ✅, ~25 🟡 (лежат у нас), ~15 ❌ (докупка). Бóльшая дыра — **мужской вокал** (кашель/боль/вздох/скрим — нет ни в одном паке). Ещё ❌: настоящий газ-хисс, idle двигателя, CO2-спрей, касса/бипы/радио-статик. Шаги — оставили (Survival Walk/Jog по surfaces; stone≈бетон). **FootstepSoundComponent НЕ берём** — single-step под anim-notify, конфликтует с нашей cycle-loop моделью, плитки/металла там тоже нет.
- **Курированный импорт (юзер выбрал «импортнуть наши 🟡 сейчас»):** 23 SoundWave в `Content/Audio/{SFX/Repair,SFX/Hazard,SFX/Item,SFX/Foley,Ambient}`. Не целые папки (лицензии+диск) — по семейству на событие. Гиганты Sonomar (Hall/Corridor/Drips ≈186 МБ) и большие лупы обрезаны до 12–25 c через stdlib `wave` прямо в импорт-скрипте (186 МБ → ~3 МБ .uasset). 11 лупов → `looping=True`.
- **Гочи импорта (в память):** Claudius `editor.run_python_script` берёт `script_path` (путь к файлу, НЕ inline-код). `AssetImportTask` с `save=True` пишет .uasset, но `EditorAssetLibrary.load_asset`/`does_asset_exist`/`list_assets` слепы в этой сессии (registry не подхватывает Python-файлы) → грузить через `unreal.load_asset(pkg+"."+name)` (полный object path), сохранять `save_loaded_asset`. Скрипты: `Scripts/import_audio_batch.py`, `Scripts/set_audio_looping.py`.
- **Врезка в код (`2f3930f`, юзер: «пака эффектов не будет, вешай что есть»):** закрытая сборка (новые UPROPERTY/компоненты) → `Result: Succeeded`, редактор перезапущен, лог чист (все FObjectFinder разрешились).
  - *ARepairable:* взрыв→`Hazard_ExplosionGas`, вставка→`Repair_Insert`, заливка→`Repair_FuelFill_Loop`; новые — `ValveTurnSound` (трещотка) + `ValveSlipSound` (срыв резьбы) в `HandleValveTurn` (вместо общего Anvil-тыка для Valve), `StarterPullLoopSound` (натяг шнура, через FillAudioComp), `EngineIdleSound`+`EngineIdleComp` (холостой ход 3D, гонится в Tick по реплиц. `bBroken` для Starter → кооп).
  - *AvaryoCharacter:* `FallSound` (удар тела в `TriggerStumble`, мультикаст), `ExtinguisherSprayLoopSound`+`ExtinguisherAudio` (луп струи в Tick по реплиц. `Held->IsSpraying()` → кооп, без нового RPC).
- **Перепроверка ВСЕЙ `RawAssets/звуки` (юзер: «проверь все все»):** нашёл 3 НЕраспакованных rar — Earthquake (рокот/обвал/LFE), **Rope Stress & Creak** (скрипы zapsplat — хоррор-эмбиент), Monster SFX (будущее). HAZMAT SUIT = НЕ звук, а 3D-модель костюма (кандидат на модель оператора). Распаковал rope+earthquake (без пароля). Ghosthack «Sci-Fi Horror»/«Hyperion» — абстрактные Atmospheres/Stingers/Hits/Gore (кандидаты на тензион-эмбиент/стинги/сплэт, тембр нейтральный).
- **2-й заход врезки (`5160a47`, юзер: «вешай что есть»):** бросок предмета (`Metal_item_drop`, мультикаст), смена слота (`Button_hover`, личный 2D в `EquipSlot` по `IsLocallyControlled`), колхоз-луп (`BotchLoopSound` через FillAudioComp при `bBotching`), **гул прожектора** (`AFloodlight` → `HumAudio` авто-старт 3D-луп), **рокот-хвост взрыва** (`Hazard_Rumble_1` — earthquake LFE, импорт+обрезка 6с, слоем в `MulticastExplosionShake`). Закрытая сборка → Succeeded, лог чист.
- **Вариативность через SoundCue (`b5a1a5a`, автономка «займись полезным»):** 6 `SC_*` (Random-нода, `randomize_without_replacement`) на самые «долбящие» — `SC_ValveRatchet`(3)/`SC_ValveStrip`(2)/`SC_Insert`(2)/`SC_MinigameHit`(Anvil×5)/`SC_Shove`(Punch×5)/`SC_BodyFall`(2); код перенаведён на cue. Касты (аптечка/зажигалка) НЕ трогал — лупятся. **Гоча:** `USoundCue` в 5.7 не имеет `construct_sound_node`/`post_edit_change` → ноды через `unreal.new_object(SoundNode*, cue)`, волна в `sound_wave_asset_ptr`, `first_node`, сохранить `save_loaded_asset`. ⚠️ Сборка Succeeded + cue на диске + пути совпадают, но финальная проверка в редакторе отложена — редактор грузился ненормально долго при ~idle CPU (машина занята генерацией модели юзером).
- **Доки:** `AUDIO_INVENTORY.md` (статус врезки), `TEST_CHECKLIST.md` (полный гайд «что слушать» по всем добавленным звукам).
- **Дальше:** rope-скрипы в систему случайного хоррор-эмбиента; эмбиенты палат/коридора — 3D в реальные уровни; на слух проверить idle/огнетушитель/cue; озвучка на anim-notify когда придёт модель оператора. Сварка/стартер-анимация и «вентиль→заварить» — в бэклоге. ❌-дыры (мужской вокал, газ-хисс, касса/бипы/радио-статик) — докупка.

### Оператор: импорт модели + 34 анимаций + тест на карте (2026-06-14, автономно)
Юзер дал `RawAssets/персонаж` (Meshy: модель 44 МБ + 35 FBX-анимаций `_biped_..._withSkin` + 145 png) — «импортируй, тестовую модель на карту, оптимизация обязательна».
- **Импорт (headless commandlet, GUI завис на модалке восстановления после моих kill'ов):** `SK_Operator` + скелет из Idle-withSkin; затем 34 анимации как AnimSequence на этот скелет. `Content/Characters/Operator` ~46 МБ, ничего >90 МБ → закоммичено (`2acc09f`). Тестовый `TestOperator` (Idle) на `Lvl_FirstPerson` @ (300,0,-13) (`376ad2b`).
- **Оптимизация:** 4 авто-LOD на меш (`EditorSkeletalMeshLibrary.regenerate_lod`), текстура → max 2K + TEXTUREGROUP_CHARACTER.
- **2 жёсткие гочи (в память [[ue-python-scripting-gotchas]]):** (1) Interchange падает на Slate-ассерте в `-unattended` commandlet → отключать cvar `Interchange.FeatureFlags.Import.FBX false` (легаси-импортёр). (2) Легаси аним-импорт «withSkin» FBX молча тянет SkeletalMesh вместо AnimSequence → `FbxImportUI.automated_import_should_detect_type=False` + `mesh_type=FBXIT_ANIMATION`. Сначала так наимпортил 34 МЕША (удалил), потом починил.
- **Скрипты:** `import_character.py` (меш+опт), `reimport_anims.py` (корректные анимации), `place_operator.py`, `delete_wrong_anims.py`.
- **⚠️ Качество/дальше:** материал только basecolor (biped-материал ссылается лишь на albedo) — normal/roughness/metallic надо доимпортить + врезать; `texture_0` исходник 4K (26.9 МБ, рантайм-кап 2K) — ужать исходник; анимации на anim-notify + AnimBP (locomotion BlendSpace, idle/walk/run/crouch + one-shot реакции) когда юзер посмотрит и одобрит; озвучка шагов/усилий на anim-notify. GUI-редактор после kill'ов капризничал — чистить автосейвы при зависании старта.

### Оператор: цвет/материал + решение по анимациям (2026-06-15)
- **Цвет («без цвета»):** ручной `M_Operator` смывал Meshy в один тон (известная засада [[ue-python-scripting-gotchas]] — родной импортный материал показывает текстуру, ручной нет). Вернул родной `Material_1` (DiffuseColorMap=texture_0) + выключил ложный emissive=basecolor. Исходная basecolor проверена глазами — цветной атлас (оранж куртка/жёлтые полосы/каска). `texture_0` 2K sRGB.
- **Тест-карта:** геймплейную `Lvl_FirstPerson` загрязнять НЕЛЬЗЯ (воткнул туда оператора+свет 30000 → лаги/засветка, юзер не смог играть; удалил, карта чистая). Сделал отдельную лёгкую **`L_AnimTest`** (`/Game/Avariika/Maps/`, пол+Directional+SkyLight+SkyAtmosphere, оператор idle) — там и смотреть анимации (или Persona двойным кликом).
- **Стратегия анимаций (юзер «без предпочтений» → решаю): ГИБРИД.** Meshy оставляем для ИГРОВЫХ экшенов (ремонт/термос/реакции/смерти/поднять — чего нет в Epic-паках). Локомоцию (ходьба/бег/strafe/повороты/присяд — у Meshy рывки в лупах + нет бег-назад/вбок) берём из БЕСПЛАТНОГО Epic/Fab пака (Animation Starter Pack / MC Sample — обычные клипы → BlendSpace; Game Animation Sample = motion-matching, сложнее, на потом). Свожу на скелет оператора через IK Retargeter. Покупать не нужно.
- **Блокер:** пак добавляет ЮЗЕР (Fab/Launcher «Add to project» — я не могу скачать). Дальше: ретаргет локомоушна → оператор, AnimBP (idle/walk/run/crouch + one-shot экшены/реакции), привязка к игровому персонажу (кооп: тело видно другим), звук шагов/усилий на anim-notify. Бросок/толчок — обрезать в UE.

### Автономная ночь (2026-06-15): аудит + кооп-фикс; дом отложен
Юзер ушёл спать, попросил автономные задачи (главная — карта на доме). План — `AUTONOMOUS_PLAN.md`.
- **Дом отложен:** PostApocalypticHouse — World Partition/стриминг; headless-дубль `LV_Horror_Light` даёт нестабильно пустую карту (WP-акторы в commandlet грузятся недетерминированно — проба 25 акторов, сборка 0). Удалил мусор, `Lvl_FirstPerson` не трогал. Дом — в открытом редакторе с юзером.
- **Аудит** (мульти-агентный, 53 агента, состязательная перепроверка): `AUDIT_REPORT.md` — 39 подтверждённых находок. Классы: незащищённый `GetWorld()` в Tick-пути (гигиена), 9 `Exec`-читов + витали 50/50 (пред-релиз), пер-фреймовые `TActorIterator`/`TObjectIterator` в Tick/HUD (перф — причина лагов), нелицензированные паки (Hospital/House/Survival_SFX/Ghosthack).
- **Применён 1 безопасный фикс:** `AExitZone::bTeamInside` → `UPROPERTY(Replicated)`+`DOREPLIFETIME` (клиенты не видели статус выхода). Закрытая сборка Succeeded.
- **НЕ применял автономно** (нужна проверка/решение юзера): перф (меняет тайминги; `ARepairable` нельзя троттлить наивно из-за курсор-мини-игры), отладка/витали (нужны сейчас), лицензии.

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
