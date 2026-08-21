# Worker Bundle: аудит материалов и анимаций

Дата read-only проверки: 2026-07-03. Проверенный корень пакета: `/Game/Modular_Workers`.

## Краткий вывод

Worker Bundle — прежде всего набор модульных мужских Skeletal Mesh, готовых собранных пресетов, материалов и текстур. Полноценной готовой locomotion-системы в пакете нет: отсутствуют `AnimBlueprint`, `BlendSpace`, `PoseSearchDatabase` и другие Motion Matching assets. Есть только 14 отдельных `AnimSequence`, демонстрационный `Character` Blueprint и UI/карты предпросмотра.

Пакет использует собственный скелет `SK_Male_Quantum_Character_Skeleton`. Текущий `ABP_Worker` ориентирован на другой скелет — `/Game/_Packs/WorkAnimations/Demo/Mannequins/Meshes/SK_Mannequin`. Поэтому прямое назначение анимаций Worker Bundle в `ABP_Worker` небезопасно; нужен отдельный проверенный retarget на активный mannequin-compatible путь.

## Инвентаризация заявленных возможностей

| Возможность | Фактическое состояние |
|---|---|
| Male Character Builder Map | Есть функциональный эквивалент: `/Game/Modular_Workers/Maps/MegaBundle_Character_Builder`. Его обслуживают `WB_Male_Modular_Character_Builder` и `CBP_Male_Quantum_Character`. |
| Ready-to-use presets / 25 presets | Есть 25 готовых полных Skeletal Mesh: `SKM_Worker_Male_1`–`SKM_Worker_Male_25`, а также 25 соответствующих body-only mesh: `SKM_Worker_Male_Body_1`–`SKM_Worker_Male_Body_25`. Есть карта `/Game/Modular_Workers/Maps/Preview_Presets` и виджет `WB_Male_Character_Presets`. |
| Motion Matching demo | Не найден. Нет `PoseSearchDatabase`, `PoseSearchSchema`, Motion Matching AnimBP или другого рабочего MM-графа. Префикс `A_MM_` у нескольких AnimSequence сам по себе не является Motion Matching системой. |
| Lyra demo test | Не найден в `/Game/Modular_Workers`; зависимостей на Lyra у проверенных demo assets также не выявлено. |
| Third Person demo | Отдельная Worker Bundle карта/система с таким назначением не найдена. Найденный в проекте `LocomotionAnimPack/Demo/ThirdPerson` относится к другому content pack. |
| Animation Blueprint | В Worker Bundle отсутствует. |
| BlendSpace | В Worker Bundle отсутствует. |
| Animation Sequence | Есть 14 штук в `/Game/Modular_Workers/Demo/Animations/Male`. |
| Pose Search Database | Отсутствует. |
| Motion Matching assets | Отсутствуют, кроме имен `A_MM_*`, которые являются обычными `AnimSequence`. |
| Master Materials | Есть `M_Quantum_Master_Material`, специализированные материалы головы, тела, глаз, зубов и волос. |
| Material Instances | Есть 118 `MaterialInstanceConstant`, включая body/arms/head и множество вариантов одежды. |
| Control Rig | Ассетов класса `ControlRigBlueprint` в пакете нет. |
| Face Control Rig | Готового Face Control Rig asset нет. Есть отдельный face-rig skeleton/meshes и morph targets, но не Control Rig UI/graph. |

## Анимации и locomotion

В пакете найдены:

- `A_MF_Idle`, `A_MF_Walk_Fwd`, `A_MF_Run_Fwd`;
- `A_MM_Idle`, `A_MM_Walk_Fwd`, `A_MM_Walk_InPlace`, `A_MM_Run_Fwd`;
- `A_MM_Jump`, `A_MM_Fall_Loop`, `A_MM_Land`;
- `A_Pose_0`–`A_Pose_3`.

Это ограниченный набор одиночных клипов, а не законченная locomotion-система. Нет strafing-набора, crouch locomotion, starts/stops, turn-in-place, directional BlendSpace, state machine или Pose Search базы. Проверенные `A_MM_Run_Fwd` и `A_MF_Run_Fwd` импортированы из FBX и используют собственный Worker skeleton; оба имеют root motion. Их import metadata указывает на авторский каталог `Modular Military Soldier`, а не на Lyra или стандартный UE mannequin content.

### Можно ли применить к `ABP_Worker`

Только после retarget и отдельной визуальной проверки. Прямая подстановка невозможна из-за разных skeleton assets:

- Worker Bundle: `/Game/Modular_Workers/Mesh/Male/SK_Male_Quantum_Character_Skeleton`;
- `ABP_Worker`: `/Game/_Packs/WorkAnimations/Demo/Mannequins/Meshes/SK_Mannequin`.

Даже после retarget эти 10 locomotion/action clips не заменяют активную Mobility/WorkAnim locomotion: покрытия направлений и состояний недостаточно. Потенциально полезны отдельные idle/pose/jump clips как экспериментальные источники, но не как замена state machine.

## Материалы кожи и одежды

Основной универсальный материал одежды — `/Game/Modular_Workers/Materials/Quanum_MasterMaterial/M_Quantum_Master_Material`. Read-only запрос Unreal Material API вернул следующие доступные параметры:

- scalar: `Brightness`, `Color Correction Value`, `Contrast`, `Metallic Intensity`, `Normal Strength`, `Roughness Contrast`, `Roughness Intensity`, `Saturation`;
- vector: `Color Correction`;
- texture: `AO_Texture`, `Alpha_Map`, `BaseColor`, `Emissive`, `Metallic_Texture`, `Normal`, `ORM`, `Roughness_Texture`;
- static switch: `AO`, `Custom_Alpha_Map`, `Emissive`, `Metallic`, `Roughness`.

