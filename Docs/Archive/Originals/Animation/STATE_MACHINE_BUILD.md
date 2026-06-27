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
