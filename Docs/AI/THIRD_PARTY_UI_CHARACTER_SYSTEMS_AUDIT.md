# Аудит сторонних UI и character systems

Дата read-only проверки: 2026-07-03.

## Область и метод проверки

Проверены исходные папки вне проекта Avariika:

- `C:/Users/admin/Downloads/MCCUnleashed_5.7`
- `C:/Users/admin/Downloads/MenuSystemPro 5.4`
- `C:/Users/admin/Downloads/Ultimate Character Customization 5.3`

Проверялись descriptors, Config, Source, имена и структура `Content`, demo maps и сериализованные имена в `.uasset`/`.umap`. Пакеты не копировались и не открывались в Unreal Editor, поэтому выводы о бинарных Blueprint основаны на структуре и доступных metadata, а не на запуске графов.

Ничего не добавлялось в `Plugins` или `Content`, plugins не включались, `.uproject` и проектные системы не менялись.

## 1. MCCUnleashed_5.7

### Версия и состав

Фактический plugin находится в:

`C:/Users/admin/Downloads/MCCUnleashed_5.7/Plugins/MCCUnleashed_5.7`

- Есть `ModularCharacterPlugin.uplugin`.
- Имя папки заявляет UE 5.7, но descriptor содержит `EngineVersion: 5.6.0`. Следовательно, подтверждённая descriptor-версия — 5.6, а не 5.7.
- Есть `Source`, `Config`, `Content`, готовые `Binaries` и промежуточные build artifacts.
- Runtime C++ module: `ModularCharacterPlugin`.
- `CanContainContent: true`.
- Исходный код зависит от `Core`, `CoreUObject`, `Engine`, `NetCore`, `Slate`, `SlateCore`; editor build дополнительно использует `UnrealEd`.
- Content: 62 `.uasset`, 2 `.umap`.
- Demo maps: `1_CharacterSelect.umap`, `2_After_CharacterSelect.umap`.

### Что это за система

Основной элемент — `UModularCharacterComponent`, который можно добавить к Actor/Pawn. Он управляет несколькими `USkeletalMeshComponent`, применяет mesh/material parts, поддерживает Leader Pose или Copy Pose и читает начальный набор из `UDataTable`.

Данные части описаны `FCharacterPart`:

- строковое имя части;
- soft reference на `USkeletalMesh`;
- массив soft references на материалы.

Система не требует именно demo Character для работы компонента, но demo flow использует собственные:

- `BP_ThirdPersonCharacter2`;
- `BP_ThirdPersonGameMode`;
- `MCP_Instance` как GameInstance-oriented хранилище через интерфейс;
- `WBP_Skin`, `WBP_SkinMenu`;
- `DT_Demo`, `DT_DemoFemale`;
- Manny/Quinn meshes, AnimBP и BlendSpace.

Своего PlayerController/HUD asset в пакете не найдено; demo обращается к PlayerController через Blueprint. SaveGame class отсутствует. Строка `SaveGame` встречается в metadata одного demo Blueprint, но отдельной законченной SaveGame-системы нет.

### Возможности

- Модульная одежда: да, на уровне универсальных mesh/material частей.
- Character Customization UI: есть минимальный demo UI.
- DataTables/DataAssets: есть DataTables; Primary Data Assets не найдены.
- SaveGame: полноценной системы нет; persistence завязана на объект, реализующий modular-character interface, в demo — GameInstance.
- Replication: да, подтверждена исходным кодом. `CharacterPartsArray` использует `ReplicatedUsing`, push-model dirty marking, `DOREPLIFETIME_WITH_PARAMS_FAST`, RepNotify и reliable server RPC для bulk/single part updates.
- Presets: отдельных preset assets нет; возможны наборы строк DataTable/массивы частей.
- Material color customization: компонент переключает готовые material interfaces. В demo не найдено вызовов dynamic material/vector/scalar parameter customization.
- SceneCapture preview: не найден; нет `SceneCapture`/`TextureRenderTarget` assets или ссылок.
- Settings/menu/input rebinding: только demo Enhanced Input для движения; полноценной settings/rebinding системы нет.

