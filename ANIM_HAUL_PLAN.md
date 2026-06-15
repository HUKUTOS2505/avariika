# Аним-улов — мастер-план

## 1. Резюме

Всего в своде **≈490 keep-записей** (часть строк — это бандлы вида «8 clips»/«13 clips»/«14 clips», поэтому реальных клипов заметно больше — порядка 550+). Источники: Paragon (chunks 0–8), GAS/GASP (chunks 0–4), Motifect (Injured/Exhausted + Emotes/Social) и уже импортированные free-паки (Zombie, Interaction, Free Interaction, Crawl, Ladder, Dynamic Falling).

| Таргет | ≈ записей | Статус |
|---|---|---|
| **operator** | ~250 | основной приоритет, закрывает открытые дыры |
| **monster** | ~145 | ТОЛЬКО КАТАЛОГ — монстр НА ПАУЗЕ |
| **hub / co-op** | ~40 | соц-эмоуты/жесты |
| **both** (dual-use) | ~55 | служат и оператору, и монстру |

Ключевой вывод: по **turn-in-place** и **idle-break** данные сильно избыточны (4–5 дублирующих наборов) — берём ОДИН чистый набор (GAS, UE5-маннекин), остальное дроп. По монстру 146 keeps — это сумма по многим героям Paragon с огромным перекрытием; для реального билда берём ОДИН цельный мувсет (рекомендация — **Steel_***).

---

## 2. Оператор

Внутри тиров отмечены **[ДЫРА]** — клипы, закрывающие открытые геймплейные пробелы.

### HIGH

**Turn-in-place [ДЫРА]** — выбрать ОДИН набор, остальное в дроп:
- РЕКОМЕНДУЮ как канон: `M_Neutral_Stand_Turn_045/090/135/180_L+R` (GAS chunk 2, 8 клипов) + `M_Neutral_Idle_turn_left/right` (GAS 0).
- Альтернатива/дубли (НЕ тащить все): `TurnInPlace_Slow/Fast`, `Turn_Left/Right_90`, `Turn_Left/Right_180` (Paragon 8); `Idle_Turn_90/180_L/R` (Paragon 3); `NonCombat_Idle_Turn_90/180_L/R` (Paragon 4); `RMB_TurnInPlace_Fast` (Paragon 6); `M_Relaxed_Stand_Turn 045/090/135/180` (GAS 3, 8 клипов).
- Ходовые повороты: `M_Neutral_Walk_Turn_L/R_045-180` (GAS 3, 14 клипов).
- Боевой вариант (both): `Steel_TurnLeft/Right_Combat` (Paragon 7).

**Throw object [ДЫРА]**:
- `Ability_Grenade_Throw` (overhand, Paragon 0) — основной.
- `RMB_Throw` (Paragon 6) — альтернативный замах.

**Traversal: climb / vault / mantle / hurdle [ДЫРА]** — основной набор GAS (UE5-маннекин):
- Climb 2.5m: `M_Neutral_Traversal_Climb_Start_2_5_run_F` (+ `M_Relaxed_..._run/walk/stand` GAS 4).
- Vault: `M_Neutral/M_Relaxed_Traversal_Vault_1_0_run_F` (+ walk/stand).
- Mantle: `M_Neutral/M_Relaxed_Traversal_Mantle_1_0_run_F` (+ walk/stand).
- Hurdle: `M_Neutral/M_Relaxed_Traversal_Hurdle_1_0_run_F` (+ walk/stand).
- Edge-catch: `M_Neutral/M_Relaxed_Traversal_Catch_Cliff_high_stand`, `M_Relaxed_Traversal_Catch_Hurdle_high_stand`.

**First-aid [ДЫРА]**:
- `bandage_arm` (самобинт), `inject_self` (укол/адреналин) — Motifect Injured.

