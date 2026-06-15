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

## ✅ УЖЕ ЕСТЬ ИЗ ПАКА (94 шт · Op_*)

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

### 🌀 Состояния / эмоции / флавор (новое из MC)

| Механика | ПАК | Применение |
|---|---|---|
| Woozy/шатание (idle+ходьба) | Op_am_StandDrunk_Idle_01 · Op_am_Drunk_Loco_Walk_01 | газ / высокая паника / дезориентация |
| Сбой инструмента (злость) | Op_am_Stand_Emotion_Frustrated_01_All · _StompFeet | провал ремонта |
| Успех ремонта (радость) | Op_am_Stand_React_Excited_01 (+Loop/Start/Stop) · _HeelClick | починил |
| Грусть | Op_am_Stand_Emotion_Sad_01 | смерть напарника / провал |
| Осмотр / поиск поломки | Op_am_Stand_Idle_03_LookAround | idle-разведка |
| Указать (кооп) · помахать · почесаться · болтать | Op_am_Stand_LookAt_05_PointRHand · Op_am_Stand_Wave_02_LHand · _Idle_06_ScratchArm · _Conv_Talk_05_Generic | кооп / флавор |

### 🔧 Действия (механики C++)

| Механика | ПАК (цель) | Fallback Meshy | Статус |
|---|---|---|---|
| Подбор предмета (E) | Op_am_Vend_Success_GrabItem | A_Collect_Object | ✅ ПАК |
| РЕМОНТ (главная) | Op_am_StandDrillLow_01_Drill (+транз) | A_Scheming_Hand_Rub | ✅ ПАК (стоя; на коленях — нет) |
| Толчок / удар по машине | Op_am_Vend_Fail_HitMachine | A_Step_Forward_and_Push | ✅ ПАК |
| Интеракт-панель (рубильник/вентиль) | Op_am_Vend_Start | — | 🟡 репурпоз (отдельного нет) |
| Бросок | — | A_Thrust_Slash | ⚠️ Meshy (в паках нет) |
| Питьё (расходник) | Op_am_Moonshine_01_Drink_Loop (+транз) | A_Stand_and_Drink | ✅ ПАК |

### 💥 Реакции на монстра

| Механика | ПАК | Fallback Meshy | Статус |
|---|---|---|---|
| Реакция на удар | Op_Hit_React_1..4 (ASP, безоружные) | A_Hit_Reaction | ✅ ПАК |
| Нокдаун / спотык-восстановление | Op_am_Ready_Fight_01_Knockdown_A / _02_Kickdown_A (+ж-версии) | — | ✅ ПАК |
| Реакция электрошок | репурпоз Op_Hit_React + дрожь | A_Electrocution_Reaction | 🟡 Meshy/репурпоз |
| Джампскейр-флинч | репурпоз Op_Hit_React / Knockdown | A_Face_Punch_Reaction | 🟡 репурпоз |

### ☠️ Смерть

| Механика | ПАК | Fallback Meshy | Статус |
|---|---|---|---|
| Смерть назад | Op_MOB1_Stand_Relaxed_Death_B_IPC | A_dying_backwards | ✅ ПАК |
| Смерть вперёд / вбок / коллапс | Op_Death_1 / Op_Death_2 / Op_Death_3 (ASP) | A_Electrocuted_Fall | ✅ ПАК |
| Падение на пол (даун, не финал) | Op_am_InjuredBelly_DropToFloor (+Loop) | A_Fall_Dead_from_Abdominal_Injury | ✅ ПАК |

> Loop = OFF, холд последнего кадра. Стыковка из любой позы → Blend ~0,15с в AnimBP.

### 🩹 Раненый (по HP)

| Механика | ПАК | Fallback Meshy | Статус |
|---|---|---|---|
| Раненый: ходьба вперёд (+Start/Stop) | Op_am_InjuredBelly_Loco_Walk_Fwd | A_Limping_Walk_3_inplace | ✅ ПАК |
| Раненый: idle + повороты L/R 90/180 | Op_am_InjuredBelly_Idle_01/02/03 + TurnL/R | — | ✅ ПАК |
| Ползание (раненый ползёт) | Op_am_ProneCrawl_CrawlStartStop_Fwd (+транз Stand↔Prone) | — | ✅ ПАК |
| Раненый идёт назад | — | A_Injured_Walk_Backward_inplace | 🟡 Meshy |

### 🤝 Даун / откачка напарника

| Механика | ПАК | Fallback Meshy | Статус |
|---|---|---|---|
| Даун-idle (лежит на полу) | Op_am_InjuredBelly_DropToFloor_Loop | A_Prone_Reach_Help | ✅ ПАК |
| Вставание (откачали/подняли) | Op_am_InjuredBelly_DropToFloor_GetUp | A_Stand_Up1 | ✅ ПАК |

### 🦵 Прыжок

| Механика | ПАК | Fallback Meshy | Статус |
|---|---|---|---|
| Прыжок с места | Op_Jump_From_Stand (ASP) | A_Regular_Jump | ✅ ПАК |
| Прыжок с разбега / приземление | Op_Jump_From_Jog (ASP) | A_Jumping_Down | ✅ ПАК |

> GAS-джамп (M_Neutral_Jump_*) — рваные motion-matching фрагменты, без чистого взлёта. Взяли ASP-джамп (классический, цельный).

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
✅ ИЗ ПАКА (94 Op_*):   локомоция все 8 направлений (walk/jog/run) + присед все направления,
                        idle+фиджеты, 2 хоррор-ходьбы, бонус-ходьбы;
                        раненый (idle/повороты/ходьба/ползание/даун↔вставание),
                        смерть (назад/вперёд/вбок/коллапс), хит-реакции, нокдаун/спотык,
                        прыжок (с места/разбега), ремонт-стоя, подбор/толчок/интеракт,
                        питьё, woozy-шатание, эмоции (злость/радость/грусть), флавор
🟡 MESHY fallback:      бросок, раненый-назад, электрошок/флинч (репурпоз Hit) — пак-версий нет
🔧 Control Rig:         нести напарника; ремонт на коленях
```

Полный проход 4 паков завершён — всё пригодное ретаргетнуто (см. ANIM_PACK_AUDIT.md).
Осталось живьём из паков взять нечего; дыры закрываются Meshy/Control Rig.
**Следующий шаг — не ретаргет, а сборка: BlendSpace + AnimBP на Op_* и подключение оператора телом игрока.**

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
