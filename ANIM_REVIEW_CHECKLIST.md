# Аниматции оператора — чеклист проверки

**156 клипов** в `Content/Characters/Operator/Anims_Retarget/Op_*`. Отмечай: `[x]` — ок, `[!]` — кривит (опиши что), `[-]` — не нужна (удалю).

### Как смотреть
Двойной клик по ассету → окно анимации, смотри в превью на: **растянутые подмышки/костюм**, **дёрганье/тряску**, **скольжение ног**. ~2-3 сек на клип.

### Приоритет
- 🔴 **проверить в первую очередь** — руки подняты/спорные (тут вероятнее косяк).
- 🟡 **UE5/RTG_UEFN** — одобрено раньше (руки-вниз), глянь плечи мельком.
- 🟢 **UE4/RTG_MCO** — проверенное семейство (ты подтвердил Bandage/PickUp), быстрый просмотр.

---

## 🔴 Ремонт / работа (смотреть первым — руки подняты)
- [!] Op_Worker_FixCircuitBoard_Loop — чинит плату (низко/средне)  левая нога приподнятая как будто он об стенку наклонился руки вроде норм работают
- [!] Op_Worker_FixLandingGear_Loop — чинит снизу    криво левая нога под текстурой права нога в правой ноге то есть часть до колена в бедре
- [!] Op_Worker_FixPanelOverhead_Loop — **руки НАД головой** (самый риск) почему то он в воздухе  и у неего место костюма растягивается  где подмышки и руки кривоваты 
- [!] Op_Worker_FixWiresOverhead_Loop — **руки НАД головой** (самый риск)  тоже в воздухе как будто чинит предмет который над ним но так же подмышки растянуты 
- [!] Op_Worker_HammerPanelStepUp_Loop — стучит по панели ноги тоже не на земле подмышки растянуты 
- [x] Op_Worker_InspectUnder_Loop — осмотр снизу
- [!] Op_Worker_welding_enter / Op_Worker_welding_exit — сварка  сама анимация норм не пойму как он будет под сварку работать
- [-] Op_am_StandDrillLow_01_Drill — ⚠️ старая дрель (та самая кривая рука?) → заменена ScifiWorker, скорее `[-]`
- [!] Op_am_StandDrillLow_Trans_Stand / Op_am_Stand_Trans_StandDrillLow — переходы дрели (с ней же `[-]`?) сама аниммация норм не понятно что делает  чел выпрямляется 

## 🟡 Локомоция — направления (GAS, UE5/RTG_UEFN, одобрено ранее)
- [x] Op_M_Neutral_Walk_Loop_F / B / FL / FR / BL / BR / LL / RR (8)
- [x] Op_M_Neutral_Run_Loop_F / B / FL / FR / BL / BR / LL / RR (8)
- [!] Op_M_Neutral_Crouch_Loop_F / B / FL / FR / BL / BR / LL / RR (8) у B правая нага оказывается под землей, у BL тоже , у BR тоже,у F также, в ообщем как я понимаю у всех

## 🟢 Локомоция вперёд + idle (MCO)
- [x] Op_MOB1_Walk_F_IPC · Op_MOB1_Jog_F_IPC · Op_MOB1_Run_F_IPC
- [x] Op_MOB1_Jog_F_to_Stand_Relaxed_RU_IPC · Op_MOB1_Run_F_to_Stand_Relaxed_RU_IPC (остановки)
- [x,!] Op_MOB1_CrouchWalk_F_IPC · Op_MOB1_Crouch_Idle_V2_IPC  у 2 анимации левый тапок под текстурой
- [x] Op_MOB1_Stand_Relaxed_Idle_v2_IPC · Op_MOB1_Stand_Relaxed_Fgt_v1_IPC · Op_MOB1_Stand_Relaxed_Fgt_v4_IPC (idle+фиджеты)
- [x] Op_Walk_02_Cheerful_Loop_IP · Op_Walk_06_Look_Around_Loop_IP · Op_Walk_13_Power_Walk_Loop_IP (бонус-ходьбы)

## 🟢 Хоррор-ходьба (MCO)
- [x] Op_SCR_Walk_Scared_Fwd_Look_Left_Loop_IP
- [x] Op_SCR_Walk_Scared_Fwd_Look_Right_Loop_IP

## 🟢 Раненый / ползание (MC)
- [x] Op_am_InjuredBelly_Idle_01 · _02_LookAtHand · _03_LookAround
- [x] Op_am_InjuredBelly_Idle_TurnL90 · _TurnL180 · _TurnR90 · _TurnR180
- [x] Op_am_InjuredBelly_Loco_Walk_Fwd · _Start · _Stop · _LookAround
- [x] Op_am_ProneCrawl_CrawlStartStop_Fwd · Op_am_ProneCrawl_Trans_Stand · Op_am_Stand_Trans_ProneCrawl

## 🟢 Даун / откачка
- [x] Op_am_InjuredBelly_DropToFloor · _DropToFloor_Loop · _DropToFloor_GetUp (MC)
- [x] Op_Knocked_Back · _Front · _Left · _Right (KnockedDown)
- [x] Op_Revive_Back · _Front · _Left · _Right
- [x] Op_Idle_Back · _Front · _Left · _Right (лежит-ждёт)
- [x] Op_am_Ready_Fight_01_Knockdown_A · Op_af_Ready_Fight_01_Knockdown_B
- [x] Op_am_Ready_Fight_02_Kickdown_A · Op_af_Ready_Fight_02_Kickdown_B

