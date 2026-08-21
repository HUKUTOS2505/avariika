# Animation and Locomotion Knowledge Base

This consolidated document preserves the source material listed below. Originals are archived under Docs/Archive/Originals/.

## Sources
- `ANIM_IMPORT_PLAN.md`
- `ANIM_PACKS_SURVEY.md`
- `LOCOMOTION_PLAN.md`
- `LOCO_BUILD_SHEET.md`
- `STATE_MACHINE_BUILD.md`
- `TURN_IN_PLACE_BUILD_SHEET.md`
- `MODULAR_WORKER_PLAN.md`

---

## Source: ANIM_IMPORT_PLAN.md

# ANIM_IMPORT_PLAN — `anims_incoming` → проект (Группа B)

> Составлено автономно 2026-06-24. Источник: `RawAssets/anims_incoming` (5 суб-паков, **1436 FBX, 2.3 ГБ**).
> Цель — занести анимы на скелет игрока и сделать пригодными к использованию в `ABP_Worker`.

## 🎯 Целевой скелет (подтверждено в редакторе)

`ABP_Worker` (восстановлен после краша 2026-06-24) сидит на:

```
/Game/FreeAnimationLibrary/Demo/Characters/Mannequins/Meshes/SK_Mannequin   (UE5-маннекен)
```

Quantum-тело (`SKM_Worker_*`) гоняется этим же скелетом через compatible-skeleton (память `quantum-compat-skeleton-save`: FBL≡WorkAnimations, один UE5-mann). **Все новые анимы игрока должны в итоге лежать на этом `SK_Mannequin`.**

Скелет Quantum-меша (`/Game/Modular_Workers/Mesh/Male/SK_Male_Quantum_Character_Skeleton`) — отдельный; на него ничего ретаргетить не нужно (тело анимируется через compatible с UE5-mann).

## 📦 Разбор 5 суб-паков

| Суб-пак | FBX | Скелет источника | Действие | Ценность |
|---|---:|---|---|---|
| **Motifect Injured & Exhausted** | 30 | AI-«stickman» (свой риг) | **импорт + IK-ретаргет → SK_Mannequin** | ⭐⭐⭐ диегетик-травмы/выносливость |
| **Motifect Emotes & Social** | ~? | AI-«stickman» (свой риг) | импорт + IK-ретаргет → SK_Mannequin | ⭐ кооп-социалка |
| **Getting Up 01/02** | 2 | UE4-маннекен (`AnimUE4_GettingUp`) | импорт на UE4-скелет + ретаргет UE4→UE5 | ⭐⭐ подъём после нокдауна |
| **Game Animation Sample** | ~1400 | **уже ретаргечено на UE5-mann** | **ПРЯМОЙ импорт на SK_Mannequin (ретаргет НЕ нужен)** | ⭐ библиотека локомоции под BlendSpace/MM |

### Почему ретаргет нельзя делать вслепую
Motifect — это **AI-генерённые клипы на собственном скелете** (README пака прямо: «Retarget the animation to your own character mesh… the stickman is for preview only»). Корректность ретаргета (поза не ломается, стопы не разъезжаются, таз на месте) **требует визуальной проверки** — авто-align IK-ретаргетера не трогает root/таз (память `ik-retarget-python`). Поэтому ретаргет оставлен на заход вместе с тобой, чтобы не наплодить кривых анимов рядом с активной работой над `ABP_Worker`.

## ✅ Рекомендуемый порядок (для захода с верификацией)

1. **Injured & Exhausted (30, высшая ценность).** Импорт FBX в `Content/Avariika/Anim/Motifect_Injured/` (создаётся Motifect-скелет). → IK Retargeter Motifect→SK_Mannequin → ретаргет 30 клипов → проверить позы в редакторе → привязать к состояниям (ранен/выдохся/limp/collapse/drag) в `ABP_Worker`.
2. **Getting Up (2).** Импорт на UE4-скелет (в проекте есть `UE4_Mannequin_Skeleton`) → ретаргет UE4→UE5 (есть op-stack пайплайн, память `ik-retarget-python`).
3. **Game Animation Sample (срез).** ПРЯМОЙ импорт нужного среза (idle/walk/run/crouch) на `SK_Mannequin` — без ретаргета. Полную 1400-библиотеку тащить только если строим motion-matching/BlendSpace.
4. **Emotes** — по желанию, последним.

## Засады
- FBX-импорт ≠ robocopy: нужен правильный целевой скелет в настройках импорта, иначе мусор.
- Motifect-клипы содержат preview-mesh (stickman) — импортировать как **animation**, не как gameplay-mesh.
- Game Anim Sample = ~1400 файлов; **не** булк-импортить целиком без нужды (тяжело + это в основном motion-matching-варианты AimOffset/Avoidance/Crouch).
- Лицензия Motifect: free для personal/commercial, **нельзя перепродавать сырые FBX**.

_Связано: [[anim-skeleton-truth]], [[quantum-compat-skeleton-save]], [[ik-retarget-python]], [[operator-character-anim-system]]._

---

## Source: ANIM_PACKS_SURVEY.md

# 🛠 Аним-паки unrealmonster.com — курированный отчёт под Quantum (UE5-Mann)

> Собрано агентами 2026-06-23 (прогон 32 страниц unrealmonster.com/animation, 930 паков → 267 релевантных).
> Легенда: 🆓 free · 💰 VIP/платно · 🧩 есть готовый AnimBP/система · 🎬 только клипы

## 🥇 БРАТЬ ПЕРВЫМИ (топ-5) — полная локомоция + traversal на Quantum, приоритет готовый AnimBP
1. **TNT Flexible Locomotion System (Multiplayer Ready)** 🆓🧩 — https://unrealmonster.com/tnt-flexible-locomotion-system-multiplayer-ready/ — готовая локомоция-система, MP-ready → прямое попадание в кооп.
2. **Kai Locomotion System (5.0/5.1)** 🆓🧩 — https://unrealmonster.com/kai-locomotion-system-5-0-5-1/ — AnimBP + кастомный CMC, 3 gait + переходы, layer-masking, Data Asset config. Plug-and-play. (нет jump/climb/slide)
3. **Advanced Locomotion Component v1.5 (5.1)** 💰🧩 — https://unrealmonster.com/advanced-locomotion-component-v1-5-5-1/ — walk/run/strafe/jump/fall, **UE4+UE5 Mann** (лучший skeleton-матч), drop-in.
4. **Climb and Vaulting Component v2.0.5 (5.3+)** 💰🧩 — https://unrealmonster.com/climb-and-vaulting-component-v-2-0-5-5-3-5-4/ — climb+vault с детектом препятствий.
5. **Procedural Turn in Place System** 🆓🧩 — https://unrealmonster.com/procedural-turn-in-place-system/ — TIP через IK поверх любого AnimBP.

**Бонус:** **TPS-FPS Character System (5.1)** 🆓🧩 — https://unrealmonster.com/tps-fps-character-system-5-1/ — локомоция + cover + 1/3 камера (оружие игнор).

## 1. Полная локомоция
**С AnimBP:** TNT 🆓 · Kai 🆓 · Advanced Locomotion Component v1.5 💰 (UE4+UE5) · Smart Locomotion 💰 · Motion Matching Male Locomotion Set 💰 (UE5 Mann male, MM+sample AnimBP) · Universal Locomotion 2.0 💰 · Ultimate Movement System 💰
**Клипы:** Male Locomotion Set 💰 (SK_Mannequin+UE4, мужской) · Big Locomotion Pack V04 🆓 (585) · Open World Animset 🆓 (500+: dir walk/jog/sprint+vault/climb/crawl/cover/slide) · 505 Survival Mocap 💰 (UE4+UE5: лок+crouch+hit+revive+carry) · Basic moving set 🆓 · Female Movement Animset Pro 🆓 (женский→ретаргет)

