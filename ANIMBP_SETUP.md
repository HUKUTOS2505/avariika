# Сборка локомоции оператора — рецепт (10 мин, руками в редакторе)

> Почему руками: UE **не отдаёт в Python** редактирование AnimGraph и добавление семплов в BlendSpace (проверено зондом — нет API). Поэтому этот шаг — стандартные клики в редакторе. Заготовки уже созданы: `Content/Characters/Operator/Locomotion/ABP_Operator` + `BS_Op_Locomotion`.

## Шаг 1 — BlendSpace `BS_Op_Locomotion` (2 мин)
1. Двойной клик → откроется BlendSpace.
2. Справа **Asset Details → Axis Settings → Horizontal Axis**: Name = `Speed`, Minimum = `0`, Maximum = `500`.
3. Перетащи на график (по горизонтали) из `Anims_Retarget`:
   - `Op_MOB1_Stand_Relaxed_Idle_v2_IPC` → позиция **0**
   - `Op_MOB1_Walk_F_IPC` → ~**150**
   - `Op_MOB1_Jog_F_IPC` → ~**300**
   - `Op_MOB1_Run_F_IPC` → ~**500**
4. Save.

## Шаг 2 — `ABP_Operator` (5 мин)
**AnimGraph:**
1. Перетащи `BS_Op_Locomotion` в граф → появится нода «Blendspace Player».
2. На пине **Speed** ноды: правый клик → **Promote to Variable** → назови `Speed`.
3. Соедини выход Blendspace Player → **Output Pose**.

**Event Graph:**
1. `Event Blueprint Update Animation` →
2. `Try Get Pawn Owner` → из него `Get Velocity` → `Vector Length` →
3. `SET Speed` (перетащи переменную Speed, режим Set), вход = результат Vector Length.

4. **Compile + Save.**

## Шаг 3 — Подключить к игроку (1 мин)
1. Открой `BP_AvaryoCharacter` (`/Game/Avariika/Blueprints`).
2. Выбери компонент **Mesh** → Details:
   - **Skeletal Mesh** = `SK_Operator` (если ещё не он).
   - **Animation → Anim Class** = `ABP_Operator`.
3. Compile + Save.

## Шаг 4 — Тест
PIE → ходи/беги: idle→walk→jog→run блендится по скорости. ✅

---

## Дальше (по желанию, тоже руками — см. `ANIMBP_PLAN.md`)
- **Прыжок**: state machine Ground↔Jump (A_Regular_Jump / A_Jumping_Down) по `IsFalling` (из `Get Movement Component → Is Falling`).
- **Присед**: второй BlendSpace на `Op_M_Neutral_Crouch_Loop_*` + переключение по `bIsCrouched`.
- **Слот монтажей** (`DefaultSlot`) для action-анимаций (ремонт/подбор/бинт/эмоуты) — играются из C++/BP поверх локомоции.
- **Оверрайды состояний** (раненый/испуг/woozy) — Layered blend / выбор позы по геймплей-флагу.

Это итеративно; для теста с ребятами достаточно Шагов 1-3 (ходячий оператор).
