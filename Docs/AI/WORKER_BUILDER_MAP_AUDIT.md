# Worker Bundle Character Builder Map Audit

Дата read-only проверки: 2026-07-03.

Проверенный asset:

`/Game/Modular_Workers/Maps/MegaBundle_Character_Builder.MegaBundle_Character_Builder`

Карта не открывалась и не сохранялась. Аудит выполнен через Asset Registry, Blueprint metadata, Widget Tree и read-only анализ сериализованных имён.

## Краткий вывод

Character Builder — демонстрационная Blueprint-система, а не reusable data-driven runtime module. Level Blueprint создаёт UI, а вся логика выбора находится в `WB_Male_Modular_Character_Builder`. Widget содержит массивы hard-referenced meshes и числовой индекс для каждого слота. После Next/Previous он напрямую меняет mesh соответствующего `SkeletalMeshComponent` в `CBP_Male_Quantum_Character`.

Для Avariika безопасно переиспользовать каталог mesh и идею slot/index selection. Нельзя переносить demo Character, Level Blueprint и widget как runtime authority: они обойдут `WorkerAppearanceComponent`, не имеют сохранения и не реализуют сетевую authority.

## 1. Кто управляет Character Builder

Управление разделено между тремя Blueprint:

1. Level Blueprint карты `MegaBundle_Character_Builder`:
   - на `BeginPlay` создаёт `WB_Male_Modular_Character_Builder`;
   - добавляет widget во viewport;
   - включает mouse cursor через PlayerController;
   - переводит view target на `CBP_Male_Quantum_Character`/`BP_Camera`;
   - содержит camera switching/rotation input.
2. `WB_Male_Modular_Character_Builder`:
   - хранит все массивы вариантов и текущие индексы;
   - обрабатывает кнопки Next/Previous/Reset;
   - применяет выбор к demo Character.
3. `CBP_Male_Quantum_Character`:
   - demo Character с 31 Blueprint component;
   - предоставляет именованные mesh components;
   - в Construction Script назначает им Leader Pose относительно основного `Mesh`.

Отдельного manager component, DataTable или Data Asset у builder map нет.

## 2. Где находятся 25 presets

Готовые полнотелые presets хранятся как отдельные Skeletal Mesh:

`/Game/Modular_Workers/Mesh/Male/Presets/SKM_Worker_Male_1` … `SKM_Worker_Male_25`

Рядом находятся 25 body-oriented вариантов:

`SKM_Worker_Male_Body_1` … `SKM_Worker_Male_Body_25`

Preset UI — `/Game/Modular_Workers/Demo/Blueprint/WB_Male_Character_Presets`.

- Widget имеет переменные `Preset` (object array) и `Preset NR` (integer).
- Кнопки 1–25 напрямую устанавливают значение `Preset NR`.
- `Character_Update` берёт mesh из массива `Preset[Preset NR]` и назначает его компоненту `Preset` demo Character.
- `WB_Male_Character_Presets` имеет прямые dependencies на все 25 `SKM_Worker_Male_*`.
- Это не DataTable, не Data Asset и не структура параметров. Каждый preset — заранее собранный цельный Skeletal Mesh со своими material slots.

Из основного builder widget preset screen открывается через создание `WB_Male_Character_Presets`; обратная кнопка создаёт builder widget заново.

## 3. Как переключаются слоты

Для каждого слота widget хранит пару:

- object array, например `Head`, `Hats`, `Clothes`;
- integer index, например `Head NR`, `Hats NR`, `Clothes NR`.

Общие функции:

- `NextPart` — увеличивает индекс и циклически возвращает его к началу по длине массива;
- `PreviusPart` — уменьшает индекс и циклически переходит к последнему элементу;
- `NextPart_SM`/`PreviusPart_SM` — аналогичный вариант для static-mesh слотов;
- `Character_Update` — получает `CBP_Male_Quantum_Character`, берёт элементы массивов по `* NR` и вызывает `Set Skinned Asset and Update` на компонентах;
- для Rifle используется `Set Static Mesh`;
- выбранный demo animation проигрывается через `Play Animation` на основном `Mesh`.