## 2. Traversal / климбинг
**С AnimBP:** Climb&Vaulting Component v2.0.5 💰 · Dynamic Parkour Trace System 💰 (9 категорий, авто-трейс) · Character Climbing 💰 · **Advanced Ladder Climbing System** 💰 (колодцы/чердаки/подвалы!) · Dynamic Wall Run 💰 · Slide Pro 🆓 · Balance Component 53+BP 💰 (трубы/балки/леса)
**Клипы:** ⭐**Adventures of climbing poles and moving heavy objects** 💰 (43: pole/pipe climb + carry heavy, **станд. UE Mann без ретаргета**) · Ultimate Traversal Anims 💰 · MultiAnim Mantling 💰 (60 mantle 0.5–2.5м) · Ledge Animation Set 💰 · Ledge Climb Pack (Motion Warping) 💰 · Advance Cover Animations 🆓 · Stairs Set 💰 · Slide/Zipline 💰 · Parkour Animations 🆓

## 3. Реакции / нокдаун / get-up
⭐**Knocked Down Animation Pack** 🆓 (knockdown+get-up, Epic Mann) · Hit Reaction Pack UE4/UE5 💰 (96, directional, **без ретаргета**) · Simple Bump Reactions 🆓 (32 stumble — толкнуло трубой) · **Severe Injury Animations** 🆓 (переломы/корчи/падение от ранения — газ/ток!) · Injured Pack 💰 (хромота/ползание/вставание) · Death MoCap 🆓 · **Healing Animations** 💰 (revive напарника) · **Dodge It Component** 🆓🧩 (directional dodge/roll BP) · Scared MoCap 💰 (для жителей-жертв)

## 4. Crawl / Prone
Severe Injury 🆓 (ползание) · Lean Lay Sit Stand Bundle 💰 (lay/prone+переходы) · Military Unarmed 💰 (prone/crawl) · General Purpose Pack 💰 (prone+kneel+lean) · MC Danger 💰 (crawl-to-safety)

## 5. Interaction / Carry (ядро ЖКХ)
**С AnimBP:** ⭐**Replicated Interaction Kit Vol 1** 🆓🧩 — https://unrealmonster.com/replicated-interaction-kit-vol-1/ — **вентили/рубильники/цепи + 13 BP + РЕПЛИКАЦИЯ + interaction Widget + IK + 1/3 камера** = буквально ЖКХ + кооп! · Interaction with ALS 3.2.1 💰 (лок+carry/push) · Interaction Toolkit 💰 · Pushable Actor 🆓
**Клипы:** ⭐**Sci-fi Worker Animset** 💰 (120+: repair/welding/drag-cargo) · MoCap Action 💰 (lift/carry/shovel/парные/вышибание двери) · Construction NPC 💰 · Farming&Mining 💰 (лопата/кирка→раскопка к трубе) · Open&Close Pack 💰 (щиток/люк/вентиль) · Simple Activations 💰 (кнопка/рычаг, Epic+IK без ретаргета) · Cleaning 💰 · Spray Painting 🆓 (разметка утечки) · Loot Anim Set 💰 (подбор из ящика)

## 6. ГОТОВЫЙ AnimBP — сводка (приоритет интеграции)
🆓: TNT Locomotion · Kai Locomotion · Procedural TIP · TPS-FPS Character · **Replicated Interaction Kit** · Slide Pro · Dodge It · Daily Life AI (фон-жители)
💰: Advanced Locomotion Component v1.5 · Smart Locomotion · MM Male Locomotion · Universal Locomotion 2.0 · Ultimate Movement · Climb&Vaulting · Dynamic Parkour · Character Climbing · Advanced Ladder · Dynamic Wall Run · Balance Component · Interaction+ALS · Interaction Toolkit · **Swim Component 150+BP** (затопленные подвалы!) · Modular Elevator (лифты подъездов)

## ⚠️ Заметки по интеграции
- **Скелет:** большинство на UE4/UE5 Mannequin → ретаргет на Quantum (UE5-Mann родословная) через IK Retargeter. **БЕЗ ретаргета:** Adventures of climbing poles, Simple Activations, Hit Reaction Pack (UE4/UE5 явно).
- **Female-паки** — только NPC/reference (бригада male-by-design).
- **Combat-паки** — брать только locomotion/reactions-срез, боёвку отбросить.
- Часть URL — листинг-страницы, не пермалинки; искать карточку по названию.

---

# 📦 УСТАНОВЛЕНО + СКАЧАНО — фактический инвентарь (2026-06-24, по живому asset registry + диску)

> Сравнение под НАШ сетап: Quantum-тело (`SK_Male_Quantum_Character_Skeleton`, UE5-Mann родословная) + уже собранный `ABP_Worker` (блендспейс + Speed/Direction + слоты work-монтажей + true-FPS).
> Решающая ось — **скелет**: насколько чисто ложится на Quantum.

## Скелет — классификация установленных паков
- **Нативный Quantum:** `Modular_Workers` (14 анимов: A_MM/A_MF idle/walk/run **только вперёд**). Идеальный скелет, нищее покрытие.
- **Чисто (UE5-Mann / станд. SK_Mannequin → compat/лёгкий ретаргет):** `WorkAnimations` (120; ABP_Manny **ходил ЧИСТО на Quantum — PIE-подтверждено**), `CitizenNPC` (84, SK_Mannequin_UE5), `JKMotion_HitReaction` UE5-половина (194/396), `InteractionKitVol1` (94), `CharacterEmotes`, `StorageUnitsSet` (108 на станд. SK_Mannequin).
- **Ретаргет UE4→UE5 нужен:** `ScifiWorkerAnimset` (146), `AnimStarterPack` (64), `ItemConsumableAnims` (60), `Interaction` (55), `MCO_Mocap_Basics` (39), `KnockedDown` (29), `ZombieAnimationPack`, `Loot_Anim_Set`, `Hospital`, `hazmat`, `Warehouse`, `IndustrialFactory`.
- **❌ Кривой как есть (НЕ брать):** `FreeAnimationLibrary` (101, СВОЯ копия SK_Mannequin → **выворачивает тело на динамике**; это и есть текущий баг локомоции). idle ок, ходьба/страйф крутит.

## Скачано, но НЕ импортировано (стейджинг: `C:\unrealEngine\avariika\анимации\` + `Downloads\`)
Системы (плагин/.uproject): **Kai Locomotion 5.1** (.uplugin), **TNT Flexible Loco** (.uplugin), **Advanced Locomotion Component v1.5** (.uproject, UE4+UE5), **SmartLocomotion 5.2**, **UniversalLocomotion**, **Motion Matching Male Locomotion Set** (UE5 + sample ABP).
Клипы: **Mobility Pro — MoCap Pack** (см. ниже), **505 Survival Mocap**, **BigLocomotionPackV04** (585), **MaleLocomotionSet**, **MoCap Action Animations**.

## ⭐ Mobility Pro — MoCap Pack (детальный разбор)
- **Скелет: UE4_Mannequin_Skeleton** → нужен ретаргет UE4→Quantum (пайплайн у нас рабочий, [[ik-retarget-python]]).
- **475 .uasset.** Покрытие mocap-уровня:
  - `Animation/In-Place` — **219** клипов: Jog/CrouchWalk 8-сторонний + **старты/стопы (to_Stand_Relaxed)** + **повороты на месте 45/90/135/180** + прыжки (Jump) + circle-strafe (CIR) + backpedal (BkPd). **In-Place** = ложится в блендспейс без root-lock костылей.
  - `Animation/Root_Motion` — **219** тех же с root motion (для точного traversal).
  - `Animation/Aim_Offset` — **18** (look 9-way стоя + присев) — под наш «взгляд головой».
  - + `Character/Mesh` (SK_Mannequin UE4) + Source FBX/Maya/Motionbuilder.

