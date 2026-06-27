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