**Drag / carry [ДЫРА — частично]**:
- `drag_self_forward` (self-drag, Motifect Injured) + `Ability_Hook_Pull` (рывок-на-себя, both).
- ⚠️ Прямого «нести/тащить тиммейта» в своде НЕТ — дыра остаётся частично открытой; ближайшее — две строки выше.

**Hide / cower + jumpscare-flinch [ДЫРА]**:
- `shrink_away_scared` (Motifect Emotes) — съёжиться от скэра.
- `curl_up_fetal` (Motifect Injured) — фетальная поза, спрятаться.
- `Steel_HitReact_Cringe` (both, Paragon 7) — флинч/вздрог на джампскейр.

**Electrocution [ДЫРА]**:
- `TazerTrap` (both, Paragon 7) — ⚠️ ВЕРИФИЦИРОВАТЬ: «бьёт током его» vs «ставит ловушку».
- Подпорка: `KnockUp` (подброс от разряда), `Stun`/`Idle_Stun` (даз).

**Trip / stumble / fall [ДЫРА]**:
- `M_Neutral_Jump_F_Land_Stumble_Lfoot` (GAS 0) — приземление в спотыкание.
- `Steel_Knockdown_Enter` (both, Paragon 7) — сбивает с ног.

**Switch / valve / lever [ДЫРА]** (уже импортировано — см. §6):
- `anim_PullLever`, `anim_ReachLever` (Free Interaction).

**Sit [ДЫРА]**:
- `M_interaction_bench_idle_loop` + `_into_F_Lfoot` + `_out_to_stand` (GAS 4) — сесть/сидеть/встать со скамьи.

**Revive / get-up [ДЫРА]**:
- `revive_gasp_awake`, `revive_slowly_rise` (Motifect Injured).

**Паника / газ (хоррор-стейты)**:
- `panic_look_around`, `stagger_dizzy`, `cough_fit` (газ/раздражитель), `crawl_exhausted` (Motifect Injured).

**Sprint (4-я скорость)** + **dodge/dive (both)**:
- `Sprint` (Paragon 6); `M_Neutral_Sprint_Loop_F` (both).
- `Dive_Fwd_Start`, `Evade_Fwd`, `RMB_Dive` (both); `M_Neutral_Run_Pivot_F_B/B_F`, `Run_Turn_L/R_090/180` (both, GAS 1).

**Rich idle-break [ДЫРА]**:
- `M_Neutral_Stand_Idle_Break_v01..v03` (both); `M_Relaxed_Stand_Idle_Break_v01..v12 + Idle_Loop` (GAS 3, 13 клипов).

### MED