## 🥇 ВЕРДИКТ: Mobility Pro
Под наш сетап — лучший, потому что у нас **уже собран рабочий ABP**, сломаны только клипы (FBL выворачивает). Mobility Pro = заменить клипы на mocap-качество без переписывания графа:
- In-Place варианты → блендспейс без костылей.
- Покрытие на годы (повороты/старты/стопы/аим).
- Цена — один ретаргет UE4→Quantum.
**Kai** — лучший по технологии (motion-matching + warping убирает скольжение by design), но это интеграция целой системы с нашим кастомным true-FPS персонажем = отдельная большая сессия. Держать как «вылизать движение позже».
**Motion Matching Male** — альтернатива на UE5-скелете (чище ретаргет), если захотим MM-путь.

## Не-локомоция — топ по категориям (уже установлено)
- **Work/ремонт (ядро ЖКХ):** `WorkAnimations` (стоит) + `ScifiWorkerAnimset` (146: welding/repair/drag-cargo).
- **Интеракция вентили/рубильники:** `InteractionKitVol1` (94, **репликация → кооп**).
- **Реакции (ток/газ/удар):** `JKMotion_HitReaction` (**396** directional) + `KnockedDown` (29).
- **Ящики/лут:** `StorageUnitsSet` (324) + `Loot_Anim_Set` (37).

---

## Source: LOCOMOTION_PLAN.md

# LOCOMOTION_PLAN — мокап-локомоция Quantum (Mobility Pro)

> 2026-06-24, вечер. Цель — «как в демо mocaponline Mobility»: спокойная ходьба, idle-вариации стоя, 8 направлений, плавность.
> Все 220 клипов Mobility Pro УЖЕ в проекте: `Content/Avariika/Anim/Locomotion/Mobility/` (`RT_MOB1_M1_*`). Докупать ничего не надо.

## ✅ Сделано (этой сессией)
- **T-поза устранена** — причина была не в compat-скелетах, а в скелет-хопе анимграфа: `ABP_Worker` переведён на скелет **WorkAnim** (как клипы), юзер вшил `BS_Loco_Mobility`. Цепочка нативна: ABP→BS→клипы→тело Quantum.
- **Сглаживание блендспейса**: `target_weight_interpolation_speed_per_sec = 6.0` (было 0 = щелчки) на `BS_Loco_Mobility` + `BS_Crouch`.
- **Скорости под блендспейс** (BP_AvaryoCharacter): `BaseWalkSpeed 500→150`, `SprintSpeed 750→450` — совпали с разметкой блендспейса (walk=150, jog=450) → нет скольжения ног, спокойнее. (Жалоба «W = лёгкий бег» была на старой 500.)
- **Бег (Shift) только вперёд** (C++ `RefreshMoveSpeed` + Tick): спринт активен только в конусе ±50° вокруг «вперёд», вбок/назад — обычная ходьба. Собрано (Build Succeeded), активно.
- **Фикс сборки**: плагин `TriangleCountViewMode` не компилился из исходников на пути с `\u` (`C:\unrealEngine\...`) — `Build.cs` определял `TRICOUNT_PLUGIN_SHADER_DIR` некавыченным путём с бэкслэшами → `\u`=escape → C4429, что **блокировало любую C++-сборку**. Закавычил + прямые слэши. (Плагин в gitignore — при переустановке фикс слетит.)

## 📋 Осталось (нужны глаза/редактор — не делать вслепую)

### 1. 8 направлений в блендспейсе (сейчас 4 → diagonals для плавности «как в демо»)
`BS_Loco_Mobility` сейчас: idle(0,0) + Walk×4 (F/L/R/B @Y=150) + Jog×4 (@Y=450). Добавить диагонали в редакторе (он корректно триангулирует; питон `sample_data` — read-only-риск, не делал). Разметка X=Direction°, Y=Speed:
- **Walk-ряд (Y=150):** 0=`Walk_F`, +45=`Walk_FR_Loop`, +90=`Walk_R`, +135=`Walk_BR_BkPd_Loop`, ±180=`Walk_B`, −45=`Walk_FL_Loop`, −90=`Walk_L`, −135=`Walk_BL_BkPd_Loop`.
- **Jog-ряд (Y=450):** те же 8 с `Jog_*` (FR/FL/BR/BL = `Jog_*_Loop`).
- (Опц.) добавить **Run** как третий ряд (Y≈600, клипы `Run_F/L/R…`) и поднять SprintSpeed под него — три темпа Walk<Jog<Run.

### 2. Idle-вариации стоя (поворот головы/разминка/переступ) — ГЛАВНОЕ по запросу
Клипы есть: **`Stand_Relaxed_Fgt_v1..v4`** (фиджеты) + `Stand_Relaxed_Conv_v1..v4` (жесты). 
⚠️ **Блокер:** `ABP_Worker` — голый (одна нода BlendSpacePlayer, без стейт-машины и без слота). Рандомные idle через Python не вшить (граф анимблюпринта недоступен из кода). Варианты:
- **(реком.)** Построить в `ABP_Worker` стейт-машину: `Idle` (Random Sequence Player по Fgt_v1..v4 + базовый Idle) ↔ `Locomotion` (BS_Loco_Mobility) ↔ старты/стопы. Это и idle-вариации, и «демо-качество». Делаем в редакторе с тобой.
- Либо взять готовый локомоция-ABP со стейт-машиной (`ABP_Manny` из WorkAnimations — на том же скелете) и перенацелить его блендспейсы на Mobility-клипы.

### 3. Облёт-камера на ПКМ (осмотр на ходу)
Движение/поворот тела — **в Blueprint** (Enhanced Input в EventGraph BP_AvaryoCharacter), в C++ их нет. Чтобы при облёте персонаж шёл прямо (а не закручивался за камерой), нужна правка BP-движения (декапл ввода от камеры) — это с тобой (риск ломать BP-граф вслепую). Пока для осмотра: **`V`** = 3-е лицо (за спиной), **`F8`** = выпрыгнуть и свободно облететь камерой (стоя).

### 4. Подтвердить визуально (твои глаза)
- W при 150 = спокойная ходьба (не бег)?
- Сглаживание 6.0 — ок / рвано / поплыло (тюнится)?
- Бег только вперёд — работает?

## Заметка
`ABP_Worker` = минимальный (BlendSpacePlayer→Output, без DefaultSlot) → **work-монтажи** (`M_Work_Fixing` и т.п.) тоже не играют — тот же блокер (нужен слот в графе). Решается той же стейт-машиной/слотом.

---

## 🏗 ПОЛНАЯ АРХИТЕКТУРА ЛОКОМОЦИИ (Mobility Pro 2.7B — весь каталог у нас локально)

Юзер прислал полный список пака (2026-06-24). Это AAA-набор → нужна **стейт-машина**, голый блендспейс не тянет. Скоуп под наш true-FPS репейр-кооп (прыжки/смерти — опционально/через нашу систему):

**EventGraph (значения для графа):** Speed=`VectorLength(Velocity)` (XY); Direction=`CalculateDirection(Velocity, ActorRot)`; bIsMoving; bShouldSprint (наш forward-gate); bIsCrouched; AimPitch/AimYaw (для aim-offset); StartDirection (угол старта).

