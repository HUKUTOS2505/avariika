# EFFECTS_INVENTORY — каталог Niagara/VFX под механики «Аварийки»

_Авто-рескан 2026-06-18 (multi-agent workflow, 8 паков → 280 spawnable-систем). Пути = /Game/...; источники паков локальны (gitignore)._

---

## Сводка

**Итого пригодных Niagara-систем: 280** (только spawnable NS_/ns_/N_ системы; Cascade-ParticleSystems, эмиттеры NE_/ne_, модули NM_/NMS_, материалы/текстуры/меши исключены авторами каталогов).

| Пак | Систем |
|---|---|
| NiagaraExamples + UltraDynamicSky/Particles | 76 |
| Realistic_Starter_VFX_Pack_Niagara_Vol2 | 60 |
| FluidFlux + FluidNinjaLive | 41 |
| IndustrialFactory | 32 |
| NiagaraExplosion01 + Fire_EXP_Vol01_Free | 30 |
| Warehouse Effects (/Game/Warehouse/Effects) | 18 |
| M5VFXVOL2 | 12 |
| Hyper/Interaction/ResourcePack (только Hyper) | 11 |

Распределение по категориям: explosion 34 · fire 32 · water_splash 48 · smoke_gas 22 · dust_debris 28 · rain 16 · sparks 17 · electric_arc 12 · mist_fog 11 · steam 10 · flood 2 · magic_misc 48.

---

## По категориям

### explosion (34)
- `/Game/NiagaraExamples/FX_Explosions/NS_Explosion` — чистый огненный взрыв, готов под взрыв газа (+`NS_Explosion_Medium`, `NS_Explosion_Small`)
- `/Game/NiagaraExamples/FX_Explosions/NS_Dirt_Explosion` — взрыв с землёй/обломками (+Medium, Small)
- `/Game/_Packs/Fire_EXP_Vol01_Free/Niagara/EXP/NS_Sub_EXP_Large_001_01` — крупный бласт, явно «gas-leak fit» (+Mid, Small)
- `/Game/M5VFXVOL2/Niagara/Explosion/NFire_Exp_00` — огне/газовый burst (+`NFire_Exp_01`, `NFire_Exp_03`)
- `/Game/NiagaraExplosion01/Niagaras/Air/N_ExplosionAir_001` — галерея воздушных бластов (+9 more)
- `/Game/NiagaraExplosion01/Niagaras/Ground/N_ExplosionGround_001` — галерея наземных бластов (+9 more)
- `/Game/NiagaraExamples/Utilities/SpriteGeneration/ExplosionRoil/NS_ExplosionRoil` — клубящийся roil-источник; `/Game/NiagaraExamples/FX_Misc/NS_FireworkBurst` — салютный burst

