# MODULAR_WORKER_PLAN — интеграция Modular Workers (Quantum) как персонажа-аватара

> 2026-06-23. Юзер: «modular worker bundle уже в проекте, давай полностью интегрировать». Сегодня юзер отдыхает (без стройки) — работаю автономно. Этот док — грунт + фазовый план; правки по ходу.

## Что за пак (факты разведки)
- **`Content/Modular_Workers/`** — модульный «Quantum» рабочий, **874 ассета**. ✅ **ЛИЦЕНЗИОННЫЙ** (юзер купил за $50 — вариант B «Modular Workers Bundle» из [[char-creator-gear-progression]], НЕ generic) → release-safe, НЕ плейсхолдер. **Заменяет пиратский CitizenNPC** как тело игрока/бригады.
- **Только мужские меши — это by design** (юзер подтвердил: женщин-работяг не нужно). Female-текстуры в паке игнорируем. Бригада = мужики.
- **Скелет: `SK_Male_Quantum_Character_Skeleton` = UE5-mannequin** (кости `spine_01..05`, `neck_01/02`, `upperarm_twist_01/02`, `ik_foot_root`, `ik_hand_gun`, `index_01`, `thumb_01`). + `SK_Quantuym_FaceRig` (лицо).
- **Сборка** (`Demo/Blueprint/CBP_Male_Quantum_Character`): тело-лидер + дочерние SkeletalMesh-компоненты по слотам через `SetMasterPoseComponent` (в 5.7 = `SetLeaderPoseComponent`). Слоты в BP: **Body, Head, Hair, Helmet(+Headphones/Drops), Hat, Cap, Glasses, Gloves, Arms, Pants**.
- **Части тела (модульно):** Head, Body_Full/Body_Bottom, Feet, Legs, FullHands/Hands, Wrist, Arms (+ варианты этничности Afro/European, тату/орнамент на руках).
- **Одежда:** Tshirt/Tshirt_Tucked, Shirt_RolledUp, Jacket (M65/Worker/Worker_Hood_1/2), Vest_Worker (hi-vis), Overalls (комбинезоны ×6), Pants (Jeans/Jumpsuit/Pants_Worker/_Full), Boots.
- **Голова/лицо:** Helmet_Worker (каска!), Hat (Cowboy/Farmer/Flat/Up), Cap, Warm_Hat, Headphones(+Micro), Hair, Beard, Glasses (Aviator/Classic/Worker), **Respirator** (= наш газо-механик!).
- **Руки:** Gloves_Worker (+Print-цвета), Watches.
- **Грудь/пояс:** Vest_Worker, Bag_Banana, Bag_Hip.
- **Пресеты:** `Mesh/Male/Presets/` — **20+ готовых SKM_Worker_Male_N** (запечённые комплекты) + дата-ассеты `PA_Worker_Male`, `PA_Worker_Male_Body`.
- **Материалы:** мастер `Quanum_MasterMaterial` + MI-варианты цветов (штаны/перчатки/каски и т.д.).
- **Демо-карты:** `Maps/MegaBundle_Character_Builder`, `Preview_Assets`, `Preview_Presets`.
- **Нет** своих AnimBP/IK-ретаргетеров → анимация через ретаргет на UE5-mann.

## Наша текущая база (факты из кода)
- **`AAvaryoCharacter : ACharacter`** — true-FPS. Тело = `GetMesh()` (сейчас **CitizenNPC, UE4-mann**), FP-руки = `FirstPersonMeshComp` (из BP), TP-камера на пружине (`ToggleCameraMode`/V).
- **Снаряжение-как-функция сейчас ИНВЕНТАРНОЕ, невидимое:** `HasGasMask()`, `HasWeldingMask()`, `HasRubberBoots()`, `IsHoldingGasDetector()` — проверяют предмет в слотах по ToolTag. Носимых мешей нет.
- **Данные владения снаряжением УЖЕ есть** (`AvariikaSaveGame.h`): `FEquipmentLevels{ bHelmet, bGloves, bGasMask, Flashlight/Tester/... }` + `FCareerStats.Cosmetics` (наклейки/цвета касок). → Modular Workers даёт этим флагам визуал.
- **Анимы:** 7 монтажей (Fix/Hit/Death/Knocked/Revive/Bandage/Drink) + локомоция — всё на **UE4-mann**, играют на CitizenNPC через compatible-skeleton.