**State Machine «Locomotion» (Stand-слой):**
1. **Idle** — `Stand_Relaxed_Idle` (база) + рандом фиджеты `Stand_Relaxed_Fgt_v1..v4` (Random Sequence Player по таймеру простоя) + опц. `Conv_v1..v4`. Вход из Stop.
2. **Turn-In-Place** (true-FPS: тело доворачивает за камерой) — из Idle когда |yaw тела − камера|>45°: `Stand_Relaxed_L/R_45/90/135/180` (+ `Turn_In_Place_L/R_Loop`).
3. **Start** — Idle→движение: `Stand_Relaxed_To_Walk/Jog/Run_{F/B/L/R}` и угловые `_To_Walk_L45..R180_Fwd` по StartDirection.
4. **Locomotion (Move)** — 2D блендспейс Direction(X −180..180) × Speed(Y): ряды **Walk@150 / Jog@450 / Run@~600**, по 8 направлений (F/FL/FR/L/R/B + бэкпеды `_BkPd_Loop` для BL/BR/L/R-назад). Это расширение текущего `BS_Loco_Mobility`.
5. **Stop** — Move→Idle: `*_To_Stand_Relaxed` (+ `_LU/_RU` по опорной ноге).
6. (опц.) **Jump** — split-джампы Start/Air/Land по темпу/направлению.

**Crouch-слой (параллельная SM, по bIsCrouched):** `Crouch_Idle(_V2)` + повороты `Crouch_L/R_45..180` + 2D `CrouchWalk` 8-way (`CrouchWalk_*_Loop`/`_BkPd`) + переходы `Crouch_To_CrouchWalk_*`/`*_To_Crouch`.

**Aim Offset (поверх всего, head/torso look):** ассет из `Stand_Relaxed_Look_{Center/U90/D90/L90/R90/диагонали}` (+ Crouch_Look_*), драйв AimPitch/AimYaw — взгляд по камере (важно для кооп-читаемости).

**Death:** `Stand_Relaxed_Death_{F/B/L/R}` — подвязать к нашей системе смерти/нокдауна (directional).

### Деление работы
- **Claude может (данные, верифицируемо):** 2D блендспейсы (Walk/Jog/Run + Crouch) c корректной разметкой; AimOffset-ассет; организация клипов. ⚠️ сэмплы через питон — риск триангуляции, после правки «открыть+Ctrl+S».
- **Только в редакторе (граф не скриптуется надёжно):** сама стейт-машина (стейты/переходы/условия), Random-idle, слой Aim Offset, слот для work-монтажей. Строим вместе (мои инструкции + твои глаза) ИЛИ ты строишь, я готовлю ассеты + верифицирую.

---

## 🎯 РЕЦЕПТ: ПОВОРОТ ГОЛОВЫ (aim offset + free-look + turn-in-place) — co-build в редакторе

Данные готовы: `AO_Stand_Look`, `AO_Crouch_Look` (в `/Game/Avariika/Anim/Locomotion/`), aim-позы в `…/Mobility/Aim/`.

**Этап 1 — голова/корпус смотрят по камере (aim offset):**
1. EventGraph `ABP_Worker`: считать **AimYaw** = `NormalizedDeltaRotator(GetControlRotation(owner), GetActorRotation(owner)).Yaw` (−180..180) и **AimPitch** = control pitch (−90..90). Set в переменные (как Speed/Direction).
2. AnimGraph: после локомоции — нода **Aim Offset 'AO_Stand_Look'** (X=AimYaw, Y=AimPitch) → **Layered blend per bone** поверх позы локомоции, маска от **spine_01** (вес 1) с blend depth ~3 (плечи/шея/голова смотрят, ноги нет). 
3. Чтобы тело НЕ снапилось мгновенно к камере: на пешке/в BeginPlay **`bUseControllerRotationYaw = false`** (сейчас true → тело крутится за камерой). Тогда камера водит взгляд, тело стоит → aim offset отрабатывает.

**Этап 2 — turn-in-place (доворот тела на ~90°):**
4. Когда `abs(AimYaw) > 90` (стоя, не двигаясь): проиграть `Stand_Relaxed_{L/R}_{45/90/135/180}` (есть 4 из 8 — доретаргетить остальные), по ходу **доворачивать ActorRotation** к камере (или Root Motion поворота), и AimYaw уменьшается к 0.
5. Реализация — стейт `TurnInPlace` в стейт-машине, вход по порогу AimYaw + bIsMoving==false, выход когда довернулись.

**Этап 3 — при движении** тело ориентируется по направлению движения/камере как обычно (bUseControllerRotationYaw обратно true в Move-стейте, либо OrientRotationToMovement) — взгляд остаётся через aim offset.

## 🎯 РЕЦЕПТ: СТЕЙТ-МАШИНА (idle-фиджеты, старты/стопы, крауч-слой) — co-build
- **AnimGraph:** `Output ← Layered blend per bone(AimOffset поверх) ← StateMachine "Locomotion" ← Blend Poses by bool(bIsCrouched)[Stand-SM / Crouch-SM]`.
- **Stand-SM:** `Idle` (Stand_Relaxed_Idle + рандом `Fgt_v1..v4` по таймеру простоя) → `Start` (`Stand_To_Walk/Jog_*`) → `Move` (`BS_Loco_Mobility`) → `Stop` (`*_To_Stand_Relaxed`) → Idle. + `TurnInPlace`.
- **Crouch-SM:** `Crouch_Idle` + `CrouchWalk` (`BS_CrouchWalk_Mobility`), переходы по Speed.
- **Слот для work-монтажей:** `DefaultSlot` между локомоцией и Output (чтобы ремонт-анимы `M_Work_*` играли).
- Рандом-idle: переменная `FidgetIndex` (рандом по таймеру) → `Blend Poses by int` среди Fgt_v1..v4 в idle-стейте, или суб-стейты.

---

## ▶ ШАГИ НА ВОЗВРАТ ЮЗЕРА (поворот головы — head-only) — автоном-подготовка 2026-06-24

**Что готово (автоном):** `AO_Head_Aim` (стоя) + `AO_Crouch_Head` (присед) — рабочие aim-offset (9 поз: бок/верх/низ/диагонали, аддитивные, оси −90..90; превью подтверждено юзером — голова смотрит во все стороны). В AnimGraph `ABP_Worker` **head-only разводка уже сделана**: `Локомоция → Layered blend per bone(neck_01) → Выход`, `AimOffset → Layered.BlendPose0` (подменяется только шея/голова, корпус/руки в локомоции). EventGraph считает AimYaw/AimPitch.

**ОСТАЛОСЬ (твои 1-2 клика, я не могу выставить проперти ноды скриптом):**
1. В AnimGraph кликни ноду **AimOffset Player** → Details → свойство **Aim Offset / Blend Space** → смени со старого `AO_Stand_Look` на **`AO_Head_Aim`**. (Старый AO_Stand_Look — битый, не аддитивный; AO_Head_Aim — рабочий.) **Compile + Save.**
2. Напиши мне «перецепил» — я включу **free-look** (`bUseControllerRotationYaw=false` в C++ BeginPlay) → проверим поворот головы в 3-м лице (бок/верх/низ/диагональ, только голова).
   - ⚠️ С free-look тело пока статично (не доворачивается) — это нормально ДО turn-in-place.

**ДАЛЬШЕ (co-build стейт-машины):**
3. **turn-in-place** — тело догоняет камеру на ~90° (клипы `Stand_Relaxed_L/R_45/90/135/180` — 4 из 8 ретаргечены, доретаргечу остальные). Тогда free-look + turn-in-place = твой каскад голова→корпус→ноги.
4. **idle-фиджеты** (`Stand_Relaxed_Fgt_v1..v4`) — стейт Idle с рандом-сменой.
5. **крауч-слой** (`BS_CrouchWalk_Mobility` + `AO_Crouch_Head`) через Blend Poses by bool(IsCrouched).
6. **слот DefaultSlot** в граф — чтобы заиграли work-монтажи (`M_Work_*`).

🔑 Битый `AO_Stand_Look` после перецепки можно удалить (не ссылается). Запасной 4-way блендспейс — `BS_Loco_Mobility_4way_bak`.