### Совместимость с Quantum Worker и Avariika

Технически компонент skeleton-agnostic: он сможет назначать Quantum Worker parts, если все mesh используют совместимый skeleton, bone hierarchy, material slots и Leader/Copy Pose схему. Но в Avariika уже есть `WorkerAppearanceComponent`, и установка второго владельца тех же SkeletalMeshComponents создаст конфликт состояния, replication и жизненного цикла.

Безопасно взять как идею:

- `FCharacterPart`-подобное data description;
- soft references на mesh/material;
- server-authoritative выбор appearance;
- RepNotify + push-model подход;
- проверку совместимости Leader Pose/Copy Pose;
- DataTable как authoring source, если это согласуется с текущей архитектурой.

Не переносить напрямую:

- demo Character, GameMode, GameInstance, AnimBP, Manny/Quinn и demo maps;
- plugin component параллельно с `WorkerAppearanceComponent`;
- `DefaultEngine.ini` redirects и console variable;
- готовые binaries 5.6 в UE 5.8;
- C++ module без отдельного 5.8 source compatibility audit и UBT build.

Отдельный риск в исходниках: компонент сам создаёт/удаляет дочерние SkeletalMeshComponents и синхронизирует состояние с GameInstance interface. Это пересекается с текущими обязанностями Character/appearance/save architecture Avariika.

## 2. MenuSystemPro 5.4

### Версия и состав

Путь: `C:/Users/admin/Downloads/MenuSystemPro 5.4`.

- Папка маркирована как UE 5.4.
- `.uplugin`, `.uproject`, `Source` и `Config` отсутствуют, поэтому версия 5.4 подтверждается только именем поставки, а не descriptor.
- Это content-only набор для миграции: 947 `.uasset`, 4 `.umap`, localization и изображения.
- Основные каталоги: `MenuSystemPro/Blueprints` и `MenuSystemPro/ExampleContent`.
- Demo maps: `SilenceMenuLevel`, `SilencePlayLevel`, `BasicLevel`, `MoonTown`.

### Архитектурные зависимости

Core содержит собственные:

- `BP_MenuSystemActor`;
- `PDA_MenuSystemConfig`;
- `BP_MenuSystemGameInstance`;
- `BP_MenuLevelGameMode`, `BP_PlayLevelGameMode`, `BP_MenuSystemGameModeComponent`;
- `BP_SampleCharacter`, `BP_SamplePlayerController`, `BP_MenuControllerComponent`;
- `BP_SettingsManager`;
- `BP_SaveGameManager`, `BP_CustomSaveGameObject`, `BP_AutoSaveTrigger`;
- большой набор базовых widgets, style Primary Data Assets и input/config Data Assets.

Полная demo-интеграция явно предполагает замену или расширение GameInstance, GameMode и PlayerController. Character/HUD для самого framework не являются обязательной концепцией, но example flow тесно связан с sample Character/Controller и menu levels.

### Возможности

- Модульная одежда: нет.
- Character Customization UI: нет полноценной кастомизации персонажа; есть example character-selection menu/button.
- DataTables/DataAssets: широко используются Data Assets и Primary Data Assets для settings, styles, input и level metadata; также присутствуют String Tables/DataTable references.
- SaveGame: полноценный Blueprint save manager и save-slot UI есть.
- Replication: отдельная доказанная server-authoritative replication implementation не выявлена. Multiplayer/server browser UI не равен gameplay replication.
- Presets: есть graphics quality presets и UI/config presets, но не character presets.
- Material color customization: только UI material effects/style colors; к коже/одежде отношения не имеет.
- SceneCapture preview: не найден.
- Settings/menu/input rebinding: сильная сторона пакета. Есть display/graphics/audio/game/multiplayer/voice settings, Enhanced Input actions/mapping contexts, keyboard/gamepad layouts, key binder и rebind dialog.

### Совместимость с Quantum Worker и Avariika

Прямой связи с Quantum Worker skeleton или `WorkerAppearanceComponent` нет. Полезность пакета — только UI/settings архитектура.

