# АВАРИЙКА — Анимации (чек-лист для сверки с Claude Code)
### версия 2.0 · ПРИОРИТЕТ: ПАКИ → потом Meshy

---

## ПРАВИЛО ИСТОЧНИКОВ (новое)

```
1. ПАКИ (ретаргет на оператора)  → ПРИОРИТЕТ. Качество выше, берём отсюда.
   Папка: Content/Characters/Operator/Anims_Retarget/Op_*
2. Meshy (на скелете оператора)  → FALLBACK. Только то, чего в паках ещё нет.
   Папка: Content/Characters/Operator/Anims/A_*
3. Control Rig                   → что не даёт никто (нести напарника).
```

Meshy-локомоция — так себе, поэтому движение/idle/присед берём из пака. Meshy держим как запас под действия/состояния, пока не заменим пак-версиями.

> ⚠️ РЕТАРГЕТ: пак-анимы на Epic-скелете → каждой нужен IK Retargeter на оператора (то, что уже сделано для 16 = Op_*). Meshy уже на скелете оператора, ретаргет не нужен.

---

## ✅ УЖЕ ЕСТЬ ИЗ ПАКА (16 шт · Op_*)

Папка: `Content/Characters/Operator/Anims_Retarget/Op_*` (имена уточни по папке)

```
ЛОКОМОЦИЯ (5+):  Op_Walk_F · Op_Jog_F · Op_Run_F (+ остановки/Stops) · Op_CrouchWalk · Op_Crouch_Idle
IDLE (3):        Op_Relaxed_Idle + 2 фиджета (Op_Fidget_1/2)
ХОРРОР (2):      Op_Scared_Walk_L · Op_Scared_Walk_R  (испуганная ходьба, озирается Л/П)
ПРОЧЕЕ (4):      Op_Death · Op_LookAround_Walk · Op_Cheerful_Walk · Op_Power_Walk
```

Покрыто паком: **движение вперёд (шаг/трусца/бег + остановки), присед (idle+ходьба), idle с фиджетами, 2 хоррор-ходьбы, смерть.** 👍

---

## ЧЕК-ЛИСТ ПО МЕХАНИКАМ

Колонки: **Приоритет (ПАК)** → **Fallback (Meshy)** → **Статус**

### 🚶 Локомоция

| Механика | ПАК (приоритет) | Fallback Meshy | Статус |
|---|---|---|---|
| Idle стоя | Op_Relaxed_Idle (+2 фиджета) | A_Idle_11 | ✅ ПАК |
| Ходьба вперёд | Op_Walk_F | A_walking_2_inplace | ✅ ПАК |
| Трусца/бег вперёд | Op_Jog_F / Op_Run_F (+stops) | A_run_fast_10_inplace | ✅ ПАК |
| Ходьба назад | Op_M_Neutral_Walk_Loop_B | A_Walk_Backward | ✅ ПАК (GAS) |
| Бег назад | Op_M_Neutral_Run_Loop_B | — | ✅ ПАК (GAS) |
| Strafe Л/П (ходьба) | Op_M_Neutral_Walk_Loop_LL/RR (+диаг FL/FR/BL/BR) | — | ✅ ПАК (GAS) |
| Strafe Л/П (бег) | Op_M_Neutral_Run_Loop_LL/RR (+диаг) | — | ✅ ПАК (GAS) |

### 🦆 Присед (Ctrl)

| Механика | ПАК | Fallback Meshy | Статус |
|---|---|---|---|
| Присед idle | Op_Crouch_Idle | A_CrouchLookAroundBow | ✅ ПАК |
| Присед вперёд | Op_CrouchWalk | A_Cautious_Crouch_Walk_Forward | ✅ ПАК |
| Присед назад | Op_M_Neutral_Crouch_Loop_B | A_Cautious_Crouch_Walk_Backward | ✅ ПАК (GAS) |
| Присед Л/П | Op_M_Neutral_Crouch_Loop_LL/RR (+диаг) | A_Cautious_Crouch_Walk_Left/Right_inplace | ✅ ПАК (GAS) |

### 😱 Хоррор (пак силён — используем!)

| Механика | ПАК | Статус |
|---|---|---|
| Испуганная ходьба, озирается Л | Op_Scared_Walk_L | ✅ ПАК |
| Испуганная ходьба, озирается П | Op_Scared_Walk_R | ✅ ПАК |

> 💡 Завязать на «монстр рядом / низкий рассудок»: подменять обычную ходьбу на Op_Scared_Walk. Сильный хоррор-эффект бесплатно.

### 🔧 Действия (механики C++)

| Механика | ПАК (цель) | Fallback Meshy | Статус |
|---|---|---|---|
| Подбор предмета (E) | 🎯 MC Sample | A_Collect_Object / A_Male_Bend_Over_Pick_Up | 🟡 Meshy fallback |
| РЕМОНТ (главная) | 🎯 MC Fix&Build / Control Rig | A_Scheming_Hand_Rub | ⚠️ заглушка Meshy |
| Толчок | 🎯 MC Sample | A_Step_Forward_and_Push | ⚠️ Meshy, обрезать |
| Бросок | 🎯 MC Sample (throw) | A_Thrust_Slash | ⚠️ Meshy, обрезать |
| Питьё (расходник) | 🎯 MC Sample | A_Stand_and_Drink | 🟡 Meshy fallback |

### 💥 Реакции на монстра

| Механика | ПАК | Fallback Meshy | Статус |
|---|---|---|---|
| Реакция на удар | 🎯 MC Sample | A_Hit_Reaction | 🟡 Meshy fallback |
| Реакция электрошок | — | A_Electrocution_Reaction | 🟡 Meshy |
| Реакция в лицо | — | A_Face_Punch_Reaction | 🟡 Meshy |