_Связано: [[plugin-anim-graph-editing]], [[tpose-skeleton-hop-anim-graph]], [[quantum-compat-skeleton-save]], [[anim-skeleton-truth]]._

---

## Source: LOCO_BUILD_SHEET.md

# Локомоция — build-лист (готовим стейт-машину утром)

Скелет: **WorkAnim** (UE5-mann). Все клипы ниже — `/Game/Avariika/Anim/Locomotion/Mobility/RT_MOB1_M1_<имя>_IP`.
ABP: `/Game/Avariika/Anim/Locomotion/ABP_Worker`. Блендспейсы готовы: `BS_Loco_Mobility` (стоя 8-way),
`BS_CrouchWalk_Mobility` (присед 8-way). Голова-аим (`AO_Head_Aim`) уже в графе после Layered blend.

Правило сборки: **[ТЫ]** = перетащить/создать ноду (плагин анимноды не добавляет),
**[Я]** = подключу провода через плагин (connect_nodes работает). Переменные уже есть:
`Speed:int`, `Direction:int`, `IsCrouched:bool`, `AimYaw/AimPitch:real`.

Нужны ещё переменные (добавлю сам или ты): `IsInAir:bool`, `IsAccelerating:bool` (для стартов/стопов),
`Speed2D:float` (по желанию, для порогов).

---

## Очередь фаз (каждая — отдельный маленький проверяемый шаг)

### Фаза 1 — Idle↔Move + «живой» idle  ⭐ начинаем с неё
Цель: вместо «голой» BS_Loco — стейт-машина с дышащим idle и переходом в движение.

Состояния SM **`Locomotion`**:
- **Idle**
  - основной клип: `Stand_Relaxed_Idle` (loop)
  - idle-break (раз в ~8–14с, рандомом): `Stand_Relaxed_Fgt_v1..v4` (мелкое переминание) и
    `Stand_Relaxed_Conv_v1..v4` (крупнее). Реализация: вложенный «Random» сиквенс-плеер ИЛИ
    мини-SM Idle→IdleBreak→Idle по таймеру.
- **Move**
  - нода `BS_Loco_Mobility` (Direction←Direction, Speed←Speed)
- Переходы: Idle→Move при `Speed > 10`; Move→Idle при `Speed < 10`.

Шаги:
1. [ТЫ] В AnimGraph правой кнопкой → **Add New State Machine**, назови `Locomotion`. Открой её.
2. [ТЫ] Внутри: добавь состояния **Idle** и **Move**. Соедини `Entry→Idle`.
3. [ТЫ] В **Move** перетащи `BS_Loco_Mobility`. В **Idle** — `Stand_Relaxed_Idle` (loop on).
4. [ТЫ] Стрелки Idle↔Move (2 перехода).
5. [Я] Подключу Direction/Speed в BS_Loco, и пины переходов (Speed>10 / <10) — если выйдет через
   connect_nodes; правила-условия (compare) при необходимости [ТЫ] кинешь Compare-ноду, [Я] свяжу.
6. [ТЫ] Выход SM `Locomotion` → во вход `Layered blend per bone [Base Pose]` (вместо текущего BS_Loco).
   (BS_Loco тогда живёт уже внутри Move, а не напрямую.)
7. **Проверка:** стоишь — лёгкое дыхание/переминание; пошёл — плавный переход в ход.

### Фаза 2 — Crouch (Blend Poses by bool)
- [ТЫ] Нода **Blend Poses by Bool**: True=присед, False=стойка.
- True-вход: мини-SM присед — **Crouch_Idle** ↔ `BS_CrouchWalk_Mobility` (те же Speed/Direction, порог 10).
- False-вход: SM `Locomotion` из Фазы 1.
- bool ← `IsCrouched` (уже есть, кормится из C++ `bIsCrouched`).
- [Я] подключу bool + оба входа. Проверка: Ctrl приседает, ходьба в приседе работает.

### Фаза 3 — Jump / In-Air
- Переменная `IsInAir` ← из C++ (`GetCharacterMovement()->IsFalling()`), [Я] добавлю геттер в AnimBP EventGraph.
- [ТЫ] **Blend Poses by Bool** (или состояние в SM): False=земля (Фаза1/2), True=воздух.
- Воздух — мини-SM: **JumpStart** (`Stand_Relaxed_Jump`) → **InAir** (apex/петля паданья) → **Land** (приземление).
  Directional при желании: `Walk_F_Jump/Jog_R_Jump/...` по направлению.
- [Я] свяжу IsInAir + переходы по `Time Remaining`/`IsFalling`.

### Фаза 4 — Старты/остановки + Turn-in-place (полировка, demo-quality)
- **Старты:** Idle→Move через состояние **Start**, клип по углу разгона:
  `Stand_Relaxed_to_Walk_F/L/R/B` + `_to_Walk_L45/L90/...` (выбор по Direction в момент старта).
- **Остановки:** Move→Idle через **Stop**, клип по направлению хода:
  `Walk_F_to_Stand_Relaxed` (+`_LU/_RU` план-фут варианты).
- **Turn-in-place** (тут оживёт голова-аим вбок!) — **требует Фазу B1**:
  - [Я] в C++ BeginPlay: `bUseControllerRotationYaw=false` (отвязать корпус от камеры).
  - Доворот корпуса: когда стоим и `|AimYaw|` перевалил ~70° — проигрываем
    `Stand_Relaxed_R_90`/`L90` (или `_45/_135/_180`) и доворачиваем root по кривой/root-motion,
    `AimYaw` гасится к 0. Голова-аим (уже собран) закрывает остаток до доворота корпуса.
  - Порядок каскада: голова (0–70°) → корпус (`R_90`/`L90`) → ноги доступают. То, что ты описывал.

---

## Карта клипов → состояния (всё уже отретаргечено, 219 шт.)

| Состояние | Клипы (RT_MOB1_M1_…_IP) |
|-----------|--------------------------|
| Idle | `Stand_Relaxed_Idle`, `_Idle_v2` |
| Idle-break | `Stand_Relaxed_Fgt_v1..v4`, `Stand_Relaxed_Conv_v1..v4` |
| Move (стоя) | `BS_Loco_Mobility` (8-way) |
| Старты | `Stand_Relaxed_to_Walk/Jog/Run_F/B/L/R` + `_L45/L90/L135/L180`, `_R45..R180` |
| Остановки | `Walk/Jog/Run_F/B/L/R_to_Stand_Relaxed` (+`_LU/_RU`) |
| Поворот на месте | `Stand_Relaxed_L45/L90/L135/L180`, `_R_45/R_90/R_135/R_180` |
| Прыжок (стоя) | `Stand_Relaxed_Jump` (+`_F/B/L/R`), `Stand_Relaxed_Hop*` |
| Прыжок (в движении) | `Walk/Jog/Run_X_Jump` (+`_LU/_RU`) |
| Crouch idle | `Crouch_Idle`, `_V2` |
| Crouch move | `BS_CrouchWalk_Mobility` (8-way) |
| Crouch повороты/старты | `Crouch_L/R_45..180`, `Crouch_to_CrouchWalk_*` |
| Смерть/нокдаун (на будущее) | `Stand_Relaxed_Death_F/B/L/R` |

## Найдено при ревизии (решения — за тобой, утром)
- **Скорости — В ПОРЯДКЕ.** BP CDO `BP_AvaryoCharacter` = `BaseWalkSpeed 225 / SprintSpeed 540`
  (проверено питоном). Это попадает между семплами блендспейса (walk150/jog450) → нормальная медленная
  ходьба. Конструктор C++ (500/750) — лишь дефолт, Blueprint его переопределяет. _Менять не надо._