Безопасно взять как идею/референс:

- data-driven описание settings;
- widget style Data Assets;
- layout settings screens;
- input rebinding UX и validation flow;
- footer/navigation patterns;
- отдельные визуальные и звуковые UI assets после проверки лицензии и зависимостей.

Не переносить напрямую:

- `BP_MenuSystemGameInstance`, GameModes, sample PlayerController/Character;
- menu/save manager целиком поверх существующих `AvariikaSaveGame`, PlayerController и HUD;
- полный input mapping набор вместо текущего Avariika input architecture;
- example maps и multiplayer UI как production networking;
- всю папку из 947 assets: dependency surface слишком велик;
- CommonUI/GameViewportClient или input-routing изменения без отдельной задачи. Сам набор не является основанием менять текущий viewport client.

Для UE 5.8 главный риск — не C++ compilation, а Blueprint/API migration, Enhanced Input behavior, устаревшие rendering/settings console variables и большой граф скрытых hard references.

## 3. Ultimate Character Customization 5.3

### Версия и состав

Фактический проект:

`C:/Users/admin/Downloads/Ultimate Character Customization 5.3/Ultimate Character Customization 5.3/UltimateCharacterCustomiz`

- Папка поставки маркирована 5.3.
- `UltimateCharacterCustomiz.uproject` содержит `EngineAssociation: 5.1`. Поэтому технически descriptor подтверждает проект UE 5.1, несмотря на имя архива 5.3.
- Есть `.uproject`, `Config`, `Content`; нет `Source` и `.uplugin`.
- Включены engine plugins `HairStrands` и `AlembicHairImporter`.
- Demo maps: `CustomizationMenu.umap`, `PlayLevel.umap`.
- Startup/default map — `CustomizationMenu`.
- Config назначает собственный `GameInstance_Customization`.

### Архитектурные зависимости

Система построена как самостоятельный Blueprint-проект и содержит:

- `Character_Player`;
- `Pawn_Customization`;
- `Manager_Customization`, `Manager_CustomizationCamera`;
- `AnimBP_CustomizationSkeletal`;
- `GameInstance_Customization`;
- `GameMode_CustomizationMenu`, `PlayerController_CustomizationMenu`;
- `GameMode_CustomizationPlay`, `GameMode_CPlay_PlayerController`;
- `WB_MasterWidget`, `WB_CustomizationMenu`, menu generators и saved-character widgets;
- `SG_CustomizedCharacters` и save structs;
- `DT_Male`, `DT_Female`, `DT_MetaHuman`, `DT_BaseMeshSetup`;
- большое количество structs/interfaces/function libraries.

Полный flow требует собственные GameInstance, menu/play GameModes, menu PlayerController, customization Pawn/Character и manager components. Это не drop-in widget library.

### Возможности

- Модульная одежда: да, через DataTables, mesh data и manager; фактическая совместимость зависит от skeleton/material contracts поставки.
- Character Customization UI: да, это основное назначение.
- DataTables/DataAssets: есть минимум четыре ключевые DataTables и большой набор Blueprint structs; Primary Data Assets не найдены.
- SaveGame: есть `SG_CustomizedCharacters`, saved-character UI, save structs и editor utility удаления saves.
- Replication: доказанной multiplayer replication нет. В бинарных metadata встречается `Replicated`, но server RPC/NetMulticast markers не найдены; это нельзя считать network-ready системой.
- Presets: отдельных assets с именем Preset не найдено. Сохранённые персонажи представлены SaveGame data; начальные варианты и доступные опции — строки DataTables.
- Material color customization: да. `Manager_Customization` содержит dynamic material creation и вызовы vector/scalar parameter updates, а data structs описывают colors, scalars, materials и global parameters.
- SceneCapture preview: не найден. `Manager_CustomizationCamera` управляет камерой, но ссылок на `SceneCapture`/`TextureRenderTarget` в Content не обнаружено; вероятнее используется отдельный customization Pawn в demo world.
- Settings/menu/input rebinding: есть собственные main/pause/customization menus, но полноценной settings/input rebinding системы уровня MenuSystemPro не найдено.