- **Throw-вариативность:** `Ability_Boomerang_Throw`, `Ability_VortexGrenade_Fire`, `Steel_Ability_Shield_Throw` (проп-щит, перешить под брошенный инструмент). [ДЫРА]
- **Traversal walk/stand-варианты:** все `_walk_F`/`_stand_F` версии Climb/Vault/Mantle/Hurdle (GAS 2/4) + edge-catch med: `Catch_Vault_med`, `Catch_Mantle_med_stand`, `Catch_Hurdle_med_stand`.
- **Прыжки/падения:** `M_Neutral_Jump_F_Land_Roll_Lfoot` (роллом), `M_Neutral_Jump_F_Start_Across`, `_Cliff_Rfoot`, `Jump_Loop_Fall`/`M_Relaxed_Jump_Loop_Fall`, `Jump_Fall_Loop` (Paragon 4).
- **Slide (новое движение, под препятствием):** `M_Neutral_Slide_FootOut_Into/Loop`, `Slide_KneesOut_Into/Loop`; relaxed-варианты (GAS 3).
- **Pivot/разворот в движении:** `Jog_Fwd_Pivot180`, `Jog_Right_Pivot180`, `Spin_JogFwdToBwd/BwdToFwd_CW` (Paragon), `M_Neutral/Relaxed_Walk_Pivot_F_B/B_F`, `Walk_Turn_090/180` (GAS 4).
- **Sprint-стейты:** `Sprint_Fwd/Bwd/Start/Stop/Pivot180` (Paragon 7), `M_Relaxed_Sprint_Loop/Start/Stop_F`, `TravelMode_Sprint`.
- **Уклоны:** `Evade_Bwd/Left/Right`, `Dive_Bwd/Left/Right_Start`, `Dive_Fwd_Roll`, `RMB_Evade_CC`, `Dash_Ability` (both).
- **Уклон/слоупы/лестницы (геом.):** `Jog_Fwd_Uphill/Downhill`, `Jog_Uphill_Fwd`, `Travelmode_Fwd_UpHill/DownHill`, `Jog_Fwd_Start_JumpRecovery`.
- **Crouch [ДЫРА — приседание/укрытие]:** `M_Neutral_Crouch_Idle_Break_v01/v02`, `Crouch_Idle_Turn_090/180_L/R`; транзишены `Transition_Stand_to_Crouch`/`Crouch_to_Stand` (Neutral+Relaxed).
- **Раненая локомоция / низкий HP:** `limp_walk_left/right_leg`, `walk_clutching_arm/side`, `collapse_to_knees`, `death_on_knees`, `barely_stand_swaying`, `exhausted_heavy_breathing`, `hands_on_knees_exhausted`, `lean_on_wall_rest`, `back_pain_crouch` (Motifect Injured).
- **Флинч/страх:** `cover_face_embarrassed`, `cover_ears`, `no_way_hands_out`, `sob_crying`, `slump_defeated`.
- **Стелс:** `WalkSneaky_Fwd` (осторожный шаг мимо монстра).
- **Sit/отдых:** `Emote_Meditate`, `M_interaction_bench_out_to_run/walk`, `sit_and_wave_goodbye`.
- **Idle-варианты:** `Idle_Long_A/B`, `Idle_Relaxed`/`idle_relaxed`, `think_chin_stroke`, idle-break v04–v06.
- **Kneel-repair [ДЫРА — частично]:** прямого нет; ближайшее — `A_PlaceDeviceFloor` (присесть-поставить, импортир.) + lever-набор + crouch idle-breaks. Дыра остаётся.

### LOW
`Ability_Block` (защитная стойка/кавер), `Emote_DeepBreath`, `Emote_PushUps`, `SelectScreen_IdleBreak`, `Idle_AmbientNoise`, `Sprint_BWD_CircleLeft`, `Steel_Slope_Fwd`, `Jog_Right_Start/Stop`, `M_Neutral_Run_Start/Stop_F`, `Run_Spin_F_B`, `Slide_*_Out`-рекавери, `avoid_idle_move` (мелкий сайдстеп), relaxed run start/stop (по 8 клипов), `Jump_*_Land_Heavy`, `massage_shoulder`, `neck_stiff_turn`, `stomp_frustrated`.

---

## 3. Монстр — каталог (⚠️ НА ПАУЗЕ, только каталог)

Монстр не собирается без отмашки. Ниже — каталог по категориям. **Огромное перекрытие** между героями Paragon → для билда брать ОДИН цельный мувсет. **Рекомендация: набор `Steel_*`** (единственный source с полным комплектом idle/walk/run/jog все направления + melee-комбо + charge + death + hitreact + knockdown + spawn). Зомби-пак уже импортирован (см. §6) и годится для быстрого прототипа.

**Idle:** `Steel_Idle`, `Steel_Idle_Combat`, `Idle`, `Idle_Combat`, `anim_Idle_A/B`, `anim_Laying_Idle_A/B` (дормант до пробуждения).

**Locomotion:** `Steel_Walk_Fwd/Bwd/Left/Right_Combat`, `Steel_Jog_Fwd/Bwd_Combat`, `Steel_Run_Fwd/Bwd_Combat`, `Combat_JogFwd`; зомби — `anim_Walk_A/B/C`, `anim_Run_A`. Ползание: `anim_Belly_Crawling_A/B` (+ shared Free Crawl prone-набор, §6).

