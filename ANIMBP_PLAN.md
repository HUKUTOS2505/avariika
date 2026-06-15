# Player AnimBP — план сборки аниматора оператора

Карта «механика → состояние → клип». Имена даны по текущим `Op_*` (прототип); на Epic-персонаже те же движения берутся напрямую из паков (UE4 — нативно, UE5 — Epic-ретаргет). **Реализуем на Epic-персонаже**, чтобы не собирать дважды.

## Слои AnimGraph (сверху вниз)
```
Base Locomotion (State Machine)
  → Override: Wounded / Scared / Woozy(gas)  (по состоянию)
  → Slot "FullBody" (action montages: ремонт/подбор/питьё/удар/эмоут)
  → Additive: Idle Breaks / Aim look
  → Down/Death (полная подмена)
```

## 1. Базовая локомоция (State Machine)
**Stand (BlendSpace 2D: Speed × Direction)**
- Idle: `Op_MOB1_Stand_Relaxed_Idle_v2_IPC`
- Walk 8 напр.: `Op_M_Neutral_Walk_Loop_F/B/FL/FR/BL/BR/LL/RR`
- Run/Jog 8 напр.: `Op_M_Neutral_Run_Loop_*` (+ `Op_MOB1_Jog_F`)
- Стопы: `Op_MOB1_Jog_F_to_Stand…`, `…Run_F_to_Stand…`
- Idle-фиджеты (additive по таймеру): `Op_MOB1_Stand_Relaxed_Fgt_v1/v4`

**Crouch (BlendSpace 2D)** — вход по Ctrl
- Idle: `Op_MOB1_Crouch_Idle_V2_IPC`
- Walk 8 напр.: `Op_M_Neutral_Crouch_Loop_*` (+ `Op_MOB1_CrouchWalk_F`)
- ⚠️ foot-IK обязателен (на Epic-скелете IK-кости есть) — иначе стопы клипуют.

**Jump (3 состояния)** — безоружные Meshy
- Start: `A_Regular_Jump` · Fall(loop) · Land: `A_Jumping_Down`

## 2. Оверрайды состояния (подмена локомоции)
- **Раненый (low HP)**: idle `Op_am_InjuredBelly_Idle_01`; ходьба `Op_am_InjuredBelly_Loco_Walk_Fwd` (+Start/Stop); повороты `…Idle_TurnL/R`.
- **Испуг (монстр рядом / низкий рассудок)**: ходьба → `Op_SCR_Walk_Scared_Fwd_Look_Left/Right`.
- **Woozy / газ**: idle `Op_am_StandDrunk_Idle_01`; ходьба `Op_am_Drunk_Loco_Walk_01`; кашель — additive.
- **Ползание (раненый ползёт)**: `Op_am_ProneCrawl_CrawlStartStop_Fwd` (+ Stand↔Prone транзишены).

## 3. Даун / откачка / смерть (полная подмена)
- Упал: `Op_am_InjuredBelly_DropToFloor` или `Op_Knocked_Front/Back/Left/Right`.
- Лежит-ждёт: `Op_am_InjuredBelly_DropToFloor_Loop` / `Op_Idle_Front/Back/…`.
- Подняли (revive): `Op_am_InjuredBelly_DropToFloor_GetUp` / `Op_Revive_Front/Back/…`.
- Нокдаун/спотык: `Op_am_Ready_Fight_01_Knockdown_A` / `…02_Kickdown_A`.
- Смерть: `Op_Death_1/2/3` (вперёд/вбок/коллапс) · `Op_MOB1_…Death_B` (назад). Loop OFF, холд последнего кадра.

## 4. Слот "FullBody" — action-монтажи (по событию механики)
- **Ремонт**: `Op_Worker_FixCircuitBoard_Loop` (осн.) · `Op_Worker_InspectUnder_Loop` · `Op_Worker_welding_enter/exit`.
- **Подбор**: `Op_PickUp1/2/3`; пол на коленях `Op_Loot_FloorPickUp_Kneel_RightHand` / `…_BendOver_RightHand`.
- **Обыск контейнеров**: `Op_Loot_HighShelf_SearchAndGrab` / `…_LowShelf` / `…_Trashcan` / `…_Hole_ArmReach` / `…_Corpse`.
- **Интеракт/панель/толчок**: `Op_am_Vend_Start` (тянется) · `Op_am_Vend_Success_GrabItem` · `Op_am_Vend_Fail_HitMachine` (толчок) · `Op_UseDevice_start/loop/end`.
- **Расходники**: бинт `Op_Bandage_start/loop/end` · питьё `Op_DrinkingPotion_start/loop/end` · еда `Op_Eat1`.
- **Удар-реакция**: `Op_Hit_React_1..4` · направленные `Op_UE4M_HitReaction_Front/Back/Left/Right_01/02`.
- **Рубильник/вентиль/цепь**: из `InteractionKitVol1` (Replicated, use-as-is) — `AM_Switch/Valve/Chain`.

## 5. Хаб / соц-эмоуты (слот, по колесу эмоутов)
- Радость/успех: `Op_am_Stand_React_Excited_01` (+HeelClick) · злость: `…Frustrated_01_All/StompFeet` · грусть: `…Sad_01`.
- Жесты: `Op_am_Stand_Wave_02_LHand` (помахать) · `…LookAt_05_PointRHand` (указать) · `…Conv_Talk_05` (болтать).
- Эмоуты-набор: `Op_Emote101..112_UE4Anim` (подписать что есть что при сборке).
- (Рукопожатие/обнять — парные, нужен Control Rig, позже.)

## Дыры (закрыть Control Rig / докупка)
- Нести ТИММЕЙТА на руках/плече.
- Бросок предмета (точного нет — репурпоз Vend_Fail или докупить).
- Парное рукопожатие/объятие.

## Порядок реализации (после покупки Epic-персонажа)
1. Locomotion BS (stand+crouch) + Jump → ходить/бегать/сидеть.
2. Foot-IK (Control Rig / IK-ноды).
3. Слот FullBody + примеры action-монтажей (ремонт/подбор/бинт).
4. Оверрайды состояний (раненый/испуг/woozy) по геймплей-флагам.
5. Даун/откачка/смерть.
6. Хаб-эмоуты (колесо).
