# SOUND_MAP — звук под механики «Аварийки»

_Курация 2026-06-19. База `Content/Audio/Lib/` (Ghosthack + Abandoned Asylum + Monster Within), импорт `.uasset`, локально (gitignore)._
_Пути = `/Game/Audio/Lib/<кат>/<файл>`. Пара к визуалу — см. `EFFECTS_INVENTORY.md`._

---

## Лучшее под каждую механику

| Механика | ⭐ Лучший файл (`Lib/…`) | Альтернатива | Как использовать |
|---|---|---|---|
| **Дверь — открыть** | `door_impact/Ghosthack-MFW_Gate_Wood_Large_Open` | `door_impact/Ghosthack-SF_Household_Cabinet_Cupboard_Slide_Door_Close_01` (лёгкая) | `ADoor` OpenSound; большие деревянные двери дома |
| **Дверь — закрыть/хлопок** | `door_impact/Ghosthack-SH_Hit_Metal_Door_Slam` | `door_impact/Ghosthack-MFW_Gate_Wood_Large_Close` | CloseSound; металл = жёстче, дерево = мягче |
| **Гаражные ворота** | `door_impact/Ghosthack-SH_Hit_Metal_Rattling_Gate` | `door_impact/Ghosthack-MFW_Gate_Stone_Large_Close` | громкий лязг ворот |
| **Вентиль (газ/вода) — крутить** | `repair_tool/Ghosthack-SF_Garage_Car_Fuel_Tank_Cap_Screw_Open_01` | `repair_tool/Ghosthack-SF_Garage_Workbench_Vise_Handle_01` (колесо-вентиль) | перекрытие трубы; `_Close_01` на обратный ход |
| **Щиток/рубильник — щёлк** | `repair_tool/Ghosthack-H_Impact_Metal_Klonk` | `repair_tool/Ghosthack-H_Impact_Metal_Strike` | `APowerSwitch` toggle (плейсхолдер — чистого брейкера в базе нет) |
| **Питание под напряжением (гул)** | `electrical/Ghosthack-M_Ambience_Power_Line` | — | ЛУП пока щиток живой (зона под током) |
| **Искры/КЗ провода** | `electrical/Ghosthack-ME_Magic_Fire_Weld_Machine_Inventor_Multiple_Short_Electrode_Sparkling_Hits` | — | живой провод/замыкание; пара к `NS_Spark_Continuous`/`NS_TeslaCoil` |
| **Сварка (дуга)** | `repair_tool/Ghosthack-ME_Magic_Fire_Weld_Machine_Inventor_Slide_Electrode_On_Metal_Sparkles_Soft` | `electrical/…_Electrode_Sparkling_Hits` | заварка трубы; пара к искрам Niagara |
| **Утечка газа (шипение)** | `gas/Ghosthack-SF_Garage_Tire_Pressure_Control_Deflate_01` | `gas/Ghosthack-SF_Air_Burst_Train_Pressure_Release_Squeak` | ЛУП у `Repairable_GasPipe`; пара к `NS_Smoke_7_acid` |
| **Газ поджёгся (горит)** | `gas/Ghosthack-ME_Magic_Fire_Gas_Flame_On_01` (поджиг) + `ambience_house/Ghosthack-ME_Magic_Fire_Gas_Flame_Burn_Loop` (горение) | `gas/Ghosthack-ME_Magic_Fire_Gas_Flame_Burn_Strong_Long_01` | one-shot поджиг → переход в луп |
| **Взрыв газа** | `door_impact/Ghosthack-E_Explosion_Gas_Real_Gas_Explosion_Close_Large_01` | `door_impact/…_Close_Small_Indoor_01` (в помещении) | пара к `NS_Explosion`; indoor-вариант для комнат |
| **Прорыв трубы (струя)** | `water/Ghosthack-SF_Garage_Mechanic_Sink_Running_Long_01` | `…_Long_02` | ЛУП у `Repairable_WaterPipe`; пара к `NS_WaterHose_SingleProjection` |
| **Капель (потолок/труба)** | `water/Abandoned_Asylum_Room_Tone_Basement_Water_Drips_SC-AA_026_A` | `…_026_B` (вариант для рандома) | пара к `Dripping_Static_Mesh`; подвал/мокрые зоны |
| **Рация** | `radio/Ghosthack-H_Transition_Old_Radio` | — | вызов диспетчеру / приём задания |
| **Ремонт — ключ/гайковёрт** | `repair_tool/Ghosthack-SF_Garage_Impact_Wrench_Use_01` | `…_Impact_Wrench_Screw_Single_01` | Hold-этап ремонта (откручивание) |
| **Ремонт — удар молотком** | `repair_tool/Ghosthack-X_Hit_Metal_Workshop` | `repair_tool/Ghosthack-H_Impact_Metal_Strike` | забивка/правка |
| **Дрель** | `repair_tool/Ghosthack-SF_Garage_Electric_Drill_Idle_Short_01` | `…_Idle_Very_Short_01` | сверление крепежа |

## Атмосфера / фон (амбиент-аварийка)

| Зона/событие | Файлы (`Lib/…`) |
|---|---|
| **Комнатный тон (дом/коридор/зал)** | `ambience_house/Abandoned_Asylum_Room_Tone_*` (Large_Hall / Long_Corridor / Large_Room / Public_Bathroom — луп под комнату) |
| **Подвал/котельная** | `ambience_house/Abandoned_Asylum_Room_Tone_Boiler_Room_Basement_SC-AA_024/025` |
| **Скрипы конструкции (напряжение)** | `creak_struct/Ghosthack-SF_Destruction_High_Pitch_Creak_01/02`, `…_Wood_Slide_Creak_01/02`, `…_Floorboard_Crack_Long_Single` — рандом-триггеры |
| **Обвал/разрушение** | `door_impact/Ghosthack-E_Demolition_Collapsing_Building_*`, `creak_struct/Ghosthack-X_Boom_Dark_Metal_Bones` |
| **Джампскейр-стингеры** (под монстра, gated) | `jumpscare/JS_Jumpscare_*`, `jumpscare/Ghosthack-SH_Stinger_*`, `jumpscare/Monster_Within_*` |

---

## Что собрать дальше (SoundCue)

Для механик с несколькими дублями — обернуть в **SoundCue с Random + Modulation** (чтобы не повторялось):
- **Дверь** (open/close × дерево/металл) — Random-нода.
- **Скрипы** (`creak_struct`, 13 файлов) — Random + питч-модуляция, рандом-таймер в амбиенте.
- **Ремонт-удары** (wrench/hammer/strike) — Random на каждый тик Hold-этапа.
- **Капель** (026_A/B) — Random + случайный интервал.

Лупы (газ-шип, вода-струя, power-line гул, газ-горение) — отдельные Cue с `Looping`.

## Привязка в коде (требует ребилда — когда закроешь редактор)
Свойства уже есть в C++ (напр. `ADoor::OpenSound`); проставить дефолты Cue на CDO/в `BeginPlay` через `LoadObject`. Аварийные лупы — `UAudioComponent` на `ARepairable`, старт/стоп по состоянию (сломан→играет). Я распишу точечно, когда дойдём до запекания.

## Пробелы базы
- **Чистого «рубильник/брейкер-щёлк» нет** — сейчас металл-klonk-плейсхолдер. Кандидат на докачку (Boom Foley / Ghosthack switch).
- **Чистого «вентиль-колесо скрип» нет** — fuel-cap-screw близко, но не идеально.
- **Голоса диспетчера/жильцов** — в базе нет (план: *Dark Russian Voices*, scrydy — см. `СКАЧАТЬ.md`).