Эти параметры доступны и у проверенных `MI_Quantum_Head` и `MI_Quantum_Body`, поскольку они наследуются от того же master material. Отдельного exposed параметра `SkinColor`, `Skin Tone` или `Melanin` Unreal Material API у этих рабочих instances не возвращает. Для изменения оттенка доступен лишь общий `Color Correction`, который не является специализированной и уже проверенной системой цвета кожи.

Для головы также существует сложный материал `/Game/Modular_Workers/Materials/Male_Body/Head_Material_Functional/M_Head_Baked_Quantum`. В его сериализованном графе встречается имя `SkinColor`, но Unreal не возвращает его как доступный scalar/vector parameter. Реально exposed skin-related параметр здесь — scalar `SkinScattering`; color parameters отсутствуют (`VECTOR []`). Поэтому `SkinColor` нельзя считать готовым вызываемым параметром кастомизации без дополнительной ручной проверки/переработки графа.

Готовые пресеты, например `SKM_Worker_Male_1`, зависят от `MI_Quantum_Head`/`MI_Quantum_Arms` и отдельных European/Afro texture sets под общим master material. Фактически пакет надёжно предоставляет texture/material variants, а не подтверждённый единый runtime skin-tone control для головы, рук и тела.

Одежда представлена большим набором Material Instances с готовыми цветами: рабочие куртки, футболки, рубашки, брюки, комбинезоны, ботинки, перчатки, каски и аксессуары. Эти instances безопаснее переиспользовать вместе с их родными mesh и textures, не меняя parent materials.

## Body variants и morph targets

У обычного тела `SKM_Quantum_Body_Full` и проверенного готового `SKM_Worker_Male_Body_1` указано `MorphTargets: 0`. Следовательно, 25 пресетов — не 25 наборов morph slider values, а 25 заранее собранных Skeletal Mesh с выбранными модулями и материалами.

Есть отдельные baked mesh variants, включая European/Afro и `SKM_Qunatum_FaceRig_Body_Hight`, но проверенный `Body_Hight` также имеет `MorphTargets: 0`. Это отдельная геометрия, не runtime body-shape morph.

Исключение — `SKM_Quantum_FaceRig`: он имеет 686 morph targets и отдельный `SK_Quantuym_FaceRig` с 870 bones. Это реальный facial deformation data, но без готового `ControlRigBlueprint`/Face Control Rig. Кроме того, face-rig skeleton отличается от обычного Worker skeleton и от skeleton `ABP_Worker`, поэтому его нельзя считать drop-in facial system.

## Как устроены 25 presets

- `SKM_Worker_Male_1`–`25` — готовые полнотелые Skeletal Mesh, уже содержащие выбранную комбинацию тела, одежды, обуви, головы/аксессуаров и material slots.
- `SKM_Worker_Male_Body_1`–`25` — соответствующие облегчённые/отдельные body-oriented сборки.
- Все проверенные presets используют `SK_Male_Quantum_Character_Skeleton`.
- `WB_Male_Character_Presets` содержит прямые зависимости на все 25 полнотелых meshes и показывает их в demo UI.
- `/Game/Modular_Workers/Maps/Preview_Presets` размещает presets как демонстрационные акторы.
- Это не Data Asset таблица параметров и не система сохранённых morph-настроек. Preset выбирает готовый mesh.

## Что можно безопасно переиспользовать в Avariika

- Готовые `SKM_Worker_Male_1`–`25` как визуальные варианты при сохранении родного Worker skeleton и material dependencies.
- Отдельные модульные предметы одежды, обуви, перчаток, касок, очков, масок и аксессуаров через существующий `WorkerAppearanceComponent` после проверки совместимости master-pose/leader-pose схемы.
- Родные Material Instances вместе с их parent materials и textures.
- Карты `MegaBundle_Character_Builder`, `Preview_Presets` и demo widgets только как справочник структуры/подбора ассетов, не как production UI.
- Отдельные `A_MM_*`/`A_MF_*` clips только как источник для контролируемого retarget-эксперимента в отдельной тестовой папке.

## Что нельзя переносить напрямую

- Нельзя назначать Worker animations непосредственно в `ABP_Worker`: skeleton assets различаются.
- Нельзя заменять текущую Mobility/WorkAnim locomotion demo-клипами Worker Bundle: отсутствует достаточное покрытие и готовый AnimBP.
- Нельзя принимать `A_MM_*` за готовый Motion Matching и подключать их без Pose Search schema/database и locomotion design.
- Нельзя назначать `CBP_Male_Quantum_Character` вместо текущего Avariika Character: это отдельный demo Character Blueprint, что создаст риск для movement, networking, inventory, vitals и appearance architecture.
- Нельзя переносить demo GameMode/PlayerController/HUD/UI или карты в рабочий runtime flow.
- Нельзя смешивать face-rig skeleton с обычным Worker skeleton или `SK_Mannequin` без отдельного facial/retarget pipeline.
- Нельзя менять parent master materials или массово заменять material slots до проверки всех модулей: presets зависят от большого набора специализированных instances, глаз, зубов, волос и skin materials.
- Нельзя обещать runtime skin-color или body-type customization только на основании найденных параметров/mesh names: обычные body meshes не имеют morph targets, а согласованность `SkinColor` между головой, руками и телом не подтверждена.

## Рекомендация

Сохранять текущий `ABP_Worker` и mannequin-compatible Mobility/WorkAnim путь как production locomotion. Worker Bundle использовать как источник внешности: presets, modular meshes, materials и textures. Если понадобятся его анимации или face rig, делать отдельный retarget/face proof-of-concept без изменения рабочего Character и AnimBP до визуальной и multiplayer-проверки.