### Совместимость с Quantum Worker и Avariika

Прямое использование Quantum Worker не подтверждено. UCC поставляет собственные Manny/MetaHuman-oriented meshes, AnimBP, hair/groom assets и DataTables. Чтобы использовать Quantum Worker, пришлось бы полностью переописать DataTables под его modules, material parameter names, skeleton и slot contracts. Это фактически интеграционный проект, а не настройка.

`WorkerAppearanceComponent` можно оставить единственным runtime owner appearance, используя UCC только как источник UI/data-flow идей. Подключать `Manager_Customization` одновременно опасно: оба компонента будут менять meshes/materials и по-разному хранить состояние.

Безопасно взять как идею/референс:

- разделение base mesh, mesh option, material/color/scalar data;
- генераторы секций и grid widgets;
- UX сохранённых персонажей;
- camera-control UX без переноса самой Pawn/GameMode схемы;
- кэширование UI selections;
- схему preview-only working copy перед Apply.

Не переносить напрямую:

- GameInstance, GameModes, PlayerController, Pawn/Character и AnimBP;
- `Manager_Customization` рядом с `WorkerAppearanceComponent`;
- SaveGame вместо `AvariikaSaveGame`;
- DefaultEngine/DefaultInput renderer, map и input settings;
- Manny/MetaHuman meshes, skeleton assumptions и groom pipeline в Worker runtime;
- material parameter updates до явного mapping на реальные Quantum Worker materials;
- demo maps/UI целиком;
- проект 5.1/5.3 в UE 5.8 без изолированной migration validation.

## Итоговая таблица

| Ассет | Что полезно | Что не использовать | Риск UE5.8 | Решение |
|---|---|---|---|---|
| MCCUnleashed_5.7 | DataTable-driven modular parts, soft references, Leader/Copy Pose подход, реальная server-authoritative replication | Demo Character/GameMode/GameInstance/AnimBP; второй appearance component; готовые binaries/config redirects | Высокий: descriptor 5.6, runtime C++, Net/Core API и component lifecycle требуют build/PIE проверки | Не подключать сейчас. Перенести идеи и при необходимости отдельно адаптировать минимальные data/replication patterns внутрь `WorkerAppearanceComponent`. |
| MenuSystemPro 5.4 | Settings UX, data-driven styles/settings, input rebinding patterns, save-slot/menu widgets как референс | GameInstance/GameModes/PlayerController, полный SaveGame/input framework, example maps, массовая миграция Content | Средне-высокий: content-only, но 947 assets, hard references и Blueprint/settings API между 5.4 и 5.8 | Использовать как визуальный и архитектурный референс; переносить только изолированные UI patterns/assets после dependency audit. |
| Ultimate Character Customization 5.3 | Customization UI decomposition, DataTable schemas, color/scalar workflow, saved-character UX, camera UX | Собственные GameInstance/GameModes/Controller/Pawn/Character/AnimBP/SaveGame/Manager, Manny/MetaHuman assumptions | Высокий: `.uproject` фактически 5.1, большой Blueprint framework и нет подтверждённой replication | Не интегрировать framework. Использовать как референс для V2 UI/data model; runtime authority оставить у Avariika Character + `WorkerAppearanceComponent`. |

## Общее решение для Avariika

Ни один из трёх пакетов нельзя безопасно перенести целиком. Текущие `AvaryoCharacter`, `WorkerAppearanceComponent`, `ABP_Worker`, PlayerController, HUD, SaveGame и server-authoritative architecture должны оставаться владельцами runtime поведения.

Практичный путь:

1. Взять из MCC модель описания частей и сетевые идеи, но не plugin component.
2. Взять из UCC композицию customization UI и data contracts как референс, адаптировав их к Quantum Worker.
3. Взять из MenuSystemPro только patterns settings/rebinding и отдельные dependency-light UI элементы.
4. Любую будущую миграцию выполнять в изолированной тестовой папке с отдельной командой, dependency audit, UE 5.8 compile/validation и host/client проверкой.