**Attack — melee / combo / swipe:** `Ability_LMB(_A/_B_Medium)`, `Ability_Combo_01-04`, `Ability_GutPunch`, `Attack01/A/B/C/D`, `Attack_GuardBreak_A`, `Melee_A/B/C`+`Melee_Combo_A-F`, `Primary_Attack_A/B/C/D` + `_Fast_A/B`, `Primary_Melee_A/B/C_Slow`, `Primary_Swing1-3`, `PrimaryAttack_LA/RA/FourStrikes`, `RMB_melee`, `Q_swing_B`, `Q_Pull_Kick`, `Steel_Attack_Melee_A/B/C` + `_Charge_A/C` + `_D_V1` + `_GuardBreak_A`, `anim_Attack_A/B/C/D`.

**Lunge / dash / aerial:** `Ability_DashingCross`, `Attack_Melee_C_Dash`, `Dash_Attack`, `In_Air_Melee`, `Melee_Air_Attack`, `PrimaryAttack_Air`, `Primary_Attack_In_Air`, `Primary_Melee_Air`, `JumpKick_Start/Loop`, `RMB_Dive`, `Steel_Test_FullDash`, `Steel_Ability_ShieldDive`, `Primary_Attack_A_InMotion`.

**Grab / pull (закрывает grab-дыру монстра):** `Ability_Hook`, `Ability_Hook_Pull`, `RMB_Pull`, `RMB_Push`.

**Ground-slam / heavy:** `Q_Slam`(+`_FallLoop`), `Steel_Ability_GroundSmash_Start/Loop/End`.

**Spawn / emerge / intro:** `Q_Intro`, `Respawn`, `RMB_Rise`(+`RMB_Land`), `Shadowstrike_End`, `LevelStart`, `Steel_LevelEntry`, `Steel_Ability_Ultimate_Intro` (⚠️ возможно VFX-завязка), `Aggro_Transition_A`, `anim_Burst_A/B` (⚠️ ВЕРИФИЦ.: gore-death ИЛИ emerge — нужны оба).

**Hit-react:** `HitReact_Front/Back/Left/Right`, `HitReact_FullBody_A`, `RMB_HitReact/Hit_Down/Hit_Up`, `Steel_HitReact_Front/Back/Left/Right/Cringe`, `KnockBack_Front/Rear`, `KnockUp`, `Steel_KnockBack(_Bwd)`, `anim_Taking_Damage_A-D`, `Ability_Stun`.

**Stagger / stun / knockdown:** `Stun_Start/Loop/End`, `Idle_Stun`, `Steel_Knockdown_Enter/Idle/Counter`, `Q_Recovery`, `Melee_A_Recovery`, `RMB_melee_recovery`, `MeleeFail` (промах).

**Death:** `Death`, `Death_Forward/Backward/Left_A`, `Steel_Death_A/B`, `anim_Impaled_A/B`; gore — `anim_Burst_*` (verify).

**Scream / roar-эквивалент:** `Emote_Taunt_Howl_T1` (вой), `Emote_Master_BattleCry_T3` (боевой клич/рёв), `Emote_EvilLaugh`.

**Jumpscare / menace-taunt:** `Emote_Taunt_CutThroat_T1`, `Emote_Taunt_WatchingYou_T1`, `Steel_HitReact_Cringe` (shared), `taunt_throat_slash`.

**Evade (монстр):** `Q_Flip_Fwd/Bwd`.

---

## 4. Хаб / кооп-взаимодействие

**Приветствие / greet** (прямого handshake НЕТ — ближайшее ниже): `Emote_Greeting`, `bow_deep_respect`, `Emote_Bow_M1`, `Emote_Taunt_Bow_T1`.

**Салют:** `Emote_Salute`, `salute_military`.