### ☠️ Смерть

| Механика | ПАК | Fallback Meshy | Статус |
|---|---|---|---|
| Смерть (общая) | Op_Death | A_Electrocuted_Fall | ✅ ПАК |
| Смерть назад (вариант) | 🎯 MC Sample | A_dying_backwards | 🟡 Meshy для направления |
| Запас | — | A_Dead · A_Fall_Dead_from_Abdominal_Injury | 🟡 Meshy |

> Loop = OFF, холд последнего кадра. Стыковка из любой позы → Blend ~0,15с в AnimBP.

### 🩹 Раненый (по HP)

| Механика | ПАК | Fallback Meshy | Статус |
|---|---|---|---|
| Хромая ходьба вперёд | 🎯 MC Sample (injured) | A_Limping_Walk_3_inplace | 🟡 Meshy fallback |
| Раненый идёт назад | — | A_Injured_Walk_Backward_inplace | 🟡 Meshy |

### 🤝 Даун / откачка напарника

| Механика | ПАК | Fallback Meshy | Статус |
|---|---|---|---|
| Лежит-зовёт (даун-idle) | 🎯 MC Sample | A_Prone_Reach_Help | 🟡 Meshy fallback |
| Вставание (подняли) | 🎯 MC Sample | A_Stand_Up1 | 🟡 Meshy fallback |

### 🦵 Прыжок

| Механика | ПАК | Fallback Meshy | Статус |
|---|---|---|---|
| Прыжок вверх | 🎯 Game Anim Sample | A_Regular_Jump | 🟡 Meshy fallback |
| Прыжок вниз / приземление | 🎯 Game Anim Sample | A_Jumping_Down | 🟡 Meshy |

### 👥 Кооп (особое)

| Механика | Источник | Статус |
|---|---|---|
| Нести раненого на руках | 🔧 Control Rig | дыра, заглушка-старт A_Prone_Reach_Help |

### 🎭 Бонус-ходьбы из пака (есть, на разнообразие)

```
Op_LookAround_Walk  → ходьба-осмотр (поиск поломок)
Op_Cheerful_Walk    → бодрая (между заходами / комедия)
Op_Power_Walk       → уверенная (когда всё чинено, идём на выход)
```

---

## 🎯 ДОБРАТЬ ИЗ ПАКОВ (приоритет ретаргета)

```
1. ✅ СДЕЛАНО: Game Animation Sample → ходьба+бег НАЗАД, strafe Л/П (ходьба+бег),
   присед назад/Л/П — все 8 направлений Walk/Run/Crouch ретаргетнуты (Op_M_Neutral_*).
2. Game Animation Sample → прыжок вверх/вниз (в работе)
3. MC Sample → действия: реально есть только am_InjuredBelly_* (раненый) + am_ProneCrawl (ползёт);
   подбор/толчок/бросок/питьё/ремонт в паках НЕТ → остаются Meshy A_* (см. аудит-отчёт)
```

## 🟡 MESHY — fallback ПОКА (заменяем по мере пак-версий)

Работают сразу (на скелете оператора, без ретаргета). Не выбрасываем — держим, пока не заменим паком: ходьба/бег назад, присед назад/Л/П, подбор, ремонт-заглушка, толчок, бросок, питьё, реакции на удар, хромая, раненый-назад, даун+вставание, прыжок, варианты смерти.

## 🔧 Control Rig (потом)

```
Нести напарника на руках/плече — два тела, паки/Meshy чисто не дают.
```

---

## СВОДКА

```
✅ ИЗ ПАКА (16):        движение-вперёд (шаг/трусца/бег+стопы), присед, idle+фиджеты,
                        2 хоррор-ходьбы, смерть, 3 бонус-ходьбы
🎯 ДОБРАТЬ ИЗ ПАКОВ:    направления (назад+strafe), прыжок (Game Anim Sample);
                        действия+реакции (MC Sample)
🟡 MESHY fallback:      то, чего из пака ещё нет — работает, заменим позже
🔧 Control Rig:         нести напарника
```

Приоритет теперь: на каждую механику сначала ищем ПАК-версию (Op_*), Meshy — временная подпорка. Главный пробел сейчас — **направления движения** (назад + strafe), их добираем из Game Animation Sample.

---

## КОМАНДА НА СВЕРКУ ДЛЯ CLAUDE CODE

> Приоритет анимаций: ПАКИ → Meshy fallback. Уже ретаргетнуто 16 пак-анимаций в Content/Characters/Operator/Anims_Retarget/Op_*.
> 1. Из Game Animation Sample доретаргетни на оператора: ходьба+бег НАЗАД, strafe Л/П (ходьба+бег), присед назад/Л/П, прыжок вверх/вниз. Это главный пробел для WASD.
> 2. Собери locomotion BlendSpace на ПАК-анимах (Op_*): idle/walk/jog/run + новые направления + crouch. Meshy-локомоцию НЕ используй (fallback только если пак-версии нет).
> 3. Из MC Sample глянь и ретаргетни: подбор, толчок, бросок, питьё, реакция на удар, хромая, даун/вставание — заменить Meshy-заглушки. Где в MC Sample нет — оставь Meshy.
> 4. Хоррор: заведи Op_Scared_Walk_L/R как подмену ходьбы при «монстр рядом».
> 5. Нести напарника — Control Rig, позже.
> Скажи, что из паков подошло и что осталось на Meshy.

---

*Документ живой. По мере ретаргета отмечай: 🟡 Meshy → ✅ ПАК.*