## Стержень: скелетный разрыв
Наши анимы — **UE4-mann (3 спайна)**; Quantum — **UE5-mann (5 спайнов)**. Прямой compatible-skeleton НЕ сработает → нужен **ретаргет UE4-mann → UE5-mann** (классический кейс Epic; пайплайн есть, см. [[ik-retarget-python]]). Альтернатива: взять локомоцию из бесплатного UE5-mann пака, ретаргетить только наши one-shot монтажи.

## Дизайн-решение (моё, по вижну — фиксирую, юзер может развернуть)
Модульный рабочий = **аватар игрока** (создание персонажа), по вижну [[char-creator-gear-progression]] «создай работягу, старт-бомж (тишка/джинсы) → зарабатывай на каску/жилет/маску/ботинки; косметика=функция».
- **TP/кооп:** тиммейты видят собранного рабочего; надетое снаряжение читается в бою (каска/жилет/маска).
- **FP:** свои руки — из Quantum (`SKM_Quantum_Hands/FullHands/Wrist` + перчатки), на том же скелете.
- **Косметика=функция через существующие флаги:** `bHelmet→Helmet_Worker`, `bGasMask→Respirator`, `bGloves→Gloves_Worker`; геттеры `HasGasMask()`… могут читать надетое.
- 🔑 **ПИВОТ для юзера:** подтвердить, что рабочий ЗАМЕНЯЕТ тело игрока (а не только NPC-бригаду). Я иду по «заменяет»; если юзер хочет иначе — фаза 2 разворачивается. Визуал (как смотрится в игре) **требует глаз юзера** — соберу/скомпилю/закоммичу, финальный «красиво ли» — за ним.

## Фазовый план
**Ф0. Пре-флайт (autonomous-safe).** Проверить, что пак импортирован/реестр видит скелет+пресеты; зафиксировать слоты; female не делаем (male-only by design). ✅ разведка сделана.

**Ф1. Ретаргет анимаций UE4-mann → Quantum (UE5-mann).** IK Rig на Quantum + IK Retargeter (UE4→UE5, можно от Epic-шаблона) → ретаргет 7 монтажей + локомоция-клипы. Проверка смоук/Persona. (Editor, headless-скрипт; визуал-кач-во — потом с юзером.)

**Ф2. Свап тела игрока на Quantum.** `BP_AvaryoCharacter`: тело `GetMesh()` → собранный рабочий (база-пресет «бомж»), AnimBP на Quantum, FP-руки → Quantum-hands. Сверить, что монтажи/локомоция играют, FP-камера/присед/предмет-в-руках не разъехались (наш `UpdateCrouchEye` завязан на FP-меш — см. [[character-camera-crouch]]).

**Ф3. Модульный аппирэнс-компонент (C++ `UWorkerAppearanceComponent`).** Дочерние SkeletalMesh-компоненты по слотам (Head/Hair/Torso/Legs/Feet/Hands/Gloves/Headgear/Face/Vest), `SetLeaderPoseComponent(Body)`; `SetSlotMesh(Slot, Mesh)`; реплицируемая структура `FWorkerAppearance` (индексы/софт-ссылки на части) → кооп видит; save/load в `AvariikaSaveGame`. Build-верифицируемо без визуала.

**Ф4. Снаряжение→визуал.** Драйв слотов от `FEquipmentLevels`: `bHelmet/bGasMask/bGloves` → показать меши; синхрон с функцией (надел маску — `HasGasMask()` true). Хук в магазин (`AvUpgrade`/покупка) + дев-команды.

**Ф5. Создание персонажа (UI).** Экран билдера (тело/лицо/волосы/борода + база-аутфит «бомж»: Tshirt+Jeans+кроссы), сохранение `FWorkerAppearance`. Можно опереться на демо `MegaBundle_Character_Builder` как референс.

**Ф6. Кооп-репликация надетого + полиш.** Тиммейты видят аппирэнс/снаряжение; LOD/перф (части тела дают draw-call'ы — скрывать невидимые под одеждой); цвета касок из `Cosmetics`.

## Что делаю автономно сегодня vs что ждёт юзера
- **Сегодня (безопасно, build/smoke-верифицируемо):** этот план; Ф3-скелет компонента `UWorkerAppearanceComponent` (логика/данные/репликация/сейв — компилится и тестируется без картинки); ретаргет-скрипты Ф1 (прогон + смоук).
- **Ждёт глаз юзера (визуал):** как смотрится свап тела (Ф2), качество ретаргета, билдер (Ф5) — соберу и закоммичу, «красиво/правильно ли» — ревью юзера, когда вернётся.

_Связано: [[char-creator-gear-progression]], [[operator-character-anim-system]], [[anim-skeleton-truth]], [[ik-retarget-python]], [[character-camera-crouch]], [[asset-licensing]]._