### fire (32)
- `/Game/IndustrialFactory/Effects/Fire_01/ns_Fire_01_01` — основной промышленный огонь (+Small/Torch/TorchSmoke/Candle, `ns_FireGuide_01_01` лента-трейл)
- `/Game/M5VFXVOL2/Niagara/Fire_for_BP/NFire_BP_00` — луп-пламя, BP-ready (+15 вариантов; `NFire_Grd_BP_00` напольный +2; Candle/Torch)
- `/Game/M5VFXVOL2/Niagara/Reference/Fireloop/3_Nblowingfire_fwd_pt` — направленный струйный факел = поджжённая газовая труба (+`4_Nbigfire_pt` крупный)
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Fire/NS_Fire_Wall` — огонь по стене (+Ceiling/Floor/Window×2/Camp×2/Torch — набор под поверхности)
- `/Game/_Packs/Fire_EXP_Vol01_Free/Niagara/Fire/Loop/NS_Sub_FireLarge_Loop_004` — крупный луп-огонь (+Mid/Small Loop&Rate, Torch = 7)
- `/Game/NiagaraExamples/FX_Misc/NS_Fire` · `/Game/FluidNinjaLive/UseCases/012_NiagaraParticleCapture/NS_Bonfire` · `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Environment/NS_Environment_5_flare` (сигнальный фаер)

### smoke_gas (22)
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Smoke/NS_Smoke_7_acid` — **единственная штатная токсично-газовая туча**
- `/Game/NiagaraExamples/FX_Smoke/NS_Smoke_Plume` — плотный плюм, «toxic-gas ready» (+`NS_Chimney_Smoke`, `NS_SmokePuffLight`, `NS_RocketTrail`)
- `/Game/IndustrialFactory/Effects/Smoke_01/ns_ChimneySmoke_01_03_Large` — крупный пром-плюм, плейсхолдер газа (+S/M; Warehouse дублирует те же 3)
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Smoke/NS_Smoke_1` — дженерик-дым (+`_2`/`_5`/`_8`, `_4_tornado`, `NS_Environment_6_burnout`)
- `/Game/M5VFXVOL2/Niagara/Reference/Smoke/5_6_Nsmoke6_pt` — тяжёлый дым (+тонкий `0_6_Nsmoke_pt`); `/Game/FluidNinjaLive/.../NS_BonfireSmoke` (+Smoke2, MagicCauldron vapor)

### mist_fog (11)
- `/Game/IndustrialFactory/Effects/Mist_01/ns_Mist_01_03_UnlitLarge` — крупная дешёвая unlit-мгла (+Lit Large/Small, Unlit Small; Warehouse дублирует ещё 4)
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Environment/NS_Environment_7_volume` — объёмный fog-volume
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Smoke/NS_Smoke_3_coldsmoke` — низовой стелющийся туман, хоррор (+`NS_Environment_2_coldsmoke`)

### water_splash (48)
- `/Game/FluidNinjaLive/UseCases/012_NiagaraParticleCapture/NS_WaterHose_SingleProjection` — напорная струя = прорыв трубы (+DoubleProjection)
- `/Game/UltraDynamicSky/Particles/Standalone/Dripping_Static_Mesh` — капель с меша/потолка (+`Dripping_Mesh`, `Dripping_Skeletal_Mesh`); `/Game/FluidNinjaLive/.../NS_LiquidDrops_WorldSpace` (+local, `NS_Demo_WateryDrizzleDown`)
- `/Game/FluidFlux/Interaction/Splash/Effects/NS_SimpleSplash` — брызги от взаимодействия (+`NS_FluxSplashArea`, `NS_FluxFoam`, `NS_Bubbles`)
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Water/NS_Water_1` — спрей/всплеск галерея (+7 more)
- `/Game/IndustrialFactory/Effects/Waterfall_01/ns_Waterfall_Splash_01_04_Single` — водопад-набор (+4 части) + `ns_FallingSludge_Green_03_02_Large` токсичная капель-жижа (+чистая/грязная ×размеры)
- `/Game/FluidNinjaLive/.../NS_Acid_DoubleProjection` — кислотный спрей (токсичная жидкость); `/Game/UltraDynamicSky/Particles/Puddle_Splash` (+Ripple, FluidGrid); footstep-splashes ×4 (+N more)

### flood (2)
- `/Game/FluidFlux/Environment/Niagara/Default/NS_FluxDefault` — драйвер симуляции поверхности воды (реальный разлив/подъём уровня)
- `/Game/FluidFlux/Surface/Meshes/Niagara/NS_InfiniteSurfaceMesh` — рендер бесконечной водной поверхности

### rain (16)
- `/Game/_Packs/Hyper/ResourcePack/Effects/Weather/System/NS_Rain_Heavy` — сильный дождь (+Light, ThunderStormWithRain, SnowImproved, Blizzard, Hailstorm = 6)
- `/Game/UltraDynamicSky/Particles/Rain` — UDS-осадки (+Snow, Radial_Storm, Weather_Particles, GPU_WeatherParticles = 5)
- `/Game/FluidNinjaLive/.../NS_Rain_NonFluidDriven_Translucent` (+`NS_FluxRain`, снег/буран); `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Environment/NS_Environment_9_storm`

### sparks (17)
- `/Game/NiagaraExamples/FX_Sparks/NS_Spark_Continuous` — непрерывный поток искр = искрящий провод (+`NS_Spark_Burst`, `NS_Spark_Impact_Looping`)
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Sparks/NS_Sparks_1` — сварка/электро-искры (+10 more)
- `/Game/NiagaraExamples/FX_Weapons/Impacts/NS_Impact_Metal` — искры удара по металлу (+`NS_MuzzleFlash`, `NS_NDC_Impacts`)

### electric_arc (12)
- `/Game/NiagaraExamples/FX_Ribbons/NS_TeslaCoil` — ветвящиеся дуги = оголённая проводка
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Lightning/NS_Lightning_1` — дуга/молния (+7 more)
- `/Game/NiagaraExamples/FX_Player/NS_Player_Electricity_Looping` — электро-аура (зона удара током); `/Game/UltraDynamicSky/Particles/Lightning_Strike` (+`Obscured_Lightning`)