**Празднование / celebrate:** `Emote_Celebration`, `Emote_HooRah`, `Emote_WhoopWhoop`, `Emote_Small_Clap_T1`, `cheer_arms_raised`, `fist_pump_victory`, `clap_enthusiastic`, `Victory_Emote`, `VictoryPose_1`, `Emote_Bravo`, `jump_for_joy`, `excited_spin`.

**Сплочение:** `Emote_RallyUp`, `Emote_RaisedFist`.

**Тонт / вызов:** `Emote_BringItOn`, `taunt_come_here`/`Emote_ComeHere`, `taunt_mock_bow`, `challenge_stance`, `Emote_Taunt`, `Emote_Taunt_BringItOn_T1`, `point_at_opponent`.

**Танцы:** `Emote_70sDance`, `dance_arm_wave`, `dance_simple_groove`, `TheEleven` (⚠️ verify), `SelectScreen_Emote`.

**Сигналы кооп:** `thumbs_up`/`thumbs_down`/`Emote_ThumbsDown`, `hold_up_wait` (стоп/жди), `shrug_i_dont_know`/`Emote_IDK`, `Emote_Taunt_Shhh_T1` (тише — стелс-сигнал), `Emote_Taunt_NoNoNo_T1`, `Emote_Master_BackToWork_T3`.

**Соц-смех/комедия:** `Emote_Laugh`, `laugh_body`, `facepalm`.

⚠️ **Дыры хаба:** «рукопожатие» и «утешить тиммейта (comfort)» прямых клипов не имеют. Заглушки: handshake → `bow_deep_respect`/`Emote_Greeting`; comfort → `hold_up_wait` + `sit_and_wave_goodbye`. Помечаю как открытые.

---

## 5. Рекомендованные батчи ретаргета (по порядку ценности)

Правило ретаргетеров: **UE Manny → `RTG_MCO_to_Operator`**, **UE5-маннекин → `RTG_UEFN_to_Operator`**.

**Батч 1 — Motifect Injured/Exhausted (~25 клипов).** Ретаргетер: проверить скелет источника; вероятно UE5-маннекин → `RTG_UEFN_to_Operator`. Зачем: за один заход закрывает first-aid (`bandage_arm`/`inject_self`), drag (`drag_self_forward`), revive (`revive_gasp_awake`/`revive_slowly_rise`), cower (`curl_up_fetal`), panic (`panic_look_around`), dizzy (`stagger_dizzy`), cough/газ (`cough_fit`), exhausted-crawl. Самая высокая геймплейная отдача — делать ПЕРВЫМ.

**Батч 2 — Motifect Emotes/Social (~30).** Тот же скелет/ретаргетер, что батч 1. Зачем: весь хаб (greet/celebrate/taunt/dance/signals) + `shrink_away_scared` (cower/джампскейр) + facepalm/laugh/slump.

**Батч 3 — Getting-Up / recovery.** Если есть отдельный пак Getting-Up — импортировать здесь; иначе это get-up/recovery из Motifect (`revive_slowly_rise`, `collapse_to_knees`) + bench-get-up (`M_interaction_bench_out_to_stand`). Ретаргетер по скелету пака.

**Батч 4 — GAS traversal + turn-in-place + sit (~50, GASP).** Источник: GASP/UE5-маннекин → `RTG_UEFN_to_Operator`. Брать: Traversal Climb/Vault/Mantle/Hurdle (run/walk/stand) + Catch-позы; ОДИН канонный turn-набор `M_Neutral_Stand_Turn_*` + `Walk_Turn_*`; bench-sit (`M_interaction_bench_*`); idle-breaks `M_Neutral_Stand_Idle_Break_v01-03`. Зачем: закрывает traversal/climb/vault/mantle/hurdle + turn-in-place + sit + rich-idle одним паком на родном UE5-скелете (чистый ретаргет).