UI предоставляет Next/Previous для Head, Arms, Gloves, Hats/Hair, Glasses, Headphones, Balaclava, Scarf, Gasmask, Clothes, Bulletproof/chest module, Pants, hip drops, Watch и других слотов. Отдельные `*_R` buttons сбрасывают конкретный индекс.

### Head Type

`Head`/`Head NR` выбирают European/Afro head variants. Обновление назначается основному body/head mesh path через `Character_Update`.

### Hats/Hair

UI называет объединённую категорию `HatHair`, но данные разделены:

- `Hats`/`Hats NR` выбирают hair/hat mesh;
- отдельный `Hair` component также присутствует;
- `Hats Rest` сбрасывает Hats и Headphones вместе, чтобы избегать конфликтующих head modules.

### Clothes и Overalls

`Clothes`/`Clothes NR` выбирают jackets, shirts и T-shirts. Отдельной переменной или компонента `Overalls` в demo Character нет. Overalls assets присутствуют в dependencies widget и представлены заранее объединёнными meshes; они применяются через существующие Clothes/Pants-related arrays и special combinations, а не через независимый runtime slot `Overalls`.

### Pants

`Pants`/`Pants NR` назначаются компоненту `Pants`. Массив содержит jeans и worker pants; overalls combinations могут одновременно закрывать части Clothes/Pants, поэтому независимое смешивание не гарантировано.

### Gloves и Arms

- `Arms`/`Arms NR` назначаются компоненту `Arms`.
- `Gloves`/`Gloves NR` назначаются компоненту `Gloves` и связанному arm module.
- Есть варианты bare hands, Afro, tattoo/ornament и готовые glove meshes.

### Glasses и face slots

`Glasses`, `Balaclava`, `Scarf`, `Gasmask` имеют отдельные arrays, indices и components. Beard assets находятся среди dependencies, но отдельного Beard control в widget tree не выявлено; beard/hair combinations частично представлены готовыми mesh variants.

## 4. SkeletalMeshComponents demo Character

В `CBP_Male_Quantum_Character` найдены следующие Blueprint components.

Основные body/customization components:

- inherited `Mesh`;
- `Preset`;
- `Arms`;
- `Clothes`;
- `Pants`;
- `Gloves`;
- `Hair`;
- `Hat`;
- `Glasses`;
- `Headphones`;
- `Balaclava`;
- `Scarf`;
- `Gasmask`;
- `Watch`.

Equipment/module components:

- `Backpack`;
- `Armor`;
- `Bulletproof`;
- `Bulletproof_Drops`;
- `Tactical_Belt`;
- `Hip_Left`, `Hip_Right`;
- `Arm_Module_Right`;
- `Helmet`, `Helmet_Module`, `Helmet_Drops`, `Helmet_Headphones`;
- `Patch_Back`, `Patch_L_Arm`, `Patch_R_Arm`, `Patch_Cap`, `Patch_Helmetn`.

Отдельно есть `Rifle` как `StaticMeshComponent`.

Construction Script вызывает `Set Leader Pose Component` для modular SkeletalMeshComponents относительно основного `Mesh`. Demo Character не содержит replicated appearance properties (`NumReplicatedProperties: 0`).

## 5. Reset All

В asset функция названа с опечаткой `Rest All`.

Она сбрасывает числовые indices почти всех категорий:

- Arms, Head, Gloves, Bracelet;
- Hats, Glasses, Headphones;
- Balaclava, Scarf, Gasmask;
- Clothes, Bulletproof, TacticalBelt, Backpack, Pants;
- right/left hip drops, Rifle;
- Bulletproof drops, Armor, under drops, Watch.

Дополнительно:

- вызывает `Helmet Rest`, который сбрасывает Helmet, Helmet Headphones, Helmet Modules и Helmet Drops;
- patches сбрасываются отдельной функцией `Rest_Patch`;
- после reset button вызывается `Character_Update`, который повторно применяет элементы массивов по сброшенным индексам.

