# AvCustomize audit

## Current state

- `B` is handled in `Source/Avaryo/Private/Game/AvaryoPlayerController.cpp`, not through an Enhanced Input action. The project is configured for Enhanced Input in `Config/DefaultInput.ini`, but customize currently uses `AAvaryoPlayerController::InputKey` plus legacy `InputComponent->BindKey`.
- Current log evidence shows the key path now reaches the controller and HUD: `[AvCustomizeInput] B pressed`, `ToggleCustomize -> open`, `HUD SetCustomize(open)`, `Root NativeConstruct`, and `Customize widget created and added to screen`.
- `Source/Avaryo/Private/UI/AvaryoHUD.cpp` owns opening and closing the customize root widget. It loads `/Game/Avariika/UI/Customize/WBP_CustomizeRoot.WBP_CustomizeRoot_C`, creates `UAvCustomizeRootWidget`, adds it to the screen, and calls `RefreshFromCharacter`.
- `Content/Avariika/UI/Customize/WBP_CustomizeRoot.uasset` and the smaller `WBP_Customize*` assets are only thin Blueprint shells. Their parent classes are the C++ widget classes; they do not contain a real designer layout. The UI layout is built in `Source/Avaryo/Private/UI/AvCustomizeWidgets.cpp`.
- `UAvCustomizeRootWidget` builds the screen in C++, handles categories, option cards, color swatches, Apply/Save, close, and preview setup.
- `Source/Avaryo/Private/Components/WorkerAppearanceComponent.cpp` contains the real mesh discovery and application logic. It scans curated folders under `/Game/Modular_Workers/Mesh/Male/...` and applies selected skeletal meshes to modular worker slots.
- `Source/Avaryo/Private/AvaryoCharacter.cpp` exposes the request path used by UI: `RequestWorkerAppearanceSlot`, `RequestWorkerAppearanceColor`, `RequestSaveWorkerAppearance`, plus console helpers `AvCustomize`, `AvWear`, and `AvColor`.
- Save/load for appearance exists in `CompanyLedgerSubsystem` and `AvariikaSaveGame`: `bHasSavedWorkerAppearance` plus `SavedWorkerAppearance`.
- A preview actor exists in `Source/Avaryo/Private/UI/AvCustomizePreviewActor.cpp`. It creates a render target, scene capture, lights, and a `WorkerAppearanceComponent`.

## Broken points

- If `B` still appears to do nothing visually in PIE, the first thing to verify is whether the current PIE run contains the latest compiled code. The current source/log path says `B` opens and creates the widget; the remaining failure is likely visibility/focus/viewport/runtime build staleness, not a missing key event.
- Input is mixed: Enhanced Input is configured globally, but customize uses legacy `InputKey` and `BindKey`. `B` is also handled twice, which can cause fragile behavior.
- The correct place for the `B` binding is `AAvaryoPlayerController`, because this is a menu/UI command. HUD should only own widget presentation. Character can keep a debug/console command, but should not be the primary menu binding.
- `SetMenuInputMode` currently creates `FInputModeGameAndUI` without assigning the customize widget as the focused widget. HUD sets focus after creating the widget, but the controller changes input mode after that, so focus can still be unstable.
- The UI has fake top tabs: `ГАРДЕРОБ`, `СНАРЯЖЕНИЕ`, `НАСТРОЙКИ`, `ПОЗЫ`, `ИНФОРМАЦИЯ`. Only wardrobe/customization is real. These should not ship in the first vertical slice.
- Apply and Save currently do the same effective thing: both call `RequestSaveWorkerAppearance()` and close the menu.
- Color swatches are hardcoded UI options. `AvColor` writes transient color state in `WorkerAppearanceComponent`; color is not part of `FWorkerAppearance`, not saved, and not loaded.
- `SetSlotByKey` supports clearing a slot with `none/off/empty`, but the UI does not expose a real remove/none option.
- The preview actor is present, but not proven visually working from the audit alone. It is a partial implementation until verified in PIE with the render target visible and changing.
- If no saved appearance exists, load code does not apply a full default modular preset automatically on spawn. Defaults exist as helper paths, but a saved or explicitly applied appearance is what drives the current persisted state.

## Confirmed assets

These categories can be populated from existing skeletal mesh assets without fake UI entries:

- `Body`: 2 assets.
- `Head`: 1 asset.
- `Hair`: 5 assets.
- `Beard`: 7 assets.
- `Torso`: 42 assets.
- `Legs`: 21 assets.
- `Headgear`: 30 assets.
- `FaceMask`: 2 assets.
- `Glasses`: 7 assets.
- `Gloves`: 6 assets.
- `Vest`: 4 assets.
- `Feet`: 1 asset.