**Батч 5 — Curated Paragon operator one-offs (~8–10).** Источник: UE Manny → `RTG_MCO_to_Operator`. Только точечно: `Ability_Grenade_Throw`/`RMB_Throw` (throw), `Sprint`, `TazerTrap` (электро, после verify), `Steel_Knockdown_Enter` (trip/fall), `Steel_HitReact_Cringe` (флинч). НЕ тащить весь Paragon-locomotion/turn — он дублирует батч 4.

**Батч 6 — Curated Paragon monster moveset (ТОЛЬКО после снятия паузы).** Источник: UE Manny → `RTG_MCO_to_Operator`, но цель ретаргета — **скелет монстра, не оператора**. Брать ОДИН набор `Steel_*` (~30: idle/walk/run/jog все стороны + melee-комбо + charge + GuardBreak + death + hitreact + knockdown + LevelEntry) + добор grab (`RMB_Pull`/`Ability_Hook_Pull`), slam (`Q_Slam`), вой/клич (`Emote_Taunt_Howl_T1`/`Emote_Master_BattleCry_T3`).

---

## 6. Уже импортированные модули (доступны сейчас)

Все ниже помечены «(imported)» — уже в `Content`. Для каждого: проверить, на каком скелете лежит. Если на родном/совместимом — **use-as-is** в AnimBP; если на чужом — разовый ретаргет.

- **ZombieAnimationPack** — монстр: `anim_Attack_A-D`, `anim_Walk_A-C`/`anim_Run_A`, `anim_Belly_Crawling_A/B`, `anim_Idle_A/B`+`Laying_Idle`, `anim_Taking_Damage_A-D`, `anim_Impaled_A/B`, `anim_Burst_A/B` (verify gore vs emerge). **Use-as-is** для прототипа монстра; при финальном SK_Monster — ретаргет.
- **Interaction pack (54)** — оператор: `A_Door_CrowbarBrake` (взлом ломом), `A_Narrow` (протиснуться), `A_StepOver` (перешагнуть), `A_KeyBoardUse`/`A_PlaceDevice(Floor)` (панель/девайс — кандидат на kneel-repair), `A_Push`, `A_Crate_Open/Close/Break`, `A_TrapBreak`, `A_Investigation`, `A_Crowbar_Attack`, `A_Phone_Full/Start/Loop/End/Click` (рация), TD-такдауны. **Use-as-is**, повесить на систему интеракций.
- **Free Interaction Animation (10)** — оператор: `anim_PullLever`/`ReachLever`/`IdleLever`/`ExitLever` (закрывает switch/valve/lever-дыру), `anim_OpenDoor_L/R`/`Push_L/R`/`Kick_L/R` (двери, в т.ч. вышиб ногой). **Use-as-is**.
- **Free Crawl Animation (10)** — both: `anim_Prone_Fwd_Loop_L/R` + `Start/Stop_L/R` + `Prone_Idle` + `Stand_To_Prone`/`Prone_To_Stand`. **Use-as-is** для раненого ползания оператора и ползания монстра.
- **Free Ladder Animation Set (12)** — оператор: полный набор `AS_Neutral_Ladder_*` (Bottom/Up Enter+Exit_L/R, Move_Up/Down_L/R, Idle_Loop_L/R) — закрывает ladder-дыру целиком. **Use-as-is**.
- **Dynamic Falling (10)** — оператор: `RM_Roll_front`, `RM_Roll_Run_F` (трип-в-ролл на бегу), `RM_Roll_back` — trip/fall-рекавери. **Use-as-is** (root-motion — проверить настройку RM в AnimBP).

---

## 7. Пропустить

Оружейные/стрелковые (rifle/pistol/aim/reload/fire/ironsights/ADS), таргетинг-aim-offset (AO_), аддитивы (_Additive), facial/HeadOnly, *_120fps-дубли, MOBA-ability-клипы, читаемые только с VFX-китом конкретного героя; **плюс** избыточные дубли turn-in-place / idle-break / melee-combo, размазанные по героям Paragon — оставляем по ОДНОМУ набору, остальное дроп.