Reset не восстанавливает сериализованный профиль и не загружает preset из хранилища. Это только установка index variables в Blueprint defaults с последующим reassignment meshes.

В widget tree есть две похожие кнопки: `RestAll` выполняет reset, а `RestAll_1` связана с переходом к экрану готовых presets. Названия не следует использовать как устойчивый production contract.

## 6. Skin color и material parameters

Builder widget не создаёт Dynamic Material Instances и не вызывает:

- `SetVectorParameterValue`;
- `SetScalarParameterValue`;
- `SetMaterial`.

В нём не найдены ссылки `SkinColor` или `Color Correction`. Skin appearance меняется выбором готовых European/Afro meshes и material/texture variants.

В пакете существует сложный материал головы с внутренним именем `SkinColor`, но оно не exposed как доступный vector/scalar parameter у используемых preset instances. У `MI_Quantum_Head`/`MI_Quantum_Body` доступен общий `Color Correction`, однако builder map его не использует. Поэтому в этой карте нет реализованного skin-color picker.

## 7. Сохранение внешности

Сохранения нет.

В builder/preset widgets и demo Character не найдены:

- SaveGame class;
- `CreateSaveGameObject`;
- `SaveGameToSlot`;
- `LoadGameFromSlot`;
- DataTable/DataAsset с текущей selection state.

Выбор существует только в переменных текущего widget instance. Переход между builder и preset widgets создаёт новые widgets, поэтому это demo presentation, а не надёжная persistence architecture.

## 8. Что безопасно перенести в WorkerAppearanceComponent

Можно переиспользовать:

- сами Quantum Worker Skeletal Mesh и их Material Instances;
- список логических slot names;
- идею `slot -> массив вариантов -> selected index`;
- совместимые Leader Pose modules;
- 25 готовых full-mesh presets как отдельный тип выбора;
- зависимости/ограничения между слотами, например Hats против Headphones и combined Overalls против отдельных Clothes/Pants.

Переносить следует не Blueprint arrays из widget, а данные:

1. Создать стабильный enum/ID слота в существующей appearance architecture.
2. Описать варианты soft references в Data Assets/DataTable или C++ configuration, не в UI widget.
3. Дать `WorkerAppearanceComponent` единоличное право назначать meshes/materials.
4. UI V2 должен только отправлять selection request/preview request.
5. Server должен валидировать и реплицировать выбранные IDs.
6. Preset должен быть набором slot IDs либо явно обозначенным full-mesh preset, а не прямой ссылкой из кнопки.

Нельзя переносить:

- Level Blueprint;
- `CBP_Male_Quantum_Character` вместо `AvaryoCharacter`;
- `WB_Male_Modular_Character_Builder` как runtime logic owner;
- прямой `GetActorOfClass` + `SetSkinnedAssetAndUpdate` из UI;
- hard-reference arrays из widget;
- demo camera/input flow;
- demo animation playback;
- Reset defaults без проекта-owned preset/schema;
- отсутствие replication/save semantics.

## Таблица переноса слотов

