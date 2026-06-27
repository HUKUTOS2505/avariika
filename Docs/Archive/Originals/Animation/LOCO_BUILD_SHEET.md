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