### steam (10)
- `/Game/IndustrialFactory/Effects/Smoke_01/ns_SteamFast_01_01` — напорная струя пара = утечка из трубы/вентиля (+`ns_Steam_01_01` медленный, `ns_ManholeSteam` S/L; Warehouse дублирует все 4)
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Smoke/NS_Smoke_6_steam` — плюм пара
- `/Game/UltraDynamicSky/Particles/Standalone/FreezingBreath` — пар-прокси (холодное дыхание)

### dust_debris (28)
- `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Destruction/NS_Destruction_1_concrete` — пыль/обломки разрушения (набор 8 по поверхностям: бетон/плитка/стекло/дерево×3/камень)
- `/Game/NiagaraExamples/FX_Weapons/Impacts/NS_Impact_Concrete` — локальная крошка (+Glass, Wood, footsteps)
- `/Game/UltraDynamicSky/Particles/Dust` — воздушная пыль на всю комнату (+`Wind_Debris`)
- `/Game/_Packs/Hyper/ResourcePack/Effects/Particles/Environment/NS_Dusty` — пылинки (+Sandstorm, Heavy_Wind); `/Game/FluidFlux/Demo/Environment/Dust/NS_FluxDust` (+песок/обломки в воде, +N more)

### magic_misc (48) — амбиент/декор/SDK-демо, не опасности
- `/Game/IndustrialFactory/Effects/Birds_01/ns_Birds_01_01` — птицы (+насекомые, листва, интерактивная флора; Warehouse дублирует)
- `/Game/NiagaraExamples/FX_Markers/NS_Marker_Location` — маркер цели/задачи (+`Marker_Target`, `Pickup_*` свечение ×4)
- `/Game/M5VFXVOL2/Niagara/Reference/distortion/9_Ndistort_pt` — тепловое марево над огнём (полезно поверх пламени)
- декор воды FluidFlux (рыбы/ряска/планктон), телепорт/бафф-ауры, `NS_Aurora`, SkeletalMesh/Dino-демо (+N more — большинство это SDK-демо и геймплейные не-опасные эффекты)

---

## Лучшее под наши аварии

| Авария | Топ-пики (`/Game` пути) | Почему |
|---|---|---|
| **ГАЗ-облако (toxic)** | `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Smoke/NS_Smoke_7_acid` | Единственная система, изначально сделанная как кислотно-токсичная туча — без перекраски |
| | `/Game/NiagaraExamples/FX_Smoke/NS_Smoke_Plume` | Плотный плюм, помеченный «toxic-gas ready»; перекрасить в жёлто-зелёный |
| | `/Game/IndustrialFactory/Effects/Smoke_01/ns_ChimneySmoke_01_03_Large` | Крупный промышленный плюм как утечка из стояка/оборудования |
| **ВОДА — струя из трубы** | `/Game/FluidNinjaLive/UseCases/012_NiagaraParticleCapture/NS_WaterHose_SingleProjection` | Направленная напорная струя — ровно прорыв трубы под давлением |
| **ВОДА — разлив-поверхность** | `/Game/FluidFlux/Environment/Niagara/Default/NS_FluxDefault` (+`/Game/FluidFlux/Surface/Meshes/Niagara/NS_InfiniteSurfaceMesh`) | Реальная симуляция растекающейся воды = лужа/подтоп на полу |
| **ВОДА — капли** | `/Game/UltraDynamicSky/Particles/Standalone/Dripping_Static_Mesh` | Капель прямо со статик-меша (труба/потолок); альтернатива — `/Game/FluidNinjaLive/UseCases/012_NiagaraParticleCapture/NS_LiquidDrops_WorldSpace` |
| **ЭЛЕКТРО — искры** | `/Game/NiagaraExamples/FX_Sparks/NS_Spark_Continuous` | Непрерывный поток искр из коротящего провода; альт `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Sparks/NS_Sparks_1` |
| **ЭЛЕКТРО — дуга-провод** | `/Game/NiagaraExamples/FX_Ribbons/NS_TeslaCoil` | Ветвящиеся дуги — идеально для оголённой/искрящей проводки; альт `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Lightning/NS_Lightning_1` |
| **ЭЛЕКТРО — разряд по воде** | `/Game/NiagaraExamples/FX_Player/NS_Player_Electricity_Looping` | Луп-аура электричества, растягивается на зону мокрого пола (точного «разряд по луже» нет — это ближайшее) |
| **ВЗРЫВ газа** | `/Game/NiagaraExamples/FX_Explosions/NS_Explosion` | Чистый огненный бласт, 3 размера, geared под взрыв газа |
| | `/Game/_Packs/Fire_EXP_Vol01_Free/Niagara/EXP/NS_Sub_EXP_Large_001_01` | Крупный бласт, помеченный «gas-leak fit» |
| | `/Game/M5VFXVOL2/Niagara/Explosion/NFire_Exp_00` | Огне/газовый burst-вариант для разнообразия |
| **ПАР** | `/Game/IndustrialFactory/Effects/Smoke_01/ns_SteamFast_01_01` | Напорная струя пара = утечка из вентиля/трубы |
| | `/Game/IndustrialFactory/Effects/Smoke_01/ns_Steam_01_01` | Медленный плюм для слабой утечки (есть и в Warehouse-паке) |
| | `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Smoke/NS_Smoke_6_steam` | Альтернативный паровой плюм |
| **ПЫЛЬ/мусор** | `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Destruction/NS_Destruction_1_concrete` | Пыль+обломки разрушения; есть весь набор по поверхностям (стекло/дерево/камень/плитка) |
| | `/Game/NiagaraExamples/FX_Weapons/Impacts/NS_Impact_Concrete` | Локальная крошка под удар/обвал в конкретной точке |
| | `/Game/UltraDynamicSky/Particles/Dust` | Висящая пыль на весь объём комнаты (атмосфера заброшки) |
| **Фон/амбиент (мгла/туман)** | `/Game/IndustrialFactory/Effects/Mist_01/ns_Mist_01_03_UnlitLarge` | Крупная дешёвая unlit-мгла для коридоров/подвала |
| | `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Smoke/NS_Smoke_3_coldsmoke` | Низовой стелющийся туман — хоррор-настроение |
| | `/Game/Realistic_Starter_VFX_Pack_Niagara_Vol2/Niagara/Environment/NS_Environment_7_volume` | Объёмный fog-volume для крупных пространств |

---

## Пробелы

Опасности с **готовым** ассетом покрыты практически все — взрыв, огонь, пар, искры, дуга, пыль, туман, капель и струя воды есть «из коробки». Реальные дефициты:

- **Разряд тока по воде (электро+лужа)** — нет ни одной специализированной системы. Решается без редактора (растянуть `NS_Player_Electricity_Looping` / `NS_Lightning_*` по площади лужи), но «честный» эффект потребует **авторинга в Niagara**.
- **Подъём/разлив воды как готовый дроп-ин** — отдельной «flood»-системы нет; есть только сим-драйверы FluidFlux (`NS_FluxDefault` + `NS_InfiniteSurfaceMesh`), которые **требуют настройки плагина FluidFlux**, а не простого спавна. Niagara-редактор не нужен, но нужна интеграция плагина.
- **Чистое «газовое облако» тонко представлено** — по сути один штатный ассет (`NS_Smoke_7_acid`) + перекраска дымов. Если нужен отдельный вид именно бытового газа (метан, без цвета) — это материал/перекраска, либо лёгкий **авторинг в Niagara**.
- **Пар достаточного «промышленного» качества** есть (`ns_SteamFast_01_01`), но как высоконапорный белый свисток он один; вариативность — за счёт перекраски/масштаба, без редактора.

Категории `rain` и `magic_misc` для нашего жанра избыточны (погода/декор/SDK-демо) — в gameplay не нужны, кроме маркеров задач (`NS_Marker_Location`) и теплового марева над огнём (`9_Ndistort_pt`).