- **Кастомизация:** в MP на клиенте не применится (HUD зовёт компонент напрямую; нужен Server RPC) — для соло ок.
- **Кастомизация:** нет скролла длинных списков и нет клавиши открытия (только `AvCustomize`) — повесить клавишу утром.

---

## Source: STATE_MACHINE_BUILD.md

# STATE_MACHINE_BUILD — сборка локомоции (co-build: ты кидаешь ноды, я соединяю)

> Метод проверен на aim-offset: **ты перетаскиваешь ноды по списку → я соединяю через плагин (connect_nodes) + настраиваю ассеты**. Плагин НЕ умеет добавлять анимноды/ставить проперти нод — поэтому добавление и проперти за тобой, разводка за мной.
> Все ДАННЫЕ готовы: 221 клип, `BS_Loco_Mobility`(8-way), `BS_CrouchWalk_Mobility`, `AO_Head_Aim`/`AO_Crouch_Head`, фиджеты `Fgt_v1..v4`, прыжки (Start/Air/Land), старты/стопы.

## Целевая структура AnimGraph
```
StateMachine "Locomotion"  ──► Blend(Crouch by IsCrouched) ──► Blend(Jump by IsInAir) ──► AimOffset(голова) ──► [Slot DefaultSlot] ──► Выход
```
Делаем ФАЗАМИ, каждую проверяем в игре, потом следующая. Не пытаемся всё разом.

## EventGraph — добавить переменные (как делали Speed/AimYaw)
- `IsCrouched` (bool) ← `Get Is Crouched` от пешки (есть готовый bool на ACharacter).
- `IsInAir`/`IsFalling` (bool) ← `GetMovementComponent → IsFalling`.
- (Speed/Direction/AimYaw/AimPitch уже есть.)

---
## ФАЗА 1 — Idle ↔ Move (ядро, даёт чистый простой/движение)
**Ты добавляешь:**
1. ПКМ в AnimGraph (пустое место) → **Add New State Machine** → имя `Locomotion`. Двойной клик — войти внутрь.
2. Внутри: ПКМ → **Add State** ×2 → имена `Idle` и `Move`.
3. В стейт `Move`: двойной клик → перетащи **`BS_Loco_Mobility`** (создаст BlendSpace player). Выйди.
4. В стейт `Idle`: перетащи **`RT_MOB1_M1_Stand_Relaxed_Idle_IP`** (Sequence Player, галка Loop).
5. Протяни стрелки: **Entry → Idle**, **Idle → Move**, **Move → Idle** (просто тяни от кружка стейта к стейту).

**Я делаю:** в `Move` соединю Direction→X, Speed→Y; условия переходов (Idle→Move: Speed>5; Move→Idle: Speed<5); вывод StateMachine → в текущую цепочку (туда, где сейчас BS_Loco_Mobility входит в AimOffset.BasePose); старый прямой BlendSpacePlayer уберём.

## ФАЗА 1b — фиджеты стоя
**Ты:** внутри `Idle` стейта вместо простого Sequence Player сделаем мини-стейт-машину или Blend Poses by Int. Распишу пошагово на месте (рандом idle — без встроенной ноды, делаем через таймер+рандом-индекс). Клипы `Fgt_v1..v4` готовы.

## ФАЗА 2 — Crouch
**Ты:** ПКМ → **Blend Poses by bool** (одна нода); перетащи `BS_CrouchWalk_Mobility` (BlendSpace player); перетащи `Get IsCrouched`.
**Я:** соединю — False=StateMachine Locomotion, True=Crouch-блендспейс, bool=IsCrouched; X/Y крауча = Direction/Speed.

## ФАЗА 3 — Jump
**Ты:** ПКМ → **Blend Poses by bool** (IsInAir); перетащи прыжковые клипы (Start/Air/Land — лучше мини-SM: Start→Air(loop)→Land).
**Я:** соединю + условия по IsFalling.

## ФАЗА 4 — старты/стопы + turn-in-place (полиш)
- Старт: `Stand_Relaxed_To_Walk/Jog_*` при Idle→Move (доб. стейт Start).
- Turn-in-place: при |AimYaw|>90 стоя — `Stand_Relaxed_R_*` (правые есть; левые через **Mirror-ноду** + `MDT_Mannequin`). ⚠️ MirrorDataTable автономно не создалась (API), создадим в редакторе на этой фазе: ПКМ в Content → Animation → Mirror Data Table → SK_Mannequin → авто-правила `_l`/`_r`.

## ФАЗА 5 — слот work-монтажей
**Ты:** ПКМ → **Slot 'DefaultSlot'** между AimOffset и Выходом.
**Я:** соединю → заиграют `M_Work_*` (ремонт/спрей и т.д.).

---
**Порядок захода:** Фаза 1 → проверка в игре → 1b → 2 → 3 → 4 → 5. Каждая фаза = пара минут (ты кидаешь 2-5 нод, я соединяю). Начинаем с твоего «готов к фазе 1».

_Связано: [[plugin-anim-graph-editing]], LOCOMOTION_PLAN.md_

---

## Source: TURN_IN_PLACE_BUILD_SHEET.md

# Turn-in-place — build-sheet (натуральный шагающий поворот тела за головой)

**Цель:** стоя поворачиваешь камеру → голова аимится (уже есть), а за порогом тело **переступает ногами и доворачивается** плавно, в ОБЕ стороны. Заменяет текущий рывковый/медленный C++-триггер монтажей.

Скелет: **WorkAnim** (UE5-mann). ABP: `/Game/Avariika/Anim/Locomotion/ABP_Worker`.
Правило сборки: **[ТЫ]** = перетащить/создать ноду в редакторе, **[Я]** = подключаю через плагин (`connect_nodes`) + C++.

---

## Почему текущее плохо (диагноз, чтобы не повторить)

`AAvaryoCharacter::UpdateTurnInPlace` (C++) + `PickTurnClip` → монтажи `M_Turn_R_*`:
1. Тело ждёт, пока камера пройдёт `TurnInPlaceStartAngle`(80°) **И остановится** (гейт `YawRate<150`, иначе монтаж пере-триггерится → заикание) → **задержка + «медленно»**.
2. 3 дискретных правых монтажа (90/135/180), ~2–2.6с каждый → грубые ступени + промах по углу → **рывки**.
3. 🔴 **Левые не работают** (`PickTurnClip` для `<0` → `nullptr`).
4. Принципиальный потолок монтаж-триггера — плавным во время вращения он быть не может.

**Вывод:** уносим поворот стоя из C++-монтажей в **анимграф (стейт-машина + root-motion)** + рантайм-`Mirror` для левых. Текущий C++-блок (монтажи) **снять**.

---

## Ассеты (что есть / чего нет)

- ✅ **Шагающие повороты с root-motion** (живой root-поворот, переступание):
  `…/Mobility/RootMotion/RT_MOB1_M1_Stand_Relaxed_R_45 / R_90 / R_135 / R_180` (есть и `_46/91/136/181` — альт-дубли, игнор).
  🔑 Брать ИМЕННО RootMotion-вариант (`enable_root_motion=True`, root d=45/90/135/180°). **НЕ** `*_IP` (root=0, мёртвые — отсюда прошлый провал).
- 🔴 **Левых нет** → зеркалить рантайм-`Mirror`-нодой (питон-бейк зеркала недружелюбен, см. WORKLOG).
- 🔴 **Нет `MirrorDataTable`** под WorkAnim-скелет → создать (Фаза T1).
- ⚠️ Старые монтажи `M_Turn_R_*` (в `/Game/` корне) + `*_IP` turn-клипы — после перехода **осиротеют**, удалить.

---

## Архитектура (рекомендуемая)

