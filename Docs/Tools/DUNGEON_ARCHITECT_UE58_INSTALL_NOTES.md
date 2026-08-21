# Dungeon Architect UE 5.8 Install Notes

Дата: 2026-07-01  
Проект: `C:\unrealEngine\avariika_UE58_sandbox`  
Плагин: `Dungeon Architect 3.5.1`, установлен из `плагины\DungeonAcfe38245a8e7V18`

## Что установлено

- Плагин скопирован в `Plugins/DungeonArchitect`.
- При копировании исключены старые `Binaries`, `Intermediate`, `Saved`, `DerivedDataCache`.
- UE 5.8 пересобрал новые Win64 editor binaries в `Plugins/DungeonArchitect/Binaries/Win64`.
- `avariika.uproject` содержит включенный plugin entry:

```json
{
  "Name": "DungeonArchitect",
  "Enabled": true,
  "MarketplaceURL": "com.epicgames.launcher://ue/marketplace/content/1a4ad2a06f174a26af8a29728241ce42"
}
```

## Измененные plugin/config файлы

- `avariika.uproject`
  - `EngineAssociation` в sandbox уже переведен на `5.8`.
  - Добавлен `DungeonArchitect`.
- `Plugins/DungeonArchitect/DungeonArchitect.uplugin`
  - `EngineVersion` изменен с `5.7.0` на `5.8.0`.
- `Plugins/DungeonArchitect/Source/DungeonArchitectRuntime/DungeonArchitectRuntime.Build.cs`
  - `bWarningsAsErrors = false`.
- `Plugins/DungeonArchitect/Source/DungeonArchitectEditor/DungeonArchitectEditor.Build.cs`
  - `bWarningsAsErrors = false`.
  - Добавлена dependency `PCG` для линковки editor-модуля.
- `Plugins/DungeonArchitect/Source/DungeonArchitectGameplay/DungeonArchitectGameplay.Build.cs`
  - `bWarningsAsErrors = false`.
- `Plugins/DungeonArchitect/Source/DungeonArchitectGameplaySupport/DungeonArchitectGameplaySupport.Build.cs`
  - `bWarningsAsErrors = false`.

## UE 5.8 compatibility patches

### PCG texture data API

Файл: `Plugins/DungeonArchitect/Source/DungeonArchitectRuntime/Public/Frameworks/PCG/Data/PCGSGFTextureData.h`

- `UPCGDungeonSGFTextureData` переведен с `UPCGBaseTextureData` на `UPCGTexture2DSingleBaseData`.
- `IsValid()` больше не помечен как `override`, потому что в UE 5.8 прежняя virtual-сигнатура в старом base больше не совпадала.
- Добавлены минимальные overrides нового texture base:
  - `GetTexture()`
  - `GetTextureRHI()`
  - `GetTextureResourceType()`
  - `RequestCPUReadback()`

Файл: `Plugins/DungeonArchitect/Source/DungeonArchitectRuntime/Private/Frameworks/PCG/Data/PCGSGFTextureData.cpp`

- PCG sampler helper принимает `UPCGTexture2DSingleBaseData*`, чтобы поля `ColorChannel`, `TexelSize`, `bUseAdvancedTiling`, `Tiling`, `CenterOffset`, `Rotation`, `bUseTileBounds`, `TileBounds`, `Width`, `Height` были доступны через UE 5.8 base.

### Viewport input gesture API

Файлы:

- `Plugins/DungeonArchitect/Source/DungeonArchitectRuntime/Public/Frameworks/Canvas/UI/Viewport/DungeonCanvasViewportClient.h`
- `Plugins/DungeonArchitect/Source/DungeonArchitectRuntime/Private/Frameworks/Canvas/UI/Viewport/DungeonCanvasViewportClient.cpp`

Изменение:

- `InputGesture` обновлен под UE 5.8: добавлен параметр `EGesturePhase GesturePhase`.

### Gameplay float warning

Файл: `Plugins/DungeonArchitect/Source/DungeonArchitectGameplay/Public/Interaction/DAGPlayerInteractionScannerComponent.h`

Изменение:

- `InteractionScanRate = 0.100` заменен на `0.100f`, чтобы убрать float truncation warning.

### Workflow app mode API

Файлы:

- `Plugins/DungeonArchitect/Source/DungeonArchitectEditor/Private/Core/Editors/SnapMapEditor/AppModes/DebugAppMode.cpp`
- `Plugins/DungeonArchitect/Source/DungeonArchitectEditor/Private/Core/Editors/SnapMapEditor/AppModes/GraphDesignAppMode.cpp`
- `Plugins/DungeonArchitect/Source/DungeonArchitectEditor/Private/Core/Editors/SnapMapEditor/AppModes/VisualizeAppMode.cpp`
- `Plugins/DungeonArchitect/Source/DungeonArchitectEditor/Private/Core/Editors/ThemeEditor/AppModes/MarkerGenerator/MarkerGeneratorAppMode.cpp`
- `Plugins/DungeonArchitect/Source/DungeonArchitectEditor/Private/Core/Editors/ThemeEditor/AppModes/ThemeGraph/ThemeGraphAppMode.cpp`

Изменение:

- Вызовы приватного в UE 5.8 `FApplicationMode::RegisterTabFactories(InTabManager)` заменены на protected API `RegisterTabFactoriesWithAppAndManager(...)`.

## Проверка

Команда:

```powershell
& 'C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat' avariikaEditor Win64 Development -project='C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject' -WaitMutex
```

Результат: `Succeeded`.

Smoke test:

```powershell
& 'C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe' 'C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject' -NoSplash -Unattended -NullRHI -ExecCmds='Quit' -log
```

Результат: editor стартовал, загрузил плагины и штатно завершил shutdown sequence.

## Оставшиеся риски

- Патчи минимальные и проверены сборкой, но не являются полноценной миграцией всех deprecated UE 5.8 API.
- `bWarningsAsErrors` отключен только для модулей Dungeon Architect, потому что UE 5.8 поднимает много deprecated warnings из PCG/Slate/Editor API.
- PCG texture path адаптирован компиляционно; нужно отдельно проверять Dungeon Architect PCG/Snap Grid Flow editor workflow на реальных тестовых ассетах.
- В smoke log остаются не связанные с Dungeon Architect ошибки проекта:
  - `Content/AnimX/Dogs/AnimBP_Dog_GH.uasset`: invalid `Get Relevant Anim Time` node.
  - `LogAutomationTest: Error: Condition failed`.
- `avariika.uproject` уже содержит другие plugin changes sandbox-миграции UE 5.8; при коммите нужно отделять DungeonArchitect-часть от чужих/ранних изменений.