| Слот | Где реализован | Что хранит | Можно ли переиспользовать | Как перенести в Avariika |
|---|---|---|---|---|
| Preset | `WB_Male_Character_Presets`; component `Preset` | 25 full Skeletal Mesh `SKM_Worker_Male_1`–`25` и индекс | Да, как asset catalog | Создать project-owned preset IDs; применять через `WorkerAppearanceComponent`, отдельно определить full-mesh/slot behavior. |
| Head Type | `Head`/`Head NR`; основной `Mesh` | European/Afro head/body variants | Да | Slot `Head` или base-body variant с проверкой совместимых рук/торса/material set. |
| Hats/Hair | `Hats`/`Hats NR`; `Hat`, `Hair` | Hair, caps, hats, warm hats | Да | Разделить `Hair` и `Headwear`; добавить compatibility rules с headphones/helmet. |
| Clothes | `Clothes`/`Clothes NR`; `Clothes` | Jackets, shirts, T-shirts | Да | Slot `UpperBody`; хранить soft mesh/material references вне UI. |
| Overalls | В dependencies и combined Clothes/Pants meshes; отдельного component нет | Готовые объединённые комбинезоны | Частично | Отдельный mutually-exclusive `Overall` mode, который блокирует/заменяет UpperBody + LowerBody + иногда Boots. |
| Pants | `Pants`/`Pants NR`; `Pants` | Jeans, worker pants и связанные variants | Да | Slot `LowerBody`; валидировать сочетание с clothes/overalls/boots. |
| Arms | `Arms`/`Arms NR`; `Arms` | Bare/Afro/tattoo/ornament arms/hands | Да, осторожно | Base skin/arms variant, согласованный с Head/Body material family. |
| Gloves | `Gloves`/`Gloves NR`; `Gloves`, `Arm_Module_Right` | Worker gloves and print/color variants | Да | Slot `Gloves`; применять server-authoritatively, не напрямую из widget. |
| Glasses | `Glasses`/`Glasses NR`; `Glasses` | Aviator, classic, worker glasses | Да | Slot `Glasses`; soft references и visibility/compatibility checks. |
| Headphones | `Headphones`/`Headphones NR`; `Headphones`, helmet headphones | Worker headphones/mic variants | Да | Slot `Headphones`; conflict rules с hats/helmets. |
| Balaclava | `Balaclava`/`Balaclava NR`; `Balaclava` | Face-covering mesh variants | Да, после clipping test | Slot `Mask`; правила с beard, glasses, helmet. |
| Scarf | `Scarf`/`Scarf NR`; `Scarf` | Neck/scarf variants | Да | Slot `NeckAccessory`; проверить clipping с clothes/armor. |
| Gasmask/Respirator | `Gasmask`/`Gasmask NR`; `Gasmask` | Face protection meshes | Да | Slot `Mask`; gameplay protection не выводить из cosmetic mesh без отдельной authority. |
| Bulletproof/Chest | `Bulletpruf`, `Bulletpruf_Drops`; `Bulletproof`, `Bulletproof_Drops` | Vests and chest attachments | Частично | Разделить cosmetic chest module и gameplay equipment state; не смешивать UI appearance с inventory. |
| Backpack | `Backpack`/`Backpack NR`; `Backpack` | Back modules | Частично | Cosmetic slot либо visual of authoritative equipment; inventory остаётся отдельной системой. |
| Hip Left/Right | hip arrays; `Hip_Left`, `Hip_Right` | Hip bags/drops | Да, визуально | Slots `HipLeft`/`HipRight`; проверить locomotion clipping. |
| Helmet modules | Helmet arrays; четыре helmet components | Helmet, drops, headphones, modules | Да, но сложно | Composite headwear definition с дочерними module IDs и compatibility rules. |
| Watch/Bracelet | `Watch`, `Bracelet` arrays; `Watch` | Wrist accessories | Да | Slot `WristAccessory`; определить left/right semantics. |
| Patches | patch arrays; пять patch components | Back/arm/cap/helmet patches | Да | Cosmetic sub-slots или material/decal IDs; не создавать отдельный gameplay system. |
| Rifle | Static-mesh array/component | Demo weapon mesh | Нет для customization | Оставить оружие/инструменты в inventory/equipment architecture Avariika. |
| Animation/Pose | `Animation`/`Anim NR`; основной `Mesh` | Demo `A_Pose_*`, `A_MM_*` | Только как референс | Не включать в appearance component; animation остаётся обязанностью `ABP_Worker`. |
| Skin color | В карте не реализован | Только готовые texture/material families | Нет как готовую функцию | Сначала создать подтверждённый parameter mapping для Head/Body/Arms; затем отдельный appearance field. |

## Итог

Builder map полезна как исчерпывающий ручной каталог Worker Bundle и как источник slot taxonomy. Её архитектуру нельзя переносить буквально. Production-реализация Avariika должна оставить UI пассивным, `WorkerAppearanceComponent` — единственным владельцем внешности, а сервер — владельцем подтверждённого replicated selection state.