## 🟢 Смерть
- [x] Op_Death_1 · Op_Death_2 · Op_Death_3 (вперёд/вбок/коллапс)
- [x] Op_MOB1_Stand_Relaxed_Death_B_IPC (назад)

## 🟢 Удары / реакции
- [x] Op_Hit_React_1 · _2 · _3 · _4 (ASP)
- [x] Op_UE4M_HitReaction_Front_01 · _02 · Back_01 · _02 · Left_01 · _02 · Right_01 · _02 (направленные)

## 🟢 Прыжок
- [!] Op_Jump_From_Stand · Op_Jump_From_Jog это прыжки с оружием, по ним видно

## 🟢 Интеракт / подбор / обыск
- [x,x,!] Op_am_Vend_Start · Op_am_Vend_Success_GrabItem · Op_am_Vend_Fail_HitMachine не понял 3 анимацию
- [x] Op_PickUp1 · Op_PickUp2 · Op_PickUp3
- [x] Op_UseDevice_start · _loop · _end (панель/девайс)
- [!] Op_Loot_Locker_GrabItem · Op_Loot_Fridge_GrabItem · Op_Loot_Corpse_GrabItem только 3 анимация работает 
- [x] Op_Loot_HighShelf_SearchAndGrab · Op_Loot_LowShelf_GrabItem · Op_Loot_Trashcan_SearchAndGrab · Op_Loot_Hole_ArmReach_GrabItem все нравится но не понял первую
- [x,!,x] Op_Loot_FloorPickUp_Kneel_RightHand · _Kneel_SmallBox · _BendOver_RightHand (на коленях/нагиб) 2 анимация не работает полностью

## 🟢 Расходники
- [x] Op_Bandage_start · _loop · _end (бинт) в целом норм 
- [!] Op_DrinkingPotion_start · _loop · _end · Op_am_Moonshine_01_Drink_Loop · Op_am_Moonshine_Trans_Stand (питьё), Op_am_Moonshine_01_Drink_Loop странная он смотрит наверх рука кривая, Op_am_Moonshine_Trans_Stand тут  нормис но  тот же странная рука и смотрит наверх
- [x] Op_Eat1 (еда)

## 🟢 Состояния (газ/паника)
- [x] Op_am_StandDrunk_Idle_01 · Op_am_Drunk_Loco_Walk_01 (шатание/woozy)

## 🟢 Эмоции / хаб (MC)
- [x] Op_am_Stand_Emotion_Frustrated_01_All · _StompFeet (злость/провал)
- [!] Op_am_Stand_React_Excited_01 · _Loop · _Start · _Stop · _HeelClick (радость/успех) все круто кроме растягивания костюма где подмышки
- [x] Op_am_Stand_Emotion_Sad_01 (грусть)
- [x] Op_am_Stand_Wave_02_LHand · Op_am_Stand_LookAt_05_PointRHand (помахать/указать)
- [x] Op_am_Stand_Conv_Talk_05_Generic · Op_am_Stand_Idle_03_LookAround · Op_am_Stand_Idle_06_ScratchArm

## 🟡 Эмоуты-хаб (CharacterEmotes, UE4-вариант) — номерные, подпиши что есть что
- [x] Op_Emote101_UE4Anim · 102 · 103 · 104 · 106 · 107 · 108 · 109 · 110 · 111 · 112 (105 удалена) у 107 растягивания костюма где подмышки

---

## Что в паузе (НЕ ретаргетнуто — UE5-проблема рук / FBX-этап)
Эти НЕ в списке выше, проверять нечего: **Motifect Injured/Emotes, GAS-traversal, Paragon one-offs, Attaku finger poses** — отложены (UE5→оператор кривит на руках; доделаю на финальном персонаже или отдельным заходом).

## Use-as-is (на своих скелетах, ретаргет не нужен — отдельная проверка)
- Собака-монстр (AnimX German Shepherd) — `/Game/AnimX`
- Кооп-интеракции switch/valve/chain — `/Game/InteractionKitVol1`
- Лестница/ползание/двери/рубильники — Free Ladder / Free Crawl / Free Interaction

---

## ИТОГ по твоей проверке (что сделано)

**Удалено (14):** StandDrill ×3 · Worker FixLandingGear / FixPanelOverhead / FixWiresOverhead / HammerPanelStepUp (плавали + подмышки) · Jump_From_Stand/Jog (с оружием) · Loot Locker/Fridge/Kneel_SmallBox (не работали) · Moonshine_Drink ×2 (смотрел вверх / кривая рука). → осталось **142**.

**Прыжок:** беру безоружные Meshy `A_Regular_Jump` (вверх) + `A_Jumping_Down` (вниз) — уже на операторе, ретаргет не нужен.

**Подмышки тянутся на поднятых руках** (FixCircuitBoard / React_Excited / Emote107 и т.п.) — это **веса плеч дешёвого Meshy-меша, не ретаргет**. На нормальном персонаже уйдёт. На прототипе оставляем как есть.

**Присед — правая нога под землёй** (все направления GAS + левый тапок у MCO-приседа) — нужен **foot-IK** (планта стопы к полу). На прототип-меше делать невыгодно → foot-IK на финальном персонаже. (Могу добавить сейчас, если надо — но это правка рига ради временного меша.)

**Оставлены рабочие:** FixCircuitBoard (ремонт), InspectUnder, welding, Vend Start/GrabItem/Fail(толчок машины — читается с пропом), Loot HighShelf/LowShelf/Trashcan/Hole/Corpse + Kneel/BendOver, Bandage, DrinkingPotion, Eat, и весь остальной [x]-набор.
