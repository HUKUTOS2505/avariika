# INCIDENT_FX_MAP — авария → эффект + звук (единый лукап)

_2026-06-19. Связывает `EFFECTS_INVENTORY.md` (Niagara) + `SOUND_MAP.md` + 16 Cue в `/Game/Audio/SFX/Cues/`._
_Назначение: привязка к `ARepairable`/механикам — лукап, а не раскопки. Niagara = `/Game/…`; SFX = `SC_*` (Cue)._

---

## Аварии (опасности)

| Авария | Niagara (эффект) | SFX луп | SFX ваншот | Заметка |
|---|---|---|---|---|
| **Газ — утечка** | `Realistic_…/Smoke/NS_Smoke_7_acid` | `SC_GasLeak_Loop` | — | облако токсичное; перекрасить под бытовой газ — gap |
| **Газ — поджёгся** | `M5VFXVOL2/…/Reference/Fireloop/3_Nblowingfire_fwd_pt` (струйный факел) | `SC_Gas_Burn_Loop` | `SC_Gas_Ignite` | поджиг → переход в луп горения |
| **Газ — взрыв** | `NiagaraExamples/FX_Explosions/NS_Explosion` | — | `SC_Explosion_Gas` | indoor-вариант звука для комнат |
| **Вода — прорыв трубы (струя)** | `FluidNinjaLive/…/NS_WaterHose_SingleProjection` | `SC_Water_Jet_Loop` | — | направленная напорная струя |
| **Вода — разлив/потоп** | `FluidFlux/…/NS_FluxDefault` + `NS_InfiniteSurfaceMesh` | (амбиент воды) | — | требует настройки плагина FluidFlux |
| **Вода — капель** | `UltraDynamicSky/Particles/Standalone/Dripping_Static_Mesh` | — | `SC_Drip` | с потолка/трубы; подвал |
| **Электро — искры/КЗ** | `NiagaraExamples/FX_Sparks/NS_Spark_Continuous` | — | `SC_Sparks` | коротящий провод |
| **Электро — дуга (живой провод)** | `NiagaraExamples/FX_Ribbons/NS_TeslaCoil` | `SC_Power_Hum_Loop` (пока под напряжением) | `SC_Sparks` | оголённая проводка |
| **Электро — разряд по воде** | `NiagaraExamples/FX_Player/NS_Player_Electricity_Looping` (растянуть по луже) | `SC_Power_Hum_Loop` | — | **gap** — честный эффект = авторинг с глазами; кандидат Poison/доп. |
| **Сварка** | `Realistic_…/Sparks/NS_Sparks_1` | — | `SC_Weld` | дуга при заварке трубы |
| **Пар (утечка)** | `IndustrialFactory/Effects/Smoke_01/ns_SteamFast_01_01` | — *(steam-loop Cue нет)* | — | высоконапорный свисток |
| **Огонь (поверхность)** | `IndustrialFactory/Effects/Fire_01/ns_Fire_01_01` / `Realistic_…/Fire/NS_Fire_Wall` | — | — | пожар на статусе «Горит» |
| **Пыль / обвал** | `Realistic_…/Destruction/NS_Destruction_1_concrete` | — | (обвал — `Lib/door_impact/E_Demolition_*`) | крошка по поверхности |

## Интерактивные объекты / действия

| Объект/действие | Niagara | SFX | Заметка |
|---|---|---|---|
| **Щиток / рубильник** | (искры при КЗ — см. выше) | `SC_Power_Hum_Loop` (живой) + `SC_Panel_Switch` (щелчок) | механика свет↔питание уже в коде (тег `PoweredLight`) |
| **Дверь** | — | `SC_Door_Open` / `SC_Door_Close` | `ADoor` |
| **Вентиль (газ/вода)** | — | `SC_Valve_Turn` | перекрытие трубы |
| **Рация** | — | `SC_Radio` | вызов диспетчеру (см. `SPEC_Radio_Haggle.md`) |
| **Ремонт (Hold-этап)** | (искры/пар по типу) | `SC_Repair_Hit` (рандом) | каждый тик откручивания/удара |
| **Скрипы (ambient)** | — | `SC_Creak` (5-вар. рандом) | эмиттер с рандом-таймером по дому |
| **Комнатный тон** | — | `SC_RoomTone_*` (Hall/Corridor/Room/Bathroom/Basement/Stairs) | AmbientSound на комнату |

## Привязка (когда закроем редактор — ребилд)
- Аварийные лупы → `UAudioComponent` на `ARepairable`, старт/стоп по `bBroken`/состоянию.
- Эффект-спавн → `UNiagaraComponent` на `ARepairable`, активировать при поломке (gated по `EffectsQuality`).
- Ваншоты (взрыв/поджиг/щёлк) → `UGameplayStatics::PlaySoundAtLocation` в точке события.
- Дверь/вентиль/рация → Cue в соответствующие свойства/вызовы.

## Дыры (нужен заход с глазами / докупка)
- **Электро-по-воде** — нет готовой системы (растяжка `NS_Player_Electricity` — временно; кандидат: **Poison Magic Niagara** из `СКАЧАТЬ.md`).
- **Газ-облако бытовое** (без зелёного) — перекраска дыма (материал/цвет-параметр).
- **Steam-loop Cue** — не собран (есть визуал `ns_SteamFast`, нет звук-лупа; в базе пневмо-burst `Lib/gas/Air_Burst_*` подойдёт — собрать Cue).
