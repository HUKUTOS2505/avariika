# Анимации из паков — финальный свод (полный проход 4 источников)

Проверены ВСЕ 4 источника, что добавлял пользователь: **1 проект** (GameAnimationSample) + **3 пака через Unreal** (AnimStarterPack, MCO_Mocap_Basics, MC_Sample). Всё пригодное ретаргетнуто на оператора.

**Итого ретаргетнуто: 94 клипа** → `Content/Characters/Operator/Anims_Retarget/Op_*`

Ретаргетеры (источник → IK_Operator):
- `RTG_UEFN_to_Operator` (GameAnimationSample / UEFN-скелет)
- `RTG_MCO_to_Operator` (IK_UE4Mann; **переиспользован и для AnimStarterPack** — общий UE4-маннекен-скелет)
- `RTG_MC_to_Operator` (MCUE5v2; спина исправлена)

---

## Что ретаргетнуто по источникам

### GameAnimationSample (UEFN) — 24
Все направления Walk/Run/Crouch (F/B/L/R + диагонали) → `Op_M_Neutral_*`. WASD-локомоция закрыта.

### MCO_Mocap_Basics — 16
Fwd-локомоция (walk/jog/run + stops), idle+фиджеты, crouch idle/walk, 2 испуганные ходьбы (`Op_SCR_*`), смерть назад, бонус-ходьбы (cheerful/look-around/power).

### MC_Sample (MCUE5v2) — 45
Пак фэнтези-комедийный (Orc/Spell/Undead/Dance — пропущено), но в нём нашлись ключевые экшены:
- **Раненый (InjuredBelly)** — полный набор: idle 01/02/03, повороты L/R 90/180, Loco_Walk_Fwd (+Start/Stop/LookAround), **DropToFloor → Loop → GetUp** (это закрывает цикл «упал/откачали»).
- **Ползание (ProneCrawl)** — CrawlStartStop_Fwd + транзишены Stand↔Prone.
- **Нокдаун/спотык** — Knockdown A/B (муж/жен), Kickdown A/B.
- **Ремонт** — `Op_am_StandDrillLow_01_Drill` (+ транзишены): работа инструментом стоя = главная механика.
- **Интеракт/подбор/толчок** — Vend_Start (тянется к панели), Vend_Success_GrabItem (подбор), Vend_Fail_HitMachine (бьёт/толкает).
- **Питьё** — Moonshine_01_Drink_Loop (+ транзишены).
- **Woozy/стаггер** (репурпоз под газ/высокую панику) — Drunk_Loco_Walk_01, StandDrunk_Idle_01.
- **Эмоции/флавор** — Frustrated (сбой тула: _All + StompFeet), React_Excited (успех: + HeelClick), Sad, Wave, LookAround idle, PointRHand, ScratchArm, Talk_Generic.

### AnimStarterPack (UE4-маннекен) — 9
Только чистые **безоружные** клипы (всё винтовка/пистолет/ironsights — пропущено):
- **Хит-реакции** Hit_React_1..4 (руки свободны) — заменяют Meshy-заглушки.
- **Смерть** Death_1/2/3 (вперёд/вбок/коллапс) — раньше была только назад.
- **Прыжок** Jump_From_Stand + Jump_From_Jog (чистые классические) — лучше Meshy и лучше рваного GAS-джампа (motion-matching фрагменты).

---

## ❌ Чего НЕТ ни в одном паке → Meshy `A_*` / Control Rig / докупка

| Механика | План | Приоритет |
|---|---|---|
| **Нести/тащить тиммейта** (два тела) | Control Rig | high |
| **Ремонт на коленях** (стоя — есть Drill; на коленях — нет) | Meshy / Control Rig | med |
| **Бросок предмета** | Meshy `A_Thrust_Slash` (обрезать) | med |
| **Рубильник/вентиль** (отдельный жест) | Meshy / репурпоз Vend_Start | med |
| **Электрошок-судорога** | репурпоз Hit_React + дрожь | med |
| **Джампскейр-флинч** | репурпоз Hit_React/Knockdown | low-med |
| **Спрятаться/cower** | Meshy | low-med |

---

## Дальше
1. Собрать locomotion BlendSpace + AnimBP на `Op_*` (idle/walk/jog/run + 8 направлений + crouch + jump).
2. Подключить оператора телом кооп-игрока.
3. State-машина состояний: норма / раненый (InjuredBelly) / даун (DropToFloor) / woozy (Drunk) / испуг (SCR_Scared) / смерть.
4. Экшен-монтажи: репайр (Drill), подбор (GrabItem), толчок (HitMachine), питьё (Moonshine), хит (Hit_React).