```
[Idle-стейт]  ──(|AimYaw|>порог && стоим && камера успокоилась)──►  [TurnRight / TurnLeft]
   ▲                                                                      │
   └──────────────(анима почти доиграла ИЛИ |AimYaw|→0)──────────────────┘

TurnRight/Left = play RootMotion-turn-клип (выбор по |AimYaw|) →
   root-motion ЙОУ крутит капсулу (CharacterMovement) → ActorYaw догоняет камеру →
   AimYaw падает → голова-аим возвращается в центр. Каскад «голова→тело», как в жизни.
Левый = тот же правый клип через Mirror-ноду (bMirror = AimYaw<0).
```

🔑 **Root Motion Mode ABP:** сейчас «Root Motion from Montages Only». Для root-motion ИЗ стейт-машины нужен **«Root Motion from Everything»**. Безопасно: локомоция на `*_IP` (root=0) → нулевой вклад; только turn-клипы дают поворот. ⚠️ Проверить, что движение (оно в BP) не конфликтует — поворот случается стоя (velocity≈0), там BP не крутит.

---

## Фазы (каждая — маленький проверяемый шаг)

### T0 — снять старое (C++)
- [Я] В `UpdateTurnInPlace` убрать стоячий монтаж-блок (`PickTurnClip`, `M_Turn`, `bTurnMontageActive`, `TurnMontageEndTime`). Оставить: фид `AimYaw` (он в EventGraph), доворот В ДВИЖЕНИИ (`FixedTurn`, velocity>10) — он ок. Снять `bUseControllerRotationYaw` остаётся false (free-look).
- [Я] Добавить экспорт в ABP: `bool bIsTurning`/порог — по необходимости (или всё решает анимграф по AimYaw).

### T1 — Mirror-инфраструктура
- [ТЫ] Создать **`MirrorDataTable`** (Animation → Mirror Data Table) на WorkAnim-скелет → Auto-map (`_l`↔`_r`, `Right`↔`Left`). Проверить root/pelvis маппинг.
- [ТЫ] Включить `enable_root_motion` на 4 RootMotion-turn-клипах, если не стоит (проверить в ассете).

### T2 — Root Motion Mode + проверка
- [ТЫ] ABP_Worker → Class Settings → **Root Motion Mode = «Root Motion from Everything»**.
- [ТЫ→PIE] Проверить: обычная ходьба/idle НЕ поехала (IP-клипы root=0). Если поехала — стоп, разбираемся (значит какой-то клип не IP).

### T3 — стейт-машина поворота (в анимграфе)
- [ТЫ] Внутри Idle-стейта (или отдельной SM перед Slot) добавить состояния **TurnRight**, **TurnLeft** + переходы из/в Idle.
- [ТЫ] В TurnRight положить **Blend Poses by int** (или мини-выбор) по «индексу угла» → клипы R_45/90/135/180. Аналогично TurnLeft (те же клипы, но через Mirror-ноду).
- [ТЫ] Кинуть **`Mirror`**-ноду (Animation → Mirror) с твоим MirrorDataTable; `bMirror` ← bool из AimYaw<0.
- [Я] Подключу: AimYaw/индекс-угла → выбор клипа; условия переходов (`|AimYaw|>порог` → Turn; `GetRelevantAnimTimeRemaining<0.1` ИЛИ `|AimYaw|<10` → Idle).
- 🔑 Выбор клипа по углу: индекс = (|AimYaw|≥157?180 : ≥112?135 : ≥67?90 : 45). Голова-аим закрывает разницу шага и точного угла (как сейчас).

### T4 — гейт/триггер (C++ ↔ анимграф)
- [Я] C++ отдаёт `AimYaw` (есть) + опц. `bTurnAllowed` (стоим И камера успокоилась — анти-стартер-дёрг). Анимграф сам решает по AimYaw.
- 🔑 Анти-заикание теперь НЕ нужно гейтить жёстко: стейт-машина играет клип целиком (root-motion), не пере-триггерит покадрово. Можно мягкий порог входа + гистерезис на выход.

### T5 — полиш
- Foot-lock (без проскальзывания) при повороте — если ноги едут, добавить foot-lock/IK (стыкуется с будущей Foot-IK террейн-фичей).
- Тюнинг порога входа (70–90°), скорости (play rate клипа), гистерезиса.
- Голова-аим: убедиться, что во время доворота тела aim плавно гаснет (AimYaw→0).

---

## Карта углов → клипы

| \|AimYaw\| | Клип (RootMotion, право; лево = тот же через Mirror) |
|-----------|------------------------------------------------------|
| 67–112°   | `RT_MOB1_M1_Stand_Relaxed_R_90` |
| 112–157°  | `RT_MOB1_M1_Stand_Relaxed_R_135` |
| ≥157°     | `RT_MOB1_M1_Stand_Relaxed_R_180` |
| 45–67° (опц.) | `RT_MOB1_M1_Stand_Relaxed_R_45` |

Порог ВХОДА в поворот ~70° (ниже — только голова-аим). Выход — по Time Remaining клипа или `|AimYaw|<10`.

---

## Решения / гочи
- 🔑 RootMotion-варианты, НЕ `*_IP`. `enable_root_motion=True`.
- 🔑 Root Motion Mode = «from Everything»; IP-локомоция = нулевой root (проверить в T2).
- 🔑 Левые = рантайм-`Mirror`+`MirrorDataTable` (НЕ питон-бейк).
- 🔑 Стейт-машина вместо C++-монтажей убирает пере-триггер-заикание и гейт «камера стоит».
- ⚠️ `bUseControllerRotationYaw=false` (free-look) — оставить; иначе тело снапится к камере и turn-in-place не нужен/не виден.
- ⚠️ После перехода: снести `M_Turn_R_*` (корень `/Game/`), `*_IP` turn-клипы, мёртвый `PickTurnClip` в C++.
- 🔑 **Durable:** система переживёт замену клипов на мокап (мокап-turn-клипы встанут в те же слоты).

---

## Потом (отдельный кусок) — Idle-вариации стоя
Когда turn-in-place готов: рандом-idle-брейки + оглядывания головой стоя. Клипы в паке есть:
- Переминания: `RT_MOB1_M1_Stand_Relaxed_Fgt_v1..v4`, `…_Conv_v1..v4`.
- Оглядывания: aim Look-позы `…/Aim/RT_MOB1_M1_Look_*` (Stand) — уже ретаргечены.
Реализация: внутри Idle-стейта мини-SM Idle→IdleBreak→Idle по таймеру (рандом), как было намечено в `LOCO_BUILD_SHEET.md` Фаза 1.

---

## Source: MODULAR_WORKER_PLAN.md

# MODULAR_WORKER_PLAN — интеграция Modular Workers (Quantum) как персонажа-аватара

> 2026-06-23. Юзер: «modular worker bundle уже в проекте, давай полностью интегрировать». Сегодня юзер отдыхает (без стройки) — работаю автономно. Этот док — грунт + фазовый план; правки по ходу.