Best first-slice categories: `Torso`, `Legs`, `Headgear`, `Hair`, `Beard`, `Glasses`, `Gloves`, `FaceMask`, `Vest`. `Body`, `Head`, and `Feet` are real but low-value because they currently have very few choices.

## Minimal vertical slice

- One local player, one customizable worker.
- `B` opens the customize screen reliably in PIE.
- `B`, `Esc`, and the close button close it reliably.
- UI shows only real mesh-backed categories.
- Option cards are built only from real `USkeletalMesh` assets discovered by `WorkerAppearanceComponent`.
- Clicking a card applies that mesh immediately to the live player.
- The preview actor either visibly updates correctly or is temporarily hidden from the slice until verified.
- Save writes `FWorkerAppearance` through `CompanyLedgerSubsystem`.
- Next PIE/spawn loads the saved `FWorkerAppearance`.
- Exclude for now: character slots, multiplayer replication changes, color persistence, fake equipment/settings/pose/info tabs, Dungeon Architect, levels, garage.

## Exact implementation order

1. Verify the running PIE log after pressing `B`: it must show `B pressed`, `ToggleCustomize -> open`, `HUD SetCustomize(open)`, `Root NativeConstruct`, and `Customize widget created`.
2. Clean up the menu input path in `Source/Avaryo/Private/Game/AvaryoPlayerController.cpp`: keep the menu toggle in PlayerController, remove duplicate fragile handling, and pass the opened widget into `FInputModeGameAndUI` focus.
3. Adjust `Source/Avaryo/Private/UI/AvaryoHUD.cpp` only as needed so `SetCustomize(true)` returns or exposes the created widget for focus and logs failures clearly.
4. Simplify `Source/Avaryo/Private/UI/AvCustomizeWidgets.cpp`: remove fake top tabs, show only real mesh categories, keep the first slice focused on wardrobe mesh selection.
5. Keep `Source/Avaryo/Private/Components/WorkerAppearanceComponent.cpp` as the asset authority. Only change it if option discovery or application fails for confirmed assets.
6. Keep save through `Source/Avaryo/Private/Game/CompanyLedgerSubsystem.cpp` and `Source/Avaryo/Public/Game/AvariikaSaveGame.h`. Do not add new save structures unless the mesh slice is already working.
7. Verify or disable the visible preview path in `Source/Avaryo/Private/UI/AvCustomizePreviewActor.cpp`. Do not depend on preview for the first working save/apply slice if it is the blocker.
8. Build, run PIE smoke test, then check logs for `Fatal`, `Ensure`, and `: Error:`. Do not commit.

## Risks / unknowns

- The editor may still be running stale binaries or Live Coding output. If the log does not show `Root NativeConstruct` and `Customize widget created`, the current source is not what PIE is executing.
- Widget visibility could still be blocked by focus/input mode ordering or viewport layering even when construction succeeds.
- Preview rendering has not been visually verified. It may construct correctly while still showing blank/incorrect output.
- Some modular meshes may need leader-pose/socket/material assumptions that only fail when applied live.
- Asset list includes at least one likely duplicate display name in headgear; option display labels may need de-duplication.
- Existing replication code is already present in `WorkerAppearanceComponent`, but the next slice should avoid expanding multiplayer behavior.

## What to check in code

- `Source/Avaryo/Private/Game/AvaryoPlayerController.cpp`: confirm `B` is not toggling twice through both `InputKey` and `BindKey`.
- `Source/Avaryo/Private/Game/AvaryoPlayerController.cpp`: confirm `FInputModeGameAndUI` focuses the actual customize widget after it is created.
- `Source/Avaryo/Private/UI/AvaryoHUD.cpp`: confirm `WBP_CustomizeRoot` load succeeds and `CreateWidget<UAvCustomizeRootWidget>` returns a valid widget.
- `Source/Avaryo/Private/UI/AvCustomizeWidgets.cpp`: confirm `NativeConstruct`, `BuildLayout`, `RebuildCategories`, and `RebuildOptions` all run in the current PIE log.
- `Source/Avaryo/Private/UI/AvCustomizeWidgets.cpp`: remove or hide top tabs that do not have real implemented systems.
- `Source/Avaryo/Private/Components/WorkerAppearanceComponent.cpp`: verify the selected category returns non-empty `GetAvailableOptions`.
- `Source/Avaryo/Private/AvaryoCharacter.cpp`: verify `RequestWorkerAppearanceSlot` reaches `AvWear` and `WorkerAppearance->SetSlotByKey`.
- `Source/Avaryo/Private/Game/CompanyLedgerSubsystem.cpp`: verify `SetSavedWorkerAppearance` is called only when Save/Apply should persist the current mesh selection.