## Что за пак (факты разведки)
- **`Content/Modular_Workers/`** — модульный «Quantum» рабочий, **874 ассета**. ✅ **ЛИЦЕНЗИОННЫЙ** (юзер купил за $50 — вариант B «Modular Workers Bundle» из [[char-creator-gear-progression]], НЕ generic) → release-safe, НЕ плейсхолдер. **Заменяет пиратский CitizenNPC** как тело игрока/бригады.
- **Только мужские меши — это by design** (юзер подтвердил: женщин-работяг не нужно). Female-текстуры в паке игнорируем. Бригада = мужики.
- **Скелет: `SK_Male_Quantum_Character_Skeleton` = UE5-mannequin** (кости `spine_01..05`, `neck_01/02`, `upperarm_twist_01/02`, `ik_foot_root`, `ik_hand_gun`, `index_01`, `thumb_01`). + `SK_Quantuym_FaceRig` (лицо).
- **Сборка** (`Demo/Blueprint/CBP_Male_Quantum_Character`): тело-лидер + дочерние SkeletalMesh-компоненты по слотам через `SetMasterPoseComponent` (в 5.7 = `SetLeaderPoseComponent`). Слоты в BP: **Body, Head, Hair, Helmet(+Headphones/Drops), Hat, Cap, Glasses, Gloves, Arms, Pants**.
- **Части тела (модульно):** Head, Body_Full/Body_Bottom, Feet, Legs, FullHands/Hands, Wrist, Arms (+ варианты этничности Afro/European, тату/орнамент на руках).
- **Одежда:** Tshirt/Tshirt_Tucked, Shirt_RolledUp, Jacket (M65/Worker/Worker_Hood_1/2), Vest_Worker (hi-vis), Overalls (комбинезоны ×6), Pants (Jeans/Jumpsuit/Pants_Worker/_Full), Boots.
- **Голова/лицо:** Helmet_Worker (каска!), Hat (Cowboy/Farmer/Flat/Up), Cap, Warm_Hat, Headphones(+Micro), Hair, Beard, Glasses (Aviator/Classic/Worker), **Respirator** (= наш газо-механик!).
- **Руки:** Gloves_Worker (+Print-цвета), Watches.
- **Грудь/пояс:** Vest_Worker, Bag_Banana, Bag_Hip.
- **Пресеты:** `Mesh/Male/Presets/` — **20+ готовых SKM_Worker_Male_N** (запечённые комплекты) + дата-ассеты `PA_Worker_Male`, `PA_Worker_Male_Body`.
- **Материалы:** мастер `Quanum_MasterMaterial` + MI-варианты цветов (штаны/перчатки/каски и т.д.).
- **Демо-карты:** `Maps/MegaBundle_Character_Builder`, `Preview_Assets`, `Preview_Presets`.
- **Нет** своих AnimBP/IK-ретаргетеров → анимация через ретаргет на UE5-mann.

## Наша текущая база (факты из кода)
- **`AAvaryoCharacter : ACharacter`** — true-FPS. Тело = `GetMesh()` (сейчас **CitizenNPC, UE4-mann**), FP-руки = `FirstPersonMeshComp` (из BP), TP-камера на пружине (`ToggleCameraMode`/V).
- **Снаряжение-как-функция сейчас ИНВЕНТАРНОЕ, невидимое:** `HasGasMask()`, `HasWeldingMask()`, `HasRubberBoots()`, `IsHoldingGasDetector()` — проверяют предмет в слотах по ToolTag. Носимых мешей нет.
- **Данные владения снаряжением УЖЕ есть** (`AvariikaSaveGame.h`): `FEquipmentLevels{ bHelmet, bGloves, bGasMask, Flashlight/Tester/... }` + `FCareerStats.Cosmetics` (наклейки/цвета касок). → Modular Workers даёт этим флагам визуал.
- **Анимы:** 7 монтажей (Fix/Hit/Death/Knocked/Revive/Bandage/Drink) + локомоция — всё на **UE4-mann**, играют на CitizenNPC через compatible-skeleton.

## Стержень: скелетный разрыв
Наши анимы — **UE4-mann (3 спайна)**; Quantum — **UE5-mann (5 спайнов)**. Прямой compatible-skeleton НЕ сработает → нужен **ретаргет UE4-mann → UE5-mann** (классический кейс Epic; пайплайн есть, см. [[ik-retarget-python]]). Альтернатива: взять локомоцию из бесплатного UE5-mann пака, ретаргетить только наши one-shot монтажи.

## Дизайн-решение (моё, по вижну — фиксирую, юзер может развернуть)
Модульный рабочий = **аватар игрока** (создание персонажа), по вижну [[char-creator-gear-progression]] «создай работягу, старт-бомж (тишка/джинсы) → зарабатывай на каску/жилет/маску/ботинки; косметика=функция».
- **TP/кооп:** тиммейты видят собранного рабочего; надетое снаряжение читается в бою (каска/жилет/маска).
- **FP:** свои руки — из Quantum (`SKM_Quantum_Hands/FullHands/Wrist` + перчатки), на том же скелете.
- **Косметика=функция через существующие флаги:** `bHelmet→Helmet_Worker`, `bGasMask→Respirator`, `bGloves→Gloves_Worker`; геттеры `HasGasMask()`… могут читать надетое.
- 🔑 **ПИВОТ для юзера:** подтвердить, что рабочий ЗАМЕНЯЕТ тело игрока (а не только NPC-бригаду). Я иду по «заменяет»; если юзер хочет иначе — фаза 2 разворачивается. Визуал (как смотрится в игре) **требует глаз юзера** — соберу/скомпилю/закоммичу, финальный «красиво ли» — за ним.

## Фазовый план
**Ф0. Пре-флайт (autonomous-safe).** Проверить, что пак импортирован/реестр видит скелет+пресеты; зафиксировать слоты; female не делаем (male-only by design). ✅ разведка сделана.

**Ф1. Ретаргет анимаций UE4-mann → Quantum (UE5-mann).** IK Rig на Quantum + IK Retargeter (UE4→UE5, можно от Epic-шаблона) → ретаргет 7 монтажей + локомоция-клипы. Проверка смоук/Persona. (Editor, headless-скрипт; визуал-кач-во — потом с юзером.)

**Ф2. Свап тела игрока на Quantum.** `BP_AvaryoCharacter`: тело `GetMesh()` → собранный рабочий (база-пресет «бомж»), AnimBP на Quantum, FP-руки → Quantum-hands. Сверить, что монтажи/локомоция играют, FP-камера/присед/предмет-в-руках не разъехались (наш `UpdateCrouchEye` завязан на FP-меш — см. [[character-camera-crouch]]).

**Ф3. Модульный аппирэнс-компонент (C++ `UWorkerAppearanceComponent`).** Дочерние SkeletalMesh-компоненты по слотам (Head/Hair/Torso/Legs/Feet/Hands/Gloves/Headgear/Face/Vest), `SetLeaderPoseComponent(Body)`; `SetSlotMesh(Slot, Mesh)`; реплицируемая структура `FWorkerAppearance` (индексы/софт-ссылки на части) → кооп видит; save/load в `AvariikaSaveGame`. Build-верифицируемо без визуала.

**Ф4. Снаряжение→визуал.** Драйв слотов от `FEquipmentLevels`: `bHelmet/bGasMask/bGloves` → показать меши; синхрон с функцией (надел маску — `HasGasMask()` true). Хук в магазин (`AvUpgrade`/покупка) + дев-команды.

**Ф5. Создание персонажа (UI).** Экран билдера (тело/лицо/волосы/борода + база-аутфит «бомж»: Tshirt+Jeans+кроссы), сохранение `FWorkerAppearance`. Можно опереться на демо `MegaBundle_Character_Builder` как референс.

**Ф6. Кооп-репликация надетого + полиш.** Тиммейты видят аппирэнс/снаряжение; LOD/перф (части тела дают draw-call'ы — скрывать невидимые под одеждой); цвета касок из `Cosmetics`.

## Что делаю автономно сегодня vs что ждёт юзера
- **Сегодня (безопасно, build/smoke-верифицируемо):** этот план; Ф3-скелет компонента `UWorkerAppearanceComponent` (логика/данные/репликация/сейв — компилится и тестируется без картинки); ретаргет-скрипты Ф1 (прогон + смоук).
- **Ждёт глаз юзера (визуал):** как смотрится свап тела (Ф2), качество ретаргета, билдер (Ф5) — соберу и закоммичу, «красиво/правильно ли» — ревью юзера, когда вернётся.

_Связано: [[char-creator-gear-progression]], [[operator-character-anim-system]], [[anim-skeleton-truth]], [[ik-retarget-python]], [[character-camera-crouch]], [[asset-licensing]]._

