# Codex Handoff Current

## Menu System Pro production closure - 2026-07-27

### Working runtime flow

- The production main-menu map is `/Game/Avariika/Maps/MainMenu/L_MainMenu`; it is now also the packaged `GameDefaultMap`, and the gameplay pause-menu return route targets the same map. `MenuHUD` mounts the project-owned Menu System Pro root only on that map, and the Canvas fallback remains intact.
- `CONTINUE` now validates the real `UAvMainMenuSavePresenceAdapter` and travels through the existing gameplay map `/Game/Avariika/Maps/Lvl_FirstPerson`. PIE reached that map with no customization widget left open. A first-run company state is prepared in memory without creating a disk slot, so Continue remains disabled until a real save exists; an unreadable existing slot is preserved instead of being overwritten by defaults.
- `NEW GAME` travels to the existing gameplay map with the `AvStartCustomize` option. `AAvaryoPlayerController` consumes that option after the gameplay HUD is ready and opens the existing `WBP_CharacterCustomizationRoot_V2` flow. It does not delete or reset the company save.
- `MULTIPLAYER` opens the authored `WBP_AvMSP_ComingSoon` dialog because no complete production server-browser flow is ready. It returns to Home with focus restored instead of leading to a dead action.
- `CREDITS` opens the authored `WBP_AvMSP_Credits` screen and returns to the originating Home button.
- `EXIT` opens the authored `WBP_AvMSP_ConfirmDialog`. `No` restores focus to `QuitTitleScreenButton`; `Yes` uses the real quit path outside PIE and deliberately leaves the Editor alive in PIE.
- `SETTINGS` keeps the existing vendor transition and the project-owned `AvariikaUserSettings` adapter. Video, Graphics and Audio tabs were reopened in PIE; no MSP JSON settings source was introduced.
- Project routing exclusively owns New Game, Continue, Multiplayer, Credits and Exit button delegates. The vendor Settings transition is retained and project code repairs focus after the transition.
- The packaged New Game route keeps a soft class reference to the production `WBP_CharacterCustomizationRoot_V2`; the game module's cook delegate includes that widget and its runtime-loaded `M_PreviewWorker_UI` material explicitly. The broken legacy `WBP_CharacterCustomizationRoot` is not pulled into the cook.

### Focus, input and persistence

- Initial focus is deterministic: `ContinueTitleScreenButton` received focus with the tested valid save, while an isolated no-save staged run disabled Continue and focused `PlayTitleScreenButton`. Disabled Continue entries are excluded from the navigation order.
- Home navigation supports keyboard arrows/W/S, D-pad, left-stick throttling, Confirm and Back. Settings, Credits and both confirmation dialogs restore focus to their originating Home buttons.
- Runtime probes reported one `WBP_AvMSP_MainMenuRoot`, zero provisional roots, six visible/enabled Home buttons and exactly one focused button in the tested valid-save state.
- Mouse and keyboard are the primary target input for AVARIIKA and their acceptance is a required release criterion. Keyboard/focus routes were exercised in PIE; a foreground-PID-verified staged Win64 run also used real `Down` then `Enter` from Continue and reached New Game/customization. Existing controller paths for D-pad/left stick/Confirm/Back were checked programmatically and must remain supported, but a separate physical-controller check is optional and does not block Menu System Pro completion.
- `Saved/SaveGames/AvariikaCompany.sav` remained SHA-256 `C4CAAD9448B01B38935A76791DA73632612870D2430D16D6925EDC8F1D77A83E` before and after New Game/Continue tests. Backup: `Saved/CodexBackups/MenuSystemProProductionClosure_20260727/SaveGames/AvariikaCompany.sav`.

### Runtime evidence, build and cook

- Fresh PIE captures:
  - `RU_MainMenu_1920x1080.png`
  - `RU_MainMenu_1280x720.png`
  - `EN_MainMenu_1920x1080.png`
  - `Continue_ValidSave_RU_1920x1080.png`
  - `Settings_RU_1920x1080.png`
  - `Credits_RU_1920x1080.png`
  - `Exit_ConfirmDialog_RU_1920x1080.png`
- EN runtime probe contained `CONTINUE`, `NEW GAME`, `MULTIPLAYER`, `SETTINGS`, `CREDITS` and `EXIT`; it retained one project root and one focused Home button.
- Final UE 5.8 UBT command completed with exit code 0 and `Result: Succeeded`. Log: `Saved/CodexDiagnostics/MenuSystemProProductionClosure_20260727/UBT_UE58_final.log`.
- Requested Win64 Development `BuildCookRun -cook -build -stage -pak` completed with `BUILD SUCCESSFUL` and `AutomationTool exiting with ExitCode=0`. Full log: `Saved/CodexDiagnostics/MenuSystemProProductionClosure_20260727/Cook_UE58_full.log`; staged build: `Saved/StagedBuilds/Windows`.
- Packaged smoke started directly on `/Game/Avariika/Maps/MainMenu/L_MainMenu`, mounted the authentic MSP actor, and accepted foreground-PID-verified real keyboard input. It then loaded `/Game/Avariika/Maps/Lvl_FirstPerson?AvStartCustomize` and created `WBP_CharacterCustomizationRoot_V2_C`. Final-package evidence: `Staged_NewGame_FinalPackage.log`.
- The first packaged smoke exposed that `AvariikaUI.csv` and code-addressed customization assets were absent from the package. Packaging now stages `Avariika/Localization` through UFS and cooks only the required thumbnail/profile directories. The final staged smoke reported zero failed String Table imports, zero `<MISSING STRING TABLE ENTRY>`, zero missing exact thumbnails/profile assets, and zero missing packages.
- Final isolated SaveGame acceptance passed in staged Win64: `Staged_NoSave_ContinueDisabled_Final.log` reported `Continue=false`, New Game initial focus and no company slot creation; `Staged_ValidSave_ContinueEnabled_Final.log` used a protected copy of the real save and reported `Continue=true` with Continue initial focus. The source and isolated copy remained SHA-256 `C4CAAD9448B01B38935A76791DA73632612870D2430D16D6925EDC8F1D77A83E`.
- Startup localization cleanup replaced the HUD constructor's eager customization Blueprint load with a soft class reference, so `AvariikaUI` is registered before the customization class CDO is loaded. A fresh UE 5.8 runtime build and full Win64 Development build/cook/stage/pak both succeeded. IoStore contains exactly the production customization root and its preview material; the legacy root remains absent.
- Final isolated staged checks are under `Saved/CodexDiagnostics/MenuSystemProStartupCleanup_20260727`: `Staged_Startup_NoSave.log` reported zero failed/missing String Table entries and `Continue=false`; `Staged_NewGame_Material.log` created `WBP_CharacterCustomizationRoot_V2_C` and reported zero missing preview material, String Table, Blueprint runtime, `Accessed None`, `SourceMenu`, fatal or assertion diagnostics.
- Diagnostics root: `Saved/CodexDiagnostics/MenuSystemProProductionClosure_20260727`.

### Files changed in this closure

- `Source/Avaryo/Private/UI/MenuHUD.cpp`
- `Source/Avaryo/Private/Avaryo.cpp`
- `Source/Avaryo/Public/UI/MenuHUD.h`
- `Source/Avaryo/Private/Game/MenuGameMode.cpp`
- `Source/Avaryo/Public/Game/MenuGameMode.h`
- `Source/Avaryo/Private/Game/AvaryoPlayerController.cpp`
- `Source/Avaryo/Public/Game/AvaryoPlayerController.h`
- `Source/Avaryo/Private/Game/CompanyLedgerSubsystem.cpp`
- `Source/Avaryo/Private/UI/AvaryoHUD.cpp`
- `Source/Avaryo/Public/UI/AvaryoHUD.h`
- `Config/DefaultEngine.ini`
- `Config/DefaultGame.ini`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`
- `PROJECT_STATE.md`

No `.uasset`, `.umap`, external Menu System Pro source package, localization CSV or SaveGame was modified in this closure. The two project Config files listed above were changed intentionally for the production startup/cook routes.

### Release acceptance status

- Required mouse/keyboard acceptance has passed for the production Menu System Pro flow, including navigation, activation, Back and focus restoration.
- Existing gamepad/focus support must not be removed or broken. A physical-controller check may be performed later as an additional compatibility test, but it is not a release requirement or completion blocker.
- The earlier startup-only 53 String Table warnings are resolved. The final isolated startup and New Game packaged logs contain no failed/missing String Table entries.
- The earlier vendor `SourceMenu` diagnostics did not reproduce in the final isolated packaged startup or New Game checks. Do not schedule an asset rewrite unless they reproduce in a current flow.
- Menu System Pro production closure is not waiting on physical-controller acceptance.

## Authored Main Menu runtime stabilization - 2026-07-20

- Fixed standalone localization registration. `AvariikaUI` is now registered explicitly from `FAvaryoGameModule::StartupModule()` and unregistered from `ShutdownModule()`; the former `EndOfEngineInit` delayed callback could be queued after that one-shot phase had already run.
- Fixed the authored shell layout by moving the background, accent, left panel, future-scene marker and modal onto explicit fill-aligned Overlay slots. Home footer no longer overlaps buttons, Settings receives usable ScrollBox height, and confirmation dialogs dim the full viewport and center correctly.
- Runtime captures passed at `1920x1080` for RU Home/Settings/Exit modal and EN Home, plus RU Home/Settings at the shipping minimum `1280x720`. Settings intentionally scrolls at 720p. Visual acceptance remains with the user.
- Fresh RU/EN standalone logs contain 0 missing String Table warnings, 0 fatal/ensure/assertion lines and 0 menu Blueprint/linker errors. Real keyboard navigation opened Settings and Exit confirmation. A physical gamepad was not tested; under the current input policy this is optional and does not block release acceptance.
- Both company/settings SaveGame hashes remained unchanged. UE 5.8 UBT result: `Succeeded` after the final layout change.
- Evidence: `Saved/CodexDiagnostics/MainMenuContinue_20260720/`.

## Menu System Pro discovery and UE 5.8 integration audit - 2026-07-19

### Decision

- Supplied package: `C:\unrealEngine\avariika_UE58_sandbox\плагины\MenuSystemPro 5.4`.
- One external supplied package was found; installed project plugins, installed UE 5.8 Engine/Marketplace plugins and migrated AVARIIKA content copies found: **0**. Therefore there is no canonical active copy to replace or duplicate.
- Classification is scenario F: the supply is a **content/localization pack**, not an Unreal plugin or standalone demo project. It contains no `.uplugin`, `.uproject`, `.Build.cs`, Source, Binaries, Intermediate or package Config. The `5.4` version is a folder/package label, not descriptor metadata; descriptor `EngineVersion` is absent.
- No installation was performed and no repair was performed. The package cannot correctly be “installed as a plugin” because it has no plugin descriptor. Production migration is deferred until a project-owned, dependency-closed migration manifest and UE 5.8 input repair are authored.
- Original supply remained read-only. The pre-change per-file package manifest contains 979 files and has SHA-256 `A6C9F614D878EA07ECC617A496D2346658E7D86C285C3A907E2746375E5B6582`; final verification was 979 present, 0 missing and 0 SHA mismatch.

### Discovery and active-copy proof

- Read-only discovery covered project `Plugins`, all `C:\Program Files\Epic Games\UE_5.8\Engine\Plugins`, the absent dedicated `Engine\Plugins\Marketplace` directory, production `Content`, the supplied folder, project descriptor, all Config, `.uplugin` dependencies, Source/binary asset strings, Asset Registry, live mounted plugins/modules and current/historical editor logs.
- No matching `.uplugin`, project plugin entry, additional plugin directory, loaded module, mount log or project asset reference was found for MenuSystemPro, Menu System Pro, Moonville, MSP or MVMenu.
- Production Asset Registry counts were 0 for `/Game/MenuSystemPro`, `/MenuSystemPro`, `/Game/MSP` and `/MSP`. Live Editor mounted EnhancedInput but not Menu System Pro, CommonUI or CommonInput.
- Duplicate plugin Name, Project/Engine shadowing and duplicate mount point are absent. Mentions of the isolated audit project in production Recent Projects are not mounts.
- Package inventory: 979 files / 792,582,894 bytes, 947 `.uasset`, 4 `.umap`, 953 staging Asset Registry entries, 217 Blueprints and 9 localization files. Demo maps: `SilenceMenuLevel`, `SilencePlayLevel`, `BasicLevel`, `MoonTown`.

### Actual content/code and dependencies

- Actual status: **Blueprint-heavy content-only pack with native engine/plugin dependencies**, not an independent code plugin and not dependency-free Blueprint-only content. No own native module or binary exists.
- Core dependencies evidenced by asset references/runtime: UMG/Slate, EnhancedInput, JsonBlueprintUtilities, AudioMixer/GameUserSettings. AVARIIKA already enables EnhancedInput and JsonBlueprintUtilities.
- Feature/demo dependencies: OnlineSubsystem/Null/Utils for host/join/server browser; AudioCapture for sample voice paths; CableComponent, ControlRig and IKRig for examples; InventorySystemPro content for example rigs; `DLSSBlueprint` only for `BP_DLSSApply`.
- The local DLSS plugin is disabled and has a UE 5.7 descriptor/stale BuildId relative to UE 5.8. Do not enable it for this integration; exclude/defer `BP_DLSSApply` or provide a separately verified adapter later.
- Common UI decision: supplied assets reference neither CommonUI nor CommonInput. They implement legacy UMG, manual widget layers/focus, input-device/glyph state and UI-only input mode. Use selected legacy framework pieces temporarily behind AVARIIKA adapters; do not enable CommonUI blindly. A future authored shell may choose CommonUI independently after a viewport/input decision.

### Isolated UE 5.8 audit and demo smoke

- Exact package files were copied to task-owned `Saved/CodexStaging/MenuSystemProUE58_20260719`; copy verification was 979/979, equal byte count and 0 hash mismatch. The supplied source folder was never opened for save.
- UE `5.8.0-55116800+++UE5+Release-5.8` opened `/Game/MenuSystemPro/ExampleContent/Maps/SilenceMenuLevel` (122 actors) and PIE. Runtime used `BP_MenuSystemGameInstance_C`, `BP_MenuLevelGameMode_C`, `BP_SamplePlayerController_C` and one `BP_MenuSystemActor`.
- Representative compile: 27/28 Success. `BP_MenuControllerComponent` fails twice because `Profile Id` no longer exists on `Make Map Player Key Args`; affected node GUIDs are `22CE142D4CA822E1AABD508DCD006BF6` and `F3B6B5EA4560B88E6497FAB4831A24B7`.
- Validation scanned 953 assets; 20 example-only assets have missing Manny/Quinn/InventorySystemPro dependencies. These sample character/pose/Control Rig assets are not selected for AVARIIKA.
- Mouse passed title/options/language/audio interaction. Enter skipped the intro. Directional keyboard navigation and Escape/Back did not pass while the controller Blueprint had compile errors. `IMC_Menu` and `IMC_AlwaysAllowed` include keyboard/mouse/gamepad mappings, but physical gamepad acceptance was not performed and gamepad navigation is not accepted.
- Demo language `en → de` applied and was restored to `en`. Output device enumeration and Master/SFX/Music/Voice controls were visible. The demo writes `ProjectSavedDir/Config/MenuSystemConfig.json` and also calls `UGameUserSettings`; staging data never entered production.
- A temporary target build succeeded, then full staging cook processed 1277 packages and failed with UAT exit 25 on the same two `BP_MenuControllerComponent` errors; `BP_DLSSApply` also warned about missing `/Script/DLSSBlueprint`. Therefore the unchanged pack is **not production/cook compatible with UE 5.8**.
- Staging cleanup: PIE=false, culture restored to en, dirty Content/Maps=0/0, staging Editor closed normally. No production multiplayer, Save/Load or packaging claim was accepted.

### Required UE 5.8 adaptation

- On a future project-owned migrated copy only, reconstruct the two `Make Map Player Key Args` nodes/remove obsolete `Profile Id` links or replace `BP_MenuControllerComponent` with the AVARIIKA input router. Require clean Blueprint compile, mouse/keyboard focus/Back regression and successful cook; preserve existing gamepad/focus support and keep physical-controller testing optional.
- Exclude/defer DLSS, online, gameplay Save/Load and sample-character dependencies from the first migration manifest. No project-specific MSP patch currently exists in production; no upgraded asset was saved into the supplied folder.

### AVARIIKA ownership and integration architecture

- GameInstance remains `/Script/Engine.GameInstance`; do not assign `BP_MenuSystemGameInstance`. A project-owned subsystem/adapter may implement only necessary lifecycle/settings hooks exposed conceptually by `BPI_GameInstanceExtension`.
- Global GameMode remains `/Game/Avariika/blueprinsTest/BP_Gamemode`; current map override remains `/Game/Avariika/Blueprints/BP_AvaryoGameMode`. Demo GameModes are reference only.
- PlayerController remains `/Game/FirstPerson/Blueprints/BP_FirstPersonPlayerController` (parent `AAvaryoPlayerController`). Do not assign `BP_SamplePlayerController`; one project input router owns cursor, input mode, Back priority, focus and temporary menu mapping contexts.
- GameViewportClient remains `/Script/Engine.GameViewportClient`; CommonGameViewportClient was not introduced.
- Settings ownership: existing AVARIIKA settings coordinator/`AvariikaUserSettings.sav` is canonical persistence; `UGameUserSettings` remains the native display/scalability apply layer. MSP settings widgets/apply helpers emit adapter requests; `MenuSystemConfig.json` is not a production source of truth.
- SaveGame ownership: CharacterRecords, ActiveCharacterId, Appearance and Company remain exclusively AVARIIKA. `BP_SaveGameManager` and sample Save/Load are deferred.
- Localization ownership: target Game, native `ru`, cultures `ru/en` and String Table `AvariikaUI` remain canonical. Selected plugin labels must map to reviewed project keys. German and plugin localization remain isolated; literal character names are never rewritten.
- Input ownership: existing project IA/IMC and `DefaultInput.ini` remain intact. Selected menu IMC is active only during menu state; plugin cursor/input mode/rebinding calls are routed through the project coordinator. Character Customization modal/camera wheel/drag rules remain authoritative.

### Selected, deferred, and authored-shell plan

- Selected candidates: menu navigation/container concepts, repaired legacy focus/navigation, input-device/glyph data, keyboard/mouse/gamepad menu actions, modal/focus restoration concepts, settings UI helpers through adapter, Pause framework concepts, loading-screen and localization integration points.
- Deferred: server browser/Host/Join/network/Steam, local multiplayer/split focus, sample character/level/GameMode/Controller/environment, plugin gameplay Save/Load, DLSS, voice chat, HDR acceptance, final intro/credits/music/SFX/loading art and all demo visual design.
- Authored Main Menu contract: CONTINUE (enabled only for valid company/gameplay save; travel later), NEW GAME (future company/character flow; no automatic reset), MULTIPLAYER (disabled/placeholder), SETTINGS (single AVARIIKA settings owner), CREDITS (future authored content), EXIT (confirmation and packaged-safe quit).
- Authored Pause contract: RESUME, SETTINGS via the same adapter, RETURN TO MAIN MENU with confirmation/future travel coordinator, EXIT TO DESKTOP with confirmation.
- Next exact task: create the project-owned authored AVARIIKA shell and minimal migration manifest, repair/replace the UE 5.8 controller component in the copied namespace, exclude deferred dependency branches, then pass clean compile, required mouse/keyboard focus tests and cook while preserving existing gamepad/focus support. A physical-controller check is optional. The final visual screen was not created and visual acceptance was not performed.

### Protected state, build and final state

- Backup: `Saved/CodexBackups/MenuSystemProIntegration_20260719/`; backup manifest SHA-256 `858CFCC981CC702B2238E2E47E6307DCEF688B9363A380FA595F74E95B667E34`.
- Production `.uproject`, DefaultEngine/Game/Input/Editor Config and both SaveGames matched preflight SHA after all tests. Company/Appearance SaveGame: `C4CAAD9448B01B38935A76791DA73632612870D2430D16D6925EDC8F1D77A83E`; Settings SaveGame: `619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7`.
- No descriptor, enabled plugin, plugin Source, project C++ or production Config changed, so a production UBT was intentionally not run. Existing `UnrealEditor-Avaryo.dll` SHA-256 is `6F77942891F445830AB8EB6CA87478162D677E266FCC07F1B8A3C932AC08D54C` and is newer than the newest project C++ source.
- Production regression smoke: UE 5.8 Editor responsive on `Lvl_FirstPerson`; PIE started; `AvCustomize` created `WBP_CharacterCustomizationRoot_V2_C`; PIE ended. Startup maps/classes were unchanged. Add/Switch/Rename/Random/RU-EN/responsive destructive/full matrix was not rerun because no production gameplay/UI asset changed; the existing accepted baseline remains the referenced truth.
- Final production state: culture/language `ru/ru`, PIE=false, dirty Content=0, dirty Maps=0, Editor responsive, Fatal=0, Ensure=0, task-owned Error=0. Startup flow is unchanged. Visual acceptance was not performed.
- Diagnostics: `Saved/CodexDiagnostics/MenuSystemProIntegration_20260719/` (19 required discovery, inventory, compatibility, ownership, conflict, smoke and architecture reports). Install/repair manifests are absent by design because neither action occurred.

## Character Customization high-resolution and ultrawide adaptation - 2026-07-18

### Continuous viewport follow-up - 2026-07-19

- This follow-up supersedes the earlier six-checkpoint-only acceptance. Correct scope statement: **Интерфейс использует непрерывную responsive policy и протестирован на репрезентативной матрице, случайных viewport-размерах и последовательном runtime resize в заявленном диапазоне.** It does not claim that every existing resolution or every device was tested, and it does not claim visual PASS.
- Production code contains no named-resolution or named-aspect allowlist. The only explicit pixel boundary is the requested shipping minimum `1280x720`; every supported layout value is derived from the live viewport, Unreal DPI, logical constraints, and available working width. The named resolution/aspect matrix exists only in `Saved/CodexDiagnostics/CustomizationResolutionAdaptation_20260718/run_continuous_viewport_fuzz.ps1`.
- The continuous tested PC landscape range is `1280x720` through `7680x2160`, including 5:4, 4:3, 16:10, 16:9, 21:9, 32:9 and arbitrary intermediate window sizes. Portrait remains outside shipping acceptance. Below-minimum or out-of-scope landscape shapes use `GracefulDegradation`; Root/RT remain valid and modal/buttons remain reachable, but full layout quality is not claimed.

#### DPI and panel policy

- `Config/DefaultEngine.ini` remains the single scale path: `ApplicationScale=1`, `UIScaleRule=ShortestSide`, no root/manual RenderScale. In the scoped landscape range, shortest side is mathematically the height, including extreme ultrawide, so ShortestSide is the desired height-oriented rule without a special ultrawide branch. In portrait it would use width, which also avoids height-driven runaway scale, but portrait acceptance is not claimed.
- Unreal's linear curve retains `720=.8`, `768=.85`, `900=.93`, `1080=1`, `1440=1.333333`, `2160=2`; values between adjacent keys interpolate continuously and endpoints clamp. Runtime examples: `1280x1024=.978222`, `1600x1200=1.111111`, `1737x941=.945944`, `5120x1440=1.333333`; 32:9 at height 1080 uses `1.0`.
- Logical panel constraints are shared across every viewport: left `Min/Preferred/Max=240/300/320`, right `384/480/520`. Supported viewports retain preferred width; when width is scarce, edge gaps interpolate `8..24`, inter-panel gaps `12..40`, then panels interpolate toward Min while preserving a `400` logical-unit preview safe minimum. Extra normal width goes to preview; cards are not horizontally stretched. Existing ellipsis/tooltips and ScrollBoxes are retained.
- Below the shipping minimum, a final positive-geometry fallback can proportionally reduce panels while preserving at least `96` logical preview units. The fuzz minimum `1000x650` produced positive preview `289.42x521.40` physical pixels, accessible modal, exact RT and no invalid geometry; status is `GracefulDegradation: BelowShippingMinimum`.

#### Visual surface, usable preview, camera and 32:9 UX

- Visual Surface, Root, background and RenderTarget always fill the physical viewport. Input acceptance remains `SB_PreviewArea`; cursor projection remains full Visual Surface. Base camera composition uses the live center of `SB_PreviewArea`, never a design-resolution coordinate or full-viewport center that ignores panels.
- The selected extreme-ultrawide policy is mode **B**. Main interactive content width is `min(logical viewport width, 2.4 * logical viewport height)` and is centered; top/footer and the visual surface remain full width. Ordinary 16:9/21:9 receive zero inset. At `5120x1440`, mode A (physical-edge panels) measured character-to-catalog distance `2008 px`; mode B measured `1176 px`, a `41.4%` reduction, while the working region still spans `3456/5120 px` and Root/background/RT remain `5120x1440`. Evidence: `extreme_ultrawide_ab_result.json`.
- Resize is driven by `FViewport::ViewportResizedEvent`, debounced to the next tick. Slate can retain the previous child arrangement for one frame, so camera composition is resampled for exactly three bounded layout frames after an event; there is no permanent viewport polling, AutoFit or screen-space character clamp. Every fuzz case checks composition against the actual live Interaction Area center.

#### Reproducible fuzz and continuous resize evidence

- Runner: `Saved/CodexDiagnostics/CustomizationResolutionAdaptation_20260718/run_continuous_viewport_fuzz.ps1`.
- Seed `2026071901`; case count `120`; exact minimum `1000x650`; exact maximum `7680x2160`; raw generated aspect range `.513274..10.116248`. The raw range intentionally includes portrait and shapes beyond shipping scope so they cannot be mistaken for supported PASS.
- Results: supported `75`, graceful degradation `36`, excluded portrait `9`, failed `0`. Degradation reasons: `BelowShippingMinimum=4`, `AspectAboveSupportedRange=26`, `AspectBelowSupportedRange=5`, combined below-minimum/below-aspect=`1`. Consolidated evidence: `viewport_fuzz_results.json` and `final_continuous_resolution_summary.json`.
- Across all 120 cases: crashes `0`, invalid/non-finite geometry `0`, outside viewport `0`, panel/preview overlap `0`, modal inaccessible `0` for all non-portrait cases, RT mismatch `0`, stale geometry `0`, SaveGame mutations `0`, duplicate roots `0`, preview actor leaks `0`. Each iteration used a real PIE viewport and live cached geometry; no per-case screenshot was created.
- Continuous resize used `33` sequential sizes in one PIE session without reopening customization. Result PASS: event-driven layout, exact RT resize, camera base/focus/composition state, stale cursor reset (`AnchorSource=None`), rename geometry, modal recentering, root/preview actor counts and SaveGame hashes all passed. A real post-route FullOutfit ScrollBox wheel changed offset `0 -> 384`. Evidence: `continuous_resize_results.json` and `continuous_resize_scrollbox_result.json`.

#### Runtime screenshots, build and cleanup

- Eight real runtime captures have exact image dimensions and SHA verification: `responsive_1280x1024_ru.png`, `responsive_1920x1200_ru.png`, `responsive_1920x1080_ru.png`, `responsive_2560x1440_ru.png`, `responsive_3440x1440_ru.png`, `responsive_5120x1440_ru.png`, `responsive_3840x2160_ru.png`, `responsive_1737x941_ru.png`. Manifest: `responsive_screenshot_manifest.json`; pixel/hash recheck: `responsive_screenshot_verification.json`.
- Final UE 5.8 UBT after production changes: `Result: Succeeded`; `UnrealEditor-Avaryo.dll` SHA-256 `6F77942891F445830AB8EB6CA87478162D677E266FCC07F1B8A3C932AC08D54C`.
- Unreal MCP: `WBP_CharacterCustomizationRoot_V2` compile Success, validation errors `0` with the same `17` existing warnings; `WBP_HeadCatalogItemCard` compile Success, errors `0` with `3` existing warnings. Evidence: `final_continuous_blueprint_validation.json`.
- Current-launch log: Fatal `0`, Ensure `0`, assertion `0`, Python errors `0`, MCP errors `0`, Blueprint compile errors `0`. The unrelated UE startup UnifiedError self-test still emits its known `17` `LogAutomationTest: Error: Condition failed` lines; six `LogUtils: Error: The Editor is currently in a play mode` lines came from editor-only PIE introspection and did not correspond to a runtime failure. Evidence: `final_continuous_log_audit.json`.
- Final state: Culture=`ru`, LEET=false, PIE=false, original maximized window placement restored, SaveGames restored to continuation-start SHA (`AvariikaCompany=8982B4AB...055`, `AvariikaUserSettings=619B5A0F...C7`), dirty Content `0`, dirty Maps `0`, Editor responsive. No `.uasset` or `.umap` was modified or saved by this follow-up.
- Remaining manual work: user visual acceptance on real 5:4/4:3/16:10/16:9/21:9/32:9 displays, especially preferred character scale/composition at 4K and the `5120x1440` working-region inset, small-text comfort, tooltip/ellipsis feel, modal focus/gamepad navigation, and OS-level window decorations/multi-monitor placement. Automated geometry and screenshots are evidence, not subjective visual PASS.

Final formulation: **Кастомизация больше не зависит от перечня конкретных разрешений: DPI плавно рассчитывается из фактической geometry viewport, панели используют единые Min/Preferred/Max constraints, а свободная ширина передаётся preview-области. Архитектура проверена на основных aspect-классах, нестандартных оконных размерах, extreme ultrawide и воспроизводимом viewport fuzz-тесте; пользовательское визуальное принятие остаётся обязательным.**

### Rejection, recovery and proven cause

- The user rejected the preceding responsive pass: `1280x720` and `1920x1080` were acceptable, while `2560x1440`, `3840x2160`, `2560x1080` and `3440x1440` looked like a tiny fixed island. This pass does **not** claim visual PASS or feature freeze; final visual acceptance remains with the user.
- The unacceptable user state was environmental, not production-UI corruption: UE `LEET` pseudo culture and a test-sized Editor window had been left active. It was recovered before production work: PIE stopped normally, Culture/Language returned to `ru`, pseudo disabled, Editor maximized because no separate original placement record existed, dirty Content/Maps `0/0`, and both SaveGames restored byte-for-byte. The abandoned working saves are retained at `Saved/CodexBackups/CustomizationResolutionAdaptation_20260718/AbandonedTestState_20260718_1737/`.
- The exact scale cause was `Config/DefaultEngine.ini`: `ApplicationScale=1`, `UIScaleRule=ShortestSide`, but the curve froze `1440` at `1.0` and raised `2160` only to `1.1`. Thus 2K stayed at 1080p physical UI dimensions and 4K gained only 10% rather than 100%. There was no active root `RenderScale`, no active manual clamp, and no double DPI application.
- `ComputeUIScale()` duplicated the stale curve but had zero call sites. It was removed rather than wired into production. Runtime diagnostics now explicitly report `ManualUIScale=1` and `ManualUIScaleApplied=false`; all scaling comes from the single Unreal DPI path.
- The rejected runtime was **not** held by a root `1280x720`/`1920x1080` SizeBox, `MaxDesiredWidth`, DesiredSize island or root ScaleBox. Its existing absolute Root geometry already matched the viewport in the prior dumps. The island impression came primarily from the frozen DPI policy and full-viewport camera composition while the useful center region was asymmetric. The pass nevertheless hardens the direct root Canvas children (`Img_CustomizeBackground`, `LayoutRoot`, `OverlayRandom`) to Fill anchors, zero offsets and no autosize so background, layout and modal cannot regress to authored design-size slots.
- The effective chain is now: actual `APlayerController::GetViewportSize` -> Unreal ShortestSide DPI (equivalent to height on the supported landscape/ultrawide set) -> no manual scale -> root absolute geometry -> panel/preview cached geometry. The render target continues to use actual viewport pixels, never DPI-scaled logical units.

### DPI policy and measured runtime geometry

- Before: `720=.8`, `768=.85`, `900=.93`, `1080=1.0`, `1440=1.0`, `2160=1.1`. After: `720=.8`, `768=.85`, `900=.93`, `1080=1.0`, `1440=1.333333`, `2160=2.0`. `ApplicationScale=1`; rule remains `ShortestSide`; no per-resolution branches or maximum-1 clamp were added.
- `1280x720`: DPI `.8`; root/layout/RT `1280x720`; top `64`, footer `42.2`, left panel `240`, right panel `384`, preview interaction `550.4x591.4`. This preserves the accepted compact 720p appearance.
- `1920x1080`: DPI `1`; root/layout/RT `1920x1080`; top `80`, footer `54`, left `300`, right `480`, preview `1008x918`. This preserves the accepted reference appearance.
- `2560x1440`: DPI `1.333333`; root/layout/RT `2560x1440`; top `106.67`, footer `71.33`, left `400`, right `640`, preview `1344x1224.67`. Text, panels, cards, hit areas, gaps and padding scale physically by 1.333 from 1080p.
- `3840x2160`: DPI `2`; root/layout/RT `3840x2160`; top `160`, footer `108`, left `600`, right `960`, preview `2016x1836`. UI physical size is exactly 2x the 1080p reference rather than a miniature 1.1x block.
- `2560x1080`: DPI `1`, identical to `1920x1080`; left/right remain `300/480`, edge gaps `24/24`, while preview expands from `1008` to `1648` physical pixels. Additional width goes to the center, not panel/card stretch.
- `3440x1440`: DPI `1.333333`, identical to `2560x1440`; left/right remain `400/640`, edge gaps `32/32`, while preview expands from `1344` to `2224` physical pixels.
- At all six points Root absolute size and LayoutRoot absolute size equal ActualViewportSize, background fills Root, top/footer span full width, panels remain at their DPI-scaled safe edges, preview is positive, and modal is within the full overlay. `OutsideViewportWidgetCount=0`, `UnexpectedClippingCount=0`, `ZeroOrNegativeGeometryCount=0`, `ModalOutsideViewport=false` and `VisualAndInteractionBoundsEqual=false` for every accepted matrix dump.
- The diagnostic zero-count now ignores a visible child whose modal ancestor is Collapsed; when the Reset modal is actually open at 1440p its absolute geometry is `1335x469`, `ModalOutsideViewport=false`, and the count is still `0`. This removes a diagnostic false positive without changing production layout.

### Visual surface, camera and interaction

- `Img_PreviewPlaceholder` remains a full-root, HitTestInvisible Visual Surface. It is still in RootPanel, remains distinct from `SB_PreviewArea`, and no rectangular crop, ScaleToFit root, constrained SceneCapture aspect, custom projection, scissor or custom view rect was introduced.
- Actual RT sizes are exactly the six viewport sizes. RT/viewport aspects are `1.777778`, `1.777778`, `1.777778`, `1.777778`, `2.370370` and `2.388889`; each RT aspect matches its viewport and full Visual Surface.
- `SB_PreviewArea` remains the only preview input acceptance area. On ultrawide it expands with the free center. Wheel/drag coordinates use its live cached geometry after resize; ScrollBox isolation proof changed torso scroll offset `0 -> 288` while camera distance remained exactly `423.702`.
- Base composition is event-driven from the usable preview center after open/resize/category settling. The character projected X tracks that center: `0.4407/0.4428` at 720p, `0.4417/0.4425` at 1080p, `0.4488/0.4521` at 2K, `0.4493/0.4521` at 4K, `0.4509/0.4521` at `2560x1080`, and `0.4641/0.4641` at `3440x1440` (character/composition). Projection/deprojection still uses the full Visual Surface.
- Composition refresh occurs only after a real viewport/geometry change; there is no permanent AutoFit or per-Tick camera fit. All dumps report `AutoFitActive=false` and `ScreenSpaceCharacterClampActive=false`.
- Real input smoke: preview wheel was handled with a valid cursor anchor (`FocusPlaneFallback` for the sampled ray), target distance changed, real drag changed yaw `0 -> 42`, modal input left camera distance/yaw unchanged, and ScrollBox wheel did not leak into preview zoom.

### Functional and localization regression

- A task-created fourth character exercised Add (`3 -> 4`), Switch to original and back, Rename start/commit/cancel, real pointer-inside-RenameHitRect Enter and Escape semantics, Reset confirmation, Random confirmation, and Delete (`4 -> 3`). The test row/name was removed before localization audit and the baseline SaveGame was then restored again in `finally`.
- Both `HeadType01` and `HeadType02` were observed in one Random attempt after the reset baseline. Category/camera smoke covered My Characters/FullBody, Presets/FullBody, Head/Head, Face/Face, Hands/Hands, Upper/UpperBody, Lower/LowerBody and Full Outfits/FullBody.
- Manual modular/equipment self-test: `PASS`; Torso/Legs/Feet exit ensemble, None exits, symmetric/combined modal policy, compatible cap, suppression, one autosave per operation and ScrollBox offset preservation all passed; FullOutfit catalog/runtime cards `31/31`, errors `{}`.
- RU/EN/LEET runtime switching reports missing translations `0`, fallback `0`, culture-invariant visible text `0`, and literal character names preserved. LEET transforms the UI only during the task-scoped test; final pseudo is false. The immediate culture callback retained one desired-width-over-cached-width sample before the next Slate layout in the functional dump; the steady-state six RU matrix dumps have `TextOverflowDetectedCount=0`, `UnexpectedClippingCount=0`, and screenshots show the intended ellipsis/no raw keys. No production text-size constants were changed.
- Current Selection remained localized, modal text localized, card aspect/panel logical widths unchanged, and RU screenshots contain normal Russian without `‡`. Existing localization foundation evidence remains applicable for full EN/LEET tooltips and raw-key audit.

### Exact runtime evidence

- All screenshots are real immersive PIE/Slate windows captured at the exact accepted ActualWindowSize and ActualViewportSize; no Designer grid/zoom/device rectangles are present. Sizes above the physical work area used off-screen `SetWindowPos` sizing only; Windows display mode, browser and other application windows were never changed.
- `Saved/CodexDiagnostics/CustomizationResolutionAdaptation_20260718/runtime_1280x720_ru.png`, SHA-256 `0E1D133523411570CECBCF38E4927A38848214C6B659063DA299EE1F3CFEEFE5`.
- `runtime_1920x1080_ru.png`, SHA-256 `11EA09A51E0CA9AC6C347190791CB3A66DCC02777402389BEB7CC3A34EC9C5C0`.
- `runtime_2560x1440_ru.png`, SHA-256 `7E10CCF8BBA279F29E4D95323C7AD73F319B0AC17724BEDA7D39E364ACC7FC0E`.
- `runtime_3840x2160_ru.png`, SHA-256 `6B499CB0EF6FBF2511CB67F8E10A0E0B172AA9C419F6C79589509F30894789B3`.
- `runtime_2560x1080_ru.png`, SHA-256 `9DCDF49DC372B40DB7EAC2180628E1D70DABC1B0C6AF86EE0A8E38F387C27424`.
- `runtime_3440x1440_ru.png`, SHA-256 `4024063402DB40F3237ED46E250C18473B46F42F7F75598EEC7E4C25C80CE7A5`.
- Per-resolution requested/actual window, viewport, aspect, DPI, Root/LayoutRoot/panel/preview/RT geometry and counters are in `runtime_*_metrics.json`; consolidated evidence is `runtime_matrix_summary.json`. Functional evidence is `functional_smoke_summary.json`; Blueprint and log evidence are `final_blueprint_validation.json` and `final_log_audit.json`.

### Production changes, build and final integrity

- Production files changed relative to the task baseline: `Config/DefaultEngine.ini`; `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp`; `Source/Avaryo/Public/UI/AvCharacterCustomizationRootWidget.h`; `Source/Avaryo/Private/UI/AvCustomizePreviewActor.cpp`; `Source/Avaryo/Public/UI/AvCustomizePreviewActor.h`. No `.uasset` or `.umap` was edited or saved. Diagnostic runners/artifacts live only under `Saved/CodexDiagnostics/CustomizationResolutionAdaptation_20260718/`.
- Final full UE 5.8 UBT after all C++ changes: exit code `0`, `Result: Succeeded`, total `20.53 seconds`. No C++ changed afterward. `UnrealEditor-Avaryo.dll` SHA-256 `8ACFB4C08751828D83993919A1D16916FD78CC7CFFA3E345C8A35543704C2446`.
- Cold Editor compile: `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2` Success and `/Game/Avariika/UI/CharacterCustomization/WBP_HeadCatalogItemCard` Success. Validation errors `0`; Root retains `17` existing orphan/unused warnings and Card `3` empty lifecycle warnings. GUID errors `0`, duplicate GUID `0`, BindWidget errors `0`, reconstruction errors `0`, invalid-parent errors `0`.
- Cold-log scoped audit: Fatal `0`, Ensure `0`, GUID/duplicate GUID `0`, BindWidget `0`, reconstruction `0`, invalid-parent `0`; final successful runtime cycle Python errors `0`. The known UE 5.8 UnifiedError startup self-test still emits its unrelated 17 deliberate `LogAutomationTest: Error: Condition failed` lines.
- Task baseline backup is `Saved/CodexBackups/CustomizationResolutionAdaptation_20260718/`; original company SaveGame SHA-256 is `4634A6E57FA347E2790251D240C0EC14DF12C25FF0D8E4F8080180317AA3EBE2`, settings SHA-256 is `619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7`. Both final files match byte-for-byte; no test character remains.
- Every test runner has a `finally` cleanup path that turns off LEET, returns Culture `ru`, stops PIE, restores the original maximized Editor placement/window mode and both SaveGames, and verifies dirty Content/Maps `0/0`. The final Editor is responsive and maximized; PIE=false, Culture=`ru`, pseudo=false, dirty Content `0`, dirty Maps `0`.
- Remaining manual work is user visual acceptance on the user's real 2K/4K/ultrawide displays, including preferred character composition, small-text comfort, every catalog/card state and tooltip/ellipsis feel. The exact screenshots and geometry prove the runtime mechanics, not subjective visual acceptance.

## Character Customization localization foundation - 2026-07-18

### Scope and resulting localization architecture

- Character Customization is localization-ready for Russian and English, but is **not feature frozen**. The separate responsive-resolution pass remains mandatory before feature freeze and before moving to the main menu. Visual acceptance belongs to the user; no visual PASS is claimed.
- The project already had the canonical localization target `Game`, native culture `ru`, and one source-backed String Table registered as `AvariikaUI` from `Content/Avariika/Localization/AvariikaUI.csv`. That architecture was retained instead of creating a parallel target or duplicate tables. `Config/Localization/Game.ini`, `Game_Export.ini` and `Game_Import.ini` now provide gather/export/import/compile for `ru` and `en`; packaging stages only `ru` and `en`. `Config/DefaultEditor.ini` explicitly exposes the project `Game.locres` path to Editor/PIE.
- Stable semantic keys use the `Customization.*` convention: `Category`, `Action`, `Common`, `Modal`, `Character`, `Section`, `Preview`, `Item.<Slot>.<StableId>` and `Preset.Loadout.<StableId>`. Asset paths, StableIds, UObject names, diagnostics and SaveGame keys are not localized.
- The source table now contains `388` customization keys: `148` item display keys, `125` per-preset loadout keys and `115` UI/modal/common/section/preset keys. One existing table was extended; no second String Table, duplicate production asset or fake shipping pseudo language was created.
- Runtime text uses `FAvLoc::Text()` / `FText::FromStringTable`; display structures expose culture-aware `FText` while retaining FString compatibility caches where Blueprint/API stability required it. The two authored widgets contain `85` direct `AvariikaUI` String Table references and `12` approved non-localizable symbols/keycaps/numeric placeholders. The remaining `7` gathered Widget Blueprint identities are only those approved invariants.

### Full text audit and translations

- Before audit: `540` occurrences total, `528` localizable (`443` C++/runtime literals plus `85` authored Text/Tooltip fields) and `12` explicit invariants. After audit: C++ hardcoded user-facing occurrences `0`; all `443` runtime occurrences and all `85` authored fields were migrated, unresolved user-facing strings `0`. Reports: `Saved/CodexDiagnostics/CustomizationLocalizationFoundation_20260718/localization_audit_before.csv` and `localization_audit_after.csv`.
- Gather used `71` project-owned source files and exactly the two active widgets. The UE 5.8 global nested-macro prepass was skipped because the unrelated `rdSplineTools` plugin contains duplicate third-party macros; the task source mainpass and exact asset gather completed with exit code `0`. After authored texts became table references, obsolete localized widget GUID entries were purged; duplicate localization GUIDs are `0`.
- RU and EN archives/locres contain all `388` customization keys. Verification: missing translations `0`, fallback/mismatch `0`, raw keys `0`, placeholder mismatches `0`; RU locres `49,878` bytes, EN locres `42,223` bytes. Natural English labels and item names are present rather than asset filenames or StableIds.
- Runtime RU F9: target `Game`, keys `388`, missing `0`, fallback `0`, culture-invariant visible text `0`, runtime catalog localized items `149`, Current Selection/localized modal true, overflow `0`. Runtime EN audited `272` root text blocks: Russian interface strings `0`; the sole visible Cyrillic value was the allowed stored literal character name. Reset and Random modal content, buttons, Current Selection, tooltips and runtime-generated catalog values resolved in English.
- UE 5.8 `LEET` runtime pseudo culture transformed customization UI while literal `WORKER 02` remained unchanged; missing `0`, fallback `0`, raw keys `0`, placeholder failures `0`, culture-invariant visible text `0`. The separate task-scoped 35% expansion stress covered all `388` strings, averaged `37.7%` expansion and preserved every format placeholder.
- Pseudo at `1280x720` identifies one deferred width issue: a Head catalog card title exceeds cached card width. Existing safe ellipsis is active and the full localized title remains available through the tooltip. It is documented in `Saved/CodexDiagnostics/CustomizationLocalizationFoundation_20260718/responsive_followup.md`; no panel-width/anchor redesign was performed.

### Literal character names and culture switching

- `FAvCharacterRecord::DisplayName` remains an FString literal and the SaveGame schema/type was not changed. Existing stored names are displayed through `FText::FromString()` and never resolved as localization keys, migrated, translated or recalculated by Culture change.
- `Customization.Character.DefaultNameFormat` is consulted only when a new record is created, then the formatted string is stored literally. Runtime proof: an English-created test record was named `WORKER 02`; after `en -> LEET -> ru` it was still exactly `WORKER 02`. The test record was later deleted and the original SaveGame restored.
- The native culture-change delegate performs a text-only refresh for runtime cards, categories, item catalogs, tooltips, modal/current-selection text and diagnostics. It does not poll Tick, rebuild the authored Widget Tree, recreate the preview actor, reset camera/ScrollBox offsets or mutate appearance. RU, EN and LEET F9 all report names changed `false`, culture-switch SaveGame commits `0`, autosaves `0` and appearance mutations `0`.
- Multi-character proof used four records during the temporary test. `WORKER 02` appearance hash was `96714DD5` before switching, restored as `96714DD5` after switching to `Character_01` (`5A3FB7EB`) and back, and cold-restarted with the same ID/name/hash and four runtime rows. Add, Switch, Rename focus/cancel, Reset modal, Delete modal and actual Delete were exercised. The final restore returned the original three records and active literal `РАБОЧИЙ 01`.

### Regression, font limits and diagnostics

- The localization pass did not change Random selection/eligibility, HeadType distribution, `ApplyHeadTypeSkinProfile()`, wardrobe compatibility, Preset Preview/Apply Preset, camera presets, cursor-centric zoom, FullBody framing, full-root Visual Surface, `SB_PreviewArea`, roster schema or ScrollBox policy. The user had already confirmed the HeadType Random correction. RU/EN/LEET screenshots retain the non-cropped full-root preview path.
- The separate white-glove rejection `HeadTypeNotInVerifiedCatalog` remains open and was not fixed or expanded in this task.
- Existing Roboto/composite fallback displayed Cyrillic, Latin, digits, apostrophe, ampersand, hyphen, ellipsis and format braces without missing glyph boxes in RU/EN. Chinese/Japanese/Korean, Arabic/Hebrew/RTL, Thai and broad complex-diacritic acceptance were not performed and are not claimed as supported.
- F9 now reports CurrentCulture/Language, native culture, target/revision/table state, localized/missing/fallback/invariant counts, runtime item count, Current Selection/modal/name invariants, culture commit/autosave/appearance counters, pseudo state and overflow. Task F9 files are `f9_ru.json`, `f9_en.json`, `f9_pseudo.json` and the multi-character dumps in the diagnostic directory.

### Build, Blueprint, screenshots, backup and final integrity

- Full UE 5.8 UBT after all C++ changes: exit code `0`, `Result: Succeeded`, total `20.10 seconds`; no C++ changed afterward. `UnrealEditor-Avaryo.dll` SHA-256: `DFF771AC953CCEF3F4F4375B7752C72B67D5DF0845E77C26947DE8072E696FE0`.
- Cold compile: `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2` Success and `/Game/Avariika/UI/CharacterCustomization/WBP_HeadCatalogItemCard` Success. Validation errors `0`; Root retains `17` existing orphan/unused warnings, Card retains `3` empty lifecycle warnings. GUID/duplicate GUID `0`, BindWidget `0`, reconstruction `0`, invalid-parent `0`.
- Authored assignment report: `85/85`, missing widgets `0`, identity failures `0`, compile failures `0`, save failures `0`. The Python commandlet's task work succeeded; its non-zero aggregate shutdown status came solely from `551` existing unrelated third-party asset errors. Final direct target Blueprint compile/validation is clean as above.
- Screenshots, all exact `1280x720`: `customization_ru.png` SHA `A493A39566B0D99B1DB9AFF7408D232303A4D74A929AF2CD9B8EAB535682F2BC`; `customization_en.png` SHA `709584A546871BDFF72C7F3DBEA3CA732881D32DA9DCE28042012F2393A0428B`; `customization_pseudo.png` SHA `0A141AED1823502D91F320DF6DF4BD3A6EF7E1014B37EC92B5A02184712A7109`. They are technical evidence only; user visual acceptance is still required.
- Backup retained at `Saved/CodexBackups/CustomizationLocalizationFoundation_20260718/` with SaveGames, settings, C++, configs, localization data, the two Widget Blueprint binaries, handoff and SHA manifest. A separately captured pristine `DefaultEditor.ini` lives under `LateScopedConfigBackup/`; no prior backup was overwritten and no git commit was created.
- Original company/appearance SaveGame restored byte-for-byte: SHA-256 `4634A6E57FA347E2790251D240C0EC14DF12C25FF0D8E4F8080180317AA3EBE2`. Settings restored byte-for-byte: `619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7`. Cold restore proof: exactly three original IDs/rows and active literal `РАБОЧИЙ 01`; no test name/record/appearance remains.
- Final Editor PID `12252` is responsive; Culture `ru`, PIE=false, dirty Content `0`, dirty Maps `0`. Cold log after startup: Fatal `0`, Ensure `0`, GUID `0`, BindWidget `0`; the only post-start `: Error:` lines are four MCP read-only `Editor is currently in a play mode` utility messages. The engine also emits its known 17 deliberate `UE::UnifiedErrorTest` startup self-test lines; task/product error count is `0`.

## Random HeadType distribution fix - 2026-07-18

### User observation and proven cause

- The user reported that repeated Random appearance generation selected only the light `HeadType01`; `HeadType02` was effectively absent. After the corrective build/runtime pass the user explicitly confirmed: `Проверил рандомно генерирует`. Visual acceptance remains with the user; Codex does not claim visual PASS.
- The exact cause was two-part and fully source-proven. `MakeRandomCompatibleMaleAppearance()` rebuilt every attempt from `MakeBaseMaleUnderwearAppearance()`, whose Head slot is hard-coded to `FactoryHead` (`HeadType01`), and Random had no Head selection stage at all. `ValidateRandomGeneratedMaleAppearance()` then required that exact `FactoryHead` and rejected every other head as `MissingFactoryHead`. `HeadType02` was therefore not in any Random Head candidate list; it was not rejected by quarantine, eligibility metadata, an off-by-one range, normalization, preview, or repeated seed initialization because it was never selected in the first place.
- The manual Head page already used the curated `GetHeadCustomizationCatalog()` and contained both intact records. Random now derives its only valid, non-null Head candidates from that exact same catalog; there is no second path list or skin-tone randomizer.

### Canonical candidates, selection and skin resolution

- Candidate 0: stable ID `HeadType01`, exact mesh `/Game/Modular_Workers/Mesh/Male/Body/Quantum_FaceRig/SKM_Quantum_FaceRig.SKM_Quantum_FaceRig`, profile `Light`.
- Candidate 1: stable ID `HeadType02`, exact mesh `/Game/Modular_Workers/Mesh/Male/Body/Quantum_FaceRig/SKM_Quantum_FaceRig_Afro.SKM_Quantum_FaceRig_Afro`, profile `Dark`.
- Both records are the same `HeadType` records exposed by the manual UI, `bAllowInUI=true`, exact path non-null/valid, distinct by stable ID and exact path. Random uses `FMath::RandRange(0, Candidates.Num() - 1)` over the two sorted records: uniform probability `0.5`/`0.5`, no weights and no `None` candidate.
- Seed source is the engine/process-global `FMath::RandRange` PRNG state: `FMath::RandRange_GlobalProcessPRNG_NoPerClickReseed`. No `FRandomStream` is recreated per click and no seed was added to SaveGame.
- The selected exact Head mesh is written to the local future `FWorkerAppearance`; `NormalizeHeadTypeSkinPresentation()` binds the serialized-compatible skin endpoint to that HeadType, general validation accepts either canonical HeadType, and the existing component rebuild applies the only material resolver, `ApplyHeadTypeSkinProfile()`, to the verified head/neck, exposed arms/hands, canonical body and derived exposed-skin modules. Clothing, gloves, hair, beard and accessories remain outside the skin-material allowlist.
- `ApplyHeadTypeSkinProfile()` remains the single runtime material/profile resolver. No new skin swatches, SkinTone random stage, serialized color variable, independent material override or extra shade was introduced.

### Distribution and runtime evidence

- Pure helper-level distribution test used the same canonical Head selector for `200` local appearances and performed no SaveGame mutation: `HeadType01=101` (`50.5%`), `HeadType02=99` (`49.5%`), `Unknown=0`, `Invalid=0`, `Fallbacks=0`, result `PASS`.
- Head catalog self-test: `PASS`, `HeadType=2`, `Headgear=22`, `Hair=5`, `Beard=7`, records `36`, errors `0`. Head/skin self-test: `PASS`, both profiles `2/2`, skin material boundary `PASS`, Tick disabled. Exhaustive Random eligibility test: `PASS`, records `142`, allowed `63`, denied `79`, denied final-validation rejects `79`, Random Gloves=None passes `32/32`.
- Runtime HeadType01 evidence: `Saved/CodexDiagnostics/RandomHeadTypeDistributionFix_20260718/runtime_head_type01.json`; selected/final stable ID `HeadType01`, exact standard FaceRig mesh, resolved/final-applied profile `Light`, head validation `PASS`, Random commit `1`, autosave `1`, fallback false. Screenshot `random_head_type01.png`, SHA-256 `F71AEBB5D2E7CFF61E3C1ABE89FC367B26E8B858ED3E1ED584DF665B420526DA`.
- Runtime HeadType02 evidence: `Saved/CodexDiagnostics/RandomHeadTypeDistributionFix_20260718/runtime_head_type02.json`; selected/final stable ID `HeadType02`, exact Afro FaceRig mesh, resolved/final-applied profile `Dark`, head validation `PASS`, Random commit `1`, autosave `1`, fallback false. Screenshot `random_head_type02.png`, SHA-256 `D58964D306F05D11FDF2E8E18FE85213590B422C07C6874104A4D0BBA08B7D75`.
- Both runtime profiles reported the same profile in the HeadType validation and the preview component's final `resolved_body_coverage.skin_profile`; screenshots are task evidence only, not a visual PASS declaration. The user performs final face/neck/body/arms/legs visual acceptance.
- Random remained one atomic active-record operation: one local future appearance, one validation, one preview application through the existing commit path, one canonical appearance commit and one autosave. `PresetPreviewAppearance` remained unchanged (`PreviewSaveUnchanged=true`); no equipment-conflict modal or checkbox suppression was added to Random. FullOutfit/Overalls remained None and the existing compatibility cleanup/eligibility policy remained intact.

### Multi-character, regression and persistence

- Three existing records were used; no test record or name was created. Before Random on character 2: character 1 appearance hash `5A3FB7EB`, character 2 `D8F8057F`, character 3 `D8A2A05C`. Random changed only active character 2 to `7759272D`; switching back proved character 1 still `5A3FB7EB` and character 3 still `D8A2A05C`.
- After a full cold Editor/PIE restart, character 3 restored as `D8A2A05C` (`HeadType01/Light`) and character 2 as `7759272D` (`HeadType02/Dark`). Evidence is retained as `cold_restart_character03.json` and `cold_restart_character02.json` in the task diagnostic folder.
- Manual HeadType architecture and both records remain intact. The Head catalog/self-test exercised both exact head/profile pairs, `ValidateHeadCustomizationSelection()` and `ApplyHeadTypeSkinProfile()` without changing the manual handler. Existing Current Selection/selected-state refresh paths were not changed. Manual/equipment compatibility self-test passed: modular selection, None exits, symmetric modal, cap compatibility, suppression, scroll preservation and one autosave per operation.
- Switch was exercised repeatedly in the isolation/cold-restart test. Reset, Add, Rename and Delete code/Blueprint/assets were not changed; their immediately preceding clean handoff regression remains applicable. Preset Preview/Apply Preset, hair, beard, headgear, cursor-centric camera and the full-root Visual Surface were not modified. F9 evidence retained `FullBody`, exact base state, full-root Visual Surface distinct from input bounds, and the existing non-cropped render path.
- The previously reproduced white-glove rejection `HeadTypeNotInVerifiedCatalog` remains a separate post-freeze bug. It was not included in or modified by this HeadType Random pass; the Random eligibility self-test still proved its canonical `Gloves=None` policy.

### Diagnostics, build, backup and final integrity

- F9 JSON now exposes `RandomHeadCandidateCount`, candidate stable IDs/paths, selected index/stable ID/path, resolved profile, head validation, fallback flag, seed source, Random commit/autosave counts, `ActiveCharacterId`, final Head stable ID/path and final applied skin profile. The development dump itself records `AppearanceMutation=false`, `Autosave=false`.
- `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2`: compile `Success`; validation warnings `17` (existing orphan/unused authored graph warnings), errors `0`. GUID/duplicate GUID `0`, BindWidget errors `0`, reconstruction errors `0`, invalid-parent errors `0`.
- Full UE 5.8 UBT after all C++ changes: exit code `0`, `Result: Succeeded`, total `26.15 seconds`. No C++ changed afterward. `UnrealEditor-Avaryo.dll` SHA-256 `BCC97FF9A9C2DF3D653F65D34F1F5F0C1335FE09191C9BB0DAC968168D1859BA`.
- Cold log audit: Fatal `0`, Ensure `0`, Blueprint errors `0`, GUID errors `0`. The engine's unrelated `UE::UnifiedErrorTest` emitted its known 17 deliberate `LogAutomationTest: Error: Condition failed` lines; two `LogUtils: Error: The Editor is currently in a play mode` lines came from read-only Editor-utility probing during active PIE. Task/product `: Error:` lines `0`.
- Baseline backup retained at `Saved/CodexBackups/RandomHeadTypeDistributionFix_20260718/`; `manifest_sha256.txt` contains SaveGame, settings, four C++ and handoff hashes. No prior backup was overwritten and no project-owned binary asset was changed.
- Final restored company/appearance SaveGame SHA-256: `4634A6E57FA347E2790251D240C0EC14DF12C25FF0D8E4F8080180317AA3EBE2`. Final settings SaveGame SHA-256: `619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7`. Both match the task baseline byte-for-byte; no test names, characters or appearances remain.
- Final Editor PID `21540` is responsive; PIE=false; dirty Content `0`, dirty Maps `0`. Visual acceptance is performed by the user.

## Preview base framing calibration after visual-surface expansion - 2026-07-18

### User reference, proven cause and canonical correction

- The acceptance reference is the former FullBody composition: the worker is visible from head/helmet through the soles, has small top/bottom margins, and occupies the free center between the left navigation and right content panel. The user then explicitly requested that the corrected worker be `10%` larger; the final canonical projected-height target is therefore `0.858` rather than the initially calibrated `0.78`. Visual and functional acceptance remains with the user; no visual PASS is claimed here.
- Runtime evidence ruled out stale manual zoom, stale focus, stale category state, UMG stretch/crop and mismatched projection. On the prechange cold open, FullBody was exactly at its old base: `Current=Target=BaseDistance=445`, `Current=Target=BaseFocus=[0,0,88]`, `manual_zoom_active=false`, cursor anchor invalid. RT, viewport and Visual Surface aspects matched; SceneCapture FOV was `25`, custom projection and constrained aspect were false.
- The exact scale regression was the old canonical `445` distance: it was authored for the former square `1024x1024` render target. After Visual Surface expansion, the render target became viewport-sized while SceneCapture retained a horizontal FOV of `25`; a wide projection has a smaller vertical FOV, so the unchanged distance made the character too large vertically. At the prechange `1719x1222` aspect `1.406710`, the projected AABB occupied approximately `1.429` of viewport height.
- The correction remains centralized in the existing camera-preset system. `ResolvePreviewCameraPresetBase` computes the one-time base distance from current combined character bounds, actual RT aspect and canonical preset target; it runs at preset application/real viewport resize, never every Tick. `PreviewFullBodyTargetHeightRatio=0.858`. FullBody base focus changed minimally from Z `88` to Z `94` to restore top/bottom composition. FOV remains `25`; MinDistance and zoom reach were not increased.
- `ResetPreviewCameraToCurrentPresetBase` is the single reset path. It assigns Base/Target distance and focus, optionally sets or interpolates Current according to the existing transition policy, clears manual zoom/focus clamp/cursor anchor state and records `last_camera_reset_reason`. It is used for customization open, category change, character change/create, viewport resize and wheel-down-to-base. Camera state is not saved to SaveGame.

### Pre/post camera, aspect and projected-bounds evidence

- Final cold-open evidence at `1275x1020`: preset `FullBody`; `Current=Target=BaseDistance=642.296`; `Current=Target=BaseFocus=[0,0,94]`; manual zoom false; anchor invalid; exact base true; projected bounds `[0.374358,0.102473,0.623593,0.959413]`; projected height `0.856940`; width `0.249235`; center `[0.498976,0.530943]`. The complete worker is present within the projection with top and bottom margin.
- The small difference from target `0.858` is animated-pose sampling. Character-switch evidence for another existing appearance settled at distance `650.272`, height `0.859444`, exact base true. A newly created factory character settled at `636.367`; the temporary character and all test mutations were later removed by byte-for-byte SaveGame restore.
- RT aspect, viewport/Visual Surface aspect and SceneCapture aspect stayed equal. At exact `1280x720`, all were `1.777778`, FullBody height was `0.859504`; at exact `1600x900`, all were `1.777778`, FullBody height was `0.857038`. At the user's actual tested `1275x1020`, all were `1.25`. SceneCapture `bConstrainAspectRatio=false`, custom projection false, no letterbox, no UV crop and no UMG image stretching were introduced.
- `AutoFitActive=false` and `ScreenSpaceCharacterClampActive=false`. Projected bounds are used only to solve a canonical base when a preset/viewport is applied; no continuous AutoFit or screen clamp exists, and manual zoom may freely move body parts beneath UI.

### Presets, lifecycle and input regression results

- Routing remains: MyCharacters/Presets/FullOutfits -> FullBody; Head -> Head; FaceProtection -> Face; HandsAccessories -> Hands; UpperEquipment -> UpperBody; LowerHip -> LowerBody. Runtime bases at aspect `1.25` were Head `204.048`/focus Z `151`, Face `147.833`/Z `158`, Hands `285.083`/Z `108`, UpperBody `316.388`/Z `119`, LowerBody `366.114`/Z `64`; each reset manual/anchor state and reached exact category base. The authored square-reference distances `170/125/235/260/300` remain the canonical meaning used to derive these aspect-correct distances.
- Real wheel zoom reached distance `322.296` from FullBody base `642.296`, set manual zoom true and a valid cursor-centric focus-plane anchor. Real full wheel-down returned exactly to `Current=Target=BaseDistance=642.296`, `Current=Target=BaseFocus=[0,0,94]`, manual false, anchor invalid; repeated wheel-down cannot move farther from the character.
- Category changes do not carry manual zoom or cursor anchor. Character switch from a manually zoomed UpperBody state reset to FullBody, cleared manual/anchor state, interpolated to exact base and recorded `CharacterChanged`. Character creation also receives canonical FullBody base. Ordinary item selection does not reset the camera: changing torso `SKM_TShirt_Blue -> SKM_Shirt_RolledUp_Green` preserved preset, distance `554.272`, base `650.272`, focus, manual=true and anchor=true.
- The task preserved the full-root `Img_PreviewPlaceholder` Visual Surface, `HitTestInvisible`, RootPanel Z order, viewport-sized transient RT, and `SB_PreviewArea` as input-only geometry. Strong manual zoom still places the character beneath top/right/footer UI without the former rectangular crop. Cursor-centric zoom remains active. Real wheel over catalog UI did not alter distance/focus; with `modal_mode=ResetToBase`, real wheel left distance, target, focus, manual and anchor unchanged.
- Add, Switch, Rename, Reset and Delete were exercised through their existing runtime handlers. Temporary character `Character_2855E9D240140DA185911ABF046F921B` was created, renamed to `Codex Preview Temp`, reset through the confirmation modal and deleted; Switch and all commits reported the expected single SaveGame commit. FullOutfit/Overalls/catalog routing and existing appearance pipeline were not changed. The final restore removed the temporary name/character and all appearance mutations.

### Diagnostics, screenshots, build and final integrity

- F9 JSON now reports current preset, camera/SceneCapture FOV, SceneCapture aspect/constrain/custom-projection flags, RT/viewport/Visual Surface/interaction geometry and aspects, current/target/base distance and focus, manual/anchor state, projected bounds/ratios/center, exact-base flag and last reset reason. Task evidence is retained under `Saved/CodexDiagnostics/PreviewBaseFramingCalibration_20260718/`.
- Required screenshots: `fullbody_base_after_fix.png` SHA-256 `FA06214A911E73C53881865ECC07A071C43D64899C282FD69C87C3071B1FFFB8`; `manual_zoom_after_fix.png` SHA-256 `1EB46D8949371046F9D07FE6E84DD9E4928E3DEBD7EDECAE91E579DE9185D52D`. They are diagnostic evidence only, not a visual acceptance declaration.
- Final full UE 5.8 UBT after the user's +10% adjustment: exit code `0`, `Result: Succeeded`, total `4.51 seconds`. No C++ changed afterward. `UnrealEditor-Avaryo.dll` SHA-256 is `D69076C321C5007E6D4A73CD22020AD2B2E543A6C4E2EB5529273C524768B9F4`.
- Cold `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2` compile: `Success`. Validation: existing warnings `17`, errors `0`. Cold log audit: Fatal `0`, Ensure `0`, Blueprint errors `0`, GUID/duplicate GUID `0`, BindWidget errors `0`, reconstruction errors `0`, invalid-parent errors `0`. The engine's unrelated startup `UE::UnifiedErrorTest` self-test emits 17 deliberate `LogAutomationTest: Error: Condition failed` lines; excluding that named engine self-test, task/product `: Error:` count is `0`.
- Backup retained at `Saved/CodexBackups/PreviewBaseFramingCalibration_20260718/`; `manifest_sha256.txt` records all baseline hashes. Company/appearance SaveGame was restored byte-for-byte to `FAAE346C23F28665A90482C961E8E543C699E2C352357F510B32CC620C0D8A05`; settings SaveGame to `619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7`.
- Final cold Editor PID `3000` is responsive; PIE=false; dirty Content `0`, dirty Maps `0`. No `.uasset`/`.umap` was edited, no duplicate asset was created, and previous backups were not overwritten. Visual and functional acceptance is performed by the user.

## Preview visual and interaction bounds separation - 2026-07-18

### Proven render path, cause and implemented separation

- Runtime path remains `SceneCapture2D -> transient UTextureRenderTarget2D -> dynamic M_PreviewWorker_UI -> Img_PreviewPlaceholder`.
- The proven crop cause was shared authored geometry: `Img_PreviewPlaceholder` lived under `Overlay_PreviewArea` / `SB_PreviewArea`, so the same `SB_PreviewArea` bounds constrained both rendering and input.
- `ConfigurePreviewVisualSurface()` moves `Img_PreviewPlaceholder` to `RootPanel`, fills the root (`anchors 0,0-1,1`, zero offsets), uses Z order `1`, `HitTestInvisible`, and inherited clipping. Background remains Z `0`, layout Z `2`, random overlay Z `100`.
- `SB_PreviewArea` remains the only preview interaction acceptance geometry. Cursor projection now uses the full-root visual surface. The preview image does not intercept mouse input.
- The transient render target follows the actual preview viewport size and resizes only when that size changes. Projection and SceneCapture aspect use the same viewport aspect.
- No C++ changed after the successful UE 5.8 UBT (`CODEX_UBT_EXIT=0`, `Result: Succeeded`). Final DLL SHA-256 is `54EAE93819F68D0C6A2380365ED8104787726340E3198C93A6CA021428A58547`; it is newer than every task-owned C++ source. No second UBT was run.

### F9 runtime evidence

- Actual F9 opened and closed the development inspector. Runtime JSON reported: `preview_render_mode=SceneCapture2D->TransientRenderTarget->MID->UMGImage`, visual widget `Img_PreviewPlaceholder`, parent `RootPanel`, interaction widget `SB_PreviewArea`, first clipping parent `NoneBeforeViewport`.
- At the final tested window geometry the visual surface was `1275x1020`, the interaction area `363x858.55`, and `preview_visual_and_interaction_bounds_equal=false`.
- Render target and viewport were both `1275x1020`, aspect `1.25`. An earlier maximized run likewise reported identical RT/viewport `1719x1222`, aspect `1.406710`.
- Visual visibility was `HitTestInvisible`; `AutoFitActive=false` and `ScreenSpaceCharacterClampActive=false` throughout the zoom tests.

### Visual, input and cursor-mapping runtime results

- Strong body zoom reached the close framing needed to move the head behind the top bar and the torso/limbs beneath the side and bottom UI. No old horizontal/vertical `SB_PreviewArea` crop line, black/grey rectangle, exposure seam, UI-overdraw, or aspect stretching was observed. Required screenshot: `Saved/CodexDiagnostics/PreviewVisualAndInputBoundsSeparation_20260718/body_zoom_after_fix.png`, SHA-256 `A47C044B61B5895EB5D78B3D1AA34A8E9A60C5B03FCE7D015D6EF9441F9EF33B`.
- LowerBody zoom reached `CurrentDistance=MinDistance=100`, `ZoomAlpha=1`. Pants stayed under the cursor-derived focus while the opposite leg continued under the right panel without the former interaction-area crop. Required screenshot: `Saved/CodexDiagnostics/PreviewVisualAndInputBoundsSeparation_20260718/hands_or_lower_zoom_after_fix.png`, SHA-256 `B4CFF54E27BB47DD8EEA3B2F1FCB153325C5F711B20291E097039993241061FC`.
- Preview wheel changed camera distance and preview drag changed yaw (`0 -> 28 degrees`). LowerBody right ScrollBox wheel changed offset `0 -> 576` while camera distance stayed at its category base. Dragging the right panel did not change yaw. Wheel over the My Characters roster left camera distance at `BaseDistance=445` and did not route to the camera.
- With `modal_mode=ResetToBase`, preview wheel and drag left distance/yaw unchanged and reported `WheelHandled=false`. No reset was confirmed and no appearance data was mutated.
- Zoom-out restored `CurrentDistance=TargetDistance=BaseDistance=445`, `CurrentFocus=TargetFocus=BaseFocus=[0,0,88]`, `ZoomAlpha=0`, and further negative wheel input could not move beyond base.
- Cursor anchors were recorded at three vertical regions: face local Z approximately `160.1` (focus Z `157.8`), hand/glove region local Z approximately `106.5` (focus Z `107.0`), and pants local Z approximately `67.8`. All were handled only while the pointer was inside `SB_PreviewArea` but projected through the full-root visual surface.
- Two clicks on the white-glove catalog item were rejected by the existing unrelated eligibility guard with `Reason=HeadTypeNotInVerifiedCatalog`. They produced no mutation and no autosave. This task did not change C++ to address that separate catalog issue.

### Backup, data integrity and final state

- Prechange backup is retained at `Saved/CodexBackups/PreviewVisualAndInputBoundsSeparation_20260718/`, including both SaveGames, four C++ files, Root WBP, preview material, handoff baseline, and `manifest_sha256.json`.
- Company/appearance SaveGame remained byte-identical to backup: `FAAE346C23F28665A90482C961E8E543C699E2C352357F510B32CC620C0D8A05`. User settings remained byte-identical: `619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7`.
- Final Editor PID `24660` is responsive. PIE is active with exactly one visible customization root; dirty Content `0`, dirty Maps `0`. No `.uasset` or `.umap` was modified or saved.

## My Characters rename text-only hit area - 2026-07-18

### Scope, user reference and proven cause

- Work stayed inside `C:\unrealEngine\avariika_UE58_sandbox`. The user-provided screenshot in the task points at the visible `РАБОЧИЙ 02` label. It is the acceptance reference for the defect; no separate visual PASS is claimed by Codex. After the runtime correction the user reported: `Я проверил робит`.
- The accepted authored rectangular runtime-row design was preserved. Card width/height, padding, square silhouette, Base Border, Selected orange Border, active/inactive colors, list order, Add, Reset/Delete, ScrollBox, SaveGame schema, appearance data and preview camera were not redesigned.
- The exact former cause was the transparent `NameButton` in the second row `SOverlay` slot. That slot used `HAlign_Fill`, `VAlign_Fill` and padding `(105,13,21,13)`, so the button geometry occupied the entire remaining name area through the right side of the row. Its `OnClicked(HandleNameClicked)` called `BeginRename()` without checking the pointer against the rendered glyph width. The display `STextBlock`, Base/Selected borders and silhouette were not independently opening Rename.
- The row-selection `SButton` remains the full-card selection target and continues to call `HandleRowClicked` from `OnClicked`. The root widget's existing `NativeOnMouseButtonDown` remains preview-camera-only and is not the roster rename route.

### Corrected display/edit architecture and hit measurement

- The wide transparent rename `SButton` and its `OnClicked` were removed. The accepted row hierarchy is now `SBox(92) -> SOverlay -> full-row selection SButton + name SOverlay`. The name overlay stays at the authored `(105,13,21,13)` position and contains the display `STextBlock` plus the fill-width `SEditableTextBox`; no card dimensions or name position changed.
- Display mode uses the unchanged Roboto `Regular` size `18`, one line, no wrap, `Ellipsis`, `ClipToBounds` and full-name tooltip. The display text is `HitTestInvisible`; it no longer supplies a large widget hit target. Rename mode collapses the display text, shows the fill-width editor, sets the current name, gives it keyboard focus and retains normal caret/selection behavior.
- `UAvMyCharacterRowWidget::NativeOnPreviewMouseButtonDown` now observes LMB before the row button. It reads `NameText->GetCachedGeometry()`, converts the screen pointer with `AbsoluteToLocal`, and measures the current `DisplayName` with Slate's renderer `FSlateFontMeasure` using the exact `DisplayNameFont`.
- DPI/application scaling is accounted for by measuring at the cached geometry's accumulated layout scale and converting the measured result back to local Slate units. Runtime evidence used `layout_scale=0.9767`, `application_scale=1.0`.
- For unclipped text, `DisplayNameVisibleWidth` is the measured full width. For clipped text, the implementation measures the Unicode ellipsis and the longest prefix fitting in `available width - ellipsis width`; visible width becomes `prefix + ellipsis`, capped by the actual cached geometry width. The hidden suffix never expands the hit area.
- `RenameHitRect` starts at the visible text's left edge and measured vertical text bounds. Tolerance is exactly `3 px` on each edge. It is not the 301 px fill slot and it is never extended through the remaining row width.
- A text hit records `Rename`, calls `BeginRename()` once and returns `FReply::Handled`, so the underlying row button cannot also select. A non-text hit records `Select`, returns unhandled to the existing full-row button, and only its `OnClicked` selects. In edit mode the preview handler records `EditField` and leaves the event to `SEditableTextBox`.
- Active-row policy is unchanged: text opens Rename; all other points perform the existing already-active selection no-op. Inactive-row policy is also unchanged: an intentional text click opens Rename without an implicit selection, while every non-text click selects that row.

### F9 diagnostics and executed runtime evidence

- Development inspector JSON now includes the requested `LastRowPointerScreenPosition`, `LastRowPointerLocalPosition`, `DisplayNameGeometrySize`, `DisplayNameMeasuredTextSize`, `DisplayNameVisibleWidth`, `RenameHitRect`, `PointerInsideRenameTextRect`, `LastRowClickResult`, `RenameHandlerCallCount`, `SelectHandlerCallCount` and `RenameCharacterId`, plus accumulated layout/application scale. Diagnostic-only geometry export was added for controlled screen-coordinate tests.
- Three existing runtime characters satisfied the minimum-two requirement. Cold runtime produced exactly `3` records and `3` runtime rows; duplicate runtime rows `0`, duplicate delegates `0`, authored sample `Collapsed`, Add index `4`.
- Inactive `Character_01` text-center click: local `[80.887,13.805]`, measured width `161.775`, rect `[-3,-3,164.775,30.645]`, inside `true`, result `Rename`, keyboard focus `true`; Rename count increased exactly once and Select count did not change. The active character remained the previously active other record.
- Active-row text-center click: inside `true`, result `Rename`, focus `true`; Rename count increased once and Select count did not change.
- Gap before text on an inactive row: local X `-12.287`, inside `false`, result `Select`; Select count increased once, Rename count stayed unchanged, and the row became active with one normal selection SaveGame commit.
- Silhouette click on the other inactive row: local X `-65.529`, inside `false`, result `Select`; Select count increased once and the character switched. Base/Selected-border, far-right and vertical-padding retries all recorded `inside=false`, `Select`, one Select-handler increment per click and no Rename increment.
- Active-row non-text no-op cases: 10 px right of visible name, far-right area, above text, below text and selected left border produced `SelectNoOpAlreadyActive`, `SaveGameCommits=0`, and no Rename-handler increment.
- Ellipsis test used the temporary 20-character `WWWWWWWWWWWWWWWWWWWW` name. Geometry width was `301`, full measured width `409.556`, calculated visible width `282.594`, and rect `[-3,-3,285.594,30.645]`. A click at local X `280.546` in the visible ellipsis area produced `Rename`; a click at local X `296.928`, more than 10 px to the right of visible text but still inside the display slot, produced only `Select`.
- Real Enter commit changed the temporary company SaveGame hash and reported `Operation=Rename`, `SaveGameCommits=1`; Rename count had increased once and Select did not. Escape/cancel retains the existing `HandleNameKeyDown -> CancelRename -> FinishRename(false)` path and does not call the ledger; cancel checks left the SaveGame hash unchanged. One physical Escape probe that lacked edit focus closed PIE and was explicitly excluded from acceptance evidence. The user subsequently confirmed the corrected runtime works.
- Switch was exercised by inactive gap/silhouette clicks. Add, Reset, Delete-last protection and ordinary Delete were not reimplemented or rerouted; their already accepted behavior and delegates remain outside the changed path. The user's starting baseline stated Add/Switch/Rename/Reset/Delete/SaveGame/Appearance/camera worked, and the user confirmed the corrective runtime after the pass.
- Authoritative per-click dumps are retained under `Saved/CodexDiagnostics/MyCharactersRenameTextOnlyHitArea_20260718/`; use the `*_retry.json` files for silhouette/right/padding/border cases and `11_ellipsis_visible_area_click.json`, `12_ellipsis_ten_px_right.json`, `13_enter_commit_retry.json` for the final ellipsis/Enter evidence. Earlier non-retry probes are retained as diagnostic history rather than deleted.

### Blueprint, build, SaveGame restore and final state

- Prechange backup: `Saved/CodexBackups/MyCharactersRenameTextOnlyHitArea_20260718/`. It contains both SaveGames, all four potentially touched UI C++ files, the handoff baseline, Root WBP binary and `manifest_sha256.json`; old backups were not overwritten and no Blueprint was duplicated.
- The first full UE 5.8 UBT failed only because `GetRenderer()` returns `FSlateRenderer*` in this engine rather than `TSharedPtr<FSlateRenderer>`. The type/null check was corrected. The final full command completed with exit code `0`, `Result: Succeeded`, total execution time `5.18 seconds`. No C++ changed after that successful UBT. Final `UnrealEditor-Avaryo.dll` SHA-256: `E2D08A71242CBF091E5B17C76A4B58A1EF73C6FDB61C78E2423A8D0D62313302`.
- Cold `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2` validation: errors `0`, the same 17 existing orphan/unused-variable warnings. Compile result: `Success`. Cold log audit: Fatal `0`, Ensure `0`, Blueprint compile errors `0`, GUID errors `0`, duplicate GUID `0`, BindWidget errors `0`, reconstruction errors `0`, invalid-parent errors `0`.
- Root WBP remained byte-identical to the prechange backup: SHA-256 `9E27B66288DBC168A71CF148B6E34CCE504C5CA678B2B7581B3F6DEEB52DE87E`. No `.uasset` or `.umap` was manually edited or saved for this pass.
- Original company/appearance SaveGame was restored byte-for-byte: SHA-256 `FAAE346C23F28665A90482C961E8E543C699E2C352357F510B32CC620C0D8A05`. Original settings SaveGame was restored byte-for-byte: SHA-256 `619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7`. Temporary long/test names were removed by this restore; backups were retained.
- Final Editor PID `24452` is responsive, PIE=false, dirty Content `0`, dirty Maps `0`. No visual PASS is claimed; final visual and functional acceptance belongs to the user.

## My Characters runtime rows match authored card design - 2026-07-17

### Scope, references and authored source of truth

- Work was restricted to `C:\unrealEngine\avariika_UE58_sandbox`. The user reference described the runtime active row as a large capsule/pill with a brown fill and a circular silhouette container. The sole design source was the existing `SB_CharacterCard01` under `VB_RightPanelMyCharacters` in `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2`.
- The authored Widget Tree, sample, Add order, title/divider, ScrollBox, Reset/Delete controls, modal, backend, SaveGame schema, appearance records and preview camera were not changed. Root WBP stayed byte-identical to its prechange backup: SHA-256 `9E27B66288DBC168A71CF148B6E34CCE504C5CA678B2B7581B3F6DEEB52DE87E`.
- Final technical screenshot: `Saved/CodexScreenshots/MyCharactersRowAuthoredVisualMatch_20260717/technical_active_inactive_add_reset_delete.png`, SHA-256 `CB2FF67808028978E84928F883E7E85D570CBA8465CC00B939785BCEC77CE10C`. It contains two runtime rows, one active and one inactive, plus Add and Reset/Delete. It is technical evidence only; no visual PASS is claimed.

### Exact authored properties and corrected runtime structure

- `SB_CharacterCard01`: no WidthOverride, `HeightOverride=92`, VerticalBox slot `Auto/Fill/Fill`, bottom padding `10`.
- `Btn_SelectCharacter01` style: `RoundedBox`, `RoundingType=FixedRadius`, corner radius `3`, zero normal/pressed padding. Normal RGBA `(0.003347,0.005182,0.007499,0.98)`, Hovered `(0.005182,0.006995,0.008568,1)`, Pressed `(0.012,0.006,0.002,1)`.
- Base frame: transparent fill, `RoundedBox/FixedRadius`, radius `3`, outline width `1`, outline RGBA `(0.025,0.035,0.045,0.85)`, authored render opacity `0.35`, padding `(4,2,4,2)`.
- Selected frame: transparent fill, `RoundedBox/FixedRadius`, radius `3`, orange outline RGBA `(0.95,0.42,0.04,1)`, width `2`, padding `(4,2,4,2)`. It is `HitTestInvisible` only for the active row and `Hidden` otherwise, so selection does not affect desired size.
- Content Overlay slot padding is `13` on all sides. Portrait is `66x66`, `Auto`, vertically centered, with right slot padding `14`. Its authored rectangular frame is RGBA `(0.0025,0.004,0.0055,0.98)`, radius `2/FixedRadius`, dark outline width `1`, and content padding `4`.
- Silhouette is the existing `/Game/Avariika/UI/icon/chel_1_.chel_1_`, a square `1254x1254` Texture2D. Runtime uses `DrawAs=Image`, a square allocation and authored tint `(0.68,0.70,0.73,1)`; no portrait asset or circular background was created. Portrait mode remains `AuthoredSilhouettePlaceholder`.
- Name text uses `/Engine/EngineFonts/Roboto.Roboto`, Typeface `Regular`, size `18`, color `(0.82,0.83,0.85,1)`, one line, no wrap, ellipsis, runtime `ClipToBounds`, vertical center and text slot padding `(12,0,8,0)`. The full display name remains the row and TextBlock tooltip. Active text remains authored light text rather than changing to orange.
- Runtime Slate hierarchy is now `SBox(Height=92) -> SButton -> SOverlay -> Base SBorder + Selected SBorder + SHorizontalBox -> 66x66 square portrait frame/Image + STextBlock`. Selection refresh changes only Selected-frame visibility; it does not rebuild rows.

### Capsule cause and removed incorrect properties

- The previous runtime implementation used one combined `RowBrush`, a selected fill `(0.12,0.055,0.012,0.75)`, selected orange text, a `NoDraw` hit-target button, incorrect content/text spacing, zero portrait-frame padding/tint and runtime row gap `8` instead of authored `10`.
- The decisive capsule cause was that runtime assigned corner radii but left `FSlateBrushOutlineSettings::RoundingType` at its default `HalfHeightRadius`. Slate therefore rounded the row and portrait to half their height and ignored the intended small radii. The final implementation explicitly uses authored `FixedRadius` for normal/hover/pressed, base, selected and portrait-frame brushes.
- The brown active-row fill was deleted. Active and inactive rows share the authored dark Button background; active state adds only the transparent orange selected frame. The large circular silhouette container is gone; the remaining portrait frame is the authored 66x66 rectangular frame.

### Runtime, Blueprint, build and final state

- Create: `1 -> 2` rows/records with exactly one new ID, `Character_2FAB154749AC14B427B0A0A051901FE7`. One Add action produced one record/row, so duplicate Add delegates were `0` by effect.
- Switch to `Character_01` succeeded; selecting it again returned the existing no-op path. Runtime row object paths were identical before switch, after switch and after no-op (`rows_rebuilt=false`). Cold desired heights were exactly `92` for both rows.
- Three close/reopen cycles were `2/2/2`, with active root count `0 -> 1` per cycle, identical ordered IDs, two unique row object names and duplicate runtime rows `0`. Sample remained `Collapsed`; list order was sample, runtime rows, `SB_AddCharacter` last.
- Real F9 key input opened the inspector (`Visible`) and a second F9 closed it (`Collapsed`). Reset opened the existing modal and confirmed with row count staying `2`. Delete opened the existing modal and changed `2 -> 1`; a repeated last-character delete left count/SHA unchanged and did not open the modal.
- The final Root WBP cold compile was `Success`; validation errors `0`. The 17 existing orphan/unused-variable warnings remain. GUID errors `0`, duplicate GUID `0`, BindWidget errors `0`, reconstruction errors `0` and invalid-parent errors `0`.
- UBT history: the first pass failed only on an incorrect `SOverlay` include path and was corrected; an intermediate build succeeded, then the technical screenshot exposed the missing `FixedRadius`. The final full UE 5.8 UBT after that runtime correction completed with exit code `0`, `Result: Succeeded`, total execution time `12.99 seconds`. No C++ changed after this final UBT. Final DLL SHA-256 is `46FDC8BA537ADBC98DA495E99B57C944FC2A1049F73E2BAA9541ACD0654BAE8B` and is newer than both changed source files.
- Final cold log audit: Fatal `0`, Ensure `0`, BindWidget `0`, duplicate GUID `0`. The log contains the engine's 17 built-in `LogAutomationTest: Condition failed` startup samples, four automation-control `Editor is currently in a play mode` messages and one failed Python probe (`OverlayRandom count=4`) from learning to scope stale widgets after reopen cycles; none references the task implementation or Root WBP compile.
- Original SaveGames were restored byte-for-byte and cold-loaded without autosave: company/appearance SHA-256 `48E7C0A3D875C72C7EAF6734CD7BD7954E53AFBDEEBB91B2440AEF9412D5E7CA`; settings SHA-256 `619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7`. Restored runtime contained only `Character_01`; no test character remains.
- Retained backup: `Saved/CodexBackups/MyCharactersRowAuthoredVisualMatch_20260717/` with SaveGames, source baselines, Root WBP binary, handoff baseline and `manifest_sha256.json`. Diagnostics: `Saved/CodexDiagnostics/MyCharactersRowAuthoredVisualMatch_20260717/`.
- Final Editor state: responsive, PIE=false, dirty Content `0`, dirty Maps `0`. Appearance isolation, SaveGame schema, camera, Reset/Delete backend and all authored assets remain unchanged. Final visual acceptance belongs to the user.

## My Characters compact authored rows for create and switch - 2026-07-17

### Corrective scope and authored source of truth

- This corrective pass was performed only in `C:\unrealEngine\avariika_UE58_sandbox`. The user-provided screenshot/design direction established that the prior use of `/Game/Avariika/UI/CharacterCustomization/WBP_HeadCatalogItemCard` made the roster look like a clothing catalogue and was not the authored My Characters design. That task-owned roster usage has been removed; the asset/class remains unchanged in purpose and continues to serve the clothing/head catalogues.
- The existing authored page was preserved: `VB_RightPanelMyCharacters -> ScrollBox_MyCharacters -> VB_MyCharactersList`, with `SB_CharacterCard01` as the Designer-visible compact-row sample and the original `SB_AddCharacter -> Btn_AddCharacter` instance. No Widget Blueprint tree was reconstructed, no widgets were renamed, no portrait area/UniformGrid/empty-state/status/delete panel was added, and no `.uasset` was edited.
- The runtime sample is explicitly `Collapsed`, receives no handler and is not counted. Runtime order verified in PIE was sample (Collapsed), existing Reset control, one compact row per record, then the unchanged authored `SB_AddCharacter` last. With two records the Add index was `4` of `5`; the same widget instance is temporarily removed/re-added with canonical `UVerticalBox` API while preserving its slot settings and GUID.

### Compact runtime row implementation

- A small native-only data widget, `UAvMyCharacterRowWidget`, is used; no new Content asset was created. Its Slate structure is `SBox(92px) -> SButton -> SBorder -> SHorizontalBox -> 66x66 silhouette frame + one-line name`. It matches the measured authored sample dimensions/colors: dark inactive background/light text, rounded 1px inactive frame, rounded 2px orange active frame and orange active name.
- Every valid `FAvCharacterRecord` produces exactly one tracked `RuntimeMyCharacterRows` entry, sorted by `SortOrder`, then `CreatedTimestamp`, then `CharacterId`. Each row stores only its stable `CharacterId`, widget/owner references and click route; it never stores a raw pointer into `CharacterRecords`.
- The shared project silhouette `/Game/Avariika/UI/icon/chel_1_.chel_1_` is used for every row. Portrait mode is `AuthoredSilhouettePlaceholder`; no head thumbnail, large portrait, selection circle/checkmark, transient capture, cache, Texture2D asset or per-character Content asset is created. Real character portraits remain Phase 2.
- Row rebuild removes only tracked runtime rows and never calls `ClearChildren()`. The authored sample, Add button and other pages are untouched. Selection refresh compares row `CharacterId` to `ActiveCharacterId` and only updates row style; it does not rebuild widgets, preview, catalogues or SaveGame.

### Create, switch, persistence and lifecycle evidence

- Existing roster backend/schema was reused without redesign: `FAvCharacterRecord`, SaveGame v5, backward-compatible in-memory legacy migration, `CreateCharacter()`, `SetActiveCharacter()`, canonical factory appearance and the legacy `SavedWorkerAppearance` mirror remain authoritative.
- `Btn_AddCharacter` is bound with `RemoveAll(this)` plus exactly one delegate. One automation click changed rows/records `1 -> 2`, generated unique ID `Character_96DE51374303A3B71995AF8DBD31017E`, selected the new canonical factory record and logged exactly `SaveGameCommits=1`. It did not open Random/modal. The row rebuild remained `2`, and roster-named `UAvHeadCatalogItemButton` instances were `0`.
- Switching to `Character_01`, then back to the factory record, logged exactly one SaveGame commit per changed selection. Selecting the already-active second ID returned no-op and logged `SaveGameCommits=0`. Row object paths were unchanged across selection (`rows_rebuilt=false`), proving selection refresh does not rebuild the roster.
- Existing FullBody camera routing is reused on create/switch; camera implementation, cursor anchor/reuse, yaw, category presets and input routing were not modified or re-tested as a camera suite.
- Clothing-page offsets were set to MyCharacters/Torso/Legs `17/29/41` and remained exactly `17/29/41` across active-character switching. Catalogues were not rebuilt by the switch.
- Three close/reopen cycles each had zero active root widgets after close, one after open and stable runtime row counts `2/2/2`, with identical ordered IDs. Duplicate runtime rows were `0`; one Add click created only one record, and changed/no-op selection logs show no duplicate delegates.
- A PIE restart cold-loaded the temporary two-record SaveGame as two rows with the same ordered IDs. Selecting the previously active second ID immediately produced the expected no-op/zero-commit result, confirming persistence of `ActiveCharacterId`.
- Appearance isolation, legacy migration, Reset, Random and Apply Preset continue through the already-established atomic `CommitActiveCharacterAppearance`/active-record backend path and were not changed by this UI correction. The temporary factory record and all test values were removed by explicit SaveGame restore. Exact visual multi-outfit acceptance remains with the user.

### Diagnostics, validation, build and final state

- F9 diagnostics now report record count, `runtime_my_character_rows_count`, selected row ID, authored sample runtime visibility, Add index, duplicate runtime row count, delegate invariant, last create/select operation and commit count, `portrait_mode=AuthoredSilhouettePlaceholder`, My Characters offset and camera preset. Real F9 input opened and closed the inspector (`Panel=Visible/Hidden`) with `AppearanceMutation=false`, `Autosave=false`.
- Cold Blueprint compile succeeded for both `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2` and `/Game/Avariika/UI/CharacterCustomization/WBP_HeadCatalogItemCard`; validation errors `0`. Known pre-existing empty lifecycle/unused authored-variable warnings remain. GUID errors `0`, duplicate GUID `0`, BindWidget/missing-widget errors `0`, reconstruction errors `0`, invalid-parent errors `0`; no Blueprint asset was changed.
- The first corrective UBT exposed only a local diagnostics parameter-name warning-as-error and failed before link. After that one-line source correction, the final full UE 5.8 UBT completed with exit code `0`, `Result: Succeeded`, total execution time `8.98 seconds`. No C++ changed after the successful build, so the final binary matches source.
- Task backup retained at `Saved/CodexBackups/MyCharactersCompactRowsCorrection_20260717/`:
  - `AvariikaCompany.sav`, original SHA-256 `478506A77F1086AB8F69D678E8076A494559C6A2424209C000D47C13E0114AD7`;
  - `AvariikaUserSettings.sav`, original SHA-256 `619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7`;
  - binary Root WBP backup, SHA-256 `A047D6ED813363F706E507A32F2C2ECFE2F43BD23572D3E671AE7C9E1EDF63B2`.
- The temporary test company SaveGame reached SHA `B3845B5B05A3806D86CF7D94592137A10A3BAAAF1C442E4EC48831C019E15C8F`; final explicit restore returned company/settings byte-for-byte to the original SHAs above. Final state: PIE false, dirty Content `0`, dirty maps `0`, Editor responsive, no new Content assets/temporary actors, no commit, unrelated worktree preserved.
- Delete UI, rename, duplicate, active-status TextBlock, real portraits/capture/cache and permanent portrait assets remain Phase 2. The user performs visual and functional acceptance, especially exact row padding/size match, orange active highlight, Add placement, varied appearances and persistence; no visual PASS is claimed.

## Cursor-centric preview zoom with preset-base return - 2026-07-17

### Continuation after disconnect and preserved implementation

- This task was resumed in `C:\unrealEngine\avariika_UE58_sandbox` after a network disconnect, without restarting or reimplementing the camera work. Existing unrelated worktree changes were preserved; no commit was created.
- The disconnected session had already extended the accepted direct `USceneCaptureComponent2D` preview architecture with cursor screen/normalized positions, a deprojected capture ray, visible managed-SkeletalMesh tracing, current-focus-plane fallback, expanded character-bounds clamping, preset base/current/target focus and distance, preset-specific min distance, `ZoomAlpha`, manual-zoom state, preset-base return and expanded F9 diagnostics. No spring arm, screen-space auto-fit or camera-architecture rewrite was introduced.
- The first runtime test found anchor drift across a stationary wheel series. The exact cause was fallback anchor recomputation against a plane through the already-shifted `CurrentCameraFocus`; every later wheel event therefore moved the plane and selected a new anchor even though the cursor had not moved.
- The correction reuses the first saved `LastCursorWorldAnchor` while manual zoom is active, `LastAnchorSource` is valid and cursor movement is at most 4 pixels (`DistSquared <= 16`). Movement beyond 4 pixels starts a new anchor series. Category transitions clear manual zoom and the previous anchor.
- The disconnected session's second and final full UE 5.8 UBT after that correction completed with exit code `0`, `Result: Succeeded`, total execution time `5.99 seconds`. No C++ changed after it, including in this continuation. `Binaries/Win64/UnrealEditor-Avaryo.dll` is dated `2026-07-17 17:35:24`, after the final source edit at `17:35:09`; no additional UBT was needed.

### Cold Blueprint and integrity validation

- Cold MCP validation and compilation after Editor restart:
  - `/Game/Avariika/UI/CharacterCustomization/WBP_HeadCatalogItemCard`: validation errors `0`, Compile Success;
  - `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2`: validation errors `0`, Compile Success.
- Known orphan lifecycle-node/unused-variable warnings remain unchanged. GUID errors `0`, duplicate GUID `0`, BindWidget errors `0`, reconstruction errors `0`, invalid-parent errors `0`. The asset tree and Widget Blueprints were not modified.

### Repeated cursor-anchor runtime evidence

- PIE was opened through the real gameplay path: PlayerController `AvCustomize` -> HUD `SetCustomize(open)` -> `WBP_CharacterCustomizationRoot_V2` creation.
- Hands base settled exactly at distance `235`, focus `[0,0,108]`. Four stationary OS wheel-up notches over the left green glove produced distances `203 -> 171 -> 139 -> 107`, `ZoomAlpha 0.2286 -> 0.4571 -> 0.6857 -> 0.9143`, and one bit-for-bit stable world anchor `[2.291,25.845,-4914.311]`. `TargetFocus` moved monotonically toward that one anchor. Evidence dumps: `appearance_inspector_20260717_174804_627.json`, `_174823_722.json`, `_174826_228.json`, `_174828_741.json`.
- Strong Hands zoom centered the left glove while the head and opposite hand naturally left the viewport. Technical capture: `Saved/CodexDiagnostics/CursorCentricPreviewZoom_20260717/hands_left_glove_strong_zoom.png`. `AutoFitActive=false` and `ScreenSpaceCharacterClampActive=false` throughout.
- Moving the cursor 495 pixels immediately recalculated the anchor from `[2.291,25.845,-4914.311]` to `[1.280,12.174,-4922.557]` (`appearance_inspector_20260717_174919_562.json`). A separate rotated-base series directly over the visible right glove held the new anchor `[-0.538,-10.223,-4919.973]` across three notches (`_180302_564`, `_180304_473`, `_180306_364`).
- Face used one stable anchor `[0.250,3.896,-4835.795]` over four stationary notches: distance `125 -> 113.75 -> 102.5 -> 91.25 -> 80`, without entering the head/mask. The face became larger and shoulders naturally cropped. Evidence: `_175035_826`, `_175038_037`, `_175040_212`, `_175042_392`; technical capture `face_strong_zoom_technical.png`.
- LowerBody kept the `LowerBody` preset, moved focus Z downward from base `64` to `28.747`, and zoomed the boots to distance `172`; the head left the viewport naturally. `AutoFitActive=false`, screen-space clamp false. Evidence: `_175246_640`; capture `lower_boot_zoom.png`.
- Runtime collision did not yield a managed-mesh trace, so the exercised source was `FocusPlaneFallback`. This is accepted: anchors matched the cursor regions, stayed within combined visible bounds, did not drift under stationary input and did not escape to the background. No artificial collision system was added merely to obtain `MeshHit`.

### Base return, transitions, routing and visible components

- Full wheel-down after Hands, Face and LowerBody returned exact preset base values: Hands `235/[0,0,108]`, Face `125/[0,0,158]`, LowerBody `300/[0,0,64]`; `ZoomAlpha=0`, `ManualZoomActive=false`. An additional wheel-down did not move the camera. `TargetDistance` never exceeded `BaseDistance`; the camera cannot move farther than the active category base.
- Real authored-button chain Hands -> Head -> LowerBody -> FullOutfits showed a true transition phase (`CategoryTransitionActive=true`, old current values interpolating toward the new base), then exact settled base. Each transition cleared manual zoom and `AnchorSource` to `None`. Yaw remained `42.0` degrees through all transitions. New cursor-centric zoom worked again after each transition.
- Input routing evidence:
  - right `ScrollBox_LegsItems` moved `0 -> 288` while camera stayed at LowerBody base;
  - wheel over left navigation changed neither camera nor the ScrollBox;
  - wheel over central preview changed camera `300 -> 268` while ScrollBox remained `288`;
  - `ResetToBase` modal blocked real wheel and a 150-pixel drag: distance/focus/yaw stayed unchanged and drag ended false;
  - Cancel closed the modal with the prior manual camera state intact and no reset.
- Real F9 key input opened and closed the development inspector (`Panel=Visible` then `Panel=Hidden`). Diagnostics include preset/base/current/target/min distance, zoom alpha, base/current/target focus, cursor screen/world anchor, source, manual/category-transition flags, combined bounds and pointer-over-preview.
- The active FullOutfit was present and visible in combined bounds; underlying Torso/Legs/Feet were all `visible=false`, while Gloves remained visible. Zoom over the visible FullOutfit worked. Selecting another FullOutfit card preserved `FullBody`, manual distance `413`, focus and yaw `42` rather than resetting the camera. Evidence: `_180033_549` and `_180055_485`.
- Green Gloves remained visible and both Hands zoom series worked. Package body-coverage self-test returned `Result=PASS`; gloves retain hands-only coverage, so they do not drive the coarse arms section off and the shoulder fix remains in force. No body-coverage code changed.
- Only one roster record exists (`Character_01`), so active-character switching was not destructively fabricated. The existing transition code clears stale anchor and returns to FullBody base, but this exact multi-record runtime case remains for user testing.

### SaveGame restore, logs and final state

- Original and explicitly restored SHA-256 values:
  - `Saved/SaveGames/AvariikaCompany.sav`: `478506A77F1086AB8F69D678E8076A494559C6A2424209C000D47C13E0114AD7`;
  - `Saved/SaveGames/AvariikaUserSettings.sav`: `619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7`.
- Backup retained at `Saved/CodexBackups/CursorCentricPreviewZoom_20260717/`; binary originals are under its `SaveGames/` subdirectory and the source baseline/manifest remain in the backup. The test FullOutfit selection temporarily changed company save to SHA `9BDBDB5B...64D0`; explicit restore returned it byte-for-byte to the original SHA above. Settings never changed.
- Final Editor state: PIE=false, dirty Content `0`, dirty Maps `0`, Editor responsive. No temporary actors survived PIE, no new Content assets were created and no Widget Blueprint/map/asset was saved.
- Final log review found no task-related Fatal, Ensure, Blueprint, GUID, BindWidget, reconstruction or invalid-parent failure. Engine startup emits its built-in UnifiedError/AutomationTest sample errors; three automation-probe errors were also logged while establishing the PIE control path (`Editor is currently in a play mode` twice and one unavailable Python attribute). They are not gameplay/asset failures and did not recur during the valid runtime tests.
- `git diff --check` still reports only the pre-existing unrelated `Config/DefaultGame.ini:121: new blank line at EOF`. Unrelated worktree state was not cleaned.
- No visual PASS is claimed. User acceptance should manually judge zoom speed/feel, exact framings, cursor placement on different characters/outfits and the unexecuted multi-character switch. Functional/numerical runtime evidence is complete, but final visual and product acceptance belongs to the user.

## My Characters roster and interactive preview camera - 2026-07-17

### Outcome and authored UI boundary

- Work was restricted to `C:\unrealEngine\avariika_UE58_sandbox`. Existing unrelated dirty source/config/map/asset work was preserved; no commit was created.
- Canonical project and customization handoff were read first. Pre-change checks were run: `git status --short`, `git diff --stat`, and `git diff --check`. The only `diff --check` issue remained the pre-existing `Config/DefaultGame.ini:121: new blank line at EOF`.
- `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2` was inspected through the real Widget Tree. `VB_RightPanelMyCharacters` is authored as:
  - `Txt_MyCharactersTitle`
  - `SB_MyCharactersTitleDivider -> Border_MyCharactersTitleDivider`
  - `ScrollBox_MyCharacters -> VB_MyCharactersList`
  - sample `SB_CharacterCard01 -> Overlay_CharacterCard01` with `Btn_SelectCharacter01`, base/selected frames, `Img_CharacterPortrait01`, `Btn_EditCharacterName01`, `Txt_CharacterName01`, and `ETB_CharacterName01`
  - `SB_AddCharacter -> Btn_AddCharacter` with the authored `+ ДОБАВИТЬ ПЕРСОНАЖА` presentation.
- The page does **not** contain a delete button or a separate active-character status/action area. No reusable character/worker roster card exists in project-owned UI; `WBP_HeadCatalogItemCard` is the only suitable temporary runtime portrait/name/selected card, but it cannot satisfy deletion by itself.
- The explicit authored-UI safety rule therefore blocked roster UI integration before any new roster `BindWidget`. The Root WBP was not modified, no runtime fallback layout was created, no raw `.uasset` edit/rename/new-object panel insertion was used, and no portrait pipeline was attached.
- Exact authored additions required to unblock the next pass:
  - `Txt_MyCharactersActiveStatus` (`UTextBlock`) under `VB_RightPanelMyCharacters`, before `ScrollBox_MyCharacters`;
  - `HB_MyCharactersActions` (`UHorizontalBox`) under `VB_RightPanelMyCharacters`, after `ScrollBox_MyCharacters`;
  - `Btn_DeleteActiveCharacter` (`UButton`) under `HB_MyCharactersActions`;
  - `Txt_DeleteActiveCharacterLabel` (`UTextBlock`) as the button content, authored text `УДАЛИТЬ`.
- Once those widgets exist, `WBP_HeadCatalogItemCard` can be used as the allowed Phase 1 temporary runtime roster card, while the page-level authored delete action targets the active card. `Btn_AddCharacter` already satisfies creation. The existing sample must remain authored and runtime rebuild must remove only tracked runtime cards.

### Existing roster and backward-compatible backend

- A roster architecture already existed before this task and was reused: `FAvCharacterRecord`, `UAvariikaSaveGame::CharacterRecords`, `ActiveCharacterId`, and `UCompanyLedgerSubsystem` active-record helpers. No parallel list was created.
- The existing serialized `FName CharacterId` type was retained to avoid breaking tagged-property loading. Newly created records receive a GUID-derived unique `FName` (`Character_<32 hex digits>`). Existing `Character_01` remains valid.
- Save schema is now v5. `FAvCharacterRecord` retains `DisplayName`, `Appearance`, `BasePresetId`, meaningful/origin/active flags and adds `SortOrder` plus `CreatedTimestamp` (Unix UTC seconds; zero means an older record).
- The actual legacy mirror in this project is `bHasSavedWorkerAppearance + SavedWorkerAppearance`. It remains synchronized from the active record in the same `Save()` operation. Gameplay readers in `AvaryoCharacter.cpp` continue using this compatibility API; no gameplay networking/equipment schema was changed.
- A legacy single-character save with no records is synthesized in memory as stable `Character_01`, preserving `SavedWorkerAppearance`, existing display data where available, and making it active. Migration no longer autosaves merely because the save was loaded; v5 metadata and other prepared migrations persist on the next explicit user change.
- Invalid/missing `ActiveCharacterId` resolves in memory to the existing active-flag record or first record without autosave. F9 retains `loaded_active_character_id_invalid` diagnostics. Duplicate IDs are diagnosed and the roster is not deleted.
- Backend Phase 1 operations are implemented but intentionally not wired to the blocked page:
  - `CreateCharacter()` creates canonical `MakeBaseMaleUnderwearAppearance()`, assigns the first free `РАБОЧИЙ 01`, `РАБОЧИЙ 02`, etc., activates it, mirrors legacy appearance and performs one SaveGame commit;
  - `SetActiveCharacter()` is a no-op for the current ID; otherwise it activates exactly one record, mirrors legacy appearance and performs one commit;
  - `DeleteCharacter()` refuses count <= 1, chooses next `SortOrder` or previous when deleting the active record, mirrors appearance and performs one commit.
- Existing `SetActiveCharacterAppearance()` already updates the active record, normalizes HeadType skin presentation, mirrors `SavedWorkerAppearance`, and performs one commit. Reset, Random and Apply Preset therefore continue to target the active record rather than creating records.
- Create/select/delete UI, delete modal mode, runtime roster cards and portrait capture/cache were not connected because the required authored delete/status widgets are missing. F9 reports runtime roster cards `0`, portrait queue/cache `0`, and the blocker string instead of pretending that integration exists.

### Interactive preview camera

- The existing preview uses one transient `AAvCustomizePreviewActor` and direct `USceneCaptureComponent2D`; there is no spring arm. The accepted direct-capture architecture was extended rather than replaced.
- Added canonical `EAvCustomizationCameraPreset`: `FullBody`, `UpperBody`, `Head`, `Face`, `Hands`, `LowerBody`.
- Preset mapping is centralized in `TransitionPreviewCameraForCategory()`:
  - My Characters, Presets, Full Outfits -> FullBody (`distance 445`, `focus Z 88`)
  - Head -> Head (`170`, `151`)
  - Face Protection -> Face (`125`, `158`)
  - Hands Accessories -> Hands (`235`, `108`)
  - Upper Equipment -> UpperBody (`260`, `119`)
  - Lower/Hip -> LowerBody (`300`, `64`).
- Category transitions use one conditional actor Tick with `FInterpTo/VInterpTo`, interpolation speed `9` (approximately the requested 0.3-0.5 second settling range). Tick disables itself after settling; there is no timer chain or competing transition.
- The existing accepted normal preview FOV remains `25`. Zoom changes target camera distance, not FOV, by `32` units per standard wheel notch and clamps exactly to `115..520`.
- LMB drag is handled only when the pointer is inside the cached geometry of the existing authored `SB_PreviewArea`. Mouse X delta rotates the preview body at `0.35 degrees/pixel`, yaw only. Root location and gameplay actor are untouched; yaw is transient and preserved across category changes.
- Wheel returns Handled only when over `SB_PreviewArea` and a zoom change was accepted. Right panel/ScrollBox, left navigation, footer and modal input remain unhandled by the preview path. Mouse-up, capture loss, Escape, modal opening and widget destruction clear drag state.
- Camera preset transition occurs only when the main category changes. Item/card selection, selected-state refresh, autosave and current-selection refresh do not call the category transition path.

### F9 diagnostics and executed runtime proof

- F9 remains functional and now additionally reports: roster count, active ID/name, exact active-record existence, legacy mirror sync, invalid loaded ID, duplicate count, selected/runtime roster status, portrait queue/cache/stale state, active appearance hash, camera preset/current/target distance/focus Z, preview yaw, transition/drag/pointer state and modal mode.
- Cold PIE loaded the restored user save without changing the file. F9 reported: MyCharacters `1`, active `Character_01`, display name `Мой персонаж`, legacy mirror synced `true`, active record exists `true`, invalid ID `false`, duplicates `0`, appearance hash `09980E8E`, FullBody `445/Z88`, modal `None`.
- Real authored category-button delegate smoke produced settled F9 records:
  - Head `170/Z151`
  - Face `125/Z158`
  - Hands `235/Z108`
  - UpperBody `260/Z119`
  - LowerBody `300/Z64`
  - FullOutfits/Presets/MyCharacters FullBody `445/Z88`.
- A real OS pointer drag over the center preview moved 150 px and F9 measured yaw `52.5` degrees with `drag=false` after release. One wheel notch changed `445 -> 413`. Repeated wheel input proved clamps exactly `115` and `520`.
- The same drag/wheel sequence over the right authored area left yaw `52.5` and distance `413` unchanged; F9 reported `pointer_over_preview=false`. Category change `Head -> MyCharacters` preserved yaw `52.5` while changing framing `170/Z151 -> 445/Z88`.
- FullOutfit regression diagnostics during the smoke remained catalog `31`, runtime cards `31`, thumbnails loaded `62`, Type01/Type02 missing `0/0`. No task-window Fatal, Ensure, `: Error:`, BindWidget, duplicate-GUID or reconstruction error was emitted.
- Reset, Random, Preset Preview, clothing selection compatibility, portrait rendering, create/select/delete UI and persistence across roster operations were not destructively re-executed in this blocked pass. Existing paths compiled and were preserved; user validation remains required.

### Build, Blueprint, saves and final state

- Full UE 5.8 UBT after the final C++ change: exit code `0`, `Result: Succeeded`. C++ was not changed after that build.
- Cold editor Blueprint validation/compile:
  - `WBP_HeadCatalogItemCard`: validation errors `0`, Compile Success;
  - `WBP_CharacterCustomizationRoot_V2`: validation errors `0`, Compile Success.
- Existing baseline orphan lifecycle nodes and unused variables remain warnings; they were not mass-refactored. GUID errors `0`, duplicate GUID `0`, BindWidget errors `0`, invalid-parent errors `0`, reconstruction errors `0`.
- Original save SHA-256 before work and after explicit restore:
  - `AvariikaCompany.sav`: `478506A77F1086AB8F69D678E8076A494559C6A2424209C000D47C13E0114AD7`
  - `AvariikaUserSettings.sav`: `619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7`.
- Backup retained outside Content: `Saved/CodexBackups/MyCharactersAndPreviewCamera_20260717/` with both original saves. No Root WBP or other project-owned asset changed, so no binary asset backup was required. No runtime portrait files/assets were created.
- Final Editor state: PIE=false, dirty Content=0, dirty Maps=0, Editor responsive. Root WBP authored tree remains unchanged.
- Visual and functional acceptance belongs to the user. No visual PASS is claimed. The user must add the four exact authored delete/status widgets, then manually accept the completed roster UI, portraits/card sizing, delete modal, multiple-character persistence, drag feel, zoom speed and all camera framings.

## Full Outfit thumbnail binding and import repair - 2026-07-17

### Scope and proven root cause

- The user screenshot showed the selected blue protective outfit correctly rendered in the large preview while most cards on the right showed gray circular placeholders. Runtime outfit meshes, exact materials, suppression, body coverage, selection, Current Selection, and the 30 package-native DemoIndex recipes were already working and were not changed.
- The current catalog remained exactly `None + DemoIndex 0..29` (31 records). Its canonical Type01/Type02 strings were already valid Unreal object paths with the object-name suffix: `/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/FullOutfits/HeadType01|HeadType02/T_UI_FullOutfit_<StableId>_Type01|Type02.T_UI_FullOutfit_<StableId>_Type01|Type02`.
- The defect was missing imported assets, not bad catalog strings or a card/brush race. The folder contained 24 old Texture2D packages, but only 12 matched the new catalog (`None` and `FullOutfit_WorkerTshirtPants_1..5`, both profiles). Exactly 50 canonical `LoadObject<UTexture2D>` calls returned null; the Editor log contained exactly 50 `Failed to find object Texture2D` warnings. Consequently 25 Type01 and 25 Type02 cards received null and used the empty/placeholder presentation.
- Twelve additional old `Overalls_PantsWorkerFull_*` textures belonged to the retired 12-card page. They had zero referencers, were not catalog paths, were backed up, removed from Content, and were not reintroduced. No source asset under `/Game/Modular_Workers` was changed.
- Before repair: expected exact paths 62, exact registered/loadable 12, missing 50, physical folder Texture2D 24, redirectors 0. Full per-record diagnostics including VisibleIndex, DemoIndex, StableId, both object paths, source presence, class/load/card/brush outcome, and placeholder reason are in `Saved/CodexDiagnostics/FullOutfitThumbnailRepair_20260717/thumbnail_chain_before.json`.

### Captures, scoped import, and manifest

- Source discovery found 22 usable captures for the current catalog: 10 prior catalog PNGs and 12 technical-control PNGs. Forty genuinely absent images were captured; nothing already correct was recaptured.
- The 40 new captures use the canonical runtime resolver and exact DemoIndex mesh recipe with no thumbnail-only companions or fallback modules. Capture profile remained 512x512, alpha 255, FinalColorLDR, exposure 0.95, key 21.2, fill 12000, soft key 72000, soft fill 25600, rim 14400, ambient 0.55, opaque dark blue-gray backdrop with `FRotator(90,0,0)`, FOV 34, distance 305, focus Z 88.
- Scoped import created only the 50 missing Texture2D assets: 40 new captures plus 10 existing technical controls. The 12 already correct Texture2D assets were reused without reimport. Catalog path corrections: 0; the catalog paths were already canonical and were deliberately preserved.
- Final source manifest: 62 PNGs total = 40 repair captures + 12 technical controls + 10 prior catalog captures. The two `None` profiles and all 60 real-outfit profiles are represented. Source pixel audit: 62/62 at 512x512, sampled alpha 255, bad dimensions 0, bad alpha 0.
- Final cold Asset Registry state: catalog 31, runtime cards 31, expected Texture2D 62, physical folder Texture2D 62, exact registered 62, exact loaded 62, missing 0, redirectors 0. Full paths and source mapping are in `thumbnail_chain_after.json`, `thumbnail_capture_manifest.json`, `thumbnail_import_manifest.json`, and `source_pixel_audit.json` under the task diagnostics directory.
- The import Python stage succeeded and saved before the project's unrelated global validation. Its commandlet exit was 1 because global validation reported the same 533 third-party/package errors and 642 warnings. The legacy cleanup commandlet also hit global validation/noise; its scoped registry operation reached 62/0, and the exact backed-up unreferenced files required literal filesystem finalization with Editor closed. Evidence is in `legacy_cleanup_manifest.json` and the two task logs under `Saved/Logs/`.

### Card assignment, HeadType, F9, and runtime

- Card flow remains synchronous and architecture-preserving: `BuildFullOutfitThumbnailRegistry()` loads exact `UTexture2D` objects, `ResolveFullOutfitItemThumbnail()` chooses Type01/Type02, `InitializeFullOutfitCatalogButton()` stores a strong transient `TObjectPtr`, and `ApplyPresentation()` assigns the same texture as the `FSlateBrush` resource object. Selected-state and availability refresh do not clear it.
- Only development diagnostics were added: F9 now reports expected/registered/loaded counts, Type01/Type02 missing counts, redirectors, placeholder cards, card/brush mismatches, active HeadType/StableId/DemoIndex, resolved object path, loaded Texture2D, failure reason, scroll offset, and end offset. No runtime appearance, catalog, recipe, SaveGame, enum, mesh, material, suppression, selection, or body-coverage logic changed.
- F9 was opened with the real F9 key. Type01 and Type02 dumps each report catalog 31, runtime cards 31, loaded 62, missing 0, placeholder 0, card/brush mismatch 0, and an exact active brush object in the correct HeadType folder. Evidence: `f9_type01.json`, `f9_type02.json`, and `f9_diagnostics.json`.
- Runtime lifecycle PASS: three opens produced 31/31/31 cards, duplicates 0, designer samples remained Collapsed, grid children 33 including the two authored samples, last card reachable, selected-state preserved, and scroll remained 2987 -> 2987 through selection and HeadType switch.
- HeadType01 and HeadType02 each resolved 31/31 valid thumbnails. All DemoIndex 0..29 were selected through the real cards; exact expected preview mesh was visible 30/30 and scroll was preserved 30/30. `None` cleared the active FullOutfit. Runtime/capture recipe parity remained the canonical WorkerAppearanceComponent path.
- Regression smoke PASS: Gloves shoulder fix, Reset, Random, Preset Preview, BodyCoverage self-test, and Random eligibility self-test. The restored old SaveGame cold-loaded with 31 cards and caused no autosave.

### Build, Blueprint, saves, and final state

- UE 5.8 full UBT: exit code 0, `Result: Succeeded`. `UnrealEditor-Avaryo.dll` is newer than every changed C++ source and no C++ changed after that build.
- `/Game/Avariika/UI/CharacterCustomization/WBP_HeadCatalogItemCard`: Compile Success. `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2`: Compile Success. Validation/GUID/duplicate-GUID/BindWidget/reconstruction/invalid-parent errors for the two target Blueprints: 0.
- The authored Widget Tree was not changed. `VB_RightPanelFullOutfits`, its ScrollBox/Grid, and exactly `DesignSample_FullOutfitNone` plus `DesignSample_FullOutfitItem` remain. No panel, switcher, empty state, runtime replacement UI, or new designer sample was added.
- Backup: `Saved/CodexBackups/FullOutfitThumbnailRepair_20260717/`. It contains both user saves, prechange C++ files, all 12 removed unreferenced legacy thumbnail assets, and `SHA256_MANIFEST.json`. The prior `FullOutfitsThirtyVariantsCorrection_20260717` backup was verified present and not overwritten.
- Restored appearance/company SHA-256: `7D03090A73314AA8B7AA8CDFA4BE88451E678AE45C9998B8345B75C6CD766FDB`. Restored settings SHA-256: `619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7`. Final PIE=false, dirty Content=0, dirty Maps=0, Editor responsive.
- Contact sheets show complete full-body figures without gray placeholders, checkerboards, black halves, or obvious clipping, but visual acceptance of every thumbnail remains with the user. No visual PASS is claimed.
- No commit was created because the worktree was already heavily dirty with unrelated modified/untracked user work before this pass; committing would have violated the rule against mixing unrelated changes.

## Full Outfits / Overalls / Gloves body-coverage corrective pass - 2026-07-17

### Proven causes

- Runtime proof was taken before editing with `Torso=None` and `Overalls_PantsWorkerFull_Bege`: F9 reported `ensemble_kind=Overalls`, lower-only ensemble coverage, `torso_stable_id=None`, and the preview contained `SKM_Quantum_Torso` but no shirt. The thumbnail path separately injected ripped `SKM_Tshirt_Tucked_White`, so thumbnail and runtime were using different compositions.
- All six affected early records are the audited bib family `Pants_Worker_Full`: `Bege`, `Blue`, `Gray`, `Green`, `LightGrn`, and `Orange`. They remain TechnicalKind=Overalls; the five later `SKM_Worker_Tshirt_Pants_1..5` records remain true FullOutfit.
- Gloves records correctly had body coverage `Hands`, but `ApplyResolvedBodyCoverage` treated that mask as a reason to hide the complete coarse FaceRig `Arms` material section. Runtime/F9 then showed `hidden_sections=LOD0:Section1:Material1` plus derived `SKM_Quantum_Hands`. A technical preview capture proved that this package module does not contain the shoulder caps, leaving circular holes at both shoulders.
- Visual inspection of all 24 prior FullOutfits source PNGs found two directly damaged captures: `Overalls_PantsWorkerFull_Gray_Type01` and `Overalls_PantsWorkerFull_Blue_Type02`. The remaining ten Overalls images were still semantically stale because they showed the thumbnail-only white tucked shirt.

### Corrective implementation

- `FAvFullOutfitCatalogItem` now exposes exact derived `FallbackTorsoMeshPath` and `FallbackTorsoCoverageMask` metadata. All six audited Overalls records use `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_Bege.SKM_TShirt_Bege` with `Torso|UpperArms` coverage.
- The canonical `WorkerAppearanceComponent::ApplyResolvedBodyCoverage` creates that fallback only when an allowed Overalls record is active and the saved Torso slot is None. It is a derived runtime component, not a SaveGame slot. An explicit Torso selection always wins. Removing Overalls returns Torso=None or the user's stored Torso unchanged.
- The fallback coverage participates in the existing section/body-module resolver. Overalls still suppress only Legs and Feet and preserve all underlying Torso/Legs/Feet values.
- Gloves-only `Hands` coverage no longer disables the coarse FaceRig Arms section. Torso/ensemble UpperArms or Forearms coverage can still hide it when clothing genuinely covers it. Thus Gloves+TorsoNone keeps shoulders/upper arms from the canonical body while the package glove mesh covers the hand/wrist region.
- Removed the thumbnail-only `SKM_Tshirt_Tucked_White` injection from `AAvCustomizePreviewActor`. Runtime, gameplay, preset preview, and thumbnail capture now converge on the same resolver and exact fallback metadata.
- F9 BodyCoverage JSON now reports `fallback_torso_path` and `fallback_torso_coverage`; `visible underlying Torso` remains None when the fallback is derived.
- No enum or SaveGame schema change was made. `EWorkerSlot::Overalls` remains append-only value 16 and every older numeric value is unchanged.

### Targeted thumbnails

- Re-captured and reimported only the 12 Overalls textures (six exact records x Type01/Type02). `None` and all ten true FullOutfit textures were not touched.
- Capture profile remained FinalColorLDR, exposure 0.95, key 21.2, fill 12000, soft key 72000, soft fill 25600, rim 14400, ambient 0.55, full-body FOV 34/distance 305/focus Z 88, opaque dark backdrop.
- All 12 new PNGs are 512x512 with sampled alpha 255; visual inspection showed complete arms and the canonical beige T-shirt. Registry cold check: 24 FullOutfits Texture2D assets, missing=0, redirectors=0, bad dimensions=0.
- The headless import manifest reports `success=true`, 12 imported assets, dirty Content/Maps 0/0, and unchanged SaveGame SHA during import. The commandlet process itself returned 1 because the project's existing global validation stage reported 533 unrelated third-party/package errors; the scoped import completed and saved before that global validation result.

### Targeted verification and final state

- `RunBodyCoverageSelfTest`: PASS; catalog remains None=1, Overalls=6, true FullOutfit=5, total cards=12.
- Runtime matrix: all six Overalls with Torso=None showed the exact fallback and diagnostic; explicit Torso removed the fallback; all five true FullOutfit records remained visible without fallback.
- Gloves matrix checked Torso=None, normal Torso, Overalls, and true FullOutfit. Gloves+TorsoNone reports hidden sections=None and no derived `SKM_Quantum_Hands`; targeted capture shows intact shoulders and upper arms.
- Preview/gameplay parity: exact Overalls, fallback T-shirt, and Gloves paths were simultaneously present in both preview and gameplay worker components.
- FullOutfits runtime cards stayed 12 after three reopens; scroll delta after selections/reopens was 0.
- `WBP_HeadCatalogItemCard` Compile Success; `WBP_CharacterCustomizationRoot_V2` Compile Success. Validation errors=0; pre-existing orphan/unused warnings were not changed. No GUID, duplicate-GUID, BindWidget, or reconstruction errors were emitted.
- Full UE 5.8 UBT: exit code 0, `Result: Succeeded`; `UnrealEditor-Avaryo.dll` timestamp is newer than all changed source files. C++ was not changed after the successful UBT.
- User saves restored from `Saved/CodexBackups/FullOutfitsCoverageCorrection_20260717/`: appearance SHA-256 `A8A4926972C1C0B061686D0E70A94F5C40E1A7620D2AFE893EDBE25A84F6A678`; settings SHA-256 `619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7`.
- Restored appearance was cold-loaded by opening customization without any autosave; SHA remained unchanged. Final PIE=false, dirty Content=0, dirty Maps=0, Editor responsive.
- Evidence: `Saved/CodexDiagnostics/FullOutfitsCoverageCorrection_20260717/`; backups are retained. Visual and functional acceptance remains with the user; no visual PASS is claimed.

## Full Outfits and package-native Overalls integration - 2026-07-17

### Authored page and routing

- Preserved the user-authored page /Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2. Its FullOutfits subtree is VB_RightPanelFullOutfits -> title/divider/current-selection area/Overlay_FullOutfitItemsArea -> ScrollBox_FullOutfitItems -> UniformGrid_FullOutfitItems -> exactly DesignSample_FullOutfitNone and DesignSample_FullOutfitItem.
- Runtime/MCP proof: ScrollBox is a child of the authored Overlay; UniformGrid is direct ScrollBox content; both Overlay slots are Fill; viewport is constrained; Txt_FullOutfitEmptyState does not exist.
- The authored unnamed inner borders remain Border_HeadTitleDivider_5 and Border_UpperCurrentSelectionDivider_1. They were not renamed because no binding requires them.
- Btn_CategoryFullOutfits uses the existing WS_RightPanelPages and SetActiveWidget(VB_RightPanelFullOutfits). No second router and no hard-coded routing index.
- Real page order: Presets 0, My Characters 1, Head 2, Face Protection 3, Hands Accessories 4, Upper Equipment 5, Lower/Hip 6, Full Outfits 7. Runtime returned actual index 7.
- Default first tab remains МОИ ПЕРСОНАЖИ; RefreshMyCharactersPage was not added to NativeConstruct.

### Asset Registry and original demo

- Full /Game/Modular_Workers audit: 878 assets — 243 SkeletalMesh, 40 StaticMesh, 118 MaterialInstanceConstant, 23 Material, 345 Texture2D, 14 AnimSequence, 2 Blueprint, 2 WidgetBlueprint and 4 World assets plus supporting classes.
- Ensemble candidates: 36 total — 30 Overalls_Modules SkeletalMeshes plus 6 Pants_Worker_Full SkeletalMeshes. All use /Game/Modular_Workers/Mesh/Male/SK_Male_Quantum_Character_Skeleton.
- Exact paths, skeleton, bounds, material slots and embedded MaterialInstances are in Saved/CodexDiagnostics/LowerHipIntegration_20260716/lower_hip_asset_registry_audit.json. Task evidence is in Saved/CodexDiagnostics/FullOutfitsIntegration_20260717/.
- Original builder categories/arrays separately expose Clothes, Chest Module, Overalls, Pants and Left Hip Drops. Its normal Pants array contains ordinary pants/body-bottom and excludes every Pants_Worker_Full asset.
- The package character uses modular Clothes/Pants/body/arms/accessory SkeletalMeshComponents with LeaderPose. Overalls_Modules are combined wearable meshes; no evidence supports splitting their shirt/pants/boots into separate user items.
- All 30 Overalls_Modules meshes contain an upper Shirt/Tshirt material, lower Pants/Jumpsuit material and Boots/Protective Boots. They replace Torso, Legs and Feet and need no separate shirt or pants.
- All 6 Pants_Worker_Full meshes contain pants + boots + bib/straps but no complete Torso/Arms garment. Torso remains visible around the bib. They are independent Overalls, not Legs and not true FullOutfit.
- No one of the 36 candidates lacks integrated footwear; the requested no-integrated-footwear control is NotApplicable, not fabricated.

### Model, mutual exclusion and old saves

- Added append-only EWorkerSlot::Overalls=16 after HipAccessory=15. Existing values remain FullOutfit=12, Headphones=13, Watch=14, HipAccessory=15.
- Old FWorkerAppearance arrays have no slot 16 and resolve Overalls=None. No migration and no autosave-on-load.
- FAvFullOutfitCatalogItem stores StableId, TechnicalKind, TargetSlot, exact mesh/material mapping, Russian names, Type01/Type02 thumbnails, family/color/sort, BodyCoverageMask, SuppressedRenderSlots, None/enabled and DisabledReason.
- WorkerAppearanceComponent::ApplyResolvedBodyCoverage remains the single derived resolver for preview, gameplay, thumbnails, preset preview and active character. Coverage/suppression are not saved.
- FullOutfit selection clears only Overalls; Overalls selection clears only FullOutfit. Underlying Torso/Legs/Feet selections remain stored.
- Old/test conflicts are not mutated on load. FullOutfit has deterministic render precedence over Overalls and F9 reports the conflict.
- Unknown old ensemble paths remain loaded, use coverage None, show НЕДОСТУПНЫЙ ВАРИАНТ and do not clear the rest of appearance.

### Exact Overalls allowlist — 6

All use TechnicalKind=Overalls, TargetSlot=Overalls, coverage Pelvis|UpperLegs|LowerLegs|Feet, and suppress rendering of Legs and Feet while keeping Torso.

- Overalls_PantsWorkerFull_Bege — БЕЖЕВЫЙ РАБОЧИЙ КОМБИНЕЗОН
  - mesh /Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Bege.SKM_Pants_Worker_Full_Bege
  - materials MI_Pants_Worker_Bege + MI_Boots_Black
- Overalls_PantsWorkerFull_Blue — СИНИЙ РАБОЧИЙ КОМБИНЕЗОН
  - mesh /Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Blue.SKM_Pants_Worker_Full_Blue
  - materials MI_Pants_Worker_Blue + MI_Boots_Black
- Overalls_PantsWorkerFull_Gray — СЕРЫЙ РАБОЧИЙ КОМБИНЕЗОН
  - mesh /Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Gray.SKM_Pants_Worker_Full_Gray
  - materials MI_Pants_Worker_Gray + MI_Boots_Black
- Overalls_PantsWorkerFull_Green — ЗЕЛЁНЫЙ РАБОЧИЙ КОМБИНЕЗОН
  - mesh /Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Green.SKM_Pants_Worker_Full_Green
  - materials MI_Pants_Worker_Green + MI_Boots_Black
- Overalls_PantsWorkerFull_LightGreen — САЛАТОВЫЙ РАБОЧИЙ КОМБИНЕЗОН
  - mesh /Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_LightGrn.SKM_Pants_Worker_Full_LightGrn
  - materials MI_Pants_Worker_LightGreen + MI_Boots_Black
- Overalls_PantsWorkerFull_Orange — ОРАНЖЕВЫЙ РАБОЧИЙ КОМБИНЕЗОН
  - mesh /Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Orange.SKM_Pants_Worker_Full_Orange
  - materials MI_Pants_Worker_Orange + MI_Boots_Bege

Exact material roots are /Game/Modular_Workers/Materials/Pants_Modules/Pants_Worker/ and /Game/Modular_Workers/Materials/Pants_Modules/Boots/, with the asset name repeated after the object-path dot.

### Exact true FullOutfit allowlist — 5

All use TechnicalKind=FullOutfit, TargetSlot=FullOutfit, coverage Torso|Pelvis|UpperArms|UpperLegs|LowerLegs|Feet, and suppress Torso, Legs and Feet. Vest, Gloves, Hip, Watch and head/face equipment are not blanket-suppressed because these exact meshes do not embed them.

- FullOutfit_WorkerTshirtPants_3 — БЕЖЕВЫЙ РАБОЧИЙ КОМПЛЕКТ
  - mesh /Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_3.SKM_Worker_Tshirt_Pants_3
  - materials MI_Tshirt_Tucked_Blue + MI_Pants_Worker_Bege + MI_Boots_Bege
- FullOutfit_WorkerTshirtPants_4 — СИНИЙ СИГНАЛЬНЫЙ КОМПЛЕКТ
  - mesh /Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_4.SKM_Worker_Tshirt_Pants_4
  - materials MI_Tshirt_Tucked_Red + MI_Pants_Worker_Blue + MI_Boots_Black
- FullOutfit_WorkerTshirtPants_2 — СЕРЫЙ РАБОЧИЙ КОМПЛЕКТ
  - mesh /Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_2.SKM_Worker_Tshirt_Pants_2
  - materials MI_Tshirt_Tucked_White + MI_Pants_Worker_Gray + MI_Boots_Black
- FullOutfit_WorkerTshirtPants_1 — ЗЕЛЁНЫЙ РАБОЧИЙ КОМПЛЕКТ 01
  - mesh /Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_1.SKM_Worker_Tshirt_Pants_1
  - materials MI_Tshirt_Tucked_Bege + MI_Pants_Worker_Green + MI_Boots_Black
- FullOutfit_WorkerTshirtPants_5 — ЗЕЛЁНЫЙ РАБОЧИЙ КОМПЛЕКТ 02
  - mesh /Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_5.SKM_Worker_Tshirt_Pants_5
  - materials MI_Tshirt_Tucked_Bege + MI_Pants_Worker_Green + MI_Boots_Bege

Exact Tshirt material root is /Game/Modular_Workers/Materials/Clothes_Modules/Tshirt_Tucked/; pants/boots use the roots above.

### Default deny

- The remaining 25 Overalls_Modules meshes are excluded:
  - Authored damage: all Overalls_Shirt_Boots/SKM_Shirt_Jumpsuit_Boots_1..5, Overalls_Worker_Tshirt/SKM_Worker_Tshirt_Jumpsuit_2, and Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_6..7.
  - Layer conflict: Overalls_Shirt_Pants_Worker/SKM_Overalls_Shirt_Pants_Worker_1..3.
  - Unverified/default deny: all Overalls_Tshirt_Boots/SKM_Overalls_Tshirt_Boots_1..5, all Overalls_Worker_Shirt/SKM_Worker_Shirt_Jumpsuit_1..5, and Overalls_Worker_Tshirt/SKM_Worker_Tshirt_Jumpsuit_1,3,4,5.
- Ordinary Jeans/Pants_Worker remain only Legs; standalone Feet/Boots remain only Feet.
- All 50 exact Presets SkeletalMeshes SKM_Worker_Male_1..25 and SKM_Worker_Male_Body_1..25 are denied as merged demo characters, not modular wearables.
- No source asset under /Game/Modular_Workers was modified.

### UI, cards, scroll and thumbnails

- Deterministic catalog: None; 6 Overalls sorted Beige/Blue/Gray/Green/LightGreen/Orange; 5 FullOutfits sorted Beige/Blue/Gray/Green/Green. Total 12 cards, 11 real.
- НЕТ clears only both ensemble slots, commits/autosaves once, restores underlying modular rendering, and is a no-op/no-autosave when already active.
- Mandatory bindings exist only for authored page/current texts/ScrollBox/UniformGrid. No Empty State, filters, internal switcher or runtime replacement layout.
- RuntimeFullOutfitCards is separately tracked. Only tracked cards are removed; ClearChildren is never used; designer samples stay authored, visible in Designer and Collapsed at runtime without handlers.
- Placement is row=VisibleIndex/2, column=VisibleIndex%2. Three opens produced 12/12/12 runtime cards; grid children were 14 including samples.
- Current Selection and card titles are one-line, no-wrap, ellipsis, clipped and full-name tooltip; the value slot is Fill with min width 0.
- ScrollBox: Vertical, WhenScrollingPossible, animated, multiplier 3, visible 5x5 scrollbar. UE 5.8 standard 32 px wheel amount gives about 96 px/notch.
- Selection and refresh do not scroll to start. Final runtime bottom offset stayed 567 -> 567, end 567. ScrollToStart is initial-build only.
- 24 new Texture2D assets exist under /Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/FullOutfits/HeadType01|HeadType02: 12 records x 2 profiles. Registry: 24 exact, missing 0, duplicates 0, redirectors 0, validation issues 0.
- Pattern: T_UI_FullOutfit_<StableId>_Type01|Type02, including None.
- Capture: 512x512, alpha 255, FinalColorLDR, exposure .95, key 21.2, fill 12000, soft key 72000, soft fill 25600, rim 14400, ambient .55, opaque dark blue-gray backdrop, FRotator(90,0,0), full-body camera FOV 34/distance 305/focus Z 88.
- Eight control captures preceded the catalog batch: None, Pants_Worker_Full_Bege, true Overalls_Modules candidate and integrated-footwear outfit for both head profiles. No visual PASS is claimed.

### Verification and lifecycle

- Body coverage self-test PASS: Torso 35 real, Legs 11, Hip 3, Overalls 6, FullOutfit 5, Save schema append-only Overalls16.
- Core runtime PASS: page 7 active, 12 unique cards, samples Collapsed, Empty State count 0, stable scroll, correct Current Selection and repeated-None SHA unchanged.
- Overalls runtime proof: exact Torso remained visible; exact underlying Legs remained stored but invisible. FullOutfit proof: Torso and Legs remained stored but invisible. None made the same exact meshes visible again.
- F9 proof: active kind and StableIds are readable; Overalls reported stable Overalls_PantsWorkerFull_Bege, coverage Pelvis|UpperLegs|LowerLegs|Feet, suppressed Feet,Legs, record exists true, parity WorkerAppearanceComponent::RebuildVisuals.
- Preview/gameplay parity PASS for true FullOutfit and suppressed underlying Torso/Legs.
- Reset PASS through existing modal; active ensemble cleared and factory modular state returned.
- Random PASS; established separate modular scheme used and both ensemble layers remained None.
- Preset PASS; paging did not change SaveGame SHA, Apply used existing one-commit/autosave path.
- Final full UE 5.8 UBT after fixing a runtime-discovered resolver temporary-array lifetime bug: exit 0, Result: Succeeded. No C++ changed after this final UBT.
- Cold Blueprints: WBP_HeadCatalogItemCard Compile Success/errors 0 with 3 existing orphan warnings; WBP_CharacterCustomizationRoot_V2 Compile Success/errors 0 with 17 existing orphan/unused warnings.
- GUID errors 0, duplicate GUID 0, BindWidget errors 0, reconstruction errors 0, invalid-parent errors 0.
- Headless texture import manifest succeeded with 24/24 and dirty 0. The process later returned 1 only because global shutdown validation surfaced 533 unrelated pre-existing third-party pack Blueprint errors; none references task assets. Subsequent cold validation issues for task assets: 0.
- Final PIE=false, dirty Content=0, dirty Maps=0. Visual and functional acceptance is the user’s; no visual PASS.

### Save restore and backups

- Restored appearance/company SHA-256: 9A8B1A816526A1F6EFFD60A622BE79FB25542CBB99FD66FB9EE7B4B20A05A4F4.
- Restored user-settings SHA-256: 619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7.
- Cold load did not autosave or change either restored SHA.
- Retained Saved/CodexBackups/FullOutfitsIntegration_20260717/:
  - AvariikaCompany.prechange.sav
  - AvariikaUserSettings.prechange.sav
  - WBP_CharacterCustomizationRoot_V2.prechange.uasset, SHA 89618CDC3B2EF152AE73CADCDF1ED49CF94179819C87083CCC0D21DECC45E56A
  - WBP_CharacterCustomizationRoot_V2.authored-autosave-preintegration.uasset, SHA 6F67CC14E426E0A9DD3121E88DB266DCA2D3F31F36CA00BECEC8F9D261A9D3F5
  - WBP_CharacterCustomizationRoot_V2.authored-saved-preintegration.uasset, SHA A047D6ED813363F706E507A32F2C2ECFE2F43BD23572D3E671AE7C9E1EDF63B2
- No duplicate Blueprint was created.

### Manual acceptance remaining

- User should inspect every record, Type01/Type02, T-shirt/rolled shirt/jacket with Overalls, FullOutfit with Headgear/Gloves/Feet/Hip, clipping/body penetration, shadows, sockets/LeaderPose/animation, wheel feel, persistence and the last card.
- Jacket + bib Overalls and the full accessory matrix have no proven catalog-level hard conflict; nothing was silently removed or blanket-disabled. Physical compatibility remains visually unproven.
- Technical controls prove identity, framing and runtime state only, not final artistic acceptance.

## Package-native body occlusion for modular clothing - 2026-07-17

### Cause and package audit

- The screenshot defect was not a clothing-material failure: the canonical FaceRig Body continued rendering its complete skin/underwear and arms below every Torso/Legs mesh. That caused skin to intersect jeans waist/pockets/legs and jacket closures/chest/shoulders/sleeves.
- Active factory Body remains `/Game/Modular_Workers/Mesh/Male/Body/Quantum_FaceRig/SKM_Qunatum_FaceRig_Body_Hight.SKM_Qunatum_FaceRig_Body_Hight`; Head remains `/Game/Modular_Workers/Mesh/Male/Body/Quantum_FaceRig/SKM_Quantum_FaceRig.SKM_Quantum_FaceRig`.
- The Body has one LOD and three LOD0 render sections/material indices:
  - section/material 0: `M_Quantum_Head1`, head/neck skin;
  - section/material 1: `M_Quantum_Arms1`, arms/hands skin;
  - section/material 2: `M_Quantum_Body1`, torso/pelvis/legs/feet skin plus underwear. Underwear is not an independent section.
- The original package demo `/Game/Modular_Workers/Demo/Blueprint/CBP_Male_Quantum_Character` uses separate `Arms`, `Clothes`, `Pants` and `Gloves` SkeletalMeshComponents with `SetLeaderPoseComponent`. Its Builder arrays prove the package-native None/replacement table:
  - Clothes None body module: `/Game/Modular_Workers/Mesh/Male/Body/European/SKM_Quantum_Torso.SKM_Quantum_Torso`;
  - Pants None body module: `/Game/Modular_Workers/Mesh/Male/Body/European/SKM_Quantum_Body_Bottom.SKM_Quantum_Body_Bottom`;
  - exposed arms module: `/Game/Modular_Workers/Mesh/Male/Arms_Modules/Arms/SKM_Quantum_Hands.SKM_Quantum_Hands`;
  - exposed wrist/hands module: `/Game/Modular_Workers/Mesh/Male/Arms_Modules/Wrist/SKM_Quantum_Wrist.SKM_Quantum_Wrist`;
  - additional audited body assets: `SKM_Quantum_Body_Full`, `SKM_Quantum_Legs`, `SKM_Quantum_UnderPants`, `SKM_Quantum_Feet`, `SKM_Quantum_FullHands`, with Afro equivalents where authored.
- No package demo evidence was found for bone scaling, `HideBoneByName`, a global transparent Body material, or source-mesh editing. The demonstrated architecture is modular body replacement on the common Quantum skeleton.

### Implementation

- Added derived `EWorkerBodyZone` bitflags: `Torso`, `Pelvis`, `UpperArms`, `Forearms`, `Hands`, `UpperLegs`, `LowerLegs`, `Feet`.
- Coverage is item-level catalog metadata (`BodyCoverageMask`) on Torso, Lower/Hip and Hands/Accessories records. It is not inferred solely from `EWorkerSlot`.
- `UWorkerAppearanceComponent::ApplyResolvedBodyCoverage()` is the single resolver called by `RebuildVisuals()`. It resolves exact active catalog records, unions their masks, resets all section visibility on every Body LOD, hides only covered FaceRig Body material sections, and creates/removes the package-native derived body modules required for open regions.
- Coarse FaceRig sections and fine package modules are intentionally combined:
  - Torso selected + Legs None: hide Body skin/underwear section and use `SKM_Quantum_Body_Bottom`;
  - Torso None + Legs selected: hide Body skin/underwear section and use `SKM_Quantum_Torso`;
  - Torso + Legs: hide Body skin/underwear section and use neither lower/torso base module, so the two authored clothing meshes meet directly;
  - short/rolled sleeve: hide FaceRig arms section and restore exposed regions with `SKM_Quantum_Hands + SKM_Quantum_Wrist`;
  - long sleeve: hide FaceRig arms section and restore hands with `SKM_Quantum_Wrist`;
  - Gloves: cover Hands and omit the Wrist module; uncovered arms are still restored when needed.
- The FaceRig head/neck section is never hidden. Current pants include the full lower silhouette/foot region in the original demo architecture, so no separate Feet module is rendered with them.
- Existing and newly created managed components are temporarily render-suppressed during the synchronous rebuild; meshes, LeaderPose, sections and skin materials are applied before visibility is restored. There is no Tick, timer or delayed second pass.
- Unknown Torso/Legs/Feet/Gloves/FullOutfit paths use coverage `None`, add `UnknownCoverageRecord` diagnostics and do not mutate appearance or hide arbitrary zones.
- No source asset under `/Game/Modular_Workers` and no project-owned `.uasset` was modified.

### Exact coverage metadata

- Every retained Legs record below uses `Pelvis | UpperLegs | LowerLegs | Feet`:
  - `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans.SKM_Jeans`
  - `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans_Bege.SKM_Jeans_Bege`
  - `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans_Black.SKM_Jeans_Black`
  - `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans_Blue.SKM_Jeans_Blue`
  - `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans_Turn.SKM_Jeans_Turn`
  - `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Bege.SKM_Pants_Worker_Bege`
  - `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Blue.SKM_Pants_Worker_Blue`
  - `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Gray.SKM_Pants_Worker_Gray`
  - `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Green.SKM_Pants_Worker_Green`
  - `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_LightGreen.SKM_Pants_Worker_LightGreen`
  - `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Orange.SKM_Pants_Worker_Orange`
- Torso family coverage:
  - `TShirt`: `Torso | UpperArms`;
  - `WorkShirt` (`Shirt_RolledUp`): `Torso | UpperArms`;
  - `WorkJacket`, `M65`, `ProtectiveJacket` (`Hood_1`) and `HoodJacket` (`Hood_2`): `Torso | UpperArms | Forearms`.
- Current Gloves records use `Hands`; current Hip records use `None`.
- The six `Pants_Worker_Full_*` records remain excluded from Legs and remain future FullOutfit/Overalls audit records. No FullOutfit UI was added.
- Current catalog counts are unchanged: Torso 35 real/36 with None, Legs 11 real/12 with None, Hip 3 real/4 with None.

### Lifecycle, parity and diagnostics

- Coverage is derived from `FWorkerAppearance` in `RebuildVisuals()` and is never copied into `FWorkerAppearance`; SaveGame schema/version are unchanged.
- Gameplay character, customization preview, preset preview, Random, Reset, old-save load and thumbnail capture all use the same `UWorkerAppearanceComponent` rebuild/resolver. Thumbnail generation required no separate fix and no thumbnail batch was run.
- `Legs=None` removes derived lower masking, calls `ShowAllMaterialSections()` for every Body LOD and restores full factory legs/feet/underwear. `Torso=None` restores factory torso/arms, or uses package `SKM_Quantum_Torso` when Legs still cover the lower region.
- Reset therefore restores the complete factory Body. Random derives fresh coverage from its generated exact records. Preset Preview derives coverage only on its preview actor and does not change active coverage before Apply. Old saves require no migration/autosave; valid records resolve automatically and unknown records remain safe/diagnostic.
- `ApplyHeadTypeSkinProfile()` now includes package-derived body modules. HeadType01 applies Light materials; HeadType02 applies Dark/Afro skin materials to exposed torso/arms/wrist modules while clothing materials are untouched.
- F9 inspector status now includes active Body mesh, implementation (`None`, `HiddenSections`, `PackageBodyModules`, or combined), resolved/visible/hidden zones, Torso/Legs/Feet/Gloves StableIds and masks, unknown records, active module paths, hidden LOD section indices, skin profile and the canonical preview/gameplay parity path.

### Technical verification

- Cold runtime self-test: `Result=PASS TorsoReal=35 LegsReal=11 HipReal=3 ShortSleeve=Torso|UpperArms LongSleeve=Torso|UpperArms|Forearms Pants=Pelvis|UpperLegs|LowerLegs|Feet SaveSchema=Unchanged`.
- Targeted transient preview controls, without screenshots:
  - Factory None: head/neck, arms and skin/underwear sections shown; no derived module;
  - `SKM_Jeans`: body/underwear section hidden, arms/head shown, `SKM_Quantum_Torso` present;
  - `SKM_TShirt_Bege`: body and arms sections hidden, `Body_Bottom + Arms + Wrist` present;
  - `SKM_Jacket_Worker_Bege` Type02: body and arms sections hidden, `Body_Bottom + Wrist` present, open modules use Dark/Afro skin material.
- Short PIE Lower/Hip smoke: 12 Legs cards and 4 Hip cards; three reopen counts `[12,12,12]`; scroll delta after selection `0`; no runtime-card duplication; no `Pants_Worker_Full` or `/Overalls_Modules/` component path.
- That PIE load also exercised the combined `SKM_TShirt_Blue + SKM_Pants_Worker_Bege` state: the preview contained the two clothing meshes plus only open `Arms + Wrist`; neither `Body_Bottom` nor `BareTorso` was present, so no hidden base module overlapped the authored waist seam.
- F9 runtime panel was created exactly once, its diagnostic dump completed with `AppearanceMutation=false Autosave=false`, and the hotkey handlers compile through `NativeOnPreviewKeyDown/NativeOnKeyDown -> ToggleAppearanceInspector`.
- `WBP_HeadCatalogItemCard`: Compile Success, validation errors 0; the same three authored orphan-event warnings remain.
- `WBP_CharacterCustomizationRoot_V2`: Compile Success, validation errors 0; the same 17 unrelated orphan/unused-variable warnings remain.
- GUID errors 0, duplicate GUID 0, BindWidget errors 0, reconstruction warnings/errors 0.
- Full UE 5.8 UBT: exit code 0, `Result: Succeeded`.
- Test-start and restored appearance SaveGame SHA-256: `760F4FAB1EA625E1C7BC19BC32BEF69F0BEABF7947EC2BFFC66A82A05DA23AFC`.
- Test-start and restored user settings SHA-256: `619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7`.
- Backups:
  - `Saved/CodexBackups/BodyOcclusionForClothing_20260717/AvariikaCompany.prechange.sav`
  - `Saved/CodexBackups/BodyOcclusionForClothing_20260717/AvariikaUserSettings.prechange.sav`
- Final state: PIE=false, dirty Content=0, dirty Maps=0, Editor responsive. The target compile/runtime intervals contain no Fatal, Ensure, Blueprint compile, BindWidget, duplicate-GUID or reconstruction error. UE startup still emits unrelated built-in `UE::UnifiedErrorTest` condition messages; one failed Codex Python probe (`get_torso_catalog` is intentionally not Python-reflected) is also recorded before the successful self-test and does not affect runtime.
- Evidence:
  - `Saved/CodexDiagnostics/BodyOcclusionForClothing_20260717/baseline_and_asset_audit.json`
  - `Saved/CodexDiagnostics/BodyOcclusionForClothing_20260717/demo_logic_audit.json`
  - `Saved/CodexDiagnostics/BodyOcclusionForClothing_20260717/cold_runtime_selftest.json`
  - `Saved/CodexDiagnostics/BodyOcclusionForClothing_20260717/runtime_coverage_controls.json`
  - `Saved/CodexDiagnostics/BodyOcclusionForClothing_20260717/cold_compile_blueprints.json`
  - `Saved/CodexDiagnostics/LowerHipOverallsCorrection_20260717/runtime_verify.json`
- The user performs visual and functional acceptance. No visual PASS is claimed. Manual checks remain: every Jeans/Pants_Worker and all 35 Torso items, waist seam, shoulders/armpits/collars/sleeves, hands/feet, Type01/Type02, shadows and animated gameplay character.

## Lower/Hip overalls classification correction - 2026-07-17

### Outcome

- Corrected only the existing authored `НИЗ И НАБЕДРЕННОЕ` page catalog. The authored Widget Designer tree, both WBP assets, runtime card architecture, selection flow, scroll behavior, autosave path and other customization pages were not rebuilt or changed.
- Removed six `Pants_Worker_Full` records from the user-facing Legs catalog. Their source-pack folder is `Pants_Modules`, but Asset Registry bounds and the existing authored audit captures prove that their geometry reaches the chest (`top Z approximately 155`) and contains a bib/straps. Product classification is future `FullOutfit / Overalls`, not `Legs` or `Hip`.
- Added an exact-path internal deny classification for those six meshes. Manual Legs validation now returns `SelectedLegsAssetClassifiedAsFutureFullOutfit` if one of those paths is presented as a new Legs selection.
- No FullOutfit UI was implemented. Existing thumbnails were reused for retained records; no thumbnail capture/import batch ran and archived thumbnails for removed records were not destructively deleted.

### Exact assets removed from Lower/Hip and reclassified as future FullOutfit / Overalls

- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Bege.SKM_Pants_Worker_Full_Bege`
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Blue.SKM_Pants_Worker_Full_Blue`
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Gray.SKM_Pants_Worker_Full_Gray`
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Green.SKM_Pants_Worker_Full_Green`
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_LightGrn.SKM_Pants_Worker_Full_LightGrn`
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Orange.SKM_Pants_Worker_Full_Orange`

### Final Legs allowlist

- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans.SKM_Jeans`
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans_Bege.SKM_Jeans_Bege`
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans_Black.SKM_Jeans_Black`
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans_Blue.SKM_Jeans_Blue`
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jeans/SKM_Jeans_Turn.SKM_Jeans_Turn`
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Bege.SKM_Pants_Worker_Bege`
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Blue.SKM_Pants_Worker_Blue`
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Gray.SKM_Pants_Worker_Gray`
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Green.SKM_Pants_Worker_Green`
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_LightGreen.SKM_Pants_Worker_LightGreen`
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Orange.SKM_Pants_Worker_Orange`

Final Legs count: 11 real items; 12 runtime cards including `НЕТ`.

### Final Hip allowlist

- `/Game/Modular_Workers/Mesh/Male/Hips_Modules/Bag_Hip/SKM_Bag_Hip_Bege.SKM_Bag_Hip_Bege`
- `/Game/Modular_Workers/Mesh/Male/Hips_Modules/Bag_Hip/SKM_Bag_Hip_Black.SKM_Bag_Hip_Black`
- `/Game/Modular_Workers/Mesh/Male/Hips_Modules/Bag_Hip/SKM_Bag_Hip_Green.SKM_Bag_Hip_Green`

Final Hip count: 3 real items; 4 runtime cards including `НЕТ`.

### SaveGame compatibility

- No SaveGame schema changed and no load-time migration/autosave was added.
- An old save that already contains one of the removed Legs paths still loads without mutation or silent corruption. The exact mesh remains preserved in the loaded appearance, while the Lower page uses the existing `НЕДОСТУПНЫЙ ВАРИАНТ` Current Selection fallback because the path is absent from the enabled catalog. Choosing `НЕТ` or any retained Legs card performs the normal validated one-slot replacement and autosave.
- Runtime selection smoke temporarily changed `AvariikaCompany.sav`; after PIE it was restored byte-for-byte to the task-start SHA-256 `AFB2BE9FD37F7F470301D1792C88DC80D6A26F6CB4CC8EAF8129716A7C4AA3EA`. `AvariikaUserSettings.sav` also byte-matches its backup at SHA-256 `619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7`.

### Verification and final state

- UE 5.8 UBT: exit code 0, `Result: Succeeded`.
- `WBP_HeadCatalogItemCard`: compile Success; validation 0 errors and the same 3 orphan-event warnings.
- `WBP_CharacterCustomizationRoot_V2`: compile Success; validation 0 errors and the same 17 unrelated orphan/unused warnings.
- Short PIE smoke: log reported `Total=16 Legs=12 Hip=4`; Legs runtime cards were exactly `000..011`, Hip exactly `012..015`; three repeated Lower page openings stayed `[12,12,12]`.
- Selection and scroll: Jeans and ordinary beige worker pants applied through the existing validated/autosave path; Legs scroll remained `1452 -> 1452`. Beige `Bag_Hip` applied through `HipAccessory`.
- Preview component paths after the smoke contained ordinary `SKM_Pants_Worker_Bege` and `SKM_Bag_Hip_Bege`; no `Pants_Worker_Full` or `/Overalls_Modules/` path was present. Runtime duplicates were absent.
- Final state: PIE=false, dirty Content=0, dirty Maps=0, Editor responsive. The task smoke interval contained no Fatal, Ensure, Blueprint compile, BindWidget, missing-widget, duplicate-GUID, invalid-parent or reconstruction error.
- Startup still emits unrelated engine/plugin self-test `LogAutomationTest: Error: Condition failed` noise and two `LogUtils: Error: The Editor is currently in a play mode` messages immediately after PIE startup; neither occurs in the Lower/Hip action interval and neither affected the successful runtime verification.
- Evidence:
  - `Saved/CodexDiagnostics/LowerHipOverallsCorrection_20260717/classification.json`
  - `Saved/CodexDiagnostics/LowerHipOverallsCorrection_20260717/runtime_verify.json`
  - `Saved/CodexBackups/LowerHipOverallsCorrection_20260717/`
- No visual PASS is claimed.

## Remove damaged Torso shirts and smooth Upper Equipment scrolling - 2026-07-16

### Outcome

- The user-reported damaged family was confirmed as all seven `Tshirt_Tucked` color meshes. They share the same authored 4,040-vertex / 7,156-triangle ripped topology and expose large holes at the chest, abdomen and lower hem; this is the intended source geometry/material-mask appearance, not a missing dependency or preview defect.
- Removed only the seven user-facing catalog records. Nothing under `/Game/Modular_Workers` was deleted or modified. The Torso catalog is now stable at 36 records: `НЕТ` plus 35 real items. Runtime cards were `[36, 36, 36]` after three repeated page openings, while the grid contains those 36 runtime cards plus the two authored Designer samples.
- Remaining real families are ordinary `Tshirt` (6), `Shirt_RolledUp` (8), `Jacket_Worker` (6), `Jacket_M65` (3), down-hood `Jacket_Worker_Hood_1` (6), and raised-hood `Jacket_Worker_Hood_2` (6).
- The authored Widget Tree, two-column layout, Current Selection ellipsis/tooltip, Raised Hood compatibility modal/suppression flow, Random/Reset behavior and SaveGame schema remain unchanged.

### Exact removed records

All seven records below were removed from `GetTorsoCatalog()` and therefore no longer create cards or new Current Selection names:

- StableId `SKM_Tshirt_Tucked_Bege`: `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Bege.SKM_Tshirt_Tucked_Bege`, canonical material `MI_Tshirt_Tucked_Bege`.
- StableId `SKM_Tshirt_Tucked_Black`: `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Black.SKM_Tshirt_Tucked_Black`, canonical material `MI_Tshirt_Tucked_Black`.
- StableId `SKM_Tshirt_Tucked_Blue`: `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Blue.SKM_Tshirt_Tucked_Blue`, canonical material `MI_Tshirt_Tucked_Blue`.
- StableId `SKM_Tshirt_Tucked_Green`: `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Green.SKM_Tshirt_Tucked_Green`, canonical material `MI_Tshirt_Tucked_Green`.
- StableId `SKM_Tshirt_Tucked_Grey`: `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Grey.SKM_Tshirt_Tucked_Grey`, canonical material spelling `MI_Tshirt_Tucked_Gray`.
- StableId `SKM_Tshirt_Tucked_Red`: `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Red.SKM_Tshirt_Tucked_Red`, canonical material `MI_Tshirt_Tucked_Red`.
- StableId `SKM_Tshirt_Tucked_White`: `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_White.SKM_Tshirt_Tucked_White`, canonical material `MI_Tshirt_Tucked_White`.

The exact Asset Registry/widget audit is saved at `Saved/CodexDiagnostics/UpperEquipmentRemoveDamagedShirtsSmoothScroll_20260716/tucked_asset_widget_audit.json`. Each mesh uses the Modular Workers male Quantum skeleton and has intact canonical BaseColor/Normal/ORM dependencies. Both 512x512 Type01/Type02 generated thumbnails remain archived; they were not deleted because destructive cleanup was unnecessary. There are no remaining catalog references to them and no missing thumbnail paths for the 36 retained records.

The same seven exact source paths remain in the existing Random visual-eligibility quarantine with condition `OpacityMaskDamage`; Random cannot select a removed StableId. An old SaveGame that already contains one still loads without mutation, migration or load-time autosave and uses the existing `НЕДОСТУПНЫЙ ВАРИАНТ` fallback until the user chooses a valid Torso item.

`DesignSample_TorsoItem` did not require a WBP edit: design-time `NativePreConstruct` resolves the first enabled real catalog record, now the intact beige ordinary `SKM_TShirt_Bege`. Both authored samples remain `Collapsed` at runtime and are not tracked as runtime cards.

### Smooth ScrollBox configuration and evidence

- Before correction: `WheelScrollMultiplier=48`, `AnimateWheelScrolling=false`, and one standard Windows wheel notch moved `ScrollOffset` by 1,536 px. The abrupt movement was caused by UE 5.8 multiplying the configured value by Slate's global 32 px scroll amount.
- A candidate multiplier of 16 was measured and rejected: it still moved 512 px per notch, more than a card row. The user-suggested numeric range 12-24 therefore does not satisfy the primary 60-120 px target with this UE 5.8 implementation.
- Final configuration: vertical orientation, `ConsumeMouseWheel=WhenScrollingPossible`, `WheelScrollMultiplier=3`, `AnimateWheelScrolling=true`, existing 5x5 scrollbar and overscroll behavior retained. A physical standard wheel notch measured exactly 96 px (`500 -> 596`), within the requested functional target and below one full card height.
- Native end offset was reached exactly (`ScrollOffset=EndOffset=3611` after viewport layout). The last runtime card `TorsoItemCard_035` remained non-collapsed, was selected successfully, and selection preserved the 3,611 offset.
- Selection tests preserved 640 px through None and a real item. Existing suppressed Raised Hood/Headgear and Raised Hood/Headphones automatic paths preserved Torso/Face offsets; modal Cancel/Reset smoke did not reset scrolling. Reopening the page does not rebuild or duplicate the catalog.

### Regression, diagnostics, build, backups, and final state

- Runtime catalog evidence: 36 cards; damaged display names and removed StableIds absent; ordinary T-shirt, work shirt, M65, Hood_1 and Hood_2 families present; authored samples and empty state correctly collapsed. Current Selection remains one-line, clipped, Ellipsis, no auto-wrap, with full tooltip.
- Raised Hood conflict logic remained operational in the user's restored suppression=true state: Headgear/Headphones conflicts resolved through the existing atomic automatic path without modal and without scroll reset. Random modal still hides the checkbox row. Reset modal opened with its checkbox row collapsed and Cancel closed it. F9 toggled its inspector Visible then back to Collapsed.
- F9 JSON now includes Torso catalog total, runtime-card count, removed-damaged count, active Torso StableId/catalog presence, ScrollOffset/EndOffset, multiplier, animation state and existing pending conflict fields.
- Final full UE 5.8 UBT after the multiplier correction: exit code 0, `Result: Succeeded` (15.29 s). No C++ changed after that build.
- Cold compile: `WBP_HeadCatalogItemCard` Success; `WBP_CharacterCustomizationRoot_V2` Success. Validation: 0 errors. BindWidget/missing-widget/duplicate-GUID/GUID/invalid-parent/reconstruction errors: 0. The existing validator still reports 17 unrelated orphan/unused warnings.
- Appearance SaveGame prechange/restored SHA-256: `EF00E172193F4FC9EFDD7399B7A50C8D07F88050680FB0D0323EB3A3E60568BD`.
- User settings prechange/restored SHA-256: `619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7`; the original suppression preference was restored exactly.
- Permanent backups, not deleted:
  - `Saved/CodexBackups/UpperEquipmentRemoveDamagedShirtsSmoothScroll_20260716/AvariikaCompany.prechange.sav`
  - `Saved/CodexBackups/UpperEquipmentRemoveDamagedShirtsSmoothScroll_20260716/AvariikaUserSettings.prechange.sav`
  - `Saved/CodexBackups/UpperEquipmentRemoveDamagedShirtsSmoothScroll_20260716/WBP_CharacterCustomizationRoot_V2.prechange.uasset` — SHA-256 `38C0B76B754CBE4591CE8710A65E75F5651A4119FC1D61013F7845CD1B875189`.
- Final state: Editor responsive, PIE=false, dirty Content=0, dirty Maps=0, both user saves byte-match their backups.
- Visual and functional acceptance remains with the user. Manual checks: physical wheel feel and rapid-event interpolation at the user's hardware/settings, scrollbar thumb drag, the remaining clothing visuals, and normal persistence after a user-driven restart. No visual PASS is claimed.

## Raised hood Headphones conflict and Current Selection overflow fix - 2026-07-16

### Outcome

- The user-reported Face Protection screenshot/behavior showed two remaining defects after the accepted Raised Hood ↔ Headgear flow: full-size work headphones intersected `Jacket_Worker_Hood_2`, and `ТЕКУЩИЙ ВЫБОР: ЖЁЛТЫЕ РАБОЧИЕ НАУШНИКИ` could render beyond the right panel.
- The existing `OverlayRandom` / `EAvAppearanceModalMode` architecture is still the only modal architecture. It now handles Raised Hood conflicts with Headgear, Headphones, or both simultaneously; no card is permanently disabled and no second popup was created.
- The existing serialized `bSuppressRaisedHoodHeadgearConflictWarning` field remains the persistence source for backward compatibility. Universal internal getter/setter aliases expose it as the Raised Hood equipment warning preference, so an already-suppressed Headgear warning also suppresses Headphones and combined warnings without migration or a reset.
- The authored page trees, 43-card Torso catalog, 86 Torso thumbnails, Face/Head/Hands catalogs, default tab, Random/Reset confirmation presentation and SaveGame appearance schema were preserved.

### Exact Headphones audit and compatibility

- The current catalog contains `НЕТ` plus eight real `EWorkerSlot::Headphones` records. `НЕТ` is compatible. No real compatible record was found.
- All eight real records use the Modular Workers male Quantum skeleton. The four standard variants share the same 15,592-vertex / 24,032-triangle circumaural cup-and-crown-band geometry; the four microphone variants share the same 20,212-vertex / 29,834-triangle geometry plus a boom microphone. Both families occupy the Hood_2 ear/crown volume; the microphone family additionally conflicts around the hood opening. Exact incompatible paths:
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers/SKM_Headphones_Worker_Green.SKM_Headphones_Worker_Green`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers/SKM_Headphones_Worker_Orange.SKM_Headphones_Worker_Orange`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers/SKM_Headphones_Worker_White.SKM_Headphones_Worker_White`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers/SKM_Headphones_Worker_Yellow.SKM_Headphones_Worker_Yellow`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers_Micro/SKM_Headph_Worker_Mic_Gr.SKM_Headph_Worker_Mic_Gr`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers_Micro/SKM_Headph_Worker_Mic_Orn.SKM_Headph_Worker_Mic_Orn`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers_Micro/SKM_Headph_Worker_Mic_Wht.SKM_Headph_Worker_Mic_Wht`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Headphones_Workers_Micro/SKM_Headph_Worker_Mic_Yel.SKM_Headph_Worker_Mic_Yel`
- Each exact catalog record now has a stable ID, compact `CurrentSelectionName`, `bConflictsWithRaisedHood=true`, and `BlockedCompatibilityTags={Torso.Hood.Raised}`. The exact-path audit is recorded in `Saved/CodexDiagnostics/HoodHeadphonesAndTextOverflow_20260716/headphones_asset_audit.json`.
- The six existing raised records remain exactly the `Jacket_Worker_Hood_2` Beige/Blue/Gray/Green/LightGr/Orange records. All `Jacket_Worker_Hood_1` records remain down-hood and do not warn.

### Shared modal and atomic multi-slot behavior

- Raised Hood with only Headphones uses `НЕСОВМЕСТИМОЕ СНАРЯЖЕНИЕ / Надеть куртку с поднятым капюшоном? / Выбранные наушники будут сняты. / ОТМЕНА / СНЯТЬ И НАДЕТЬ`.
- Raised Hood with both Headgear and Headphones uses one modal and the warning `Выбранные головной убор и наушники будут сняты.` Confirm clears both slots and applies Torso in one operation; it never shows sequential popups.
- Reverse Headphones selection uses `НЕСОВМЕСТИМОЕ СНАРЯЖЕНИЕ / Надеть выбранные наушники? / Куртка с поднятым капюшоном будет снята. / ОТМЕНА / СНЯТЬ И НАДЕТЬ` and clears only Torso before applying the exact Headphones target.
- Pending state now retains stable target identity and expected Headgear/Headphones/Torso stable IDs plus `bClearHeadgear`, `bClearHeadphones`, and `bClearTorso`; it contains no transient catalog-record pointer. Confirm re-resolves and revalidates all expected identities and is guarded against double execution.
- Atomic resolution copies `ActiveCharacterAppearance`, changes every affected slot in that copy, validates once, applies preview once, commits once, performs exactly one appearance autosave, and refreshes UI once. It never mixes `PresetPreviewAppearance` and never clears unrelated slots.
- Cancel, Escape, backdrop and teardown clear only pending modal state. The suppression preference changes only after a successful checked Confirm. With suppression enabled, the identical multi-slot operation runs immediately without modal/toast/sound/delay.
- Runtime logs contain `AppearanceAutosaveCount=1` for Headphones-only, reverse Headphones, and simultaneous Headgear+Headphones operations. Tested Torso and Headphones scroll offsets remained unchanged through confirm/cancel; Face cards stayed at nine (`НЕТ` + eight real) without rebuild/duplication.

### Random, old saves, and diagnostics

- Random cleanup/validation now treats Raised Hood + conflicting Headphones exactly like incompatible Headgear: Headphones becomes `None` when a raised hood is generated. Suppression does not affect Random, and the existing Gloves policy is unchanged.
- Old appearance saves containing Raised Hood with Headgear, Headphones, or both still load unchanged: no modal, migration, slot clearing, or autosave occurs during load. The next manual conflicting selection uses the new flow.
- F9 diagnostics retain the existing inspector and add active Headphones stable ID, Headphones conflict, total conflict-slot count, pending clear flags and the universal suppression state. Runtime startup logged successful creation of the development-only F9 inspector.

### Current Selection overflow correction

- Actual authored Face widgets are `HB_FaceCurrentSelection`, `Txt_FaceCurrentSelection`, and `Txt_FaceCurrentSelectionValue`. The cause was an allocated Fill slot whose content alignment remained Left, while the value lacked a consistently enforced zero minimum width, one-line ellipsis and clipping policy; its desired text width could paint beyond the panel.
- The label is now Auto-sized and one-line. The value is Fill (`weight=1`), `HAlign_Fill`, `VAlign_Center`, `MinDesiredWidth=0`, `AutoWrapText=false`, `TextOverflowPolicy=Ellipsis`, and `ClipToBounds`; the full display name is its tooltip. The same safe helper is applied to Head, Face, Hands and Upper Current Selection rows without changing panel width.
- Standard headphone current-selection values were shortened while card names/tooltips retain the full names:
  - `ЗЕЛЁНЫЕ РАБОЧИЕ НАУШНИКИ` → `ЗЕЛЁНЫЕ НАУШНИКИ`
  - `ОРАНЖЕВЫЕ РАБОЧИЕ НАУШНИКИ` → `ОРАНЖЕВЫЕ НАУШНИКИ`
  - `БЕЛЫЕ РАБОЧИЕ НАУШНИКИ` → `БЕЛЫЕ НАУШНИКИ`
  - `ЖЁЛТЫЕ РАБОЧИЕ НАУШНИКИ` → `ЖЁЛТЫЕ НАУШНИКИ`
- Runtime property evidence confirms `ЖЁЛТЫЕ НАУШНИКИ`, full tooltip `ЖЁЛТЫЕ РАБОЧИЕ НАУШНИКИ`, wrap disabled, Ellipsis, minimum width 0 and `ClipToBounds`: `Saved/CodexDiagnostics/HoodHeadphonesAndTextOverflow_20260716/current_selection_probe.json`.

### Build, Blueprint integrity, backups, and final state

- Full UE 5.8 UBT command completed with exit code 0 and `Result: Succeeded` (26.31 s). No C++ was changed after that successful build.
- Cold-editor compile results: `WBP_HeadCatalogItemCard` = Success; `WBP_CharacterCustomizationRoot_V2` = Success. Blueprint validation reports 0 errors. Compile/log scan reports BindWidget errors 0, missing-widget errors 0, duplicate/GUID errors 0, invalid-parent errors 0 and reconstruction errors 0. The validator still lists 17 pre-existing unused/orphan graph warnings; they are not compile/reconstruction failures and were not introduced by this correction.
- Prechange appearance SHA-256, restored exactly after testing: `54DBE8DAC21E05D4E9031DEEE0063538C93263689DF86597B8AFE7AA60D9DFF0`.
- Prechange settings SHA-256, restored exactly after testing: `619B5A0F44E21785B3D07D58B77F8388CDAC5CD1667ED332DA0EB8A6A47FD0C7`.
- Permanent backups (not deleted):
  - `Saved/CodexBackups/HoodHeadphonesAndTextOverflow_20260716/AvariikaCompany.prechange.sav`
  - `Saved/CodexBackups/HoodHeadphonesAndTextOverflow_20260716/AvariikaUserSettings.prechange.sav`
  - `Saved/CodexBackups/HoodHeadphonesAndTextOverflow_20260716/WBP_CharacterCustomizationRoot_V2.prechange.uasset` — SHA-256 `5F207C9D50D7FBDC7025D1E646CC6B7B9B4AA0A7FF94DEC4549CD65865687C29`
- Final technical state: Editor responsive, PIE=false, dirty Content=0, dirty Maps=0, both user saves match their prechange SHA values. No thumbnails were rebuilt.
- Visual and functional acceptance remains with the user. Manual checks should cover actual collision appearance for both headphone families and all colors, physical mouse/gamepad modal input, the ellipsis at the user's target resolution, tooltip presentation, scroll feel, and persistence in the user's normal restart flow. No visual PASS is claimed.

## Raised hood conflict modal and suppress warning preference - 2026-07-16

### Outcome and authored modal

- Reused the existing authored `OverlayRandom` modal; no second popup or parallel modal manager was added. Its backdrop, centered dark/orange dialog, warning icon, divider and two-button layout remain the shared presentation for Random, Reset and equipment-conflict modes.
- Canonical Widget Blueprint Editor APIs added exactly one authored row in `VB_RandomAppearanceDialogContent`, between `Txt_RandomAppearanceDialogWarning` and `HB_RandomAppearanceDialogActions`:
  - `HB_EquipmentConflictDontShowAgain`
    - `CheckBox_EquipmentConflictDontShowAgain`
    - `Txt_EquipmentConflictDontShowAgain`
- The authored row defaults to `Collapsed`; the checkbox defaults unchecked and is focusable. The label is `БОЛЬШЕ НЕ ПОКАЗЫВАТЬ ЭТО ПРЕДУПРЕЖДЕНИЕ`. The row uses centered Auto sizing, bottom padding 20, checkbox/text spacing 12 and 14-point light-gray label text. Runtime checkbox styling uses a dark unchecked box and orange checked/hover states.
- The three new authored widgets are required `BindWidget` fields. There is no runtime checkbox fallback or authored-modal reconstruction.

### Modal modes, text and pending-action safety

- `EAvAppearanceModalMode` is the sole action discriminator: `None`, `RandomAppearance`, `ResetToBase`, `EquipTorsoAndClearHeadgear`, `EquipHeadgearAndClearTorso`. Actions are never inferred from a `TextBlock` value.
- Random retains `СЛУЧАЙНЫЙ ВНЕШНИЙ ВИД / Создать новый случайный образ? / Текущий внешний вид персонажа будет заменён. / ОТМЕНА / СОЗДАТЬ`. Reset retains its prior text and behavior. Both modes force the checkbox unchecked and its row `Collapsed`.
- Torso conflict uses `НЕСОВМЕСТИМОЕ СНАРЯЖЕНИЕ / Надеть куртку с поднятым капюшоном? / Выбранный головной убор будет снят. / ОТМЕНА / СНЯТЬ И НАДЕТЬ`.
- Reverse conflict uses `НЕСОВМЕСТИМОЕ СНАРЯЖЕНИЕ / Надеть выбранный головной убор? / Куртка с поднятым капюшоном будет снята. / ОТМЕНА / СНЯТЬ И НАДЕТЬ`.
- `FAvPendingEquipmentConflict` owns only stable/value state: modal mode, target StableId/slot/exact mesh/material identity, expected conflicting StableId/slot, display names and validity. No raw pointer to a transient catalog record is retained. Confirm re-resolves the target record and verifies exact identity, current expected conflict and valid preview. Confirm is guarded against double execution.
- Cancel, backdrop, Escape/Back and `NativeDestruct` clear modal/pending state and restore the checkbox to unchecked without changing appearance or the preference. Enter/gamepad Accept confirms; Tab/Shift+Tab stays in modal controls.

### Exact raised-hood Torso audit

- `Jacket_Worker_Hood_2` is the raised-hood geometry: it surrounds the crown, temples and ears. Its six exact catalog records have `bHasRaisedHood=true` plus `CompatibilityTags={Torso.Hood.Raised, ConflictsWithAuditedHeadgear}`; their StableIds are the mesh asset names:
  - `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Bege.SKM_Jacket_Worker_Hood_2_Bege`
  - `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Blue.SKM_Jacket_Worker_Hood_2_Blue`
  - `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Gray.SKM_Jacket_Worker_Hood_2_Gray`
  - `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Green.SKM_Jacket_Worker_Hood_2_Green`
  - `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_LightGr.SKM_Jacket_Worker_Hood_2_LightGr`
  - `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Orange.SKM_Jacket_Worker_Hood_2_Orange`
- A down-hood alternative exists: all six `Jacket_Worker_Hood_1` color records. Hood_1 is explicitly not tagged raised and does not trigger this warning.
- The Torso catalog remains exactly 43 cards (`НЕТ` + 42 real), with the same 86 Torso thumbnails. No card was hidden or made permanently disabled.

### Exact incompatible Headgear audit

- All 22 current Headgear records occupy or protrude through at least one Hood_2 crown/temple/ear volume, so the exact-path incompatibility set contains all 22. There is currently no compatible non-None Headgear record. This is an individual exact-path set, not a substring/family blanket rule:
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Bege.SKM_Cap_Bege`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Black.SKM_Cap_Black`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Blue.SKM_Cap_Blue`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Color.SKM_Cap_Color`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Red.SKM_Cap_Red`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Cap/SKM_Cap_Street.SKM_Cap_Street`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Cowboy/SKM_Hat_Cowboy.SKM_Hat_Cowboy`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Farmer/SKM_Hat_Cowboy.SKM_Hat_Cowboy`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Flat/SKM_Hat_Flat_Blue.SKM_Hat_Flat_Blue`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Flat/SKM_Hat_Flat_Brown.SKM_Hat_Flat_Brown`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Flat/SKM_Hat_Flat_Grey.SKM_Hat_Flat_Grey`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Hat_Up/SKM_Hat_Up.SKM_Hat_Up`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Blue.SKM_Helmet_Worker_Blue`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Green.SKM_Helmet_Worker_Green`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Orange.SKM_Helmet_Worker_Orange`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Red.SKM_Helmet_Worker_Red`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_White.SKM_Helmet_Worker_White`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Helmet_Worker/SKM_Helmet_Worker_Yellow.SKM_Helmet_Worker_Yellow`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Warm_Hat/SKM_Warm_Hat_Bege.SKM_Warm_Hat_Bege`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Warm_Hat/SKM_Warm_Hat_Black.SKM_Warm_Hat_Black`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Warm_Hat/SKM_Warm_Hat_Gray.SKM_Warm_Hat_Gray`
  - `/Game/Modular_Workers/Mesh/Male/Head_Modules/Warm_Hat/SKM_Warm_Hat_Green.SKM_Warm_Hat_Green`
- `НЕТ` remains compatible and immediately selectable. Headgear cards remain enabled so clicks enter the confirmation flow rather than blocking the category.

### Atomic conflict resolution and persistence

- Without suppression: selecting raised Hood_2 over incompatible Headgear, or Headgear over raised Hood_2, opens the shared modal and leaves appearance/selection/scroll untouched until confirmation. Confirm clears only the conflicting slot and applies the requested item.
- The two-slot mutation is atomic: copy `ActiveCharacterAppearance`, clear one conflicting slot, set the target exact mesh/material identity, validate once, apply preview once, perform one `CommitActiveCharacterAppearance`, one appearance autosave and one UI refresh. It never touches Head, Hair, Beard, Glasses, FaceMask, Headphones, Gloves, Watch, Legs, Feet, Vest or FullOutfit, and never mixes `ActiveCharacterAppearance` with `PresetPreviewAppearance`.
- Selected states and current-selection labels refresh in place through `UpdateTorsoSelectionCards`, `RefreshHeadCardSelectionStates`, `RefreshTorsoCurrentSelection` and `RefreshHeadCurrentSelection`; neither catalog rebuilds. Runtime Torso count stayed 43 after three repeated opens, Designer samples stayed `Collapsed`, and tested Torso/Headgear scroll offsets stayed 300/250 through cancel/confirm/automatic resolution.
- Preference is `bSuppressRaisedHoodHeadgearConflictWarning` in a dedicated backward-compatible `UAvariikaUserSettingsSaveGame`, slot `AvariikaUserSettings`, user index 0. It is not part of the appearance map, preset, `EWorkerSlot` or `AvariikaCompany` SaveGame. Missing/old settings default to `false`; public ledger getter/setter support later Settings UI and `setter(false)` re-enables the modal.
- Preference changes to `true` only after a successful atomic conflict confirmation with the checkbox checked. Cancel/Escape/backdrop/destruct never persist it. With `true`, the same atomic action runs immediately without modal, toast, sound or delay. Preference persistence is separate; appearance still autosaves exactly once.
- Old appearance saves that already contain raised hood + incompatible Headgear are loaded unchanged: no modal, silent migration or autosave occurs during load. The conflict appears in diagnostics and the new behavior starts on the next manual conflicting selection.
- Random remains compatibility-aware and does not generate raised Hood_2 + incompatible Headgear; suppression does not alter Random policy. Random and Reset confirmations both retained their prior flow.

### Diagnostics and executed verification

- F9 Appearance Inspector now reports active Torso StableId, `RaisedHood`, active Headgear StableId, `HeadgearConflictsWithRaisedHood`, active-conflict state, modal mode, pending target StableId, suppression preference and checkbox state while a conflict modal is open.
- Runtime flow evidence: `Saved/CodexDiagnostics/HoodConflictDontShowAgain_20260716/runtime_flow.json`. It verifies Random/Reset row collapse, both conflict directions, Cancel with checked checkbox preserving appearance/company SHA and leaving settings absent, checked/unchecked confirms, one-slot removal, exact target application, scroll preservation and suppression auto-resolution. Runtime logs report `AppearanceAutosaveCount=1` for both conflict directions.
- `Saved/CodexDiagnostics/HoodConflictDontShowAgain_20260716/random_reset_confirm.json` separately executes the Random `СОЗДАТЬ` and Reset confirm actions: both close the shared modal, keep the checkbox row collapsed, do not create/change the suppression setting and do not open an equipment-conflict modal.
- Cold-restart evidence: `Saved/CodexDiagnostics/HoodConflictDontShowAgain_20260716/cold_persistence.json`. The saved `true` preference survived a cold editor restart; the next conflict resolved automatically with modal `Collapsed`. The same run retained `[43,43,43,43]` Torso card counts over three reopens and both Designer samples `Collapsed`.
- After tests, the test-only `AvariikaUserSettings.sav` was removed, restoring the original default/missing setting (`false`). The original appearance SaveGame was restored byte-for-byte.

### Build, Blueprint integrity and backups

- Full UE 5.8 UBT: `avariikaEditor Win64 Development`, exit code `0`, `Result: Succeeded`. No C++ was changed after the successful build.
- Cold compile: `/Game/Avariika/UI/CharacterCustomization/WBP_HeadCatalogItemCard` = Success; `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2` = Success. BindWidget/missing-widget/invalid-parent errors = 0; GUID errors = 0; duplicate GUID = 0; orphaned widget variables = 0; reconstruction warnings = 0. The new row/checkbox/text each exist exactly once under the correct parent.
- Appearance SaveGame backup: `Saved/CodexBackups/HoodConflictDontShowAgain_20260716/AvariikaCompany.prechange.sav`, SHA-256 `49BC4707434C5A9C7F267AEE62A7CBE99F3CA4703CA33AC7B80CA371AC633071`.
- WBP backup: `Saved/CodexBackups/HoodConflictDontShowAgain_20260716/WBP_CharacterCustomizationRoot_V2.prechange.uasset`, SHA-256 `D5A680D9258BD068D0D6BF906A78F9151E1906AEB26D2A913F2EEC03CCA450F4`.
- Final appearance SaveGame SHA matches the prechange SHA. Final state is PIE false and dirty Content/maps `0/0`; unrelated pre-existing Git work remains untouched.
- Visual and functional acceptance belongs to the user; no visual PASS is claimed.

### Future Settings TODO

- Add `ПРЕДУПРЕЖДЕНИЯ О НЕСОВМЕСТИМОМ СНАРЯЖЕНИИ` or `СБРОСИТЬ СКРЫТЫЕ ПРЕДУПРЕЖДЕНИЯ` to the future Settings menu, using the existing preference getter/setter so the warning can be re-enabled.

## Upper Equipment scroll and full Torso catalog corrective pass - 2026-07-16

### Outcome and authored layout

- The user-authored `VB_RightPanelUpperEquipment` page was retained intact: title/divider, current-selection area and `Overlay_UpperItemsArea` containing `ScrollBox_TorsoItems -> UniformGrid_TorsoItems` plus sibling `Txt_TorsoEmptyState`. Exactly two Designer samples remain: `DesignSample_TorsoNone` and `DesignSample_TorsoItem`. No subcategories, page-local WidgetSwitcher, replacement page, Canvas fallback, manual mouse-wheel override or Tick logic was added.
- The exact scroll defect had two authored-layout causes: `Overlay_UpperItemsArea` used a `VerticalBoxSlot` with `Size=Auto`, so the child ScrollBox received its complete desired height instead of a bounded viewport; additionally, `Txt_TorsoEmptyState` was incorrectly a second child of `ScrollBox_TorsoItems` instead of the Overlay sibling shown in the authored specification.
- Canonical Widget Blueprint APIs changed `Overlay_UpperItemsArea` to `Size=Fill`, fill weight `1`, H/V alignment `Fill`; moved `Txt_TorsoEmptyState` to Overlay child index 1; set the ScrollBox Overlay slot to H/V `Fill`, zero padding, Visible/enabled/opacity 1. Empty State is `Collapsed` while real records exist. ScrollBox runtime settings are Vertical, `ConsumeMouseWheel=WhenScrollingPossible`, wheel multiplier `48`, scrollbar Visible, thickness `5x5`, no forced always-show.
- The root category still routes through the existing `WS_RightPanelPages` to the authored `VB_RightPanelUpperEquipment`. It builds once on first open and scrolls to start once. Later opens update selection/availability only; no rebuild and no offset reset. The first tab remains `МОИ ПЕРСОНАЖИ`; `RefreshMyCharactersPage()` was not added to `NativeConstruct()`.

### Scroll verification

- Cold PIE with the full `43`-record catalog (`42` real + `НЕТ`) produced more grid rows than the bounded viewport. Real mouse-wheel input moved the view from the first row to the last row; real scrollbar-thumb drag moved it back upward. The last orange Hood_2 card became fully visible and remained clickable. Selecting it updated preview, selected state and Current Selection without jumping to the top.
- Three category leave/return cycles retained the offset and the runtime count. Screenshots: `Saved/CodexDiagnostics/UpperEquipmentScrollAndCatalog_20260716/upper_after_mousewheel.png`, `upper_after_scrollbar_drag.png`, `upper_after_last_selection.png`, `upper_after_three_reopens.png`, `upper_back_at_top.png`.
- Exact numeric `ScrollOffset/EndOffset` was not claimed: the runtime MCP sampler was blocked by the editor tool mutex during UI events. The before-state structurally had no bounded range (`Auto` parent); after-state was verified through native UI wheel/thumb movement and last-card reachability. Separate injected 12/30-card builds were not retained; the complete 43-record state is the stronger executed runtime case. User acceptance of wheel feel/speed remains pending.

### Full package audit and classification

- Asset Registry scanned all `878` assets under `/Game/Modular_Workers`: `243` SkeletalMesh, `40` StaticMesh, `118` MaterialInstanceConstant, `2` Blueprint, `2` WidgetBlueprint and `4` demo World assets. Demo builder/preset Blueprints and showcase maps were included. No StaticMesh is used as a Torso wearable.
- Search/classification result: `122` potential upper/outfit records (`42` modular Torso + `30` unified FullOutfit + `50` merged demo presets), plus `21` lower-body modules. Unique eligible Torso meshes: `42`; additional mesh+alternate-material user-facing variants: `0`; enabled user-facing records: `42`; denied non-Torso outfit/preset records: `80`.
- Complete machine-readable evidence (exact path, class, skeleton, bounds, material slots and classification reason): `Saved/CodexDiagnostics/UpperEquipmentScrollAndCatalog_20260716/full_asset_registry_audit.json`. Individual canonical-material Type01/Type02 evidence for every Torso mesh: `AuditCaptures/all_capture_manifest.json`, `all_contact_HeadType01.png`, `all_contact_HeadType02.png`.
- Every eligible Torso mesh uses `/Game/Modular_Workers/Mesh/Male/SK_Male_Quantum_Character_Skeleton`, has upper-body bounds/sections only, follows the existing leader-pose architecture and rendered with its canonical embedded material on both Type01 and Type02. The old blanket denies for all M65, all Tucked, most RolledUp, damaged/plain TShirts, plain Worker Jackets and Green/LightGr/Orange hood variants were removed. Tucked/torn appearances are authored fabric damage with the existing Body visible underneath, not missing opacity dependencies.

### Exact Torso allowlist (42)

Each line below is an exact object path and an enabled record:

- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_Bege.SKM_TShirt_Bege`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_Black.SKM_TShirt_Black`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_Blue.SKM_TShirt_Blue`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_Gray.SKM_TShirt_Gray`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_Green.SKM_TShirt_Green`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_Red.SKM_TShirt_Red`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Bege.SKM_Tshirt_Tucked_Bege`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Black.SKM_Tshirt_Tucked_Black`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Blue.SKM_Tshirt_Tucked_Blue`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Green.SKM_Tshirt_Tucked_Green`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Grey.SKM_Tshirt_Tucked_Grey`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_Red.SKM_Tshirt_Tucked_Red`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt_Tucked/SKM_Tshirt_Tucked_White.SKM_Tshirt_Tucked_White`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp/SKM_Shirt_RolledUp_Bege.SKM_Shirt_RolledUp_Bege`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp/SKM_Shirt_RolledUp_Black.SKM_Shirt_RolledUp_Black`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp/SKM_Shirt_RolledUp_Blue.SKM_Shirt_RolledUp_Blue`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp/SKM_Shirt_RolledUp_Green.SKM_Shirt_RolledUp_Green`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp/SKM_Shirt_RolledUp_Red.SKM_Shirt_RolledUp_Red`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp/SKM_Shirt_RolledUp_RedCage.SKM_Shirt_RolledUp_RedCage`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp/SKM_Shirt_RolledUp_White.SKM_Shirt_RolledUp_White`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp/SKM_Shirt_RolledUp_White_Cage.SKM_Shirt_RolledUp_White_Cage`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker/SKM_Jacket_Worker_Bege.SKM_Jacket_Worker_Bege`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker/SKM_Jacket_Worker_Blue.SKM_Jacket_Worker_Blue`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker/SKM_Jacket_Worker_Gray.SKM_Jacket_Worker_Gray`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker/SKM_Jacket_Worker_Green.SKM_Jacket_Worker_Green`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker/SKM_Jacket_Worker_LightGreen.SKM_Jacket_Worker_LightGreen`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker/SKM_Jacket_Worker_Orange.SKM_Jacket_Worker_Orange`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_M65/SKM_Jacket_M65_Bege.SKM_Jacket_M65_Bege`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_M65/SKM_Jacket_M65_Black.SKM_Jacket_M65_Black`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_M65/SKM_Jacket_M65_Green.SKM_Jacket_M65_Green`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1/SKM_Jacket_Worker_Hood_1_Bege.SKM_Jacket_Worker_Hood_1_Bege`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1/SKM_Jacket_Worker_Hood_1_Blue.SKM_Jacket_Worker_Hood_1_Blue`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1/SKM_Jacket_Worker_Hood_1_Gray.SKM_Jacket_Worker_Hood_1_Gray`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1/SKM_Jacket_Worker_Hood_1_Green.SKM_Jacket_Worker_Hood_1_Green`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1/SKM_Jacket_Worker_Hood_1_LightGr.SKM_Jacket_Worker_Hood_1_LightGr`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1/SKM_Jacket_Worker_Hood_1_Orange.SKM_Jacket_Worker_Hood_1_Orange`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Bege.SKM_Jacket_Worker_Hood_2_Bege`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Blue.SKM_Jacket_Worker_Hood_2_Blue`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Gray.SKM_Jacket_Worker_Hood_2_Gray`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Green.SKM_Jacket_Worker_Hood_2_Green`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_LightGr.SKM_Jacket_Worker_Hood_2_LightGr`
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Orange.SKM_Jacket_Worker_Hood_2_Orange`

### Materials, names and stable records

- No synthetic material-only duplicates were added. Each mesh carries its canonical color MaterialInstance. Exact mapping is same-family/same-color: `Tshirt/MI_TShirt_*`, `Tshirt_Tucked/MI_Tshirt_Tucked_*` (`SKM ... Grey` maps canonical `MI ... Gray`), `Shirt_RolledUp/MI_Shirt_RolledUp_*`, `Jacket_M65/MI_Jacket_M65_*`, and `Jacket_Worker/MI_Jacket_Worker_*`; `LightGr` hood meshes map canonical `MI_Jacket_Worker_LightGreen`. Hood_1/Hood_2 contain two slots and both receive that exact same family/color MI. Every complete exact slot mapping is stored in `FAvTorsoCatalogItem.ExactMaterialOverrides` and in the audit JSON.
- `FAvTorsoCatalogItem` now has `StableId`, `Family`, `Color`, `SortOrder` in addition to exact mesh/material/thumbnail paths and compatibility fields. StableId is mesh/material based, not the display label.
- Russian labels combine color with these exact product labels: `ФУТБОЛКА`, `ПОТЁРТАЯ ФУТБОЛКА`, `РАБОЧАЯ РУБАШКА`, `РАБОЧАЯ КУРТКА`, `КУРТКА M65`, `РАБОЧАЯ КУРТКА С КАПЮШОНОМ`, `УТЕПЛЁННАЯ РАБОЧАЯ КУРТКА`. Color labels used: `БЕЖЕВАЯ`, `ЧЁРНАЯ`, `СИНЯЯ`, `СЕРАЯ`, `ЗЕЛЁНАЯ`, `СВЕТЛО-ЗЕЛЁНАЯ`, `ОРАНЖЕВАЯ`, `КРАСНАЯ`, `БЕЛАЯ`; cage records use `КРАСНАЯ/БЕЛАЯ РУБАШКА В КЛЕТКУ`.
- Final catalog count is `43`: first `НЕТ` plus `42` real enabled records. `НЕТ` clears only `EWorkerSlot::Torso`; it leaves HeadType, skin profile, Gloves, Watch, Vest, Legs, Feet, Headgear and every unrelated slot unchanged. Reselecting the current record is a no-op. Selection uses the existing ManualCustomized commit/autosave path and never rebuilds the catalog.

### Exact denylist and lower-slot classification

- FullOutfit denylist (`30`): all of the following are unified meshes with upper body + full pants/jumpsuit + footwear, three canonical clothing/pants/boots sections and full-body bounds; each is individually denied from `EWorkerSlot::Torso`:
  - `/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Boots/SKM_Shirt_Jumpsuit_Boots_{1,2,3,4,5}`
  - `/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Pants_Worker/SKM_Overalls_Shirt_Pants_Worker_{1,2,3}`
  - `/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Tshirt_Boots/SKM_Overalls_Tshirt_Boots_{1,2,3,4,5}`
  - `/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Shirt/SKM_Worker_Shirt_Jumpsuit_{1,2,3,4,5}`
  - `/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt/SKM_Worker_Tshirt_Jumpsuit_{1,2,3,4,5}`
  - `/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_{1,2,3,4,5,6,7}`
  For every package above, the exact object path is `Package.AssetName` (for example `.../SKM_Shirt_Jumpsuit_Boots_1.SKM_Shirt_Jumpsuit_Boots_1`).
- Merged-demo denylist (`50`): exact `/Game/Modular_Workers/Mesh/Male/Presets/SKM_Worker_Male_N.SKM_Worker_Male_N` and `/Game/Modular_Workers/Mesh/Male/Presets/SKM_Worker_Male_Body_N.SKM_Worker_Male_Body_N`, for every `N` in `{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25}`. Each is denied because it is a merged full demo character with full-body bounds and 15-19 material sections, not a modular Torso asset.
- Legs/lower-module classification (`21`, not FullOutfit): exact Jeans `SKM_Jeans`, `_Bege`, `_Black`, `_Blue`, `_Turn`; Jumpsuit `_Bege`, `_Black`, `_Blue`, `_DarkBlue`; Pants_Worker `_Bege`, `_Blue`, `_Gray`, `_Green`, `_LightGreen`, `_Orange`; Pants_Worker_Full `_Bege`, `_Blue`, `_Gray`, `_Green`, `_LightGrn`, `_Orange`, all under their matching `/Game/Modular_Workers/Mesh/Male/Pants_Modules/{Jeans,Jumpsuit,Pants_Worker,Pants_Worker_Full}/` folders with `Package.AssetName` object paths. They contain lower-body geometry (some include boots) and no Torso section.
- Standalone Feet assets in this package: `0`. Some Pants/FullOutfit meshes integrate boots, but they are not standalone `EWorkerSlot::Feet` candidates.

### Thumbnails, runtime lifecycle and compatibility

- Final thumbnails: `43` Type01 + `43` Type02 = `86`. Existing `24` correct textures were retained; only `62` missing textures were created/imported. Every source/asset is `512x512`, alpha min/max `255`; redirectors `0`, missing paths `0`, unexpected/stale paths `0`, duplicates `0`, wrong dimensions `0`. Evidence: `thumbnail_asset_verification.json` and `thumbnail_source_alpha_verification.json`.
- Exact thumbnail path convention: `/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/UpperEquipment/Torso/HeadType01/T_UI_Torso_<StableMeshName>_Type01` and Type02 equivalent; `НЕТ` uses StableMeshName `None`. Lighting remained exposure `0.95`, key `21.2`, fill `12000`, soft key `72000`, soft fill `25600`, rim `14400`, ambient `0.55`, FinalColorLDR and opaque blue-gray backdrop at `FRotator(90,0,0)`. Torso camera remained FOV `34`, distance `180`, focus Z `125`.
- `RuntimeTorsoCards` and parallel catalog indices are the only runtime-card tracking. Rebuild/destruct removes only tracked cards, never calls `ClearChildren()` and never touches the two authored samples. Layout remains exactly two columns: `Row=VisibleIndex/2`, `Column=VisibleIndex%2`. Repeat opens do not rebuild. Selection updates only the old/new card state and compatibility, preserving scroll.
- Designer samples stay exactly two and visible in Designer; both are Collapsed at runtime, excluded from `RuntimeTorsoCards`, receive no handlers and do not affect runtime layout.
- No other slot is automatically removed. `НЕТ` is always enabled. No Torso+Gloves/Watch/Vest restriction was introduced without demonstrated item-level conflict; representative combinations remained technically composable. Exhaustive clipping/compatibility is explicitly left for user visual acceptance.

### Build, integrity, backup and final state

- Full UE 5.8 UBT ran after the final C++ change and compiled UHT plus `AvCharacterCustomizationRootWidget.cpp` and `WorkerAppearanceComponent.cpp`: exit code `0`, `Result: Succeeded` (compile run 24.48s; confirmation run also exit 0). No C++ changed afterward.
- Cold editor compile: `WBP_HeadCatalogItemCard = Success`; `WBP_CharacterCustomizationRoot_V2 = Success`. GUID errors `0`, duplicate GUID `0`, BindWidget/missing-widget/invalid-parent/reconstruction/orphaned-widget-reference errors `0`. Existing unconnected default lifecycle graph stubs were baseline graph warnings, not widget GUID/orphan failures.
- New backups outside Content: `Saved/CodexBackups/UpperEquipmentScrollAndCatalog_20260716/AvariikaCompany.prechange.sav`, SHA-256 `4F2A456814571B48D1CA9AD593C66BDFA2C2B732D1C96AF0E8794473006DA9B5`; `WBP_CharacterCustomizationRoot_V2.prechange.uasset`, SHA-256 `F6CEC61DA13026A1EE42F30B519411AAAA2A7B2517BC4385B0E029C41961A93F`. Backups were retained.
- PIE is false. The user SaveGame was restored byte-for-byte after autosave tests; final SHA-256 equals the backup SHA. Dirty Content/maps are `0/0`; the editor is responsive and left open for user acceptance. The broad pre-existing dirty Git worktree and unrelated user files were preserved. Final `git diff --check` reports only the pre-existing `Config/DefaultGame.ini:121: new blank line at EOF`.
- User must manually accept actual wheel feel/speed, all 42 records on Type01/Type02, thumbnail framing, sleeve/body/material clipping, Gloves/Watch, Torso+Legs, Torso+Vest and persistence after closing/restarting. No visual PASS is claimed; exhaustive per-item/per-combination visual correctness remains unproven.

## Upper Equipment single Torso catalog integration - 2026-07-16

- Found and retained the user-authored `VB_RightPanelUpperEquipment` tree in `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2`: title/divider, current-selection row, `Overlay_UpperItemsArea`, `ScrollBox_TorsoItems -> UniformGrid_TorsoItems`, exactly `DesignSample_TorsoNone` and `DesignSample_TorsoItem`, plus `Txt_TorsoEmptyState`. Names/parents matched the requested structure; Empty State was initially Collapsed. No subcategory buttons, page-local WidgetSwitcher, `Page_Torso`, replacement UI or runtime fallback composition was added. The default first category remains `МОИ ПЕРСОНАЖИ`; `RefreshMyCharactersPage()` was not added to `NativeConstruct()`.
- `UpperGear` now uses the existing category router and `WS_RightPanelPages->SetActiveWidget(VB_RightPanelUpperEquipment)`. First open refreshes Current Selection, builds the catalog and scrolls once; repeat opens rebuild stable cards without Tick-driven work or repeat scrolling.
- Exact Asset Registry audit: `42` upper-shaped `Clothes_Modules` SkeletalMesh candidates on `/Game/Modular_Workers/Mesh/Male/SK_Male_Quantum_Character_Skeleton`, plus complete-overall candidates. Evidence with object paths, skeleton, bounds and embedded material slots: `Saved/CodexDiagnostics/UpperEquipment_20260716/torso_candidate_audit.json`. All Clothes bounds are upper-body-only; `Overalls_*_Pants` and `Overalls_*_Boots` contain lower body/feet and are classified `EWorkerSlot::FullOutfit`, not Torso.
- Manual Torso remains default-deny and reuses the existing independently `VerifiedIntact` clothing evidence without changing Random eligibility. Exact allowlist (`11`): `Jacket_Worker_Hood_1/{SKM_Jacket_Worker_Hood_1_Bege,_Blue,_Gray}`, `Jacket_Worker_Hood_2/{SKM_Jacket_Worker_Hood_2_Bege,_Blue,_Gray}`, `Shirt_RolledUp/{SKM_Shirt_RolledUp_RedCage,_White_Cage}`, and `Tshirt/{SKM_TShirt_Bege,_Black,_Blue}` under `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/`. Every path is stored as a full object path.
- Denied Torso-shaped candidates (`31`): all `Jacket_M65` (Bege/Black/Green), all `Tshirt_Tucked` (Bege/Black/Blue/Green/Grey/Red/White), non-cage `Shirt_RolledUp` (Bege/Black/Blue/Green/Red/White), torn `Tshirt` (Gray/Green/Red), every plain `Jacket_Worker` variant, and Hood_1/Hood_2 Green/LightGr/Orange. Reasons are the existing exact-path `OpacityMaskDamage`, `Authored ripped material-mask`, `SleeveCuffDamage`, or shared-geometry quarantine findings; global eligibility was not modified.
- FullOutfit candidates retained for the future `ЦЕЛЬНЫЕ КОМПЛЕКТЫ` page: every SkeletalMesh under `/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants`, `Overalls_Shirt_Pants_Worker`, `Overalls_Worker_Tshirt`, `Overalls_Worker_Shirt`, `Overalls_Tshirt_Boots`, and `Overalls_Shirt_Boots`. They were denied from this page because their geometry contains pants and/or footwear, irrespective of naming.
- Each allowed color is a distinct SkeletalMesh with its exact embedded MaterialInstance. No runtime override is required, but `FAvTorsoCatalogItem.ExactMaterialOverrides` records the audited material mapping. Examples: `SKM_TShirt_Bege -> MI_TShirt_Bege`, `SKM_Shirt_RolledUp_RedCage -> MI_Shirt_RolledUp_RedCage`, Hood_1/Hood_2 variants -> matching `MI_Jacket_Worker_{Bege,Blue,Gray}` (Hood_2 has two slots using the same exact instance).
- Catalog record fields: exact mesh/material paths, Russian Display/Current Selection names, Type01/02 thumbnails, None/enabled/disabled reason and Gloves/Watch/Vest compatibility metadata. Russian labels use color plus product type: `... ФУТБОЛКА`, `... РАБОЧАЯ РУБАШКА`, `... РАБОЧАЯ КУРТКА С КАПЮШОНОМ`, `... УТЕПЛЁННАЯ РАБОЧАЯ КУРТКА`. Total `12`, real items `11`, first item `НЕТ`.
- Selection applies only `EWorkerSlot::Torso` through the existing `CommitActiveCharacterAppearance(...ManualCustomized)` autosave path. Reselect is a no-op. `НЕТ` clears only Torso; HeadType, Gloves, Watch, Vest, Legs, Feet and unrelated slots are untouched. Unknown legacy Torso paths display `НЕДОСТУПНЫЙ ВАРИАНТ` until a valid new choice. No enum order, SaveGame schema, Reset, Random policy, preset state or Head/Face/Hands logic changed.
- No compatibility was pre-blocked without proof. Records retain item-level Gloves/Watch/Vest metadata for future proven conflicts; no automatic removal exists. Hood items retain existing headgear-family validation behavior. Gloves/Watch/Vest combinations remain for user visual acceptance.
- Runtime uses only `RuntimeTorsoCards`; rebuild/destruct removes tracked/name-prefixed runtime Torso cards, never calls `ClearChildren()` and never removes authored samples. Two-column layout is `row=VisibleIndex/2`, `column=VisibleIndex%2`. Runtime technical test passed counts `12 -> 12 -> 12`, authored page active, samples Collapsed, representative Torso applied, reselect stable, `НЕТ` cleared Torso, and Current Selection updated. Evidence: `runtime_verification.json`.
- Designer samples are configured in `NativePreConstruct`: `DesignSample_TorsoNone = НЕТ` with the None Type01 thumbnail and `DesignSample_TorsoItem` uses the first curated real Torso item. They remain visible/editable in Designer, are Collapsed at runtime, excluded from `RuntimeTorsoCards`, and receive no runtime handler.
- Created/imported exactly `24` Torso Texture2D assets under `/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/UpperEquipment/Torso/{HeadType01,HeadType02}`: None pair plus 11 product pairs. All are `512x512`, export alpha `255`, missing/stale/duplicate paths `0`, redirectors `0`. Lighting remained exposure `0.95`, key `21.2`, fill `12000`, soft key `72000`, soft fill `25600`, rim `14400`, ambient `0.55`, FinalColorLDR and opaque blue-gray backdrop. Torso camera: FOV `34`, distance `180`, focus Z `125`. Control capture and full manifests: `control_capture_manifest.json`, `capture_manifest.json`; registry proof: `asset_verification.json`.
- Final cold compile: `WBP_HeadCatalogItemCard = Success`, `WBP_CharacterCustomizationRoot_V2 = Success`; added-without-GUID `0`, deleted-still-referenced GUID `0`, duplicate GUID `0`, BindWidget `0`, missing-widget `0`, invalid-parent `0`, Widget reconstruction warnings `0`. Final UE 5.8 UBT after the last C++ change: exit code `0`, `Result: Succeeded`, 6.16s; no C++ changed afterward.
- Backup paths outside Content: `Saved/CodexBackups/UpperEquipment_20260716/AvariikaCompany.prechange.sav` SHA-256 `2B0670D9C169F67D5D197CAC2F56585BCC5F87DDC7CBCDF7D9B26DCE41F413DF`; `WBP_CharacterCustomizationRoot_V2.prechange.uasset` SHA-256 `8992267962D92B774189F8D00E2AD04508F3312AD7AF6E78F571D89A0DF13E96`. PIE stopped and user SaveGame restored byte-for-byte; working SHA matches backup. Final dirty content/maps `0/0`, Editor responsive before normal close.
- Visual and functional acceptance remains with the user. Manually inspect all 11 products for Type01/02 skin, final thumbnail framing, sleeve/body clipping, Gloves/Watch seams, Torso+Legs and Torso+Vest combinations, persistence after restart, and hood/headgear conflicts. No visual PASS is claimed; unblocked accessory combinations and exhaustive per-item physical clipping remain visually unproven.

## Hands glove catalog compatibility and text overflow fix - 2026-07-16

- This was a narrow follow-up on the existing authored `РУКИ И АКСЕССУАРЫ` page. Its Widget Tree, two pages, grids, filters and four existing Designer samples were retained; no authored widget was added, renamed or reconstructed. Prechange backups outside Content: `Saved/CodexBackups/HandsCompatibility_20260716/AvariikaCompany.prechange.sav` (SHA-256 `95564F238F2928041C62E22BD9EFC62BD2832C1D05F1D353885C6645B3F0F352`), `WBP_CharacterCustomizationRoot_V2.prechange.uasset` (SHA-256 `0F6EA089424896D86FB8B17DBD15E5E0D30A0986B511012D7E283A0FFFEAF153`), and `WBP_HeadCatalogItemCard.prechange.uasset` (SHA-256 `9BF116EB2341AB8A70DA62A5FE724A45E917DC5F130FCE105EF2DF174913A9E7`).
- Removed the wrist-covering `/Gloves_Worker/SKM_Gloves_Worker` and `SKM_Gloves_Worker_Brown` from the user-facing catalog and restored default-deny/quarantine behavior for them. Their mesh assets were not deleted. Their four obsolete generated yellow/brown thumbnail packages were removed with the purpose-built Unreal MCP `delete_asset` tool after all WBP references had been replaced.
- The exact Gloves allowlist now contains four distinct SkeletalMesh assets, not one mesh with runtime material overrides: `/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker_Print/SKM_Arms_Gloves_Print_Gray`, `_Green`, `_Orange`, `_Yellow`. All use `/Game/Modular_Workers/Mesh/Male/SK_Male_Quantum_Character_Skeleton`, the shared `PA_Arms_Gloves_Print_PhysicsAsset`, identical bounds/geometry architecture and one `M_Gloves_Print` slot. Their exact embedded instances are respectively `MI_Gloves_Worker_Print_Gray`, `_Green`, `_Orange`, `_Yellow` under `/Game/Modular_Workers/Materials/Arms_Modules/Gloves_Worker_Print/`. The compact identical bounds (`X±55.505, Y±9.408, Z±10.173` around wrist height Z 99.767) identify a bilateral hand/wrist glove module rather than full arms; all use the existing leader-pose Gloves component path. Audit evidence: `Saved/CodexDiagnostics/HandsCompatibility_20260716/audit.json`.
- Catalog order is exactly `НЕТ`, `БЕЛЫЕ ПЕРЧАТКИ`, `ЗЕЛЁНЫЕ ПЕРЧАТКИ`, `ОРАНЖЕВЫЕ ПЕРЧАТКИ`, `ЖЁЛТЫЕ ПЕРЧАТКИ`; Gloves count is 5. Watches composition and `EWorkerSlot::Watch` are unchanged, count 5. Card Watch names are now `БЕЖЕВЫЕ — ЛЕВАЯ`, `ЧЁРНЫЕ — ЛЕВАЯ`, `БЕЖЕВЫЕ — ПРАВАЯ`, `ЧЁРНЫЕ — ПРАВАЯ`; Current Selection uses the fuller `..., ЛЕВАЯ/ПРАВАЯ РУКА` forms. No enum order, SaveGame semantics, Random, Reset, Head or Face Protection catalog changed.
- `FAvHandsAccessoriesCatalogItem` now carries a separate Current Selection label and `bCompatibleWithWatch`. Runtime item-level compatibility is re-evaluated both when cards are built and again on click. `НЕТ` is always enabled. A Gloves record marked incompatible is disabled while Watch is selected with `НЕСОВМЕСТИМО С ВЫБРАННЫМИ ЧАСАМИ`; any Watch product is disabled when a legacy/non-allowlisted glove path is present with `СНИМИТЕ ПЕРЧАТКИ`. Disabled clicks are no-ops. Neither category button is blocked, neither slot is auto-cleared, and the other product is never silently replaced. Legacy unknown Gloves display `НЕДОСТУПНЫЙ ВАРИАНТ` until the user explicitly selects a valid item.
- The complete 4×4 matrix passed technically. Every Print glove was combined in turn with all four unchanged `SKM_Watches_Military_{L/R}_{Bege/Black}` items; all 16 entries had both exact meshes simultaneously present in preview components, and selecting either side never removed the other. Visual wrist clipping remains for user acceptance. Evidence: `runtime_verification.json`.
- Captured/reimported the exact 10 Gloves textures: None Type01/02 plus Gray/Green/Orange/Yellow Type01/02. All capture sources are 512×512 with sampled alpha min/max 255, the accepted Hands lighting/backdrop/camera profile, no black lower area or horizontal backdrop line. Watches were not recaptured. Cold registry reports Gloves textures 10, redirectors 0 and no obsolete yellow/brown catalog textures. Evidence: `capture_manifest.json`, `import_manifest.json`, `cold_verification.json`.
- Existing `DesignSample_GlovesNone` now shows `НЕТ` with the new None Type01 thumbnail; existing `DesignSample_GlovesItem` shows `БЕЛЫЕ ПЕРЧАТКИ` with Gray/white Type01. No sample was created. Runtime still collapses samples and excludes them from tracked arrays/selection.
- Reusable card overflow was fixed without changing card width or the two-column grid: `Txt_CardTitle` is single-line, Auto Wrap false, Ellipsis, ClipToBounds, full-name tooltip, and retains authored right padding 44 px for the selection circle. The same persisted WBP property applies to Head, Face Protection and Hands cards. `Txt_HandsCurrentSelectionValue` is single-line, Ellipsis, ClipToBounds, full-value tooltip and remains in its constrained HorizontalBox Fill slot; `ТЕКУЩИЙ ВЫБОР:` was not moved. Runtime audit verified all 10 populated Hands card titles use Ellipsis with at least 44 px right padding.
- Runtime switching Gloves→Watches→Gloves retained stable card counts 5/5 without duplicates and only tracked runtime children are removed; no `ClearChildren()` path was added. `НЕТ` clears only its own slot, reselect is a no-op, and manual commit/autosave remains unchanged. Original SaveGame was restored byte-for-byte after testing; working/backup SHA-256 both equal `95564F238F2928041C62E22BD9EFC62BD2832C1D05F1D353885C6645B3F0F352`.
- Final full UE 5.8 UBT after the last C++ change: exit code 0, `Result: Succeeded` (24.66s); no C++ changed afterward. Cold Blueprint Compile: `WBP_HeadCatalogItemCard = Success`, `WBP_CharacterCustomizationRoot_V2 = Success`. Cold log has 0 added-without-GUID, deleted-still-referenced GUID, duplicate GUID, BindWidget, missing-widget, invalid-parent and runtime-reconstruction errors. Final state: PIE=false, Editor responsive, Save All complete, dirty content/maps 0/0.
- Visual and functional acceptance remains with the user. Manually verify the four colors, sleeve/body seams, every glove with left/right Watches, watch-face readability, final ellipsis/tooltips and persistence after a normal restart. No visual PASS is claimed; exhaustive physical clipping and duplicate underlying hand geometry cannot be conclusively accepted without the user's visual review.

## Hands Accessories Gloves and Watches integration - 2026-07-15

- The user-authored `РУКИ И АКСЕССУАРЫ` tree in `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2` was found intact and retained: `VB_RightPanelHandsAccessories`, its two authored filter buttons/current-selection area, and `WS_HandsAccessoriesSectionPages` with exactly `0=Page_Gloves`, `1=Page_Watches`. All requested grids, ScrollBoxes, Empty States and four Designer samples had the requested parents; no replacement/runtime tree or `ClearChildren()` path was introduced. Baseline compiles for this WBP and `WBP_HeadCatalogItemCard` were Success and GUID errors were 0.
- Backups remain outside Content: `Saved/CodexBackups/HandsAccessories_20260716/AvariikaCompany.prechange.sav`, SHA-256 `5CF7025933F56945647BC9D459A8B81993963B9634B106C939861ECEC5C399E3`; and `WBP_CharacterCustomizationRoot_V2.prechange.uasset`, SHA-256 `642DEDE877CA4460572A9A5DE2E0404B1EB7E079B522E49FBC871C4150184148`. Runtime selection/autosave was exercised, PIE stopped, and the original SaveGame was restored byte-for-byte; working and backup SHA match.
- Gloves audit found two paired product meshes under `Gloves_Worker`: exact allowlist `/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker/SKM_Gloves_Worker` and `.../SKM_Gloves_Worker_Brown`. Both use the Quantum modular skeleton/leader-pose architecture and represent intentional fingerless work gloves. The previous Random quarantine/damage evidence was not erased; only these exact paths were admitted to the manual catalog following the user's functional confirmation. Four `Gloves_Worker_Print/SKM_Arms_Gloves_Print_{Gray,Green,Orange,Yellow}` whole-arm meshes remain default-denied because independent combination eligibility was not proven. Gloves uses the existing `EWorkerSlot::Gloves`; no duplicate slot was created.
- Watches audit found four compatible Quantum modular meshes, all allowlisted exactly: `/Game/Modular_Workers/Mesh/Male/Arms_Modules/Watches/SKM_Watches_Military_L_Bege`, `_L_Black`, `_R_Bege`, `_R_Black`. The CitizenNPC `MI_glass_watch` match is material-only and denied; `SKM_Quantum_Wrist` variants are base body wrist modules, not watch products, and denied. `EWorkerSlot::Watch` was appended strictly after `Headphones`; all earlier numeric values remain unchanged.
- `FWorkerAppearance` stores slots in its existing map. Old SaveGames lack a Watch key, so `Get(Watch)` resolves to None without changing other entries. Generic component lookup/application, preview, legacy UI, F9 inspector, validation, diagnostics and slot iteration now include Watch. Copy/equality/reset continue through the map; Reset gives None. Random was not extended, so Gloves and Watch remain None under Random. Gloves and Watch selection mutate only their own slot and coexist independently.
- Structured Hands catalog contains 3 Gloves records (`НЕТ`, `ЖЁЛТЫЕ РАБОЧИЕ ПЕРЧАТКИ`, `КОРИЧНЕВЫЕ РАБОЧИЕ ПЕРЧАТКИ`) and 5 Watches records (`НЕТ` plus beige/black, left/right curated Russian names). Each enabled product has section, exact slot/mesh, exact Type01/Type02 thumbnail, enabled/None fields and default-deny validation. Re-selecting the current value is a no-op; `НЕТ` clears only the active Gloves or Watch slot. Real selection uses the existing manual commit/autosave path and does not cross Active/Preset appearance state.
- Created/imported 16 exact Hands Accessories Texture2D assets: Gloves 6 and Watches 10, including separate None pairs. Lighting stayed at exposure `0.95`, key `21.2`, fill `12000`, soft key `72000`, soft fill `25600`, rim `14400`, ambient `0.55`, `FinalColorLDR`, opaque backdrop/alpha 255. Category camera is FOV 34, distance 150, focus Z 105; an initially clipped Watch capture was rejected and recaptured before import. Cold exports of all 16 loaded Texture2D assets prove `512x512`; registry count is 16, redirectors 0, duplicates/missing exact paths 0. Evidence: `Saved/CodexDiagnostics/HandsAccessories_20260716/{capture_manifest,import_manifest,cold_verification}.json` and `ColdExports/`.
- The four existing authored samples were not recreated or renamed. `NativePreConstruct` gives Gloves/Watch None plus one real representative each, using correct Type01 textures. They remain visible in Designer, are Collapsed by exact binding in runtime, excluded from runtime arrays and have no selection handler. Both authored Empty States default Collapsed and runtime logic shows each only if its actual section catalog is empty.
- `SelectHandsAccessoriesSection()` owns selected section, the `0/1` switcher mapping, `RefreshHandsAccessoriesFilterVisuals()`, Current Selection, active-only rebuild and ScrollBox top. The visual helper updates both actual authored button frame/fill/icon/text sets with the established Head/Face orange selected and neutral unselected style; no Tick/ForceLayoutPrepass path exists.
- Runtime cards are tracked per `EAvHandsAccessoriesSection`; a rebuild removes only tracked/name-prefixed runtime cards for that section, preserves Designer samples and lays out two columns. Short runtime verification passed `0→1→0`, selected color following the switcher, stable counts Gloves 3 / Watches 5, samples Collapsed, Current Selection updates, and no duplicate accumulation. It also proved Gloves and Watch can be applied together and cleared independently. Evidence: `runtime_verification.json`.
- Final full UE 5.8 UBT after the last C++ camera change: exit code 0, `Result: Succeeded` (4.29s); no C++ changed afterward. Final cold Blueprint Compile: `WBP_HeadCatalogItemCard = Success`, `WBP_CharacterCustomizationRoot_V2 = Success`; startup-log audit found 0 GUID, duplicate-GUID, BindWidget, missing-widget, invalid-parent or reconstruction diagnostics. Interactive cold verification ended PIE=false, Editor responsive, Save All complete and dirty content/maps `0/0`. A later read/export-only commandlet transiently dirtied eight unrelated third-party Niagara/SkeletalMesh packages at startup; none was saved and this does not alter the recorded interactive 0/0 state.
- Visual and functional acceptance remains with the user. Manually verify framing/readability, glove appearance and sleeves, Watch face/strap readability, left/right variants, Gloves+Watch clipping, all selection/current-label behavior and persistence after normal restart. No visual PASS is claimed; exhaustive per-sleeve/per-item physical compatibility remains visually unproven.

## Face Protection thumbnails, filter state and Headphones integration - 2026-07-15

- Scope stayed on the existing `ЛИЦО И ЗАЩИТА` implementation. The GUID repair was not repeated: baseline and final cold compiles had GUID errors `0`, and the existing authored tree was retained. Prechange backups are `Saved/CodexBackups/FaceProtectionHeadphones_20260715/WBP_CharacterCustomizationRoot_V2.prechange.uasset` (SHA-256 `9F641B50FE5D17FE4EEBE31F365815950875E7511EB5EBBFFD7AB76E7AEB1BE8`) and `AvariikaCompany.prechange.sav` (SHA-256 `0DA35684D8E3B1DC0DD0287A0FA414490659D2632BF903C5B183AD6F1CE1BAFF`). The 22 replaced thumbnail packages also remain under `Saved/AssetBackups/FaceProtectionHeadphones_20260715/Thumbnails`.
- The black lower half and horizontal edge were already baked into all 22 Glasses/Respirator source textures. `/Engine/BasicShapes/Plane` is an XY plane, but `AAvCustomizePreviewActor` had rotated the opaque thumbnail backdrop with yaw `FRotator(0,90,0)`, leaving it horizontal and intersecting the portrait. The canonical fix is pitch `FRotator(90,0,0)`. Camera and accepted capture profile were not retuned: exposure `0.95`, key `21.2`, fill `12000`, soft key `72000`, soft fill `25600`, rim `14400`, ambient `0.55`, `FinalColorLDR`, FOV `28`, distance `112`, focus Z `156`, opaque alpha `255`.
- Re-captured/re-imported all exact existing Face Protection targets: Glasses `16` (`T_UI_None_Glasses_Type01/02` plus the Type01/02 pairs generated for Aviator, Aviator Brown, Aviator Gold, Classic, SunGlass, Worker Orange and Worker White); Respirator `6` (`T_UI_None_Respirator_Type01/02` plus Type01/02 pairs for `SKM_Respirator` and `SKM_Respirator_Black`). All are `512x512`; the cold registry audit found all `40` total Face Protection textures, redirectors `0`, missing paths `0`. Evidence and the complete exact-path list are in `Saved/CodexDiagnostics/FaceProtectionHeadphones_20260715/capture_manifest.json`, `import_manifest.json` and `cold_verification.json`.
- Selected-state was stuck on Glasses because switching the `WidgetSwitcher` did not refresh all three copied button internals. All three handlers now call the single `SelectFaceProtectionSection(NewSection)` path. It stores `SelectedFaceProtectionSection`, maps Glasses/Respirator/Headphones to indices `0/1/2`, calls `RefreshFaceProtectionFilterVisuals()`, refreshes Current Selection, rebuilds only the active section and scrolls its ScrollBox to the top. The helper passes the actual authored frame/fill/icon/text widgets to the same `ConfigureHeadFilterButton` styling used by the Head filters; there is no Tick update or replacement button.
- Short runtime verification passed indices `0→1→2→0`, active frame colors Glasses→Respirator→Headphones→Glasses, and stable runtime card counts `8/3/9` with no accumulation. Current Selection refreshes immediately per active section. Evidence: `runtime_verification.json`.
- Added `EWorkerSlot::Headphones` strictly after the previous last member `FullOutfit`; existing numeric values were not changed. `FWorkerAppearance` already serializes slots as a map, so an old SaveGame simply has no Headphones key and `Get(Headphones)` resolves to None. The task-start old SaveGame loaded during cold PIE, selection/autosave was exercised, then the original backup was restored byte-for-byte. Random generation was not extended, so Headphones remains None under Random.
- Curated exact-path allowlist contains eight canonical Modular Workers meshes on `/Game/Modular_Workers/Mesh/Male/SK_Male_Quantum_Character_Skeleton`: `Headphones_Workers/SKM_Headphones_Worker_Green`, `_Orange`, `_White`, `_Yellow`; and `Headphones_Workers_Micro/SKM_Headph_Worker_Mic_Gr`, `_Orn`, `_Wht`, `_Yel`. All use the existing modular leader-pose architecture. `/Game/CitizenNPC/CharacterParts/Meshes/UE4_Mannequin/Hats_Hair/SK_ma_hat_headset_01` is denied because it uses the incompatible 68-bone CitizenNPC skeleton. Default deny is enforced in `ValidateHeadCustomizationSelection`; no substring candidate enters the catalog.
- Headphones catalog has `9` enabled records: `НЕТ` plus eight items named `ЗЕЛЁНЫЕ/ОРАНЖЕВЫЕ/БЕЛЫЕ/ЖЁЛТЫЕ РАБОЧИЕ НАУШНИКИ` and `ЗЕЛЁНАЯ/ОРАНЖЕВАЯ/БЕЛАЯ/ЖЁЛТАЯ ГАРНИТУРА`. Each record uses `WorkerSlot=Headphones`, an exact mesh path and exact Type01/Type02 texture. Created `18` Headphones thumbnails (None pair plus eight product pairs), with short stable object names `T_UI_HP_<Mesh>_Type01/02`; an intermediate overlong-name import was removed through `EditorAssetLibrary` before the final import. No global Hair, Headgear, Glasses or FaceMask clearing was added.
- `НЕТ` clears only `EWorkerSlot::Headphones`; selecting the current value is a no-op; item selection uses the existing `CommitManualAppearance`/autosave path and marks the state ManualCustomized. A short technical selection applied exact `SKM_Headphones_Worker_Green` to the preview, changed Current Selection, and then removed it with `НЕТ`; evidence: `headphones_selection_verification.json`. The user SaveGame was restored afterward and working/backup SHA-256 both equal `0DA35684D8E3B1DC0DD0287A0FA414490659D2632BF903C5B183AD6F1CE1BAFF`.
- Added exactly two authored Headphones samples via official UE Widget Blueprint editor utilities: `DesignSample_FaceHeadphonesNone` and `DesignSample_FaceHeadphonesItem`, under the existing `UniformGrid_HeadphonesItems` at row 0 columns 0/1. `NativePreConstruct` supplies the None and representative real item previews. All six Face Protection samples are collapsed by exact name at runtime, excluded from runtime arrays and have no selection handlers. The existing Widget Tree/pages/grids were not rebuilt and no `ClearChildren()` path was introduced.
- C++ integration covers catalog/validation, runtime and preview slot application, legacy customization UI, `AvWear` parsing and F9 inspector loops/labels. No appearance fields were reordered and no unrelated Head/Random/preset logic changed. Final full UE 5.8 UBT after the last C++ change: exit code `0`, `Result: Succeeded` (`5.99s`). No C++ changed after that build.
- Cold Blueprint Compile: `/Game/Avariika/UI/CharacterCustomization/WBP_HeadCatalogItemCard = Success`; `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2 = Success`. Final cold log has `0` added-without-GUID, deleted-still-referenced GUID, duplicate GUID, BindWidget, invalid-parent and runtime-reconstruction errors. Cold audit: dirty content/maps `0/0`, redirectors `0`, orphan names `0`.
- Visual acceptance remains with the user. Manually verify final crop/readability of all portraits, headset appearance on both head types, clipping with individual Hair/Headgear combinations, all item selections, Current Selection Russian labels and persistence after a normal close/reopen. No visual PASS is claimed; coexistence of every individual mesh/hair/helmet combination remains visually unproven.

## Widget Blueprint GUID integrity repair - 2026-07-15

- Scope was limited to `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2`; selected-state work, C++, catalogs, thumbnails, appearance slots, SaveGame and gameplay logic were not changed.
- Full unique GUID error list from Message Log/current Editor log: `Widget [HB_FaceCurrentSelection] was added but did not get a GUID`; the same error for `DesignSample_FaceGlassesNone`, `DesignSample_FaceGlassesItem`, `DesignSample_FaceRespiratorNone` and `DesignSample_FaceRespiratorItem`; plus `Variable [...] was deleted but still has a GUID referenced` for `HB_FaceFilterCurrentSelection`, `DesignSample_HeadType01_1` and `DesignSample_HeadType02_1`. No additional unique GUID error was present.
- Exact cause was the preceding authoring script `Saved/CodexDiagnostics/BeigeCapFaceProtection_20260714/author_face_protection_designer_samples.py`: it renamed three source widgets with raw `UObject.rename()` and created the two Respirator samples with `unreal.new_object()` plus direct `add_child_to_uniform_grid()`. Those operations bypassed `UWidgetBlueprint::WidgetVariableNameToGuidMap`, leaving five new names unregistered and the three old names registered.
- UE 5.8 `FWidgetBlueprintCompilerContext` is the canonical repair path here: its built-in variable-GUID validation adds missing source-widget entries and removes keys absent from the source tree. The compiler fixup was allowed to repair the registry, followed by repeated compile and save. No GUID was manually invented or assigned.
- All five affected widgets were restored in place; none was recreated during this repair. `HB_FaceCurrentSelection` remains under `VB_FaceCurrentSelectionArea`. The two Glasses samples remain in `UniformGrid_GlassesItems` at row 0, columns 0/1; the two Respirator samples remain in `UniformGrid_RespiratorItems` at row 0, columns 0/1. Classes, hierarchy, Fill alignments, visibility, content and layout were preserved. The authored Widget Tree was not rebuilt and no runtime composition was added.
- Orphaned GUID registry entries for `HB_FaceFilterCurrentSelection`, `DesignSample_HeadType01_1` and `DesignSample_HeadType02_1` were removed by the compiler fixup. Cold source-tree audit reports each old name count `0`, each required target count `1`; no duplicate GUID, missing widget, BindWidget, invalid-parent or runtime-reconstruction warning was emitted.
- Compile before restart: `WBP_CharacterCustomizationRoot_V2 = Success` on three consecutive compiles after the repair/save, with no new GUID diagnostics.
- Cold restart was performed through normal `SystemLibrary.quit_editor()` after `Save All=true`, `PIE=false`, dirty content/maps `0/0`. Cold Editor PID `5052` is responsive. Compile after restart: `WBP_HeadCatalogItemCard = Success`; `WBP_CharacterCustomizationRoot_V2 = Success`. New startup log contains 0 GUID errors in the required categories.
- Binary pre-repair backup: `Saved/CodexBackups/WidgetGuidRepair_20260715/WBP_CharacterCustomizationRoot_V2.uasset`; SHA-256 `2685DFC0DB7871CA9AC510BB9AD5E59D00DB7747312633D26CE24D85115F627D` (940227 bytes), matching the source at backup time. No redirector or Content Browser duplicate was created.
- C++ changed: no. UBT was therefore not run. Final technical state: `PIE=false`, Save All completed, dirty content packages `0`, dirty map packages `0`; no commit was created. Visual acceptance of Designer placement/layout and PIE runtime collapsing remains with the user; no visual PASS is claimed.

## Beige cap thumbnail fix and Face Protection page integration - 2026-07-14

- This continuation preserved the working Head page, its four authored `WS_HeadSectionPages`, Headgear/Hair/Beard population, card selected indicator, live preview, preset/active-appearance boundary, Random/Reset/F9 behavior and SaveGame schema. The user-authored Face Protection Widget Tree was found and retained; no replacement page, runtime Widget Tree construction or `ClearChildren()` composition was introduced.
- The beige-cap defect was isolated to the Type02 source. `T_UI_Headgear_Cap_SKM_Cap_Bege_Type02` had been captured against the default-lit `/Engine/BasicShapes/Cube`: the approved high-intensity thumbnail lights illuminated its faces differently, producing the visible grey/checker-like backdrop and lower boundary. This was a physical backdrop/material artifact, not a catalog-path or alpha-selection error. Type01 already had the uniform Headgear background and was not captured/imported; its package SHA-256 remains `A3218DBC5CB590A76451EEC8A1AE10F8FF65E8FA7CBB406CA802382835B6BB58`.
- Only `/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/Headgear/HeadType02/T_UI_Headgear_Cap_SKM_Cap_Bege_Type02` was re-captured and re-imported. The capture used the approved Headgear profile: exposure `0.95`, key `21.2`, fill `12000`, soft key `72000`, soft fill `25600`, rim `14400`, ambient `0.55`, `FinalColorLDR`, FOV `28`, distance `112`, focus Z `156`, and forced alpha `255`. Its source SHA-256 is `F5FB23105954247BBBD2EF97A6286F8883778883034AA495FFC5093530B74221`; the final package SHA-256 is `24A8AAA59625427C8297F37E90C7F92416472D6C7B4BA5FE3E612E7BB8E7A48C`.
- Future thumbnail capture now uses the project-owned unlit opaque plane `/Game/Avariika/UI/CharacterCustomization/Materials/MI_AvThumbnailBackdrop_Unlit` (parent `M_AvThumbnailBackdrop_Unlit`) instead of the lit cube. The calibrated material color is `(0.10,0.11,0.13,1)`. The fixed PNG has alpha `255` at every pixel and technically matches the dark Headgear background samples. Cold verification found the exact Texture2D path, one asset with that name, no redirector, no stale copy, Type01 unchanged, and dirty packages `0/0`. Evidence: `Saved/CodexDiagnostics/BeigeCapFaceProtection_20260714/beige_cap_capture_manifest.json`, `beige_cap_import_manifest.json`, and `beige_cap_cold_verification.json`; the prechange Type02 package remains under `Saved/AssetBackups/BeigeCapFaceProtection_20260714/BeigeCap`.
- The authored `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2` Face Protection structure is present. Both target WBP assets compiled `Success` before C++ changes. `WS_FaceProtectionSectionPages` has exactly `0=Page_Glasses`, `1=Page_Respirator`, `2=Page_Headphones`, default active index `0`; the title is `ЛИЦО И ЗАЩИТА`, current value defaults to `НЕТ`, filter labels are `ОЧКИ` / `РЕСПИРАТОР` / `НАУШНИКИ`, and all three authored Empty States use `НЕТ ДОСТУПНЫХ ВАРИАНТОВ`. The switcher/pages/ScrollBoxes retain authored Fill alignment. The sole harmless naming mismatch `HB_FaceFilterCurrentSelection` was renamed to `HB_FaceCurrentSelection`; the hierarchy was not rebuilt.
- Required `BindWidget` coverage was added for `WS_FaceProtectionSectionPages`; all three filter buttons; `Txt_FaceCurrentSelection` / `Txt_FaceCurrentSelectionValue`; and the ScrollBox/UniformGrid/EmptyState triplets for Glasses, Respirator and Headphones. `VB_RightPanelFaceProtection` is also bound so `SelectCategory(FaceProtection)` activates the user-authored panel. `GetFaceProtectionSectionPageIndex()` owns `0/1/2`, while dedicated grid/ScrollBox/EmptyState helpers own the section-to-container mappings.
- Asset Registry/default-deny audit approved seven canonical-skeleton Glasses meshes for `EWorkerSlot::Glasses`: Aviator, Aviator Brown, Aviator Gold, Classic, SunGlass, Worker Orange and Worker White under `/Game/Modular_Workers/Mesh/Male/Face_Modules`. It approved two Respirator meshes for `EWorkerSlot::FaceMask`: `SKM_Respirator` and `SKM_Respirator_Black`. Every enabled product is a curated exact object path already carrying `VerifiedIntact` eligibility/evidence; substring matches are not admitted.
- Curated Russian Glasses names are `ОЧКИ АВИАТОР`, `АВИАТОР КОРИЧНЕВЫЕ`, `АВИАТОР ЗОЛОТЫЕ`, `КЛАССИЧЕСКИЕ ОЧКИ`, `СОЛНЦЕЗАЩИТНЫЕ ОЧКИ`, `ЗАЩИТНЫЕ ОЧКИ ОРАНЖЕВЫЕ`, and `ЗАЩИТНЫЕ ОЧКИ БЕЛЫЕ`. Respirator names are `РЕСПИРАТОР` and `ЧЁРНЫЙ РЕСПИРАТОР`. Each enabled section begins with its own `НЕТ` record.
- Eight canonical-skeleton Headphones candidates were found: four `SKM_Headphones_Worker_{Green,Orange,White,Yellow}` and four `SKM_Headph_Worker_Mic_{Gr,Orn,Wht,Yel}` under the Modular Workers `Head_Modules` folders. A CitizenNPC headset candidate uses the wrong skeleton. There is no existing `Headphones`/`HearingProtection` slot, and coexistence or intended mutual exclusion with Headgear is not proven. Therefore no enum member, component, appearance field or serialization field was added, and Headphones was not incorrectly aliased to Headgear. Its page is fully wired but intentionally shows its per-section Empty State; this is the remaining blocker pending product/slot intent.
- `EAvFaceProtectionSection` and `FAvFaceProtectionCatalogItem` provide the structured catalog fields requested by the task: section, Worker slot, exact mesh path, curated display name, exact Type01/Type02 thumbnails, None flag, enabled flag and disabled reason. `UAvHeadCatalogItemButton` remains the shared reusable WBP base and now carries either a Head payload or a Face Protection payload into the root handler.
- Runtime cards are separated in `TMap<EAvFaceProtectionSection, TArray<TObjectPtr<UAvHeadCatalogItemButton>>> RuntimeFaceProtectionSectionCards`. A section rebuild removes only its tracked `FaceProtectionItemCard_*` children, keeps authored samples, uses two columns, and never clears the authored grid. `NativeDestruct()` removes cards from all Face Protection sections. A technical `0 -> 1 -> 2 -> 0` switch cycle retained exactly `8` Glasses cards, `3` Respirator cards and `0` Headphones cards without duplicates.
- Current Selection is recalculated from `ActiveCharacterAppearance` and the exact catalog record on every section switch. It displays `НЕТ` for a null section slot or Headphones, otherwise the curated Russian name. Clicking `НЕТ` clears only the current record's slot; an item click sets only its exact Glasses/FaceMask mesh, uses the existing manual commit/autosave path with `ManualCustomized`, and a repeated current item is a no-op. HeadType, skin presentation and the other Face Protection slot are not changed by this path. Selection mutation was intentionally left for the user's functional acceptance; the technical runtime audit switched filters only.
- Designer parity uses the existing `/Game/Avariika/UI/CharacterCustomization/WBP_HeadCatalogItemCard`. Glasses samples are `DesignSample_FaceGlassesNone` and `DesignSample_FaceGlassesItem` (Aviator); Respirator samples are `DesignSample_FaceRespiratorNone` and `DesignSample_FaceRespiratorItem` (Respirator). They are visible in Designer with real Type01 thumbnails, but exact-name runtime validation collapses them and excludes them from runtime arrays/selection. No Headphones sample was authored because no safe product slot exists.
- Captured/imported exactly `22` new Face Protection Texture2D assets: Glasses `16` (`НЕТ` plus seven products, each Type01/Type02) and Respirator `6` (`НЕТ` plus two products, each Type01/Type02). Headphones created `0`; existing HeadType/Headgear/Hair/Beard assets were not part of this batch. All captures use the common Headgear lighting, `FinalColorLDR`, standard FOV `28` / distance `112` / focus Z `156`, the unlit opaque plane, `512x512`, and alpha `255` for every pixel. The exact product registry never falls back from Type02 to Type01 or to a generic HeadType icon.
- Cold asset verification passed all `22/22`: exact loaded Texture2D object paths, current package hashes matching the post-import hashes, UI texture settings, package saved, registry count exactly `22`, redirectors `0`, duplicate asset names `0`, dirty content/maps `0/0`. Runtime initialization and all three filter switches produced `[AvCustomizeFaceThumbnail]` missing count `0`, BindWidget errors `0`, missing-structure errors `0`, invalid-parent errors `0`, and reconstruction warnings `0`. Evidence is under `Saved/CodexDiagnostics/BeigeCapFaceProtection_20260714`, especially `face_protection_capture_manifest.json`, `face_protection_import_manifest.json`, `face_assets_and_designer_cold_verification.json`, and `face_runtime_wiring.json`.
- Final Blueprint Compile after the cold restart: `/Game/Avariika/UI/CharacterCustomization/WBP_HeadCatalogItemCard = Success`; `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2 = Success`.
- Final full UE 5.8 UBT compiled the last C++ change and linked `UnrealEditor-Avaryo.dll`: exit code `0`, `Result: Succeeded`, `6.31s`. Log: `Saved/CodexDiagnostics/BeigeCapFaceProtection_20260714/UBT_FaceProtection_Final.log`. No C++ changed after this build.
- SaveGame schema was not changed. Working `Saved/SaveGames/AvariikaCompany.sav` and retained task backup `Saved/SaveGameBackups/BeigeCapFaceProtection_20260714/AvariikaCompany.prechange.sav` both have SHA-256 `FE6E6F7AB7F38D37F553BCCD569C0160096DDFF4174A99910657B5EA43599A3C`; no restoration was needed and the backup was not deleted.
- Final technical state: the verified MCP Editor PID `27556` completed Save All with `PIE=false` and dirty content/maps `0/0`, then was closed normally after a second cold Editor appeared. The remaining cold-started Editor PID `23676` is responsive; its separate startup log has PIE starts `0`, Blueprint errors `0`, ensure failures `0`, and save errors `0`. No commit was created; unrelated dirty/untracked worktree state was preserved. Overall `git diff --check` still reports only the pre-existing unrelated `Config/DefaultGame.ini:121 new blank line at EOF`.
- The user must manually accept the repaired beige-cap Type02 background/crop, Type01/Type02 Face Protection portraits, product readability, category button appearance, Face page layout/scrolling, exact selection/`НЕТ` behavior, Glasses/Respirator coexistence, current-selection labels and close/reopen persistence. No visual or functional PASS is claimed by Codex.

## Full HeadType Hair Beard thumbnail batch using approved lighting - 2026-07-14

- The user manually accepted the six Type02 control thumbnails from the preceding control proof. This batch reused that exact approved `CommonHeadgear` capture profile without tuning: manual exposure `0.95`, key `21.2`, fill `12000`, soft key `72000`, soft fill `25600`, rim `14400`, ambient/skylight `0.55`, `FinalColorLDR`, and the shared dark-blue backdrop `(0.018,0.032,0.055)`.
- Category-specific values remained camera-only: HeadType/Hair use FOV `28`, distance `112`, focus Z `156`; Beard uses FOV `27`, distance `92`, focus Z `156.5`. Live preview, `ApplyHeadTypeSkinProfile()`, HeadType material-family mapping, underwear, UI, WidgetSwitcher, ScrollBoxes, selected indicator and SaveGame schema were not changed.
- Captured and imported exactly `30` Texture2D assets: HeadType `2`; Hair `10` item assets plus `2` NONE assets = `12`; Beard `14` item assets plus `2` NONE assets = `16`. The manifest contains `15` Type01 and `15` Type02 records with zero TypeId/path mismatches. All 30 runtime capture log lines report `LightingProfile=CommonHeadgear`, `ExposureBias=0.95` and `CaptureSource=FinalColorLDR`.
- Headgear was not captured or imported. Both capture and import manifests explicitly report no Headgear records; imported Headgear path count is `0`. The already-approved Headgear catalog remains the reference and was left unchanged by this batch.
- Capture/import evidence is under `Saved/CodexDiagnostics/FullHeadTypeHairBeardThumbnailBatch_20260714`. Capture passed `30/30`; import passed `30/30`, changed/saved exactly 30 target packages, and left scoped dirty content/maps empty. Pre-import copies of all 30 targets remain under `Saved/AssetBackups/FullHeadTypeHairBeardThumbnailBatch_20260714/Thumbnails`.
- After a normal Editor close and cold start, `cold_import_verification.json` passed `30/30`. `full_batch_asset_audit.json` also passed `30/30`, confirming exact loaded Texture2D object paths, capture/package/import timestamps, source hashes, current package hashes matching post-import hashes, no redirectors, and exactly one package with each name. UI texture settings remain `sRGB`, `TC_EDITOR_ICON`, `TEXTUREGROUP_UI`, clamp, never-stream and no mipmaps.
- Short runtime registry verification opened all four Head pages without selecting an item: HeadType `2/2`, Headgear `23/23`, Hair `6/6`, Beard `8/8` cards each resolved to exactly one unique category-correct Texture2D; missing/invalid cards `0`, and `[AvCustomizeHeadThumbnail] Missing exact thumbnail` log count `0`. The authoritative result is `runtime_thumbnail_resource_verification.json`. Earlier legacy verifier outputs in the same folder are non-authoritative because they assumed a single transient `Img_CardThumbnail` object per reused card while UE retained multiple WidgetTree instances; their resource paths were present and identical.
- Blueprint Compile after cold reload: `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2 = Success`; `/Game/Avariika/UI/CharacterCustomization/WBP_HeadCatalogItemCard = Success`. Results are recorded in `blueprint_compile_results.json`.
- No C++ changed in this approved full-batch iteration, so the conditional full UBT was not rerun. The authoritative immediately preceding build remains `Saved/CodexDiagnostics/HeadThumbnailExposureControlProof_20260714/ubt_full.log`: exit code `0`, `Result: Succeeded`, `5.57s`; `AvCustomizePreviewActor.cpp` still predates that successful build.
- The technical runtime check changed SaveGame on PIE shutdown; with `PIE=false`, the task-start backup was restored as required. Working file and retained `Saved/SaveGameBackups/FullHeadTypeHairBeardThumbnailBatch_20260714/AvariikaCompany.prechange.sav` both have SHA-256 `B70204202B0BD92D1C083920B710587201126E9C7DB5FE30B9371161FBA11358`.
- Final technical state: cold-started Editor PID `22680` is responsive, `PIE=false`, Save All completed, dirty content packages `0`, dirty map packages `0`. No commit was created and unrelated worktree state was preserved. The user performs visual acceptance of the full HeadType/Hair/Beard set; no visual PASS is claimed.

## Head thumbnail exposure control proof using Headgear reference - 2026-07-14

- The preceding `Head thumbnail exposure rebalance for dark previews` was not visually accepted: the user's screenshots showed HeadType, Hair and Beard still underexposed, especially Type02, while Headgear remained readable and became the reference. The working four-page `WS_HeadSectionPages`, population, selection indicator, card/ScrollBox layout, Russian labels, SaveGame schema and live preview were preserved.
- The previous values did apply, but they were not equivalent to the working Headgear bake. Runtime logs prove that the accepted Headgear capture used `LightingMultiplier=8.0`, manual exposure `0.95`, `FinalColorLDR`, FOV `28`, distance `112`, focus Z `156`; the failed rebalance actually used standard HeadType/Hair `1.75 / 0.55` and Beard `2.25 / 0.80` with a separate lower-face lighting profile. Raising Beard exposure alone therefore could not restore the missing key/fill/ambient detail.
- Capture-vs-import audit proved the failure was in capture, not Texture2D import/cache. The previous batch PNGs were already dark: whole-image/face-crop mean luminance was `6.62/11.06` for HeadType02 and Hair None Type02, `5.70/9.82` for Hair Back Type02, and about `20/23-24` for Beard controls, versus `36.43/56.40` for the accepted Headgear source. `Saved/CodexDiagnostics/HeadThumbnailExposureControlProof_20260714/current_30_asset_audit.json` verifies all 30 prior targets by source/package/import timestamps, exact loaded object path, source and package hashes, no redirector, and exactly one package with each name. All 30 Texture2D packages really had been updated and cold-loaded; there was no stale in-memory copy or second catalog.
- `AvCustomizePreviewActor.cpp` now separates `CommonThumbnailLightingProfile` from `FThumbnailCameraProfile`. All HeadType/Headgear/Hair/Beard captures use the Headgear reference lighting: manual exposure `0.95`, `FinalColorLDR`, key `21.2`, fill `12000`, soft key `72000`, soft fill `25600`, rim `14400`, ambient/skylight `0.55`, key rotation `(-32,-34,0)`, the Headgear light offsets/targets, and backdrop color `(0.018,0.032,0.055)`. Capture logs now print the actual profile, exposure, camera, every light intensity and backdrop value, so future batches can be checked against runtime state rather than assumed multipliers.
- Only framing remains category-specific: `StandardHead` is FOV `28`, distance `112`, focus `(0,0,156)` for HeadType/Headgear/Hair; `BeardCamera` is FOV `27`, distance `92`, focus `(0,0,156.5)`. Beard no longer owns an independent exposure or base lighting profile. `ApplyHeadTypeSkinProfile()`, HeadType Light/Dark mapping, material application order, live character materials, underwear and live-preview restore values were not changed.
- Exactly six Type02 control sources were captured and imported; no full batch was started:
  - `/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/HeadType/T_UI_HeadType_02`
  - `/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/None/Hair/T_UI_None_Hair_Type02`
  - `/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/Hair/HeadType02/T_UI_Hair_Hair_SKM_Hair_Back_Type02`
  - `/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/None/Beard/T_UI_None_Beard_Type02`
  - `/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/Beard/HeadType02/T_UI_Beard_Beard_SKM_Beard_Type02`
  - `/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/Headgear/HeadType02/T_UI_Headgear_Cap_SKM_Cap_Bege_Type02`
- Evidence is under `Saved/CodexDiagnostics/HeadThumbnailExposureControlProof_20260714`: the capture manifest records six `LightingProfile=CommonHeadgear` runtime states and `full_batch_started=false`; import changed/saved exactly six packages; the cold-restart verification passed `6/6`, matching exact loaded object paths, source and post-import package hashes, UI texture settings, no redirector and no duplicate. The new sources are technically clustered around whole-image mean luminance `57.77-61.56`, including the new Headgear reference at `60.01`; this is technical consistency evidence, not a visual PASS. Pre-import asset backups remain under `Saved/AssetBackups/HeadThumbnailExposureControlProof_20260714/Controls`.
- Full UE 5.8 UBT: the first attempt exposed one stale removed focus-symbol reference and is retained as `ubt_attempt1_failed.log`; after that correction, the authoritative full build succeeded with exit code `0`, `Result: Succeeded`, `5.57s`, compiling `AvCustomizePreviewActor.cpp` and linking `UnrealEditor-Avaryo.dll`. Log: `Saved/CodexDiagnostics/HeadThumbnailExposureControlProof_20260714/ubt_full.log`. No C++ changed after this successful build.
- Blueprint Compile after the cold restart: `WBP_HeadCatalogItemCard = Success`; `WBP_CharacterCustomizationRoot_V2 = Success`. Results are recorded in `Saved/CodexDiagnostics/HeadThumbnailExposureControlProof_20260714/blueprint_compile_results.json`.
- The current user SaveGame and retained task backup `Saved/SaveGameBackups/HeadThumbnailExposureControlProof_20260714/AvariikaCompany.prechange.sav` both have SHA-256 `B70204202B0BD92D1C083920B710587201126E9C7DB5FE30B9371161FBA11358`; capture/import did not mutate it and no restoration was needed.
- Final technical state: cold-started Editor PID `24208` is responsive, `PIE=false`, Save All completed, dirty content packages `0`, dirty map packages `0`. No commit was created and unrelated worktree changes were preserved. The user must manually compare the six controls, especially Type02 eyes/cheeks/jaw/beard/neck/chest readability and Hair separation, and decide whether to authorize the remaining batch. No visual PASS is claimed.

## Head thumbnail exposure rebalance for dark previews - 2026-07-14

- This iteration continued from the working four-page Head UI. `WS_HeadSectionPages`, Headgear/Hair/Beard population, card selection/indicator styling, exact thumbnail registry, SaveGame schema, and live-preview behavior were not changed.
- The thumbnails became too dark because the preceding natural-skin correction reduced prepared capture from the old overexposed `8x` lighting profile to exactly `1x`, and reduced manual exposure from `0.95` to `0.35` (`Beard: 1.10 -> 0.50`). That removed roughly three stops from the prepared portraits. The material profile was correct, but Type02 and the beard/mouth/chin zone became underexposed at card size.
- The correction is isolated to `AAvCustomizePreviewActor::SetThumbnailCaptureMode()`. Standard HeadType/Hair capture now uses lighting multiplier `1.75` and exposure bias `0.55`. Beard keeps its existing lower-face camera/light positions and dedicated framing, but now uses multiplier `2.25` and exposure bias `0.80` so the mouth, chin, jaw, neck and shoulder separation survives on Type02. These values remain far below the former `8x / 0.95-1.10` profile that caused pale/plastic skin.
- The dark-blue backdrop, FOV, camera distance, focus, color grading, vignette, canonical HeadType normalization, and `UWorkerAppearanceComponent::ApplyHeadTypeSkinProfile()` were left unchanged. When thumbnail mode is disabled, the live preview still restores its existing `1x` lights and `0.35` exposure. European/Afro Head/Arms/Body families and the masked body material were not modified, so the earlier yellow/orange chest mismatch fix remains structurally intact.
- Re-captured and re-imported only the 30 assets affected by the immediately preceding dark-profile batch: HeadType `2`, Hair `10`, None/Hair `2`, Beard `14`, and None/Beard `2`. Headgear and the remaining 46 thumbnail assets were not touched. Capture/import manifests are under `Saved/CodexDiagnostics/HeadThumbnailExposureRebalance_20260714`; prechange copies of all 30 Texture2D assets are under `Saved/AssetBackups/HeadThumbnailExposureRebalance_20260714/Thumbnails`.
- Capture completed `30/30`; import completed `30/30` with all 30 target `.uasset` files changed. Final cold-session verification loaded and checked `30/30` Texture2D assets, their `512x512` PNG sources, exact post-import hashes, `sRGB`, `TC_EDITOR_ICON`, `TEXTUREGROUP_UI`, no mipmaps, clamp addressing and never-stream settings. No thumbnail package or map remained dirty.
- Full UE 5.8 UBT succeeded after the C++ change: exit code `0`, `Result: Succeeded`, `5.08s`; `AvCustomizePreviewActor.cpp` compiled and `UnrealEditor-Avaryo.dll` linked. Log: `Saved/CodexDiagnostics/HeadThumbnailExposureRebalance_20260714/ubt_full.log`. No C++ changed after that build.
- The task-start SaveGame and retained backup `Saved/SaveGameBackups/HeadThumbnailExposureRebalance_20260714/AvariikaCompany.prechange.sav` both have SHA-256 `0C19B33DD4567C6838D52B729E487726F95F4B01CA534B2B437314E7F2F5D36F`; capture/import did not mutate the file.
- Final state after a cold restart: Editor PID `23512`, responsive; Save All completed; `PIE=false`; dirty content packages `0`; dirty map packages `0`; preview actors `0`. The user must manually accept Type01/Type02 brightness, Beard readability, visible face/neck/shoulders/chest, and absence of the old yellow/orange mismatch or empty cards. This is not a visual PASS.

## Head section pages and thumbnail profile consistency - 2026-07-14

- The user-authored `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2` structure was found intact. `WS_HeadSectionPages` has the required four pages in stable order: `0=HeadType`, `1=Headgear`, `2=Hair`, `3=Beard`; no runtime replacement tree was created.
- Required `BindWidget` fields now cover `WS_HeadSectionPages` and the authored container triplets for every page: `ScrollBox_HeadItems` / `UniformGrid_HeadItems` / `Txt_HeadEmptyState`, `ScrollBox_HeadgearItems` / `UniformGrid_HeadgearItems` / `Txt_HeadgearEmptyState`, `ScrollBox_HairItems` / `UniformGrid_HairItems` / `Txt_HairEmptyState`, and `ScrollBox_BeardItems` / `UniformGrid_BeardItems` / `Txt_BeardEmptyState`. `EnsureHeadPageComposition()` validates only this static structure plus the card class; cancelled HeadSkin widgets are not referenced.
- One `SelectHeadSection()` path owns filter switching. `GetHeadSectionPageIndex()` supplies the switcher index, while `GetGridForHeadSection()`, `GetScrollBoxForHeadSection()` and `GetEmptyStateForHeadSection()` supply the matching page containers. The selected button, current-selection line, page contents and active ScrollBox position refresh together; initial section/page is HeadType/0.
- Runtime cards are separated by `TMap<EAvHeadCustomizationSection, TArray<TObjectPtr<UAvHeadCatalogItemButton>>> RuntimeHeadSectionCards`. A page rebuild removes only that page's tracked runtime cards, never calls `ClearChildren()` on the authored grids, and preserves/collapses the two HeadType Designer samples at runtime. `NativeDestruct()` removes tracked cards from all four pages before clearing the map, preventing reopen accumulation.
- Each page owns its Empty State. Records present means visible ScrollBox/collapsed Empty State; no records means collapsed ScrollBox/visible `НЕТ ДОСТУПНЫХ ВАРИАНТОВ`. Current Selection uses the structured catalog `DisplayName` for that section (`ТИП ГОЛОВЫ 01/02`, `НЕТ`, or the selected Russian item name), never a filename-derived UI label.
- Previously Headgear/Hair/Beard appeared only through the common HeadType grid because `RebuildHeadItemCards()` was hard-wired to `UniformGrid_HeadItems` and one shared card array. The authored switcher pages existed but had no C++ bindings or per-page runtime storage.
- `GetActiveHeadTypeId()` is now the sole card-presentation selector and returns only `HeadType01` or `HeadType02` from the active appearance head mesh. Structured catalog records carry exact `ThumbnailType01` and `ThumbnailType02` object paths. The card registry consumes those fields directly; a missing exact texture clears the reusable card image instead of retaining the card Blueprint's Type01 Designer brush as a product fallback. A HeadType change invalidates all dependent page cards, rebuilding the open page immediately and hidden pages on their next opening.
- The yellow/orange face-versus-chest boundary came from the stale capture presentation: head and body were not being rebuilt through the same HeadType material-family path, and the capture layer could apply raw `SkinColor`/additional correction after selecting materials. `SetThumbnailCaptureItemPreview()` now normalizes the HeadType endpoint and performs one `SetPreviewAppearance()` rebuild, reaching the existing `UWorkerAppearanceComponent::ApplyHeadTypeSkinProfile()` exactly once. Verified canonical Body slots are `0=M_Quantum_Head1`, `1=M_Quantum_Arms1`, `2=M_Quantum_Body1`; Type01 uses European Head/Arms/Body textures and Type02 uses Afro Head/Arms/Body textures. The masked Body slot keeps authored underwear outside the skin correction.
- Capture presentation now uses the same opaque dark-blue `OpaqueDarkBlueShared` backdrop for all four categories and both HeadTypes. Category framing remains intentional (including Beard framing), while Type01/Type02 within a category share resolution, backdrop, alpha, camera/light profile and exposure. The live-preview skin calibration/profile was not changed; the code change is restricted to capture normalization/presentation and shared backdrop.
- One control batch captured 14 requested cases successfully, then one deterministic full batch captured and re-imported all 76 existing product assets: HeadType `2`, Headgear `44`, Hair `10`, Beard `14`, and None cards `6`. All sources are `512x512`, alpha is forced to `255`, all 38 Type01/Type02 pairs have distinct source hashes, and all imported Texture2D assets retain UI texture settings. Asset backups are under `Saved/AssetBackups/HeadSectionPagesThumbnailProfiles_20260714/Thumbnails`; capture/import evidence is under `Saved/CodexDiagnostics/SkinToneMaterialConsistency_20260714`.
- Full UE 5.8 UBT completed before the final cold Editor start: exit code `0`, `Result: Succeeded`, `25.71s`. No C++ changed after that build. Blueprint Compile after thumbnail import: `WBP_HeadCatalogItemCard = Success`, `WBP_CharacterCustomizationRoot_V2 = Success`. The cold-session log has zero BindWidget, missing-widget, stale HeadSkin, invalid-parent, task ensure or assertion matches.
- SaveGame was neither reset nor mutated. Working file and `Saved/SaveGameBackups/HeadSectionPagesThumbnailProfiles_20260714/AvariikaCompany.prechange.sav` both have SHA-256 `FFFFF9BB3B2448D4727158E7CB7AEE8853772518E2C137168C780CA166AF4AA4`.
- Final state: `PIE=false`, Save All complete, dirty content packages `0`, dirty map packages `0`, Editor open and responsive; no commit was created and unrelated worktree state was preserved. The user still needs to visually/functionally accept page switching, per-section selection text/Empty State, Type01/Type02 card switching, face-neck-shoulder-chest consistency and unchanged underwear. This is not a visual PASS.

## HeadType thumbnail color consistency follow-up - 2026-07-14

- Root cause was a stale Type01 Texture2D captured before the canonical body slot names were handled. The old capture profiled `M_Quantum_Head1`, but missed the actual `M_Quantum_Arms1` and `M_Quantum_Body1` slots, leaving the shoulder/chest regions on a different source presentation and producing the visible yellow/orange boundary below the face.
- Re-captured only `T_UI_HeadType_01` at 512x512 through `AAvCustomizePreviewActor::SetThumbnailCaptureItemPreview -> SetPreviewAppearance -> UWorkerAppearanceComponent::ApplyAppearance -> ApplyHeadTypeSkinProfile`. The capture manifest confirms Light MIDs on body indices `0=M_Quantum_Head1`, `1=M_Quantum_Arms1`, and `2=M_Quantum_Body1`; the masked body material continues to exclude authored underwear pixels.
- Re-imported only `/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/HeadType/T_UI_HeadType_01`. Its UI texture settings remain `sRGB=true`, `TC_EDITOR_ICON`, `TEXTUREGROUP_UI`, no mipmaps, clamp addressing and never-stream. `T_UI_HeadType_02` and all live-preview C++ were untouched by this follow-up.
- Evidence: `Saved/CodexDiagnostics/HeadTypeThumbnailColorConsistency_20260714/capture_manifest.json`, generated PNG, and `finalize_manifest.json`. The target asset backup is `Saved/AssetBackups/HeadTypeThumbnailColorConsistency_20260714/T_UI_HeadType_01.prechange.uasset`.
- No C++ changed in this follow-up, so no additional UBT was required; the immediately preceding full UBT remains authoritative: exit code `0`, `Result: Succeeded`, `26.23s`, and no C++ was changed afterward.
- Follow-up SaveGame backup: `Saved/SaveGameBackups/HeadTypeThumbnailColorConsistency_20260714/AvariikaCompany.prechange.sav`. Working and backup SHA-256 are both `7BD6A33590CC99AFB0597BF717E7E316771C267B1A7152CDCA2025CB77114FA6`; capture/import did not mutate SaveGame.
- Final editor state after the narrow import: `PIE=false`, Save All complete, dirty content/maps `0/0`, Editor open and responsive. Visual acceptance of Type01 face/neck/shoulder/chest consistency remains with the user; this is not a visual PASS.

## HeadType skin profile live-preview fix - 2026-07-14

### Resume/build state

- Continued in `C:/unrealEngine/avariika_UE58_sandbox`; the existing HeadType -> Light/Dark mapping and accumulated dirty worktree were preserved. No commit was created.
- The preceding session stopped while the redirected full-build log was still showing only UBT startup. `Saved/CodexDiagnostics/HeadSkinPreviewQuickFix_20260714/ubt.log` therefore has no footer, but the authoritative `C:/Users/admin/AppData/Local/UnrealBuildTool/Log.txt` reached compile, link and metadata and records `Result: Succeeded`, `23.90s`, with no compile errors. Its outer shell exit code was not retained. All seven task-scoped C++ timestamps predated that build.
- Final build after this fix: `Saved/CodexDiagnostics/HeadTypeSkinProfileLiveFix_20260714/ubt_full.log`, exit code `0`, `Result: Succeeded`, `26.23s`. It compiled `WorkerAppearanceComponent.cpp`, `AvCharacterCustomizationRootWidget.cpp`, `AvCustomizePreviewActor.cpp`, `CompanyLedgerSubsystem.cpp` and linked `UnrealEditor-Avaryo.dll`. No C++ changed after this successful UBT.

### Product logic and stale UI removal

- The product exposes exactly two HeadType-owned profiles: `HeadType01 -> Light` and `HeadType02 -> Dark`. Persisted `SkinColor` remains only for SaveGame/schema compatibility and is normalized to the corresponding endpoint; arbitrary legacy values cannot override the mapped HeadType.
- The seven-swatch skin UI is cancelled. Removed from product C++: both general/Head skin button handlers, swatch binding/refresh/selected-state helpers, design/runtime selected-index state, `Button_Skin01..07`, `Border_Skin01..07SelectedFrame`, `VB_HeadSkinToneArea`, `Txt_HeadSkinToneLabel`, `HB_HeadSkinSwatches`, `Button_HeadSkin01..07`, `Img_HeadSkin01..07SelectedFrame`, Head-page visibility logic, and every `EnsureHeadPageComposition()` missing-widget requirement for them.
- Exact stale-widget search now returns zero matches. Cold-start `Saved/Logs/avariika.log` has zero `HeadSkin`, `SkinSwatch`, `Static Head page is incomplete`, `[AvCustomizeHeadDesigner]`, task ensure or assertion matches; the missing HeadSkin popup is gone.

### Mismatch cause and unified material profile

- The correct prepared HeadType thumbnails were created from the exact FaceRig head meshes and their authored European/Afro source materials using the earlier calibrated correction curve. The interrupted live implementation instead treated raw endpoint RGB as the profile, used a stronger/different correction set, added an extreme `Brightness=6.0` special case to the Afro head, and kept the canonical body on European body textures for both HeadTypes. Thumbnail and live therefore no longer shared a material profile.
- `HeadType01` was pale/grey because raw `(1.00, 0.80, 0.64)` plus the new stronger correction replaced the warmer/saturated calibrated thumbnail correction.
- `HeadType02` was too light because the Afro head source received the extra `Brightness=6.0`, while the canonical body still used the European masked body family and a raw dark multiplier instead of the authored Afro family.
- One runtime helper now owns the complete operation: `UWorkerAppearanceComponent::ApplyHeadTypeSkinProfile()`. `RebuildVisuals()` calls it once after all slot components are built; `SetSkinColor()` only normalizes the persisted endpoint and delegates to the same helper. Live pawn, local preview actor, Head selection, SaveGame load, Factory, Random, Reset and thumbnail capture all reach this path through `ApplyAppearance`/`ApplyLocalPreviewAppearance`/`RebuildVisuals`.
- The thumbnail helper now places the endpoint into `FWorkerAppearance` before its single `SetPreviewAppearance()` rebuild. The separate `AAvCustomizePreviewActor::SetPreviewSkinColor()` path and the old preview-after-appearance tint call were removed, so no correction is stacked over an already profiled MID.
- `HeadType01 / Light` uses `MI_Quantum_Head` or `MI_Quantum_Head_Alpha`, `MI_Quantum_Arms`, and the European Body BaseColor/Normal/ORM textures.
- `HeadType02 / Dark` uses `MI_Quantum_Head_Afro` or `MI_Quantum_Head_Afro_Alpha`, `MI_Quantum_Arms_Afro`, and the Afro Body BaseColor/Normal/ORM textures. European and Afro families are no longer mixed.
- Read-only Unreal MCP confirmed the canonical `SKM_Qunatum_FaceRig_Body_Hight` indices: `0=M_Quantum_Head1`, `1=M_Quantum_Arms1`, `2=M_Quantum_Body1`. Only these verified skin-bearing slots are profiled. Index 2 uses the existing project-owned `MI_AvQuantum_Body_SkinMasked` with the selected family textures, so correction affects open body skin while authored underwear pixels remain outside the skin mask. Non-canonical Body-region materials and clothing/accessory materials are not touched.
- The scalar/vector correction curve in the helper is the same calibrated base curve that produced the current prepared thumbnails; the Light/Dark RGB endpoints are persisted selectors, not direct final presentation colors.

### SaveGame, assets and final editor state

- Backup created before work: `Saved/SaveGameBackups/HeadTypeSkinProfileLiveFix_20260714/AvariikaCompany.prechange.sav`. Working and backup files are both `4657` bytes with SHA-256 `105199C1093049D0EF81DA4B3C1E509DA9FD55FB90AC8539D6E9D01C4EC4C0C0`; this task did not mutate or reset the user SaveGame.
- Incremental C++ changes: `WorkerAppearanceComponent.h/.cpp`, `AvCharacterCustomizationRootWidget.h/.cpp`, and `AvCustomizePreviewActor.h/.cpp`. The already-saved `CompanyLedgerSubsystem.cpp` v4 normalization was inspected and preserved unchanged. This handoff was updated.
- No WBP layout or thumbnail was intentionally edited. Before the required pre-build close, the user-started Editor already reported `WBP_CharacterCustomizationRoot_V2` dirty; the mandated `Save All` persisted that existing package state, then reported dirty content/maps `0/0`.
- Editor was closed normally after `Save All`, UBT succeeded, and Editor was cold-started as PID `13452` on DLL SHA-256 `63C476C263D1458C38C558564C050B2E65C42239D45250FF5658B6DC3A304D9C`.
- Final state: `PIE=false`, `Save All` successful, dirty content packages `0`, dirty map packages `0`, editor-world preview actors `0`; Editor is open and responsive. Visual acceptance was intentionally not performed by Codex.

### User acceptance still required

- The user must visually compare live `HeadType01` with its Type01 card and live `HeadType02` with its Type02 card, including head/neck/arms/torso/legs/feet consistency and unchanged underwear. This implementation/build result is not a visual PASS.

## Head page Widget Designer parity and Russian display names - 2026-07-14

### Resume state and preserved partial refactor

- Work resumed in `C:/unrealEngine/avariika_UE58_sandbox` from the interrupted `Head page Widget Designer parity and Russian display names` task. The existing C++ refactor, asset backups, thumbnails, SkinTone behavior, Beard capture profile, Random, Reset, Gloves=None policy and SaveGame schema were preserved; no commit was created.
- The six requested source files were inspected before further edits. `UAvHeadCatalogItemButton` was already a Blueprintable `UUserWidget`; `UAvCharacterCustomizationRootWidget` already contained most of the design-time sample/card-class refactor; structured Russian catalog names were already present in `WorkerAppearanceComponent`. The diff was incomplete rather than restarted.
- Both target assets already existed, as did Autosave/recovery copies and `Saved/AssetBackups/HeadDesignTimeParity_20260714/WBP_CharacterCustomizationRoot_V2.prechange.uasset`. No backup or recovery asset was deleted.
- A pre-continuation task diff was saved at `Saved/CodexDiagnostics/HeadDesignTimeParity_20260714/resume_diff_before.patch`: `293341` bytes, SHA-256 `0BB420B4ADDFCD708E424411BAF0FCF53CAD6BC7DF5E6040926EEF23C3848E75`.
- The original Designer/runtime divergence came from constructing the Head page's static title, divider, filters, current-selection row and card internals at runtime. Widget Designer could see only the incomplete authored tree, while runtime C++ supplied the missing visual hierarchy. The final implementation uses one authored Head page tree and one authored reusable card WBP in both contexts.

### C++ completion

- `UAvHeadCatalogItemButton` remains a `UUserWidget`, not a `UButton`. It owns the exact required `BindWidget` references `Border_CardFrame`, `Txt_CardTitle`, `Img_CardThumbnail`, `Border_CardSelectionCircle`, `Txt_CardSelectedCheck`, and `Btn_CardHitTarget`.
- `NativePreConstruct`, `NativeConstruct`, `InitializeHeadCatalogButton`, `SetDesignTimePreview`, `ApplyPresentation`, `SetSelectedState`, `TriggerCatalogSelectionForAutomation`, and the hit-target click handler are complete. Automation and the actual invisible `Btn_CardHitTarget` route through the same selection handler.
- The interrupted build evidence contained one historical compiler error: `UBorder::GetBrush` does not exist in UE 5.8 (`C2039`, `ubt_attempt1_failed.log`). The final code reads `Border_CardSelectionCircle->Background`, updates the brush and calls `SetBrush`. No additional task compiler errors remained.
- `NativePreConstruct` on the root loads the same structured Head catalog, selects HeadType, refreshes filters/current value, shows the ScrollBox, hides Empty State and configures the two samples. It does not commit appearance, autosave, mutate SaveGame or create runtime cards.
- `RebuildHeadItemCards` creates only `UAvHeadCatalogItemButton` instances with `CreateWidget` from `HeadCatalogItemCardClass`, passes owner/section/catalog index/None flag/thumbnail/DisplayName/disabled reason/tooltip/exact path, adds two cards per row, and tracks live cards in `HeadItemButtons`. It does not manually create card borders, images or text.
- `UniformGrid_HeadItems->ClearChildren()` is not used. Rebuild removes tracked runtime cards, preserves the two authored sample pointers, collapses the samples at runtime and includes a defensive stale-runtime-card sweep that explicitly excludes those samples.
- A full customization close/reopen test exposed the remaining lifecycle defect: `NativeDestruct` had reset `HeadItemButtons` without removing the live cards from the reused Blueprint grid. The first proof showed six children after reopen: two samples plus four runtime cards. `NativeDestruct` now removes every tracked runtime card before resetting the array; the defensive rebuild sweep makes reuse idempotent.
- `EnsureHeadPageComposition` is validation only. Missing required widgets produce `ensureMsgf` and a clear error log; it does not reconstruct or restyle static UI.
- `EnsureHeadFilterContent` is removed and no call remains. There is no Head-page static `ConstructWidget` fallback and no Head-specific `Cast<UButton>` assumption.
- `NativeTick` does not rebuild the page or call `ForceLayoutPrepass`. It only compares the integer viewport size against the cached size and refreshes fullscreen layout when that size changes.

### Reusable card Widget Blueprint

- Asset: `/Game/Avariika/UI/CharacterCustomization/WBP_HeadCatalogItemCard`.
- Parent: `/Script/Avaryo.AvHeadCatalogItemButton`; no substitute parent was used.
- Final authored tree, including the harmless desired-size wrappers used by the actual asset:

```text
RootPanel
└─ SB_CardDesiredSize
   └─ Overlay_CardRoot
      ├─ Border_CardFrame
      ├─ VB_CardContent
      │  ├─ Overlay_CardHeader
      │  │  ├─ Border_CardHeaderBackground
      │  │  ├─ Txt_CardTitle
      │  │  └─ SB_CardSelectionIndicator
      │  │     └─ Overlay_CardSelectionIndicator
      │  │        ├─ Border_CardSelectionCircle
      │  │        └─ Txt_CardSelectedCheck
      │  └─ ScaleBox_CardThumbnail
      │     └─ Img_CardThumbnail
      └─ Btn_CardHitTarget
```

- Design defaults are `DesignerDisplayName=ТИП ГОЛОВЫ 01`, `bDesignerSelected=true` and the real Type01 texture `/Game/Avariika/UI/CharacterCustomization/Thumbnails/Real/HeadType/T_UI_HeadType_01.T_UI_HeadType_01`.
- The selected card displays the orange frame/title/indicator/check; the unselected presentation retains the dark frame. The full-size hit target remains visually transparent.
- Final asset SHA-256: `0D17C88C4085F8B8EABD3C0145B61A46C5CA7FD298107F515F5528C06A67BBBD`.

### Final authored Head page tree and Designer/runtime boundary

- `HeadCatalogItemCardClass` on `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2` is assigned to `WBP_HeadCatalogItemCard`.
- Final `VB_RightPanelHead` tree:

```text
VB_RightPanelHead
├─ Txt_HeadTitle
├─ SB_HeadTitleDivider
│  └─ Border_HeadTitleDivider
├─ UniformGrid_HeadSubcategories
│  ├─ Btn_HeadFilterHeadType
│  │  └─ Border_HeadFilterHeadTypeFrame
│  │     └─ Border_HeadFilterHeadTypeFill
│  │        └─ HB_HeadFilterHeadTypeContent
│  │           ├─ SB_HeadFilterHeadTypeIcon
│  │           │  └─ Img_HeadFilterHeadTypeIcon
│  │           └─ Txt_HeadFilterHeadType
│  ├─ Btn_HeadFilterHeadgear
│  │  └─ Border_HeadFilterHeadgearFrame
│  │     └─ Border_HeadFilterHeadgearFill
│  │        └─ HB_HeadFilterHeadgearContent
│  │           ├─ SB_HeadFilterHeadgearIcon
│  │           │  └─ Img_HeadFilterHeadgearIcon
│  │           └─ Txt_HeadFilterHeadgear
│  ├─ Btn_HeadFilterHair
│  │  └─ Border_HeadFilterHairFrame
│  │     └─ Border_HeadFilterHairFill
│  │        └─ HB_HeadFilterHairContent
│  │           ├─ SB_HeadFilterHairIcon
│  │           │  └─ Img_HeadFilterHairIcon
│  │           └─ Txt_HeadFilterHair
│  └─ Btn_HeadFilterBeard
│     └─ Border_HeadFilterBeardFrame
│        └─ Border_HeadFilterBeardFill
│           └─ HB_HeadFilterBeardContent
│              ├─ SB_HeadFilterBeardIcon
│              │  └─ Img_HeadFilterBeardIcon
│              └─ Txt_HeadFilterBeard
├─ VB_HeadCurrentSelectionArea
│  ├─ HB_HeadCurrentSelection
│  │  ├─ Txt_HeadCurrentSelection
│  │  └─ Txt_HeadCurrentSelectionValue
│  └─ SB_HeadCurrentSelectionDivider
│     └─ Border_HeadCurrentSelectionDivider
├─ ScrollBox_HeadItems
│  └─ UniformGrid_HeadItems
│     ├─ DesignSample_HeadType01
│     └─ DesignSample_HeadType02
└─ Txt_HeadEmptyState
```

- Both samples are instances of `WBP_HeadCatalogItemCard`. Type01 uses the real Type01 thumbnail and is selected; Type02 uses the real Type02 thumbnail and is unselected. Empty State is collapsed in Designer.
- Page padding, title/divider, the 2x2 filter layout, filter icon/text sizing and padding, selected/normal fills and frames, current-selection typography, ScrollBox padding, card desired size/gaps and Empty State placement are authored in the WBP rather than restyled every runtime construction.
- C++ retains only dynamic state: active/inactive filter colors, current-selection text, live record population, selected state, disabled/tooltip state, click handlers and responsive viewport refresh.
- Final root asset SHA-256 after final compile/save: `13600DD958495443D7098ED2F660FB9B09087B10B8550FF0ECF473673A3EED96`.

### Russian exact-path display names

- The UI reads `FAvHeadCustomizationCatalogItem::DisplayName`; localization/name mapping is not embedded in `RebuildHeadItemCards`. Asset names and exact paths were not renamed.
- HeadType: `ТИП ГОЛОВЫ 01`, `ТИП ГОЛОВЫ 02`.
- Headgear: `НЕТ`, `БЕЖЕВАЯ КЕПКА`, `ЧЁРНАЯ КЕПКА`, `СИНЯЯ КЕПКА`, `ДВУХЦВЕТНАЯ КЕПКА`, `КРАСНАЯ КЕПКА`, `УЛИЧНАЯ КЕПКА`, `ЧЁРНАЯ КОВБОЙСКАЯ ШЛЯПА`, `КОВБОЙСКАЯ ШЛЯПА`, `СИНЯЯ ПЛОСКАЯ КЕПКА`, `КОРИЧНЕВАЯ ПЛОСКАЯ КЕПКА`, `СЕРАЯ ПЛОСКАЯ КЕПКА`, `ЧЁРНАЯ ШАПКА С ОТВОРОТОМ`, `СИНЯЯ РАБОЧАЯ КАСКА`, `ЗЕЛЁНАЯ РАБОЧАЯ КАСКА`, `ОРАНЖЕВАЯ РАБОЧАЯ КАСКА`, `КРАСНАЯ РАБОЧАЯ КАСКА`, `БЕЛАЯ РАБОЧАЯ КАСКА`, `РАБОЧАЯ КАСКА`, `ТЁПЛАЯ ШАПКА`, `ЧЁРНАЯ ТЁПЛАЯ ШАПКА`, `СЕРАЯ ТЁПЛАЯ ШАПКА`, `ЗЕЛЁНАЯ ТЁПЛАЯ ШАПКА`.
- Hair: `НЕТ`, `ЗАЧЁС НАЗАД`, `ДЛИННЫЕ ВОЛОСЫ`, `ИРОКЕЗ`, `КОРОТКИЕ ВОЛОСЫ`, `БРИТАЯ ГОЛОВА`.
- Beard: `НЕТ`, `БОРОДА 01`, `БОРОДА 02`, `БОРОДА 03`, `БОРОДА 04`, `ДЛИННАЯ БОРОДА 01`, `ДЛИННАЯ БОРОДА 02`, `УСЫ`.
- Current catalog diagnostics prove `2` HeadType, `22` Headgear, `5` Hair and `7` Beard asset records; the three optional sections add one synthetic `НЕТ` card each. Exact path/name evidence remains in `Saved/CodexDiagnostics/HeadCustomization_20260713/head_catalog.json`.

### Blueprint, build and cold-restart verification

- Both `WBP_HeadCatalogItemCard` and `WBP_CharacterCustomizationRoot_V2` compiled `Success` after the final cold restart. No BindWidget errors, missing-widget messages, invalid-parent warnings or duplicate widget names were reported.
- First continuation UBT: `Result: Succeeded`, `14.05s`, `Saved/CodexDiagnostics/HeadDesignTimeParity_20260714/ubt_first_full.log`.
- After the close/reopen lifecycle fix, a new full UE 5.8 UBT rebuilt `AvCharacterCustomizationRootWidget.cpp`, linked `UnrealEditor-Avaryo.dll`, and finished `Result: Succeeded`, `14.22s`. Final log: `Saved/CodexDiagnostics/HeadDesignTimeParity_20260714/ubt_lifecycle_fix.log`.
- No C++ file changed after that build. Final relevant hashes: `AvCharacterCustomizationRootWidget.cpp=4F19344FF0773D8FEAD6B7DCCACC129BD78529E351D7906B3F9AC6CDB73F2FC7`; `AvCharacterCustomizationRootWidget.h=7304549BC9C0FA89D3C139443F310D2B0E66C541BE7A81EABBAF88D6BAD32CED`; `AvHeadCatalogItemButton.cpp=1F55C0BA828AA291243EB077021B35123612A9A1DFF1219F2DDD092B018623B6`; `AvHeadCatalogItemButton.h=1CCB13C128241AA2DF8EAE689B1905DD117EF130F52EEC96B99B93213CA80BF0`; final DLL `93901AC53C895A1DF46C6706E8FBFA4E8A32512A45A45A06D78053DCFF447206`.
- The Editor was closed through its normal workflow, then cold-started as PID `26488` on the final DLL. It remained responsive, both WBP assets were reopened/compiled, Designer was reopened and runtime PIE was rerun.

### Designer/runtime visual comparison and regression

- Designer at the recorded `1280x720`/DPI `0.8` view immediately shows Head title/divider, the 2x2 filters, selected HeadType, current selection, two real HeadType portraits, Type01 selected, Type02 unselected and Empty State hidden. No PIE is required for this view.
- Final runtime visually matches the authored layout; it replaces only the two collapsed samples with two live HeadType cards. The ScrollBox and larger Headgear/Hair/Beard lists display normally.
- Required screenshots exist under `Saved/CodexScreenshots/HeadDesignTimeParity_20260714`: `03_designer_after.png`, `04_runtime_after.png`, `05_designer_runtime_side_by_side.png`, `06_designer_headtype.png`, `07_runtime_headtype.png`, `08_designer_card_widget.png`, `09_runtime_headgear_ru.png`, `10_runtime_hair_ru.png`, and `11_runtime_beard_ru.png`. `03_designer_after_final_cold.png` additionally records the final cold Designer reopen.
- Runtime exact selection regression passed after the final cold start: HeadType01, HeadType02, SkinTone03 full-body path, beige Headgear plus `НЕТ`, slicked-back Hair plus `НЕТ`, Beard01 plus `НЕТ`, then HeadType01 again. UI current values and actual preview/player skeletal meshes were recorded in `runtime_head_selection_regression_final_cold.json`; errors are empty.
- Five Head-category leave/reopen cycles retained exactly four grid children. Five full customization close/reopen cycles also retained one active root and exactly four children each: two collapsed samples and `HeadItemCard_0_000`/`HeadItemCard_0_001`. Evidence: `duplicate_reopen_5x.json` and `duplicate_full_close_reopen_5x.json` (`all_counts_stable=true`). Old roots visible to UObject iteration were pending GC only; exactly one root was in viewport each cycle.
- Preset paging three times without Apply changed only the preview. The live player component set and SaveGame SHA were byte-identical before/after; evidence: `preset_paging_without_apply.json`.
- Random passed production validation with `PreviewSaveUnchanged=true`, eligible/intact visual assets, no invalid components and Gloves=None. Evidence: `random_regression.json` plus the final `[AvCustomizeRandom] ... Validation=PASS` log.
- Reset confirmation returned Factory HeadType01/body, no optional clothing and Gloves=None; final log records `Origin=0`. Evidence: `reset_factory_regression.json`.
- F9 toggled the development Appearance Inspector from `Collapsed` to `Visible` and back to hidden without appearance mutation or autosave. Evidence: `f9_inspector_regression.json` and `[AvAppearanceInspector] Panel=Visible/Hidden` logs.
- Selected state, Russian current-selection values, optional clearing, real thumbnails, Headgear hair suppression, close/reopen persistence, no duplicate cards, ScrollBox behavior and exclusion of samples from runtime all passed. Opening Designer did not write SaveGame.

### SaveGame, final Editor state and Git

- Reused baseline backup: `Saved/SaveGameBackups/HeadDesignTimeParity_20260714/AvariikaCompany.prechange.sav`; it was not deleted or duplicated unnecessarily.
- Baseline and final restored `Saved/SaveGames/AvariikaCompany.sav` are both `7461` bytes with SHA-256 `59A37C6E5C65E85686D391631676D9DA99DF4CDD98FF9AC2E3FCD92EB9A84865`.
- Final state from `final_editor_state.json`: `PIE=false`, dirty content packages `0`, dirty map packages `0`, editor-world `AvCustomizePreviewActor` count `0`. Save All completed and Editor PID `26488` is responsive.
- Final task entries remain intentionally uncommitted: the two `WorkerAppearanceComponent` files are tracked modifications; the four UI C++ files, two WBP assets and this handoff are untracked in the inherited dirty sandbox.
- Overall `git status --short`: `615` entries (`31` tracked changes, `584` untracked). Overall tracked `git diff --stat`: `31 files changed, 5123 insertions(+), 2111 deletions(-)`. The only `git diff --check` issue is the pre-existing unrelated `Config/DefaultGame.ini:121 new blank line at EOF`; the six task C++ files pass scoped `git diff --check`.
- With the four untracked UI sources temporarily marked intent-to-add only for measurement and then reset, the six task source paths report `7370 insertions, 55 deletions`; this includes the inherited accumulated `WorkerAppearanceComponent` diff, not only this continuation.

### Designer limitations that remain intentional

- The complete static Head page, both representative HeadType cards and reusable card styling are visually editable in Designer.
- Designer intentionally cannot show all live catalog records, active SaveGame selection, integrated-hood disabled states, runtime tooltips, runtime ScrollBox population, click commits, the captured 3D preview actor or viewport-dependent runtime response. Those remain dynamic C++ behavior; two real samples provide the authored visual parity reference without mutating gameplay state.

## Beard thumbnail readability fix - 2026-07-14

### Outcome and cause

- The remaining Head-page product defect was the shared portrait capture profile: Beard used the same `FOV=28`, camera distance `112`, focus `Z=156` and upper-face-biased light targets as HeadType/Hair/Headgear. At card size the beard occupied too little area; on dark `HeadType02`, moustache and short beard detail merged into skin/shadow, and Long/Long 2 depended on a full-size source view to read clearly.
- Beard now has a dedicated prepared-thumbnail capture profile. Runtime item selection, commit/autosave, `ActiveCharacterAppearance`, SaveGame schema, Random, Reset, SkinTone, Gloves=None, right-panel structure and card grid were not changed.
- `NONE` is captured through the same Beard profile with `ItemSlot=Beard` and `Item=None`; it remains a real clean-shaven portrait, not a generic fallback.

### Beard-specific framing, light and background

- Standard Head/Hair/Headgear capture remains unchanged at `FOV=28`, distance `112`, focus `Z=156`.
- Beard uses `FOV=27`, camera distance `92`, focus `Z=156.5`. This makes the lower face about one quarter larger while keeping the face, neck, shoulders and both long-beard tips inside the square source.
- One category-level profile was sufficient. No per-item camera/focus offsets are used.
- Beard lighting moves the point/rect fill and rim targets down from the forehead toward the mouth/chin/jaw, increases lower-face fill, rim and ambient separation, slightly reduces the warm key, and raises manual exposure bias from `0.95` to `1.10`. Hair volume remains shaded rather than flat, but dark strands no longer collapse into one black patch on `HeadType02`.
- The existing blue-grey background style is retained. Only the Beard capture backdrop material was lifted from `(0.018, 0.032, 0.055)` to `(0.026, 0.045, 0.074)`; clear color and vignette style were not redesigned.

### Capture, import and visual verification

- Re-captured and reimported exactly 16 existing Texture2D assets: `NONE`, `BEARD`, `BEARD 02`, `BEARD 03`, `BEARD 4`, `BEARD LONG`, `BEARD LONG 2`, and `MOUSTACHE` for both `HeadType01` and worst-case dark `HeadType02`.
- Capture manifest: `Saved/CodexDiagnostics/BeardThumbnailVisibilityFix_20260714/capture_manifest.json` (`16/16`, 16 unique PNG hashes, SaveGame unchanged).
- Asset import used `AssetImportTask`/AssetTools and explicit Texture2D saves; no `.uasset` was manually edited. All 16 retained `sRGB`, `TC_EDITOR_ICON`, `TEXTUREGROUP_UI`, `NeverStream=true`, and `TMGS_NO_MIPMAPS`.
- The successful import changed all 16 intended `.uasset` hashes. The headless commandlet returned `-1` only because eight unrelated Niagara/BackPack packages became transiently dirty while the project loaded; scoped Beard dirty packages were zero and those unrelated packages were not saved. A subsequent cold Editor verification loaded and hash-checked all `16/16` target assets with final dirty content/maps `0/0`: `post_cold_restart_import_verification.json`.
- Runtime registry verification passed with no missing thumbnail widgets: HeadType `2/2`, Headgear `23/23`, Hair `6/6`, Beard `8/8`. Evidence: `runtime_head_thumbnail_registry.json`.
- Required full-size visual evidence is under `Saved/CodexScreenshots/BeardThumbnailVisibilityFix_20260714/`: `01_beard_none.png`, `02_beard.png`, `03_beard_02.png`, `04_beard_long.png`, `05_beard_long_2.png`, `06_moustache.png`, `07_beard_grid_full.png`, and `08_beard_side_by_side_comparison.png`. `07b_beard_grid_bottom.png` shows the fully visible Long 2/Moustache row at real UI card size.
- Visual result on `HeadType02`: Moustache reads as moustache immediately; Beard/Beard 02/Beard 03/Beard 4 have distinguishable jaw/chin coverage; Long has a fuller front mass while Long 2 has a different open-front/side-heavy silhouette. The before/after sheet demonstrates the larger lower-face share and stronger strand separation.

### Build, cold restart and final state

- Full UE 5.8 UBT succeeded after the C++ change: `AvCustomizePreviewActor.cpp` compiled, `Result: Succeeded`, total `20.24s`. Log: `Saved/CodexDiagnostics/BeardThumbnailVisibilityFix_20260714/ubt_full.log`.
- No C++ file changed after that successful build. Current source hashes: `AvCustomizePreviewActor.cpp=38BD883A26199F27523D1BE5EA48EC3BA9E5BCD7FF5A4CB0EB548BA7CA9129E2`; header `F790E3632621A2B8F707519D5B5928B3CB03A95FB3F6C6D3DEC9A15C00789E77`; DLL `10A35EB656A514F93A4612CA3576674D1CCD997401A5006246EDDF8991E2D721`.
- Cold Editor restart was completed after UBT for final capture, and a second cold restart was completed after Texture2D import for actual UI/runtime verification.
- Final Editor state: `PIE=false`, `Save All` successful, dirty content/maps `0/0`, transient editor preview actors `0`.
- SaveGame remained byte-identical to the pre-task backup: `03E2E8BE298A9AA13441A13F83A1C8CE6FC2FB398AA3EAD745AEFF54B2DC1C92`.
- No commit was created. The sandbox remains heavily dirty from pre-existing work; task source comparison against `Saved/SourceBackups/BeardThumbnailVisibilityFix_20260714` is `AvCustomizePreviewActor.cpp +102/-27` and header `+1`, plus the 16 Beard Texture2D reimports and this handoff.

### Intentional limits

- Final verification exercised capture, import, real UI registry resolution and visual card readability for both head/skin presentations. Random, Reset and Apply/selection were not clicked again because their code paths were outside this narrow change and doing so would rewrite the user SaveGame; their source and schema were untouched.
- Static thumbnails cannot prove every future beard material, new head asset or future card size. New Beard catalog entries must be captured through the Beard slot/profile and added to the exact-path registry.

## Glove integrity closure and Gloves=None runtime policy - 2026-07-13

### Closure status

- The glove/wrist integrity task is closed for the current audited catalog. The defect is not a random LOD failure and not an incidental Torso conflict. Both known `Gloves_Worker` variants use authored open-fingertip/insufficient-full-hand-coverage geometry. With Body hidden the shortened glove geometry and authored openings remain visible; with the canonical Body enabled, canonical skin and fingers are visible through/after those openings. The canonical Body is therefore supplying the fingers seen in the combined preview; the glove assets are not closed full-hand gloves.
- Both glove meshes have exactly one LOD. `Force LOD0` and `Auto LOD` produced the same openings and the same Body-on/Body-off result. LOD switching cannot repair the defect.
- Runtime policy is now `Gloves=None`. Random scan, fallback, and final validation do not accept either known glove. Canonical Body hands remain present; hands are not hidden globally.
- No new allowed Torso or FullOutfit was quarantined during the exhaustive `Gloves=None` matrix or the idle audit. The previously conservative `Jacket_Worker` shared sleeve/cuff quarantine remains required.

### Exact glove assets and material evidence

- Default mesh: `/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker/SKM_Gloves_Worker.SKM_Gloves_Worker`.
  - Material: `/Game/Modular_Workers/Materials/Arms_Modules/Arms_Gloves_Worker/MI_Gloves_Worker.MI_Gloves_Worker`.
  - Base color: `/Game/Modular_Workers/Textures/Arms_Modules/Gloves_Worker/T_ArmsGlovesWorker_Yellow_BaseColor.T_ArmsGlovesWorker_Yellow_BaseColor`.
  - Status: `Quarantined`, `VisualCondition=Quarantined`, `AllowInRandom=false`, `QuarantineReason=InsufficientFullHandCoverage; SharedOpenFingerGeometry; NeedsIndependentColorAudit`.
- Brown mesh: `/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker/SKM_Gloves_Worker_Brown.SKM_Gloves_Worker_Brown`.
  - Material: `/Game/Modular_Workers/Materials/Arms_Modules/Arms_Gloves_Worker/MI_Gloves_Worker_Brown.MI_Gloves_Worker_Brown`.
  - Base color: `/Game/Modular_Workers/Textures/Arms_Modules/Gloves_Worker/T_ArmsGlovesWorker_Brown_BaseColor.T_ArmsGlovesWorker_Brown_BaseColor`.
  - Status: `VerifiedDamaged`, `VisualCondition=BodyClipping`, `AllowInRandom=false`, `QuarantineReason=InsufficientFullHandCoverage; AuthoredOpenFingertips; CanonicalBodyFingersVisible; SharedOpenFingerGeometry`.
- Both variants use the same normal and ORM layouts:
  - `/Game/Modular_Workers/Textures/Arms_Modules/Gloves_Worker/T_ArmsGlovesWorker_Normal.T_ArmsGlovesWorker_Normal`;
  - `/Game/Modular_Workers/Textures/Arms_Modules/Gloves_Worker/T_ArmsGlovesWorker_OcclusionRoughnessMetallic.T_ArmsGlovesWorker_OcclusionRoughnessMetallic`.
- Isolation evidence is in `Saved/CodexScreenshots/GloveIntegrityFix_20260713/Isolation`. The required files exist: `01_gloves_brown_without_body.png`, `02_gloves_brown_with_body.png`, `03_gloves_default_without_body.png`, `04_gloves_default_with_body.png`, `05_brown_fingertips_closeup.png`, and `06_default_fingertips_closeup.png`. `Gloves_Isolation.jpg` is the contact sheet.

### Existing cuff quarantine retained

- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker/SKM_Jacket_Worker_Blue` remains `VerifiedDamaged / OpacityMaskDamage` because the authored opening above both reflective cuffs is visible independently of gloves.
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker/SKM_Jacket_Worker_Bege` and `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker/SKM_Jacket_Worker_Gray` remain quarantined through `Jacket_Worker_SharedSleeveMaskLayout` pending independent cuff evidence.
- The sampled idle evidence continues to show the already-known `Jacket_Worker` opening. No allowed Hood 1, Hood 2, rolled-shirt, T-shirt, or FullOutfit group acquired a new quarantine.

### Cold-start runtime catalog and final validation

- Unreal Editor was cold-started on `C:/unrealEngine/avariika_UE58_sandbox/avariika.uproject` with the UE 5.8 Editor. PIE opened `Character Customization` through the real player-controller/HUD path.
- First runtime catalog build logged `Serial=1 Source=AssetRegistry LocalRebuild=true CachedCandidates=false`. No stale static/cached candidate catalog was reused.
- Candidate counts relevant to this closure: `Gloves Before=2 Eligible=0 Rejected=2`, `Torso Eligible=11`, `Legs Eligible=4`, and `FullOutfit Eligible=5`.
- For both exact glove paths, runtime logs establish the full chain: exact selected/eligibility path, audit status, visual condition, `Allow=false`, quarantine reason, scan rejection, and final-validation rejection.
- Every production Random catalog dump reported `Slot=7 Candidates=0`. Random scan cannot choose a glove, fallback passed without a glove, and final validation rejects appearances containing either exact glove path. `Gloves=None` validates.

### Exhaustive automation

- `Avariika.WorkerAppearance.RandomEligibilityExhaustive` completed with `Result={Success}`, one test performed, zero `AddError`, and empty `Errors={}`.
- Counters: `Records=142`, `Allowed=63`, `Denied=79`, `ScanCandidates=63`, `AllowedFinalValidationPasses=63`, `DeniedFinalValidationRejects=79`, `LegacyCarryoverRejects=79`, `GloveBodyBaselineRejects=2`, `GloveTorsoPairRejects=22`, `GloveFullOutfitPairRejects=10`, `GlovesNoneValidationPasses=16`, and `RandomGlovesNonePasses=32`; `Fallback=PASS`.
- The implementation was inspected before execution: `++RandomGlovesNonePasses` is present in the successful random-safety branch, so the final count `32` is real rather than an unincremented expected value.
- Evidence: `Saved/CodexDiagnostics/GloveIntegrityFix_20260713/automation_result.json` and `automation_controller_full.log` (SHA-256 `113124844D785F769790EA4E9458DC62568AC8E5AAE51EB53E979969682C3856`).

### Deterministic Gloves=None matrix and idle animation

- Deterministic visual audit covered the canonical Body plus all 11 final allowed Torsos and all 5 final allowed FullOutfits: 17 configurations, 34 left/right PNGs. Every runtime component scan had no `/Gloves_Worker/` component.
- Allowed Torso coverage: Hood 1 Beige/Blue/Gray, Hood 2 Beige/Blue/Gray, Shirt RolledUp RedCage/White_Cage, and TShirt Beige/Black/Blue. Allowed FullOutfit coverage: `SKM_Worker_Tshirt_Pants_1..5`.
- Visual outcome: complete canonical hands/fingers, no hidden hands, wrist gaps, hand-through-cuff clipping, black/transparent holes, missing fingers, z-fighting, or invalid cuff junctions in the audited fixed camera/light/skin/LOD0 views. No allowed asset needed `NeedsCompatibleClosedGlove`.
- Contact sheets: `Torso_GlovesNone_Left.jpg`, `Torso_GlovesNone_Right.jpg`, `FullOutfit_GlovesNone_Left.jpg`, and `FullOutfit_GlovesNone_Right.jpg` under `Saved/CodexScreenshots/GloveIntegrityFix_20260713`.
- Idle audit used `/Game/Modular_Workers/Demo/Animations/Male/A_MM_Idle.A_MM_Idle` at neutral `0.00`, intermediate `0.75`, maximum sampled hand displacement/bend `1.25`, and returned neutral `0.00`.
- Idle cases: canonical Body, T-shirt short sleeve, quarantined `Jacket_Worker`, allowed Hood 1, allowed Hood 2, allowed rolled shirt, and one allowed FullOutfit. Both hands were captured at every point: 56 PNGs plus seven idle contact sheets.
- All allowed groups stayed visually clean through the sampled animation points. No animation-only allowed-pair quarantine was required. The already-quarantined `Jacket_Worker` opening persisted as expected.

### Development-only audit commands and non-mutation boundary

- Commands retained because they provide reusable deterministic visual-audit value:
  - `Av.AppearanceAudit.SetPreview <Torso|None> <Gloves|None> <FullOutfit|None> [Lower|None]`;
  - `Av.AppearanceAudit.SetIdleTime <Seconds>`;
  - `Av.AppearanceAudit.ResumeIdle`.
- They are compiled only under `#if WITH_EDITOR || !UE_BUILD_SHIPPING`; Shipping excludes them. They operate on the transient preview actor and do not call `CommitActiveCharacterAppearance`, change appearance origin, or invoke autosave.
- A boundary probe exercised SetPreview, SetIdleTime, ResumeIdle, Inspector hide/body/show-all, Gloves-row Toggle, Force LOD0/Auto LOD, and Dump JSON. `appearance_origin=RandomGenerated`, `preview_source=ActiveCharacterAppearance`, and the active stable combination remained unchanged. The SaveGame SHA remained `3552547FB2FB80E566C25D225CAE30640E69B377559D3BC4B19251479875C81A` across the preview-only sequence.
- Separate preset paging without Apply also preserved the live pawn appearance and its test-state SaveGame SHA (`54BC8380E2F9CF1C9E452B1D25FE29102B0B75D5428D1B75B7D5FE55ED98AE83` before/after). Closing and reopening customization on separate frames restored the current active preview with `Gloves=None`; transient inspector/LOD state did not survive.

### Production Random stress and regression smoke

- Final evidence run: 200 consecutive calls through `AvaryoHUD.TriggerCustomizeRandomAppearance`, the production HUD/runtime path. Results: `200/200` production validation PASS, `Gloves=None 200/200`, eligibility/lower coverage/footwear/preview-save-unchanged `200/200`, invalid components `0`, visible modals `0`, and scoped `Fatal/Ensure/Assertion/: Error:` count `0`.
- Scheme distribution was 148 separate Torso+Lower and 52 FullOutfit. All 11 allowed Torso paths and all 5 allowed FullOutfit paths appeared at least once. No quarantined glove or `Jacket_Worker` path appeared.
- Thirty indices distributed over the 200-run were captured as alternating left/right hand-and-cuff close-ups; 30 additional full-character captures were retained. `RandomStress_Closeups.jpg` was visually reviewed as a contact sheet. No wrist gap, hand clipping, damaged allowed cuff, missing hand/finger, missing lower/footwear, slot conflict, preset copy, preview mutation, invalid component, or modal was found.
- Evidence: `Saved/CodexDiagnostics/GloveIntegrityFix_20260713/random200_result.json`; final runtime log lines `21264..28728`; full copied runtime log `runtime_full.log` (SHA-256 `BD4DF3C30C1610FD7D0DF1AC08990D7A22B232E265A629BF3BF353CC86E2694A`).
- Regression smoke passed: Reset confirmation produced Factory Body+Head with complete hands and disabled Reset; first Random and repeated Random produced dressed `Gloves=None` appearances with no modal; second Reset returned Factory; first Random after Reset passed; empty Gloves-row Toggle preserved all populated slots; Show All, LOD0/Auto, and Dump JSON passed; preset paging without Apply preserved active appearance and SaveGame; separate-frame close/reopen restored active preview.
- Evidence: `Saved/CodexDiagnostics/GloveIntegrityFix_20260713/regression_smoke_result.json`.

### Screenshots and evidence inventory

- Closure-specific PNG count: 156 (`6` required isolation, `34` deterministic Gloves=None, `56` idle, `30` Random close-up, `30` additional Random full-character). The complete task screenshot tree contains 182 PNGs when the 26 earlier problem/isolation evidence files are included.
- Contact-sheet count: 13 JPGs (`Gloves_Isolation`, four deterministic matrix sheets, seven idle sheets, and `RandomStress_Closeups`).
- Root evidence directories:
  - `Saved/CodexScreenshots/GloveIntegrityFix_20260713/Isolation`;
  - `Saved/CodexScreenshots/GloveIntegrityFix_20260713/GlovesNoneAudit`;
  - `Saved/CodexScreenshots/GloveIntegrityFix_20260713/IdleAudit`;
  - `Saved/CodexScreenshots/GloveIntegrityFix_20260713/RandomStress`;
  - `Saved/CodexDiagnostics/GloveIntegrityFix_20260713`.

### Build, SaveGame, clean editor state, and worktree

- The last full UBT log still proves compilation of `AvCustomizePreviewActor.cpp` and `WorkerAppearanceComponent.cpp`, followed by `Result: Succeeded` and `Total execution time: 10.11 seconds`.
- No `.cpp`, `.h`, or `.Build.cs` file changed after that successful build. Therefore no redundant build or Live Coding compile was run. Final DLL SHA-256: `2D61E99BB53A213EC43714B587E6DAD40523B9EE560C4384BDC39DEE8D0F6F25`.
- Task source hashes after the build remained: `WorkerAppearanceComponent.cpp=2C0709A9A5F101CC964F07626E73D6C43D001667F2E04FED70EA2F75A5297F7F`; `AvCustomizePreviewActor.cpp=71BACBCEED3B204374518D198EEE8A1E818EF97345ADB5FC997BFE1D687F5C11`.
- SaveGame backup: `Saved/SaveGameBackups/GloveIntegrityFix_20260713/AvariikaCompany.prechange.sav`. Initial, backup, and final restored SHA-256 are all `3552547FB2FB80E566C25D225CAE30640E69B377559D3BC4B19251479875C81A`. The backup was retained.
- Final editor state: `PIE=false`; `Save All` returned success; dirty content packages `0`; dirty map packages `0`; editor-level preview actors `0`; no temporary gameplay actors/assets were retained.
- No commit was created. Final `git status --short`: 610 lines (`31` tracked changes and `579` untracked entries), all belonging to the pre-existing dirty sandbox plus this untracked handoff/source/evidence work. Overall tracked `git diff --stat`: `31 files changed, 4050 insertions(+), 2097 deletions(-)`.
- Task-scoped comparison against `Saved/SourceBackups/GloveIntegrityFix_20260713`: `WorkerAppearanceComponent.cpp +194/-11`, `AvCustomizePreviewActor.cpp +135/-0`, and `WorkerAppearanceEligibilityTests.cpp` byte-identical. Task diff check is clean.
- No `.uasset` or `.umap` was created or modified by this glove task. The five tracked binary modifications already present in the sandbox remain `ABP_Worker.uasset`, `BP_AvaryoCharacter.uasset`, `L_Dom.umap`, `Lvl_FirstPerson.umap`, and `BP_GameMode.uasset`; the large untracked animation/map asset set also predates this closure and was left untouched.

### Evidence limitations and intentional legacy state

- Static visual evidence is exhaustive across the currently allowed Torso/FullOutfit catalog, but animation evidence samples four deterministic idle times rather than every animation frame or every possible future animation.
- Shipping exclusion was verified from compile guards; a Shipping package was not built because this task required the already-successful Editor target and C++ did not change afterward.
- Several auxiliary Python/MCP harness probes produced recoverable scripting errors while discovering reflected API names, and an immediate close+reopen in one Python tick retained deferred-destruction preview actors. Those probes are not production failures. The final Random scope is clean, and close/reopen was repeated on separate frames and passed.
- The byte-for-byte restored user SaveGame intentionally preserves its pre-fix legacy active appearance, which contains quarantined `Jacket_Worker`/Brown-glove paths. This does not repopulate the runtime Random catalog: cold scan remains `Gloves candidates=0`, production Random always replaces it with `Gloves=None`, and explicit final validation rejects both glove paths. The file was not silently rewritten because the task explicitly required restoring the user's original SaveGame.

## Development Appearance Slot Inspector - 2026-07-13

### Outcome

- Added a development-only Appearance Slot Inspector over Character Customization. It is a transient C++-constructed UMG overlay, is hidden by default, opens/closes with `F9`, and is compiled/created only under `#if WITH_EDITOR || !UE_BUILD_SHIPPING`. No Widget Blueprint asset, appearance asset, map, material or SaveGame schema was modified.
- The panel lists all real `EWorkerSlot` values `0..12` (`Body`, `Head`, `Hair`, `Beard`, `Torso`, `Legs`, `Feet`, `Gloves`, `Headgear`, `FaceMask`, `Glasses`, `Vest`, `FullOutfit`). Every row shows presence/visibility, the runtime skeletal-mesh name, first runtime material and current/forced LOD; exact object/package/material paths are available through the tooltip, clipboard JSON and file dump.
- The actions are `SHOW ALL`, `HIDE CLOTHING`, `BODY ONLY`, `HIDE BODY`, `SHOW BODY`, `CLEAR SOLO`, `COPY APPEARANCE`, `DUMP JSON`, `FORCE LOD 0`, `AUTO LOD`, plus per-slot `HIDE/SHOW` and `SOLO`.
- Toggle/Solo/LOD state is runtime-only preview state. It never clears or replaces a mesh, destroys a component, changes materials, changes `FWorkerAppearance`, calls `CommitActiveCharacterAppearance`, changes appearance origin/meaningful state, or invokes autosave. The implementation only calls visibility/hidden-in-game/forced-LOD render-state operations on preview components.
- The full UBT build succeeded, the final DLL was tested after a cold Editor restart, all requested runtime checks passed, the user SaveGame was restored byte-for-byte, dirty content/map packages are `0/0`, and no commit was created.

### Architecture and exact slot-to-component mapping

- `UWorkerAppearanceComponent` already owns the authoritative `TMap<EWorkerSlot, TObjectPtr<USkeletalMeshComponent>> SlotComps`. A guarded `GetAppearanceInspectorSlotComponent(EWorkerSlot)` accessor exposes that exact mapping to development inspection; no component-name guessing is used.
- `AAvCustomizePreviewActor` owns the transient inspector state: hidden-slot set, optional solo slot, independent Body-hide flag and force-LOD0 flag. `ApplyAppearanceInspectorState()` resolves components afresh through `SlotComps`, so there are no retained/stale component pointers.
- `SetPreviewAppearance()` and the slot-color rebuild path reapply the inspector state after normal appearance construction. This is why Random can replace meshes while a slot remains hidden and the row immediately changes to the new exact asset. `SHOW ALL` clears all debug hiding/solo and restores normal preview visibility without touching leader pose, meshes or materials.
- Solo shows `Body + Head + selected slot` by default and hides other present slots. Pressing `HIDE BODY` after Solo gives the requested selected-slot-without-Body comparison. `CLEAR SOLO` and `SHOW ALL` both provide a deterministic recovery path.
- Rows are generated from the enum with one payload-aware `UAvAppearanceInspectorSlotButton` class rather than 26 separate handlers. The panel is attached to the existing root `CanvasPanel` at runtime. The WBP was inspected read-only and not saved or compiled because no binary asset was changed.
- `F9` is routed reliably through the development-only PlayerController/HUD path when Character Customization is open, with a root-widget key fallback. In Shipping these entry points are inert and the overlay is never constructed.
- Clipboard support required adding `ApplicationCore` to `Avaryo.Build.cs`. `COPY APPEARANCE` writes the same structured JSON to the system clipboard and Output Log.

### Runtime verification

- Real keyboard `F9` opened and closed the initially hidden panel. The runtime log records `Panel=Visible/Hidden AppearanceMutation=false Autosave=false`.
- All 13 rows were present. Empty slots displayed `None/Empty`; populated rows displayed the correct current component, mesh and material. Torso and Legs individual Toggle operations hid only their exact mapped components.
- `HIDE BODY` left clothing visible. `BODY ONLY` displayed only canonical Body and Head. `SHOW BODY`, `SHOW ALL` and `CLEAR SOLO` restored the expected visibility.
- Torso Solo and Legs Solo passed both with Body and without Body. FullOutfit Solo was exercised on exact asset `/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_2.SKM_Worker_Tshirt_Pants_2`; its row remained visible while Gloves, Headgear and FaceMask were hidden.
- `FORCE LOD 0` changed every present component row to `LOD0/P0`; `AUTO LOD` returned the rows to `AUTO/P0`. Both only changed the preview component's forced LOD model.
- `COPY APPEARANCE` produced valid JSON in the clipboard and Output Log. `DUMP JSON` produced valid 13-slot files under `Saved/CodexDiagnostics/AppearanceInspector`.
- Random while Torso was hidden rebuilt the appearance with `SKM_Jacket_Worker_Hood_2_Bege`, refreshed the row, and kept the new Torso component hidden. `SHOW ALL` then displayed that new Torso. This proves state reapplication and absence of stale component pointers.
- Reset after debug toggles returned exactly the canonical Factory Body and Head with Torso/Legs/FullOutfit empty. The first Random after Reset produced `SKM_Jacket_Worker_Hood_2_Bege + SKM_Pants_Worker_Blue`, passed final validation and opened no modal. Repeated Random remained modal-free and continued to report `PreviewSaveUnchanged=true`.
- Closing/reopening the inspector did not change the active appearance. No appearance mutation or autosave was emitted by Toggle, Solo, visibility, LOD, Copy or Dump actions.

### SaveGame/autosave boundary

- Pre-change file: `Saved/SaveGames/AvariikaCompany.sav`, 7548 bytes, SHA-256 `78D2202A58A1A46DC8FDA43A0C88A6318189462381F40A55949A9E81FBC22B2D`.
- Retained backup: `Saved/SaveGameBackups/AppearanceInspector_20260713/AvariikaCompany.prechange.sav`, same size and SHA-256.
- The SaveGame hash before the debug actions and after Toggle/Solo/Body visibility/LOD/Copy/Dump was identical to the pre-change SHA. Deliberate Random/Reset smoke correctly exercised the production save path and changed the test copy; after PIE ended, the retained backup was restored. Final SaveGame size/SHA exactly match the pre-change values.
- Runtime state lives only on the transient preview actor/root widget and is absent from all serialization and save/autosave calls.

### Diagnostic result for the reported small points

- The reproduced hood appearance was:
  - Torso `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Bege.SKM_Jacket_Worker_Hood_2_Bege`.
  - Torso runtime materials: two sections using `/Game/Modular_Workers/Materials/Clothes_Modules/Jacket_Worker/MI_Jacket_Worker_Bege.MI_Jacket_Worker_Bege`.
  - Legs `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_Gray.SKM_Pants_Worker_Gray` with `MI_Pants_Worker_Gray` and `MI_Boots_Black`.
- Sleeve/cuff/shoulder/reflective-strip surface details remained on the floating hood jacket in Torso Solo after Body was hidden. They therefore belong to the Torso mesh/material presentation and are not clipping from the canonical Body.
- Pants/pocket/crotch/reflective-strip surface details remained on the floating pants in Legs Solo after Body was hidden. They therefore belong to the Legs mesh/material presentation and are not Body clipping or Torso overlap.
- The bib/overall in the exercised FullOutfit is part of the FullOutfit component. The previously diagnosed torn `Pants_Worker_Full` bib remains part of `EWorkerSlot::Legs`; there is still no separate Vest component involved.
- Forced LOD0 and Auto produced no visible change in the reproduced points at the fixed audit camera, so this run found no evidence that those points are caused by an LOD switch.
- The inspector proves component attribution and the Body/LOD boundaries. It does not by itself distinguish authored geometry from a specific material section or opacity texture inside the same component; no material or asset repair was attempted in this task.

### Evidence

- Primary diagnostic dump: `Saved/CodexDiagnostics/AppearanceInspector/appearance_inspector_20260713_125133_876.json`, 12,943 bytes, valid JSON with 13 slots, SHA-256 `93267CAE04074EFA138B62BD5542AD53BE1A0ED2EE2A5E39C338D89F7DD465A9`.
- The dump includes timestamp, origin, active category, preview source, stable slot combination, every exact appearance/runtime object and package path, material asset/runtime paths, slot visibility, forced/predicted LOD and explicit `appearance_mutated=false`, `autosave_invoked=false` diagnostics.
- Screenshot directory: `Saved/CodexScreenshots/AppearanceInspector_20260713`, 24 nonempty PNGs, 32,758,346 bytes total. Required `01_panel_open.png` through `10_show_all_restored.png` are present.
- Additional proof includes `11_random_reapplied_hidden_torso.png`, `12_random_show_all.png`, `13_fulloutfit_solo.png`, `14_problem_hood_torso_solo_with_body.png`, `15_problem_hood_torso_solo_without_body.png`, `16_problem_hood_legs_solo_with_body.png`, `17_problem_hood_legs_solo_without_body.png`, `18_problem_hood_force_lod0.png`, `19_problem_hood_auto_lod.png`, `20_reset_factory.png` and `21_first_random_after_reset.png`.
- Source backups are retained under `Saved/SourceBackups/AppearanceInspector_20260713`.

### Build, final state and limitations

- Text/source files changed for this inspector: `Source/Avaryo/Avaryo.Build.cs`; `Source/Avaryo/{Public,Private}/Components/WorkerAppearanceComponent.{h,cpp}`; `Source/Avaryo/{Public,Private}/UI/AvCustomizePreviewActor.{h,cpp}`; `Source/Avaryo/{Public,Private}/UI/AvAppearanceInspectorSlotButton.{h,cpp}`; `Source/Avaryo/{Public,Private}/UI/AvCharacterCustomizationRootWidget.{h,cpp}`; `Source/Avaryo/{Public,Private}/UI/AvaryoHUD.{h,cpp}`; `Source/Avaryo/Private/Game/AvaryoPlayerController.cpp`; and this handoff. No binary asset was changed.
- Final command: `C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat avariikaEditor Win64 Development -project=C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject -WaitMutex`.
- Final UBT result: `Succeeded` in 5.51 seconds. The first compile exposed a C4458 shadowed local and the first link exposed the missing `ApplicationCore` dependency; both were corrected before the successful build and cold restart.
- Final Editor state: open and responsive, PIE stopped. Save All reported success. Dirty packages: total `0`, content `0`, map `0`. No temporary actor/asset was created and no `.uasset`/`.umap` was changed.
- Final `git status --short`: 610 existing/work-session entries (`31` tracked, `579` untracked). The pre-task baseline was 608 (`31` tracked, `577` untracked); unrelated user/prior-task changes were preserved. Relevant tracked files remain modified and the C++ UI/handoff files that were already outside the repository index remain untracked. No commit or cleanup was performed.
- Final overall tracked `git diff --stat`: `31 files changed, 3867 insertions(+), 2097 deletions(-)`. Task-path `git diff --check` was clean. Because several relevant UI sources and this handoff are untracked, they do not appear in Git's tracked diff stat; the successful UBT/cold runtime is the verification for their compiled contents.
- Runtime/project log audit found no task-code Fatal, ensure, assertion or inspector error. UE 5.8 still emits its existing built-in startup UnifiedError/Automation self-test records and two startup `LogUtils: Error: The Editor is currently in a play mode` records. One read-only Python discovery attempt used the unavailable name `WidgetBlueprintLibrary`; it failed harmlessly and was replaced by the available `WidgetLibrary`/object iterator. These are test-environment records, not inspector runtime failures.
- Rotation/zoom was intentionally not started. The current camera supports the requested component isolation but not pixel-level magnification or per-material-section hiding; exact material paths are supplied for the next focused asset/material investigation.

## Post-fix Random visual regression and exhaustive asset-level re-audit - 2026-07-13

### Outcome

- The previous 100-Random/24-frame conclusion was false. `after_visual_23.png` itself contained the same large torn bib openings and had been incorrectly counted as visually intact. Random sampling was removed as the proof of clothing-catalog integrity.
- The three new user-visible regressions were identified without pressing Random again. All three used a `Pants_Worker_Full` mesh in `EWorkerSlot::Legs` (`5`). The chest bib/panel is authored geometry belonging to that lower-slot mesh; it is not part of the Torso jacket, Vest or FullOutfit.
- Exact-path normalization and lookup were working. The root cause was incorrect manual classification: four exact `Pants_Worker_Full` color variants had exact eligibility entries but were marked `Intact`/allowed even though their shared bib geometry/mask is torn.
- All six variants in the `Pants_Worker_Full` shared geometry group are now runtime-quarantined. The four formerly allowed variants were individually reproduced and classified as damaged; `LightGrn` and `Orange` were already denied but remain in the same group quarantine.
- The existing eligibility layer was extended; no second Random catalog was created. An asset is eligible only when its exact record is `AssetVerifiedIntact`, its visual condition is `Intact`, its exact path matches, its evidence exists on disk and its shared geometry group is not quarantined.
- The deterministic clothing audit covered all 27 meshes that were allowed before this re-audit: 14 Torso, 8 Lower and 5 FullOutfit. It produced 73 required angle screenshots. Result: 23 exact meshes remain verified intact and four `Pants_Worker_Full` meshes are verified damaged/removed.
- The final cold-build runtime produced 200/200 valid Random results and covered every remaining allowed clothing mesh: Torso 14/14, Lower 4/4, FullOutfit 5/5. This stress result is supplementary to the deterministic evidence, not its replacement.
- Full UBT succeeded, the registered exhaustive automation test succeeded, Factory/Reset/Random/preset paging smoke passed, the user SaveGame was restored byte-for-byte, and dirty content/map packages are `0/0`. No commit was created.

### Three post-fix regression records

The complete records, including every `EWorkerSlot`, exact object/package path, family, old eligibility, material instances, attempts, rejected candidates, cleanup, validation and preview-boundary fields, are in `Saved/CodexDiagnostics/RandomVisualIntegrityFix_20260712/post_fix_regression_three_results.json`.

1. `2026.07.12-21.27.46:563`, `Scheme=Separate`, attempt 1, rejected 0, cleanup 0, fallback false, validation `PASS`, `PreviewSaveUnchanged=true`:
   - Body: `/Game/Modular_Workers/Mesh/Male/Body/Quantum_FaceRig/SKM_Qunatum_FaceRig_Body_Hight.SKM_Qunatum_FaceRig_Body_Hight`.
   - Head: `/Game/Modular_Workers/Mesh/Male/Body/Quantum_FaceRig/SKM_Quantum_FaceRig.SKM_Quantum_FaceRig`.
   - Torso: `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1/SKM_Jacket_Worker_Hood_1_Gray.SKM_Jacket_Worker_Hood_1_Gray`; material `MI_Jacket_Worker_Gray`.
   - Legs/bib: `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Blue.SKM_Pants_Worker_Full_Blue`; materials `MI_Pants_Worker_Blue`, `MI_Boots_Black`.
   - FaceMask: `/Game/Modular_Workers/Mesh/Male/Face_Modules/Respirator/SKM_Respirator_Black.SKM_Respirator_Black`; Skin `0.950,0.680,0.500`.
2. `2026.07.12-21.27.59:800`, `Scheme=Separate`, attempt 1, rejected 0, cleanup 1, fallback false, validation `PASS`, `PreviewSaveUnchanged=true`:
   - Body and Head: same canonical exact paths as record 1.
   - Torso: `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Blue.SKM_Jacket_Worker_Hood_2_Blue`; material `MI_Jacket_Worker_Blue`.
   - Legs/bib: `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Gray.SKM_Pants_Worker_Full_Gray`; materials `MI_Pants_Worker_Gray`, `MI_Boots_Black`.
   - Gloves `/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker/SKM_Gloves_Worker.SKM_Gloves_Worker`; FaceMask `/Game/Modular_Workers/Mesh/Male/Face_Modules/Respirator/SKM_Respirator.SKM_Respirator`; Skin `0.660,0.400,0.260`.
3. `2026.07.12-21.28.00:836`, `Scheme=Separate`, attempt 1, rejected 0, cleanup 1, fallback false, validation `PASS`, `PreviewSaveUnchanged=true`:
   - Body and Head: same canonical exact paths as record 1.
   - Beard `/Game/Modular_Workers/Mesh/Male/Face_Modules/Beard/SKM_Moustache.SKM_Moustache`.
   - Torso: `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_Blue.SKM_Jacket_Worker_Hood_2_Blue`; material `MI_Jacket_Worker_Blue`.
   - Legs/bib: `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Green.SKM_Pants_Worker_Full_Green`; materials `MI_Pants_Worker_Green`, `MI_Boots_Black`.
   - Gloves `/Game/Modular_Workers/Mesh/Male/Arms_Modules/Gloves_Worker/SKM_Gloves_Worker.SKM_Gloves_Worker`; Glasses `/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Aviator/SKM_Glasses_Aviator.SKM_Glasses_Aviator`; Skin `0.500,0.300,0.190`.

Deterministic reproductions were saved before any new Random action:

- `Saved/CodexScreenshots/RandomVisualIntegrityFix_20260712/PostFixRegression/regression_after_fix_01.png`, SHA-256 `5EBE54333C63F4E5012E670B73297459627DBDF6B4EF58199AB7771625314B49`.
- `Saved/CodexScreenshots/RandomVisualIntegrityFix_20260712/PostFixRegression/regression_after_fix_02.png`, SHA-256 `F844DE57360DD6D904921AABCF913D5730E0DF2833693C041CA64354F4598820`.
- `Saved/CodexScreenshots/RandomVisualIntegrityFix_20260712/PostFixRegression/regression_after_fix_03.png`, SHA-256 `29D6BAD1BF3AF947BF39F4FBBEB552F836E21458B1A7307FC77E69682D14F8B6`.

### Root cause and bib attribution

- `AvExactMeshPath(PackagePath)` builds `PackagePath + "." + AssetName`; scan uses `FAssetData::GetSoftObjectPath()` and final validation performs the same exact object-path lookup. The `.AssetName` suffix was not lost, package/object paths were not confused and redirectors were not bypassing the lookup.
- Missing metadata was already denied. Structural family flags did not override an exact deny. Scan and final validation already visited Torso, Legs, Vest and FullOutfit, including fallback. The catalog is local and rebuilt from Asset Registry; no cross-session candidate cache was used.
- The old records for `SKM_Pants_Worker_Full_{Bege,Blue,Gray,Green}` explicitly said `VisualCondition=Intact`, `bAllowInRandom=true`. Consequently scan and final validation correctly accepted bad metadata. This was classification error, not an implementation path bypass.
- The bib changes Blue/Gray/Green with the Legs mesh while the Torso may remain identical. `Pants_Worker_Full` has pants/bib and boots material slots and geometry extending from footwear through the chest/back bib. Hood jackets expose their own tactical-jacket material slots only.
- `Vest` exists as slot 11 but has zero eligible candidates and was absent in all three results. `FullOutfit` exists as slot 12 and was also absent. The visible damage is authored in the Legs asset.
- Isolation with canonical Body+Head, a neutral verified T-shirt and no accessories showed the same back/side/lower-bib tears in every formerly allowed `Pants_Worker_Full` variant. No shared master material was changed.

### Exact asset-level eligibility implementation

- `FAvRandomAssetEligibility` now stores `AssetPath`, `AuditStatus`, `VisualCondition`, `SharedGeometryGroup`, `SharedMaterialGroup`, `AuditEvidence`, `LastAuditDate`, `AuditVersion`, `QuarantineReason`, `RequiredUnderlayer`, incompatible exact assets/families, required families and Body coverage.
- Audit status values are `Unverified`, `VerifiedIntact`, `VerifiedDamaged`, `Quarantined`. The runtime allow condition is derived, not independently trusted: only `VerifiedIntact + Intact` becomes `bAllowInRandom=true`.
- `Unverified` remains deny. Family verification cannot replace exact asset verification. Group quarantine is checked after exact lookup and cannot be overridden by a color variant.
- Selected-result logs now prove the chain `SelectedExactPath -> SelectedPackagePath -> EligibilityExactPath -> AuditStatus -> VisualCondition -> Allow -> SharedGeometryGroup -> GroupQuarantined -> AuditEvidence -> final Result`.
- Final validation repeats exact metadata, group quarantine, evidence and incompatibility checks for every selected non-factory slot. Fallback is validated through the same path. A saved/legacy appearance is duplicate-check input only and cannot carry a denied asset into a newly generated result.
- Catalog logs include a serial and explicitly say `Source=AssetRegistry LocalRebuild=true CachedCandidates=false PreviousAppearanceUse=DuplicateCheckOnly`.
- Allowed clothing evidence points to the deterministic per-asset directories. The 45 allowed accessory records point to existing files under `AssetAudit/Accessories/accessory_<Slot>_<Asset>.png`. The exhaustive self-test now verifies physical `FileExists`/`DirectoryExists`, not merely a nonempty evidence string.

### Shared geometry/material quarantine

Runtime quarantines the complete shared geometry group `Pants_Worker_Full` with reason `PostFixVisualRegression; AuthoredBibDamage; SharedDamagedGeometryOrMask; NeedsDeterministicAssetAudit`:

- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Bege.SKM_Pants_Worker_Full_Bege`.
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Blue.SKM_Pants_Worker_Full_Blue`.
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Gray.SKM_Pants_Worker_Full_Gray`.
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Green.SKM_Pants_Worker_Full_Green`.
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_LightGrn.SKM_Pants_Worker_Full_LightGrn`.
- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Orange.SKM_Pants_Worker_Full_Orange`.

The four variants in the former allowlist were individually visually proven damaged and are `VerifiedDamaged` in the re-audit manifest. `LightGrn` and `Orange` were already denied; they remain quarantined because they share the same geometry/material-mask family. Runtime allowed count changed from 72 to 68: clothing 27 -> 23, accessories remain 45.

### Deterministic exhaustive clothing audit

- Before manifest: `Saved/CodexDiagnostics/RandomVisualIntegrityFix_20260712/curated_clothing_manifest_before_reaudit.json`; Torso 14, Lower 8, FullOutfit 5, Vest 0, total 27.
- After manifest: `Saved/CodexDiagnostics/RandomVisualIntegrityFix_20260712/curated_clothing_manifest_after_reaudit.json`; VerifiedIntact Torso 14, Lower 4, FullOutfit 5, Vest 0, total 23; VerifiedDamaged 4; NeedsRepair 4.
- All assets used the canonical Body+Head, fixed skin/camera/lighting and no occluding Vest/accessories. Torso used one verified neutral Lower; Lower used a verified neutral T-shirt; FullOutfit excluded separate Torso/Lower/Vest.
- Required screenshots: 42 Torso (`14 x front/left/right`), 16 Lower (`8 x front/back`), 15 FullOutfit (`5 x front/left/right`) = 73. All are nonempty. Eight contact sheets are additional evidence.
- Location: `Saved/CodexScreenshots/RandomVisualIntegrityFix_20260712/ExhaustiveAssetAudit/{Torso,Lower,FullOutfit}`.
- Visual result: all 14 Torso meshes passed chest, abdomen, sides, armpits, shoulders, sleeves, collar, hem, bib/panel and Body-visibility review. The four standard `Pants_Worker` Lower meshes passed waist/crotch/back/thigh/knee/leg/footwear review. All five allowed FullOutfits passed front and both 3/4 views. All four `Pants_Worker_Full` variants failed authored bib/back/side tear review.

### Complete VerifiedIntact clothing after re-audit

Torso (14):

- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker/SKM_Jacket_Worker_{Bege,Blue,Gray}`.
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1/SKM_Jacket_Worker_Hood_1_{Bege,Blue,Gray}`.
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_2/SKM_Jacket_Worker_Hood_2_{Bege,Blue,Gray}`.
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Shirt_RolledUp/{SKM_Shirt_RolledUp_RedCage,SKM_Shirt_RolledUp_White_Cage}`.
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_{Bege,Black,Blue}`.

Lower (4, integrated footwear):

- `/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker/SKM_Pants_Worker_{Bege,Blue,Gray,Green}`.

FullOutfit (5):

- `/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_{1,2,3,4,5}`.

Vest: none. Braced names above expand to separate exact object paths with the identical `.AssetName` suffix; the manifest contains every expanded object/package path.

### Complete damaged result from the former allowlist

The only failures among the 27 previously allowed clothing assets were:

- `SKM_Pants_Worker_Full_Bege` - `VerifiedDamaged`, `OpacityMaskDamage`, `NeedsRepair`.
- `SKM_Pants_Worker_Full_Blue` - `VerifiedDamaged`, `OpacityMaskDamage`, `NeedsRepair`.
- `SKM_Pants_Worker_Full_Gray` - `VerifiedDamaged`, `OpacityMaskDamage`, `NeedsRepair`.
- `SKM_Pants_Worker_Full_Green` - `VerifiedDamaged`, `OpacityMaskDamage`, `NeedsRepair`.

They are not repaired in this task and no damaged variant was restored for diversity. The broader pre-existing deny list remains documented in the previous 2026-07-12 section below.

### Exhaustive automation and final 200-Random stress

- Added registered test `Avariika.WorkerAppearance.RandomEligibilityExhaustive` in `Source/Avaryo/Private/Tests/WorkerAppearanceEligibilityTests.cpp`.
- Direct C++ result after the final cold build: `PASS Records=142 Allowed=68 Denied=74 ScanCandidates=68 AllowedFinalValidationPasses=68 DeniedFinalValidationRejects=74 LegacyCarryoverRejects=74 Fallback=PASS Errors={}`.
- Unreal Automation Controller found exactly one matching test and completed it with `Result={Success}`; queue empty, one test performed.
- For every allowed record the test proves canonical exact object path, `AssetVerifiedIntact`, `Intact`, group not quarantined, no deny/quarantine reason, physical evidence exists, scan presence and final-validation acceptance.
- For every denied record it proves absence from scan, rejection by final validation, absence from fallback and inability of a saved/legacy previous appearance to carry it into a new Random result.
- Final 200-run markers in `Saved/Logs/avariika.log`: `CODEX_POSTFIX_FINAL_RANDOM200_BEGIN` at line 4654, `CODEX_POSTFIX_FINAL_RANDOM200_END` at line 12225; actual UI delegate path `Btn_RandomAppearance.OnClicked.Broadcast`.
- 200 requested/applied; 200 validation PASS, 200 visual-quality PASS, 200 eligibility true, 200 `Unverified=false`, 200 `Damaged=false`, 200 `Quarantined=false`, 200 lower coverage true, 200 preview-save unchanged, 200 conflicts zero, 200 invalid components zero, 200 preset false. Rejects/fallback/modal/quarantined paths/missing lower/missing footwear/preset copies/preview mutations: 0.
- In the final Random/automation/Save-All range, Fatal 0, Ensure 0, `: Error:` 0, Assertion 0 and automation failures 0.

Final exact coverage counts:

- Torso: `Jacket_Worker_Bege 7`, `Blue 2`, `Gray 11`; `Hood_1_Bege 8`, `Blue 10`, `Gray 9`; `Hood_2_Bege 18`, `Blue 6`, `Gray 17`; `Shirt_RolledUp_RedCage 9`, `White_Cage 12`; `TShirt_Bege 13`, `Black 24`, `Blue 13` = 14/14 covered.
- Lower: `Pants_Worker_Bege 40`, `Blue 48`, `Gray 37`, `Green 34` = 4/4 covered.
- FullOutfit: `SKM_Worker_Tshirt_Pants_1 12`, `_2 8`, `_3 9`, `_4 8`, `_5 4` = 5/5 covered.
- Separate scheme 159; FullOutfit scheme 41. No deterministic coverage supplement was required because RNG covered all 23 exact clothing assets.
- Machine-readable result: `Saved/CodexDiagnostics/RandomVisualIntegrityFix_20260712/post_fix_random_200_summary.json`.

### Build and regression smoke

- Final command: `UE_5.8/Engine/Build/BatchFiles/Build.bat avariikaEditor Win64 Development -project=C:/unrealEngine/avariika_UE58_sandbox/avariika.uproject -WaitMutex`.
- The final compile linked the updated `WorkerAppearanceComponent.cpp` and test into the editor target; UBT `Result: Succeeded` in 5.15 seconds. A second exact command returned exit code 0, target up to date, `Result: Succeeded` in 0.74 seconds. Only the existing preferred-compiler warning remained.
- Reset from a meaningful Random opened the existing confirmation modal. Confirm applied the canonical appearance slots: complete factory Body plus Head only. The hidden base character mesh and first-person mesh are not worker appearance slots.
- First Random after confirmed Reset: validation PASS, intact Torso+Lower with integrated footwear, no modal. Repeated Random: validation PASS, no modal.
- Three real `Btn_PresetNext.OnClicked` broadcasts without Apply left active player mesh paths identical. SaveGame SHA-256 was `840357D8FC7CC079CCEE9F384D0ADF721B854CCD8C7EE15CB53DF5E5E7140F93` before and after paging.
- Every applied stress result logged `Origin=RandomGenerated` and traversed the existing ledger autosave path. Origin/modal/reset/preset code was not rewritten by this eligibility change.

### SaveGame, screenshots, Editor state and worktree

- Pre-change SaveGame and retained backup: `Saved/SaveGameBackups/RandomVisualIntegrityFix_20260712/PostFixRegression_20260713/AvariikaCompany.prechange.sav`, 7000 bytes, SHA-256 `A2CE0BAFBA01396B518236588E36C58C824EABB44F56C26C520580FB5150A762`.
- Final working `Saved/SaveGames/AvariikaCompany.sav` was restored from that backup and has the same SHA-256. Backup was not deleted.
- Required visual evidence created by this task: three regression screenshots plus 73 exhaustive audit screenshots = 76. Eight contact sheets are extra, for 84 image artifacts in these two task subtrees.
- Final Editor is open in the sandbox; PIE false. Final Save All returned true. Dirty content packages 0; dirty map packages 0.
- `git status --short`: 608 entries because the large pre-existing unrelated/untracked worktree was preserved. Task-visible source/docs status: modified `WorkerAppearanceComponent.cpp/.h`, untracked `WorkerAppearanceEligibilityTests.cpp`, untracked handoff.
- Full tracked `git diff --stat`: 31 files, 3828 insertions, 2097 deletions. Task-relevant tracked source diff: two WorkerAppearance files, 1963 insertions, 41 deletions; the test and handoff are untracked and therefore absent from that stat.
- No staging, revert, cleanup, commit or modification of unrelated user work was performed.

### Evidence and proof limits

- The three PNGs are deterministic reproductions of the exact logged meshes/materials after the original manual states had already been navigated; the log/JSON preserves the original timestamps and exact combinations.
- Visual intact/damaged classification remains a human/agent review of fixed front/side/back imagery, not a mathematical proof of every possible animation deformation. Every allowed clothing mesh nevertheless has its own required screenshots and no clothing result relies on Random sampling.
- UE 5.8 emits 17 `LogAutomationTest: Error: Condition failed` lines during engine startup immediately after its own `UE::UnifiedErrorTest` diagnostics. They occur before task test markers. The final Random/automation/Save-All evidence range has zero Fatal/Ensure/Error/Assertion, and the project automation test itself reports Success.
- Direct Python decoding of protected SaveGame record fields was not used as proof. Random origin is proven by the applied runtime log and the existing C++ ledger call; autosave is additionally proven by the changed test SaveGame hash and unchanged hash during preset-only paging, followed by exact backup restoration.

## Random visual integrity curated eligibility fix - 2026-07-12

### Outcome

- Random now uses one exact-path curated eligibility layer inside the existing slot scanner and generator. No second catalog was created.
- A Random result must pass both structural validation and visual eligibility validation. Missing metadata means `Unverified`, and `Unverified` clothing is denied by default.
- All 97 scanned clothing meshes and 45 optional accessory candidates were audited at LOD0. Every audited pack mesh currently has exactly one runtime LOD.
- 142 candidates existed before the visual filter; 72 are eligible and 70 are denied. Clothing is 27 eligible of 97; all 45 audited accessories are eligible.
- The four invalid Jumpsuit LOD0 section mappings were saved through Unreal Editor and no longer warn after a cold Editor restart. They remain quarantined because their visual crotch/bib integrity is not proven.
- Full UBT succeeded. The real runtime UI path applied 100/100 Random results with 100 structural PASS and 100 visual PASS. Twenty-four additional runtime results were visually reviewed at 2048x2048.
- No commit was created. The large pre-existing unrelated/untracked worktree was preserved.

### Last seven manual Random records after `CODEX_RVI_BASELINE`

Chronological pairing is preserved even when a user's visual description did not identify the same authored slot structure as the runtime log. Complete timestamps, Skin, all slots, families, attempts, cleanup and preview-boundary fields are in `Saved/CodexDiagnostics/RandomVisualIntegrityFix_20260712/manual_random_combinations.json`.

1. `Yellow Vest + blue hood jacket + black pants` -> `Scheme=FullOutfit`; exact clothing mesh `/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Boots/SKM_Shirt_Jumpsuit_Boots_2.SKM_Shirt_Jumpsuit_Boots_2`; classified `AuthoredDamage` because the Jumpsuit/bib/crotch family has an authored brown/skin-like crotch region.
2. `Checked shirt + blue denim jumpsuit + boots` -> `Scheme=FullOutfit`; `/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Boots/SKM_Shirt_Jumpsuit_Boots_1.SKM_Shirt_Jumpsuit_Boots_1`; `AuthoredDamage`, same crotch/bib family.
3. `Yellow Vest + brown T-shirt + brown pants` -> `Torso=/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_Red.SKM_TShirt_Red` (`OpacityMaskDamage`), `Legs=/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Bege.SKM_Pants_Worker_Full_Bege` (`Intact`), `Vest=/Game/Modular_Workers/Mesh/Male/Chest_Modules/Vest_Worker/SKM_Vest_Worker_Yellow.SKM_Vest_Worker_Yellow` (`OpacityMaskDamage`).
4. `Checked shirt + black jumpsuit + boots` -> `Scheme=FullOutfit`; `/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Shirt_Boots/SKM_Shirt_Jumpsuit_Boots_4.SKM_Shirt_Jumpsuit_Boots_4`; `AuthoredDamage`, same crotch/bib family.
5. `Yellow Vest + dark hood jacket + green pants` -> `Torso=/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker/SKM_Jacket_Worker_Gray.SKM_Jacket_Worker_Gray` (`Intact`), `Legs=/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Green.SKM_Pants_Worker_Full_Green` (`Intact`), `Vest=/Game/Modular_Workers/Mesh/Male/Chest_Modules/Vest_Worker/SKM_Vest_Worker_Yellow.SKM_Vest_Worker_Yellow` (`OpacityMaskDamage`).
6. `Blue Vest + dark T-shirt + orange pants` -> `Torso=/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Tshirt/SKM_TShirt_Black.SKM_TShirt_Black` (`Intact`), `Legs=/Game/Modular_Workers/Mesh/Male/Pants_Modules/Pants_Worker_Full/SKM_Pants_Worker_Full_Orange.SKM_Pants_Worker_Full_Orange` (`OpacityMaskDamage`), `Vest=/Game/Modular_Workers/Mesh/Male/Chest_Modules/Vest_Worker/SKM_Vest_Worker_Blue.SKM_Vest_Worker_Blue` (`OpacityMaskDamage`).
7. `Bright-green hood jacket + brown bib/overall + brown pants` -> `Torso=/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1/SKM_Jacket_Worker_Hood_1_LightGr.SKM_Jacket_Worker_Hood_1_LightGr` (`OpacityMaskDamage`), `Legs=/Game/Modular_Workers/Mesh/Male/Pants_Modules/Jumpsuit/SKM_Jumpsuit_Bege.SKM_Jumpsuit_Bege` (`InvalidLODMaterialSection` plus authored bib/crotch damage).

All seven had structural validation `PASS`, attempts 1, rejected 0, reroll false, fallback false and `PreviewSaveUnchanged=true`. Cleanup was 0 for records 1-6 and 1 for record 7.

### Root causes and material evidence

- Structural validation previously knew only folder family coverage, footwear, FullOutfit conflicts and hood/headgear conflicts. `FAvRandomCandidate` contained only `Asset` and `Family`; it had no damaged/intact state, underlayer requirement or exact asset eligibility. Therefore a correct slot graph could still contain authored holes or bad material sections.
- The pack clothing materials use masked rendering through `/Game/Modular_Workers/Materials/Quanum_MasterMaterial/M_Quantum_Master_Material.M_Quantum_Master_Material`; opacity mask is connected and the affected instances enable authored alpha data. Repeated holes on different underlayers are in the clothing alpha/geometry, not caused by the underlayer.
- Yellow and blue vests use `/Game/Modular_Workers/Materials/Chest_Modules/Vest_Worker/MI_Helmet_Worker_Yellow.MI_Helmet_Worker_Yellow` and `MI_Helmet_Worker_Blue.MI_Helmet_Worker_Blue`. The same repeated alpha-mask holes appear over different intact torso meshes: `OpacityMaskDamage`.
- The four Jumpsuits had two material slots but serialized LOD0 Section 1 material index 2. Loader fallback selected index 1. Their exact clothing instances are `/Game/Modular_Workers/Materials/Pants_Modules/Jumpsuit/MI_Jumpsuit_DarkBlue`, `MI_Jumpsuit_Bege`, `MI_Jumpsuit_Blue` and `MI_Jumpsuit_Black`; boots use `/Game/Modular_Workers/Materials/Pants_Modules/Boots/MI_Boots_Black`. This was `InvalidLODMaterialSection`; visible bib/crotch damage independently keeps the family out of Random.
- `/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_6` combines `MI_Tshirt_Tucked_Bege`, `MI_Pants_Worker_LightGreen` and `MI_Boots_Black`; its side opening is authored masked damage, not runtime clipping.
- `/Game/Modular_Workers/Mesh/Male/Overalls_Modules/Overalls_Worker_Tshirt/SKM_Worker_Tshirt_Jumpsuit_2` combines `MI_Boots_Bege`, `MI_Tshirt_Tucked_Bege` and `MI_Jumpsuit_Blue`; its Jumpsuit/crotch family is not visually safe.
- `/Game/Modular_Workers/Mesh/Male/Clothes_Modules/Jacket_Worker_Hood_1/SKM_Jacket_Worker_Hood_1_LightGr` uses `/Game/Modular_Workers/Materials/Clothes_Modules/Jacket_Worker/MI_Jacket_Worker_LightGreen.MI_Jacket_Worker_LightGreen` in both sections; the bright-green tears are authored alpha-mask damage.
- `SKM_Shirt_Jumpsuit_Boots_1/2/4` use the masked Jumpsuit Blue/Black instances together with white checked rolled-shirt and protective-boots instances. The brown/skin-like crotch region follows the authored FullOutfit/Jumpsuit construction rather than the canonical Body or Random layering.
- No `BodyClipping` or required body-region hiding was found among eligible assets. The canonical complete Body remains present; no full-Body global hide and no old modular BareTorso/BareHands/UnderPants/Feet stack was introduced.

### Exact-path eligibility architecture

- Implemented in `Source/Avaryo/Private/Components/WorkerAppearanceComponent.cpp` beside the existing `RandomFamilies` scanner.
- `EAvRandomVisualCondition` values: `Unverified`, `Intact`, `AuthoredDamage`, `OpacityMaskDamage`, `InvalidLODMaterialSection`, `BodyClipping`, `LayerConflict`, `MissingRequiredUnderlayer`, `Quarantined`.
- `FAvRandomAssetEligibility` is keyed by exact `FSoftObjectPath` and carries `bAllowInRandom`, slot, `VisualCondition`, `QuarantineReason`, exact `RequiredUnderlayer`, `RequiredFamilies`, `IncompatibleFamilies` and `BodyCoverageRequirement`.
- `FAvRandomCandidate` now carries the resolved eligibility record. The existing asset scan rejects missing metadata and all non-`Intact` entries before candidate selection, logging exact asset path, condition and reason.
- `ValidateRandomGeneratedMaleAppearance()` rechecks exact eligibility for every selected non-factory slot before structural checks, including FullOutfit and fallback results. Factory Body and Head are explicitly exempt from the clothing allowlist.
- `DescribeRandomVisualEligibility()` emits exact slot path, eligibility, condition, family, underlayer and Body coverage into every applied `[AvCustomizeRandom]` log. UI logs also retain Scheme, lower coverage, integrated footwear, conflicts, origin, preset=false, preview boundary and stable combination.
- Default policy is deny: a newly added or otherwise unlisted clothing asset is `Unverified` and cannot enter Random.

### Audited families and candidate counts

| Slot/family | Checked | Eligible | Denied |
|---|---:|---:|---:|
| Hair | 5 | 5 | 0 |
| Beard | 7 | 7 | 0 |
| Jacket_M65 | 3 | 0 | 3 |
| Jacket_Worker | 6 | 3 | 3 |
| Jacket_Worker_Hood_1 | 6 | 3 | 3 |
| Jacket_Worker_Hood_2 | 6 | 3 | 3 |
| Shirt_RolledUp | 8 | 2 | 6 |
| Tshirt | 6 | 3 | 3 |
| Tshirt_Tucked | 7 | 0 | 7 |
| Jeans | 5 | 0 | 5 |
| Jumpsuit | 4 | 0 | 4 |
| Pants_Worker | 6 | 4 | 2 |
| Pants_Worker_Full | 6 | 4 | 2 |
| Gloves | 2 | 2 | 0 |
| Headgear | 22 | 22 | 0 |
| FaceProtection | 2 | 2 | 0 |
| Glasses | 7 | 7 | 0 |
| Vest_Worker | 4 | 0 | 4 |
| Overalls_Shirt_Boots | 5 | 0 | 5 |
| Overalls_Shirt_Pants_Worker | 3 | 0 | 3 |
| Overalls_Tshirt_Boots | 5 | 0 | 5 |
| Overalls_Worker_Shirt | 5 | 0 | 5 |
| Overalls_Worker_Tshirt | 5 | 0 | 5 |
| Overalls_Worker_Tshirt_Pants | 7 | 5 | 2 |
| **Total** | **142** | **72** | **70** |

Slot totals before -> after: Hair 5->5, Beard 7->7, Torso 42->14, Lower 21->8, Gloves 2->2, Headgear 22->22, FaceProtection 2->2, Glasses 7->7, Vest 4->0, FullOutfit 30->5.

### Eligible intact clothing replacements

- Torso: `Jacket_Worker/{SKM_Jacket_Worker_Bege, SKM_Jacket_Worker_Blue, SKM_Jacket_Worker_Gray}`.
- Torso: `Jacket_Worker_Hood_1/{SKM_Jacket_Worker_Hood_1_Bege, _Blue, _Gray}`.
- Torso: `Jacket_Worker_Hood_2/{SKM_Jacket_Worker_Hood_2_Bege, _Blue, _Gray}`.
- Torso: `Shirt_RolledUp/{SKM_Shirt_RolledUp_RedCage, SKM_Shirt_RolledUp_White_Cage}`.
- Torso: `Tshirt/{SKM_TShirt_Bege, SKM_TShirt_Black, SKM_TShirt_Blue}`.
- Lower with integrated footwear: `Pants_Worker/{SKM_Pants_Worker_Bege, _Blue, _Gray, _Green}`.
- Lower with integrated footwear: `Pants_Worker_Full/{SKM_Pants_Worker_Full_Bege, _Blue, _Gray, _Green}`.
- FullOutfit: `Overalls_Worker_Tshirt_Pants/{SKM_Worker_Tshirt_Pants_1, _2, _3, _4, _5}`.
- All 45 exact optional accessory paths audited in Hair, Beard, Gloves, Headgear, Respirator and Glasses are intact and remain eligible.

### Denied/quarantined exact groups

- `OpacityMaskDamage`: all four `Vest_Worker` meshes; `Jacket_M65_{Bege,Black,Green}`; `Jacket_Worker_{Green,LightGreen,Orange}`; Hood 1/2 `{Green,LightGr,Orange}`; solid `Shirt_RolledUp_{Bege,Black,Blue,Green,Red,White}`; `TShirt_{Gray,Green,Red}`; all seven `Tshirt_Tucked`; `Pants_Worker_{LightGreen,Orange}`; `Pants_Worker_Full_{LightGrn,Orange}`.
- `InvalidLODMaterialSection` plus failed visual audit: `SKM_Jumpsuit_{DarkBlue,Bege,Blue,Black}`.
- `MissingRequiredUnderlayer`/footwear: `Jeans/{SKM_Jeans, SKM_Jeans_Bege, SKM_Jeans_Black, SKM_Jeans_Blue, SKM_Jeans_Turn}`. No visually verified standalone footwear candidate exists.
- `AuthoredDamage`: all five `Overalls_Shirt_Boots/SKM_Shirt_Jumpsuit_Boots_1..5`; `Overalls_Worker_Tshirt/SKM_Worker_Tshirt_Jumpsuit_2`; `Overalls_Worker_Tshirt_Pants/SKM_Worker_Tshirt_Pants_6..7`.
- `LayerConflict`: `Overalls_Shirt_Pants_Worker/SKM_Overalls_Shirt_Pants_Worker_1..3`, whose combined shirt layer inherits the damaged rolled-shirt material family.
- `Unverified`, denied by default: all five `Overalls_Tshirt_Boots`; all five `Overalls_Worker_Shirt`; `Overalls_Worker_Tshirt/SKM_Worker_Tshirt_Jumpsuit_{1,3,4,5}`. These 14 exact assets/families are the remaining unverified clothing set.
- No intact `Vest_Worker` replacement was found. Vest is temporarily unused instead of allowing a damaged asset.

### Jumpsuit section repair

- Binary backups made before save: `Saved/AssetBackups/RandomVisualIntegrityFix_20260712/JumpsuitSections/`.
- Backup/source SHA-256 matched before modification for all four assets.
- Unreal's Skeletal Mesh Editor subsystem exposed the resolved LOD0 mapping as section slots `[-1, 1]` but no material-slot setter. The loader's native 2->1 repair was saved through `EditorAssetLibrary.save_loaded_asset(..., only_if_is_dirty=false)` for each exact mesh; no `.uasset` was edited as text.
- After full Editor close/build/restart, all four loaded with one LOD, two sections and resolved mapping `[-1, 1]`; no `Fix LOD 0 Section 1 Material index from 2 to 1` warning appeared in the cold-load verification range.
- They remain visually quarantined; section warning removal was not treated as visual PASS.

### Build and automated runtime results

- Full command: `UE_5.8/Engine/Build/BatchFiles/Build.bat avariikaEditor Win64 Development -project=C:/unrealEngine/avariika_UE58_sandbox/avariika.uproject -WaitMutex`.
- UBT `Result: Succeeded`; total execution time 60.72 seconds. `WorkerAppearanceComponent.cpp`, `AvCharacterCustomizationRootWidget.cpp` and the module linked into `UnrealEditor-Avaryo.dll`.
- 100-run markers: `CODEX_RVI_RANDOM100_BEGIN` -> `CODEX_RVI_RANDOM100_END` in `Saved/Logs/avariika.log`, lines 2880-4885.
- Runtime path: `Btn_RandomAppearance.OnClicked -> HandleRandomAppearanceRequested -> ExecuteExistingRandomAppearance`.
- 100 requested, 100 applied, 100 structural PASS, 100 visual PASS, 100 unique stable combinations.
- 75 Separate, 25 FullOutfit. Attempt distribution: attempt 1 = 100. Validation rejects 0, rerolls 0, fallback 0, modal 0.
- Damaged/quarantined 0; `Unverified` 0; missing lower 0; missing footwear 0; FullOutfit conflict 0; hood/headgear conflict 0; preset copies 0; preview mutations 0; invalid components 0.
- Fatal/Error/Ensure/Assertion count inside the 100-run marker range: 0.
- Machine-readable result: `Saved/CodexDiagnostics/RandomVisualIntegrityFix_20260712/random100_summary.json`.

### Twenty-four-frame visual review

- Files: `Saved/CodexScreenshots/RandomVisualIntegrityFix_20260712/after_visual_01.png` through `after_visual_24.png`, all 2048x2048.
- Contact sheets: `contact_after_01.png` through `contact_after_04.png`.
- 24 unique results: 19 Separate and 5 FullOutfit. Views include full front, full side, torso, head/face protection, waist/crotch front and waist side.
- Reviewed defects: authored tears 0, opacity holes 0, visible Body/underwear through clothing 0, brown crotch patches 0, clipping 0, upper/lower gaps 0, missing footwear 0, hood/headgear conflicts 0.
- Included intact hood jackets, non-hood jackets, checked shirts, T-shirts, both safe lower families, five FullOutfit results, gloves and varied headgear/face protection. No vest result was included because all four vest assets are damaged.
- Before evidence: `before_problem_01.png` through `before_problem_04.png`; additional manual and focused audit evidence is under the same task directory and `AssetAudit/`.
- Machine-readable result: `Saved/CodexDiagnostics/RandomVisualIntegrityFix_20260712/visual24_summary.json`.

### Regression smoke

- Reset button opened the existing confirmation modal; Confirm returned the preview to exactly canonical Body + Head and authored underwear. No clothing or legacy BareTorso/BareHands/UnderPants/Feet components remained.
- First Random after Reset opened no modal and had upper coverage, lower coverage and integrated footwear; the next Random also opened no modal and produced a different combination.
- Three real `Btn_PresetNext.OnClicked` events changed the preview only. Active player appearance component paths were byte-for-byte equivalent before/after, and SaveGame SHA-256 remained `A2108888521382D7274243D75801CE00FDC72F54DC92707DB0CABBE6DAC40AD4` before/after paging without Apply.
- Random confirmation/origin, Reset, preset-preview boundary, autosave, character name, second-character data and the existing modal were not rewritten by this fix.

### SaveGame, final Editor state and worktree

- Working SaveGame before task edits: SHA-256 `778DE7D7A3CE780367E4D6F028D8DB9C5E8800AEDA317AD967B0A81C788F9780`, 6946 bytes.
- Preserved user backup: `Saved/SaveGameBackups/RandomVisualIntegrityFix_20260712/AvariikaCompany.prechange.sav`, SHA-256 `1261A0146064EFB3762E2C3DFB19B9298B96A558830190C0E58479941A15A1C4`, 8580 bytes.
- Test-mutated working SaveGame before restoration: SHA-256 `A2108888521382D7274243D75801CE00FDC72F54DC92707DB0CABBE6DAC40AD4`.
- Final working SaveGame after restoration: SHA-256 `1261A0146064EFB3762E2C3DFB19B9298B96A558830190C0E58479941A15A1C4`; exact backup match. Backup retained.
- Final Editor is open on the sandbox, PIE false. Final Save All returned true. Dirty content packages 0; dirty map packages 0.
- `git status --short` has 607 entries because the large pre-existing unrelated/untracked worktree remains. Task-relevant visible status includes modified `WorkerAppearanceComponent.cpp/.h` and untracked `AvCharacterCustomizationRootWidget.cpp` plus this handoff; Saved diagnostics/screenshots and pack content are ignored/untracked by repository policy.
- `git diff --stat`: 31 tracked files, 3389 insertions, 2079 deletions. This includes extensive unrelated pre-existing changes; no cleanup, revert, staging or commit was performed.

### Evidence paths

- Manual seven-result JSON: `Saved/CodexDiagnostics/RandomVisualIntegrityFix_20260712/manual_random_combinations.json`.
- 100-run summary: `Saved/CodexDiagnostics/RandomVisualIntegrityFix_20260712/random100_summary.json`.
- 24-frame summary: `Saved/CodexDiagnostics/RandomVisualIntegrityFix_20260712/visual24_summary.json`.
- Screenshots: `Saved/CodexScreenshots/RandomVisualIntegrityFix_20260712/`.
- Jumpsuit asset backups: `Saved/AssetBackups/RandomVisualIntegrityFix_20260712/JumpsuitSections/`.
- Manual/audit source log after Editor rotation: `Saved/Logs/avariika-backup-2026.07.12-20.40.14.log`.
- Final build/runtime log: `Saved/Logs/avariika.log`.

## Random mandatory clothing / Reset Confirm fixed-path verification - 2026-07-12

Scope:

- Continued from the interrupted session without rerunning the completed 50-Random audit or Reset Cancel.
- Repeated only the affected runtime path through real UI delegates:
  `RandomGenerated -> Reset button -> Reset confirmation modal -> Confirm -> Factory -> first Random`.
- Sandbox only: `C:\unrealEngine\avariika_UE58_sandbox`, UE `5.8.0`.

Saved 50-run evidence from the preceding session:

- Evidence log: `Saved/Logs/avariika-backup-2026.07.12-17.20.27.log`, markers `CODEX_RANDOM50_BEGIN|count=50` through `CODEX_RANDOM50_END|count=50`.
- 50/50 applied results, 50 runtime probes, 50 unique stable slot combinations.
- 43 `Separate`, 7 `FullOutfit`.
- 0 missing lower coverage in applied results; 0 missing footwear in applied results.
- 0 preset results; 0 `FullOutfit` with separate upper/lower conflict; 0 hood + separate Headgear conflict.
- `PresetPreviewAppearance` unchanged in all 50 applied results.
- 14 controlled rerolls for Jeans/no boot coverage; 12 compatibility cleanup removals of Headgear for hood; fallback 0.
- Invalid skeletal component sum 0; Fatal/Ensure/Assertion/Error count 0 inside the 50-run range.
- Twelve saved visual samples were already checked and were not repeated.

Narrow Reset fix:

- Removed the premature `PresetPreviewAppearance = FWorkerAppearance();` from `UAvCharacterCustomizationRootWidget::ExecuteResetToBase()` in `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp`.
- The preview payload must not be replaced by an empty appearance before the active factory appearance is committed. An empty staged appearance has no valid Body and an intervening/re-entrant preview apply can therefore fail to rebuild the preview and leave the previously rendered Random/preset components visible.
- Reset now delegates preview cleanup to the existing common commit path:
  - `CommitActiveCharacterAppearance()` sets `bHasPresetPreviewAppearance = false` and calls `ApplyActiveCharacterToPreview()`.
  - `ApplyActiveCharacterToPreview()` again deactivates preset-preview state and applies `ActiveCharacterAppearance` to the preview actor.
  - `SelectCategory(MyCharacters)` also calls `ApplyActiveCharacterToPreview()`.
- This architecture intentionally treats the stored `PresetPreviewAppearance` payload as inactive when `bHasPresetPreviewAppearance == false`; it is not mixed into or substituted for `ActiveCharacterAppearance`.
- Static review of the other paths confirms the same boundary: preset paging creates/activates the temporary payload; returning to My Characters deactivates it; Apply Preset commits the selected preview then deactivates it; Random and Reset commit active appearances through the common path. `RefreshMyCharactersPage()` was not added to `NativeConstruct()`.

Build evidence:

- UBT log: `C:\Users\admin\AppData\Local\UnrealBuildTool\Log.txt`.
- `Result: Succeeded`.
- `Total execution time: 5.24 seconds`.
- The build compiled `AvCharacterCustomizationRootWidget.cpp` and linked `UnrealEditor-Avaryo.dll` at `2026-07-12 17:20:44 UTC`.
- The edited source timestamp is `2026-07-12 17:19:18 UTC`; no C++/header/Build.cs file is newer than the built DLL.

Fixed Reset Confirm runtime result:

- Editor was already open on the correct sandbox project; PIE initially false; initial dirty content/map packages 0/0.
- Customization opened through the PIE PlayerController `AvCustomize` gameplay path.
- A complete RandomGenerated appearance was created through the runtime `Btn_RandomAppearance.OnClicked` delegate.
- Before Reset: validation PASS, `Origin=RandomGenerated`, `LowerCoverage=true`, `Footwear=Integrated`, `Preset=false`, `HeadConflict=false`, Body and Head present, all seven preview skeletal components valid/visible/active, Reset enabled, modal collapsed, and `PreviewSaveUnchanged=true`.
- `Btn_ResetAppearance.OnClicked` opened the existing modal. Runtime title was `СБРОСИТЬ ВНЕШНИЙ ВИД?`; modal visibility was `Visible`.
- `Btn_RandomAppearanceCreate.OnClicked` confirmed the Reset action.
- The exact `ExecuteResetToBase()` request committed the canonical factory appearance with `bHasMeaningfulAppearance=false`, `BaseMaleUnderwear`, and `EAvAppearanceOrigin::Factory`; autosave log was emitted.
- After Confirm: Reset disabled (opacity `0.4`), modal collapsed, My Characters active, and both preview and active-character runtime appearance contained exactly these visible/active meshes:
  - Body: `/Game/Modular_Workers/Mesh/Male/Body/Quantum_FaceRig/SKM_Qunatum_FaceRig_Body_Hight.SKM_Qunatum_FaceRig_Body_Hight`
  - Head: `/Game/Modular_Workers/Mesh/Male/Body/Quantum_FaceRig/SKM_Quantum_FaceRig.SKM_Quantum_FaceRig`
- The legacy full-preset `CharacterMesh0` and first-person hazmat mesh were hidden. No Worker clothes or separate BareTorso/BareHands/Legs/Feet/UnderPants components were stacked over the full factory Body.
- Fullscreen visual verification confirms head, torso, pelvis, both arms/hands, both legs/feet, and authored underwear are present immediately after Confirm; the previous Random appearance is absent.

First Random after Reset:

- Executed exactly once through `Btn_RandomAppearance.OnClicked` while Factory was active.
- No modal opened (`Overlay_RandomAppearanceConfirmModal=Collapsed`).
- Validation PASS after one controlled rejected candidate for missing footwear: `Attempts=2`, `MissingFootwear=1`, `Fallback=false`.
- Applied result: `Origin=RandomGenerated`, meaningful/Reset enabled, `Scheme=Separate`, `LowerCoverage=true`, `Footwear=Integrated`, `Preset=false`, `HeadConflict=false`, `PreviewSaveUnchanged=true`.
- Result contained canonical Body+Head, T-shirt, black Jumpsuit with integrated footwear, gloves, flat hat, aviator glasses, beard, and worker vest. All nine preview skeletal components were valid/visible/active.
- Autosave completed.

Screenshots:

- Factory immediately after fixed Confirm Reset: `Saved/CodexScreenshots/RandomMandatoryClothingFix_20260712/reset_factory_after_confirm_fixed.png`
- First Random after Reset: `Saved/CodexScreenshots/RandomMandatoryClothingFix_20260712/first_random_after_reset_fixed.png`

SaveGame preservation:

- Backup retained: `Saved/SaveGameBackups/RandomMandatoryClothingFix_20260712/AvariikaCompany.prechange.sav`.
- SHA-256 before this test: `E4F20FFC29AABAFF00069A78EE7A073A7B0148735BF6DAA33D31012E33FEAE83`.
- SHA-256 after Reset only: `8B1529FE7DA219EEE43074A67926A2F0E68E4B832B91CAB70A1131A8F4EF0056`.
- SHA-256 after the complete test: `BA58F4D19C65DE5982BDB5DFC337B178710423AA1AD01FA4F95CEC9A59D882C9`.
- SHA-256 after restoration: `E4F20FFC29AABAFF00069A78EE7A073A7B0148735BF6DAA33D31012E33FEAE83` (exact match to original and backup).

Final editor/log state:

- PIE stopped normally; `Save All` completed.
- Dirty content packages: 0.
- Dirty map packages: 0.
- No save dialog, temporary test asset, or debug C++ code was left.
- Formal verification range in `Saved/Logs/avariika.log`: `CODEX_FIXED_RESET|TEST_RANGE_BEGIN` through `CODEX_FIXED_RESET|TEST_RANGE_END`; no Fatal, Ensure, Assertion, `: Error:`, `LogPython: Error`, `LogTemp: Error`, or `Result=FAIL` occurred in that range.
- Before the formal marker, three failed read-only Python introspection probes logged API errors (`Text.to_string`, `is_registered`, `get_hidden_in_game`), and PIE startup emitted the existing MCP/Editor `The Editor is currently in a play mode` noise. These were automation-probe errors, not gameplay/Reset failures, and caused no asset or source mutation.

Git state:

- Branch: `main`.
- Existing worktree remains very dirty: 607 short-status entries (`3 D`, `28 M`, `576 ??`). No unrelated work was reverted.
- Task files are currently untracked in this baseline: `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp` and `Docs/AI/CODEX_HANDOFF_CURRENT.md`.
- Tracked `git diff --stat`: `31 files changed, 2883 insertions(+), 2079 deletions(-)`; five tracked binary files are included. The untracked customization source and handoff are not represented by that stat.
- No commit was created because the worktree contains a large unrelated/untracked baseline and the task-scoped source is not tracked by the current Git index.

Remaining verification limits:

- Protected C++ fields `ActiveCharacterAppearanceOrigin`, `bActiveCharacterHasMeaningfulAppearance`, and `bHasPresetPreviewAppearance` are not directly readable through Unreal Python. Their values were proven through the exact compiled commit arguments/path, Reset enabled state, runtime validation log, autosave log, and preview/active-character component state.
- No full Preset/Apply/Random/Reset regression suite was rerun; the unchanged paths were checked statically as requested.

## Character customization category visual state fix - 2026-07-09

Scope:

- Narrow fix for the left category menu visual state in `WBP_CharacterCustomizationRoot_V2`.
- Did not touch randomize logic, Apply/save/reopen logic, `WorkerAppearance`, preview/material/framing, Modular Police, the right panel, fonts, layout, or icon assets.

Diagnosis:

- Designer defaults had multiple inactive category active layers visible after the manual rename.
- Wrongly visible before the fix:
  - `Border_CategoryHeadActiveBG`
  - `Border_CategoryHairHeadwearActiveBG`, `SB_CategoryHairHeadwearActiveLine`, `Border_CategoryHairHeadwearActiveLine`
  - `Border_CategoryFaceProtectionActiveBG`, `SB_CategoryFaceProtectionActiveLine`, `Border_CategoryFaceProtectionActiveLine`
  - `Border_CategoryHandsAccessoriesActiveBG`, `SB_CategoryHandsAccessoriesActiveLine`, `Border_CategoryHandsAccessoriesActiveLine`
  - `Border_CategoryUpperGearActiveBG`, `SB_CategoryUpperGearActiveLine`, `Border_CategoryUpperGearActiveLine`
  - `Border_CategoryLowerHipActiveBG`, `SB_CategoryLowerHipActiveLine`, `Border_CategoryLowerHipActiveLine`
  - `Border_CategoryFullOutfitsActiveBG`, `SB_CategoryFullOutfitsActiveLine`, `Border_CategoryFullOutfitsActiveLine`
- That stacked the per-row active-line widgets into the apparent long orange vertical line and made `Head` look active while `Presets` was selected.
- Runtime C++ had `SelectCategory()` storing only the enum while `BindCategoryButtons()` owns the click path, so category clicks also needed a small native visual-state refresh after the Designer-default cleanup.

Implemented:

- In the WBP Designer defaults, only Presets active BG/line remains `Visible`; the other category active BG/line widgets are `Collapsed`.
- Added `UAvCharacterCustomizationRootWidget::RefreshCategoryVisuals()` and `ApplyCategoryVisual(...)`.
- `SelectCategory()` now refreshes the eight renamed rows so exactly one category has visible active BG/line and orange text/icon.
- Active tint stays the existing orange direction: `(0.95, 0.42, 0.04, 1.0)`.
- Inactive category text/icon tint is gray: `(0.56, 0.58, 0.62, 1.0)`.

Build:

- UBT build succeeded with UE 5.8:
  - `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject" -WaitMutex`
  - Warnings only: existing compiler-version/deprecated engine API warnings.

PIE verification:

- Editor was reopened after the build.
- PIE started through `LevelEditorSubsystem.editor_request_begin_play()`.
- Customization opened through the PIE PlayerController `AvCustomize` console path.
- First open runtime dump:
  - Presets active BG/line/text/icon visible/orange.
  - Head, HairHeadwear, FaceProtection, HandsAccessories, UpperGear, LowerHip, FullOutfits active BG/line collapsed and text/icon gray.
- `HandleCategoryHeadClicked()` runtime check:
  - only Head active BG/line/text/icon visible/orange.
  - Presets and all other rows collapsed/gray.
- `HandleCategoryHairHeadwearClicked()` runtime check:
  - only HairHeadwear active BG/line/text/icon visible/orange.
  - Head, Presets, and all other rows collapsed/gray.
- Visual screenshots confirm no full-height orange line and no simultaneous active categories.

Stale Blueprint warning:

- WBP compile still succeeds with warnings.
- Exact stale graph warning in the log:
  - `On Clicked (Btn_CategoryHair)  не содержит допустимый соответствующий компонент!`
- This is a stale Blueprint event graph reference from the old widget name. It did not break runtime category state because the C++ click path now owns and refreshes the category visuals. Left as TODO; no WBP graph edit was made.

Screenshots:

- `Saved/CodexScreenshots/CustomizeCategoryVisualState_20260709/pie_first_open_active_presets_fixed.png`
- `Saved/CodexScreenshots/CustomizeCategoryVisualState_20260709/pie_category_head_active_fixed.png`
- `Saved/CodexScreenshots/CustomizeCategoryVisualState_20260709/pie_category_hair_headwear_active_fixed.png`

Changed files:

- `Content/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2.uasset`
- `Source/Avaryo/Public/UI/AvCharacterCustomizationRootWidget.h`
- `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`
- `Docs/AI/CHARACTER_CUSTOMIZATION_CATEGORY_RENAME_PLAN.md`

## Character customization rename restore final PIE verification - 2026-07-09

Scope:

- Continued from the interrupted `CustomizeRenameRestore` verification in `C:\unrealEngine\avariika_UE58_sandbox`.
- No new rename work was started.
- No visual design, layout, colors, fonts, icons, preview/material/framing, `WorkerAppearance`, Modular Police, Blueprint graph, `.uasset`, or `.umap` changes were made.
- Documentation only was updated after verification.

Build status:

- The preceding C++ change set had already built successfully before this pass.
- No C++ or asset changes were made in this pass, so no new UBT build was required.

Editor / PIE status:

- Unreal Editor was already open in UE 5.8 for the sandbox project and MCP was reachable.
- PIE was started through `LevelEditorSubsystem.editor_request_begin_play()`.
- Customization was opened through the real PIE PlayerController `AvCustomize` path.
- PIE was stopped normally after the checks through `LevelEditorSubsystem.editor_request_end_play()`.

Verification results:

- First open showed the new 8-category left menu in order:
  - `Btn_CategoryPresets`
  - `Btn_CategoryHead`
  - `Btn_CategoryHairHeadwear`
  - `Btn_CategoryFaceProtection`
  - `Btn_CategoryHandsAccessories`
  - `Btn_CategoryUpperGear`
  - `Btn_CategoryLowerHip`
  - `Btn_CategoryFullOutfits`
- MCP `get_widget_tree` confirmed the WBP asset contains the same 8 button names and the matching renamed row children.
- `Btn_RandomAppearance` remains under `VB_RandomAppearance`, outside `VB_CharacterCategories`.
- HUD automation path was verified with `hud.call_method("TriggerCustomizeRandomAppearance")`.
- Runtime log confirmed randomize through C++:
  - `[AvCustomizeRandom] Random appearance -> Worker 24, skin tone 7`
- Screenshot after the UFUNCTION trigger showed the preview/right panel changed from Worker 25 to Worker 24 and skin tone changed.
- Randomize did not become an active category: `ПРЕСЕТЫ` stayed visually active, and the bottom randomize control remained outside the category selected state.
- Apply/save/reopen was verified:
  - Before apply body: `/Game/Modular_Workers/Mesh/Male/Presets/SKM_Worker_Male_25.SKM_Worker_Male_25`
  - Apply path log: `[AvCustomizeApply] Apply clicked -> RequestApplyWorkerAppearance (Apply+Save).`
  - Save path log: `[AvCustomize] Applied worker appearance saved as default.`
  - After apply body: `/Game/Modular_Workers/Mesh/Male/Presets/SKM_Worker_Male_24.SKM_Worker_Male_24`
  - Reopen showed Worker 24 still selected.

Log / warning status:

- No missing `BindWidgetOptional` warnings for old category names were observed during the final PIE open/random/apply/reopen flow.
- No C++ warning about missing old category buttons was observed.
- Current C++ does not emit individual per-category or random-button bind logs; binding was verified by the WBP tree, C++ `BindWidgetOptional` fields, and successful runtime behavior. The Apply button does still log `Bound visible Apply+Save button: Btn_SaveCharacter`.
- The exact stale Blueprint compile warning previously present in the log is:
  - `On Clicked (Btn_CategoryHair)  не содержит допустимый соответствующий компонент!`
- That stale warning appears to be an old WBP event graph reference left behind by the rename. It did not block runtime creation, randomize, Apply, save, or reopen in PIE. Left as TODO; no WBP graph edit was made.
- The log also contains automation noise from this verification pass:
  - `LogPython: Error: Python Error: 'WorkerAppearanceComponent' object has no attribute 'get_appearance'`
  - protected-property read errors from a diagnostic probe.
  - These came from read-only Python introspection attempts and are not runtime customization failures.

Artifacts:

- First open after rename restore: `Saved/CodexScreenshots/CustomizeRenameRestore_20260708/pie_first_open_after_rename_restore_final.png`
- After HUD UFUNCTION random trigger: `Saved/CodexScreenshots/CustomizeRenameRestore_20260708/pie_after_hud_random_ufunction_final.png`
- Reopen after Apply/save: `Saved/CodexScreenshots/CustomizeRenameRestore_20260708/pie_reopen_after_apply_random_final_2.png`
- Diagnostics:
  - `Saved/CodexDiagnostics/CustomizeRenameRestore_20260708/01_first_open_live.json`
  - `Saved/CodexDiagnostics/CustomizeRenameRestore_20260708/02_after_hud_random_trigger.json`
  - `Saved/CodexDiagnostics/CustomizeRenameRestore_20260708/03_apply_save_reopen.json`
  - `Saved/CodexDiagnostics/CustomizeRenameRestore_20260708/04_reopen_command_after_apply.json`

Changed files in this pass:

- `Docs/AI/CODEX_HANDOFF_CURRENT.md`
- `Docs/AI/CHARACTER_CUSTOMIZATION_CATEGORY_RENAME_PLAN.md`

## Character customization category rename plan - 2026-07-08

Scope:

- Documentation-only planning pass for safe semantic renames in the left category menu of `WBP_CharacterCustomizationRoot_V2`.
- No WBP widget rename was performed.
- No `.uasset` / `.umap` edit, no C++ edit, no build, no commit/reset.

Changed files:

- `Docs/AI/CHARACTER_CUSTOMIZATION_CATEGORY_RENAME_PLAN.md`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Findings:

- Read-only asset string inventory found 83 current category-related widget names in `WBP_CharacterCustomizationRoot_V2.uasset`, including copied fragments.
- Current C++ depends on category button names only through `BindWidgetOptional` fields in `UAvCharacterCustomizationRootWidget` and `BindCategoryButtons()`.
- The highest-risk stale names are copied/old semantic names such as `Btn_CategoryBeard`, `Btn_CategoryBalaclava`, `Btn_CategoryHands`, `Btn_CategoryBeard_1`, `Btn_CategoryHands_1`, and copied `CategoryHead_7` fragments.
- `Btn_CategoryHair` exists and is C++ bound, but matching `Txt_CategoryHair`, `Img_CategoryHair`, `SB_CategoryHairIcon`, and `Border_CategoryHair*` names were not found in the asset string inventory, so its row children must be confirmed visually in Designer before rename.

Next:

- Rename is still not done.
- Next step requires either direct Unreal Editor/Designer control or explicit permission for an MCP-based rename workflow.
- Before any actual rename, visually confirm the WBP hierarchy row by row and keep randomize/apply/skin/preview widgets out of the category rename pass.

## Character customization initial category parity - 2026-07-08

Scope:

- Continued the narrow runtime C++ fix for first-open category menu parity in `WBP_CharacterCustomizationRoot_V2`.
- No design changes, no new menu, no Apply/save/reopen changes, no `WorkerAppearance`, preview/material/framing, Modular Police, inventory, or randomize behavior changes.
- No manual `.uasset` / `.umap` edits.

Changed files:

- `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp`
- `Source/Avaryo/Private/UI/AvaryoHUD.cpp`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Root cause:

- The remaining first-open mismatch was caused by runtime focus pushes after Blueprint construction:
  - `UAvCharacterCustomizationRootWidget::NativeConstruct()` called `SetKeyboardFocus()`.
  - `AAvaryoHUD::SetCustomize()` called `SetUserFocus(PC)` and `SetKeyboardFocus()` after `AddToViewport()`.
- That focus sync could put the freshly opened WBP into Slate focused state immediately on open, changing the active `PRESETS` row perception/tint versus the Designer initial state.

Implemented:

- Removed the root widget's startup `SetKeyboardFocus()` call.
- Removed HUD post-add `SetUserFocus()` / `SetKeyboardFocus()` for the customization widget.
- Left category C++ scoped to click binding and selected-category data state; no category text, color, opacity, padding, render transform, font, size, or layout writes were added back.
- Rechecked `UAvCharacterCustomizationRootWidget`: no `NativeTick`, no `RefreshCategoryVisuals`, no `ApplyCategoryVisual`, and no category responsive slot rewrite path is active.
- The remaining `SetKeyboardFocus()` in `UAvCustomizeRootWidget` belongs to the older programmatic widget path; the real `AvCustomize` path loads `WBP_CharacterCustomizationRoot_V2_C` as `UAvCharacterCustomizationRootWidget`.

Build:

- UBT build succeeded:
  - `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject" -WaitMutex`
  - Warnings only: existing compiler-version/deprecated API warnings.

PIE verification:

- Unreal Editor was saved/closed normally before UBT, then reopened.
- PIE was started through the real editor Play flow.
- `AvCustomize` was executed on the PIE PlayerController through `SystemLibrary.execute_console_command(world, 'AvCustomize', pc)`.
- Runtime opened the first customization screen with `PRESETS` active and the Designer-authored category menu visible, including the intended two-line `HAIR AND HEADWEAR` row wrap.
- PIE was stopped after verification.

Artifacts:

- Runtime first-open screenshot: `Saved/CodexScreenshots/CustomizeInitialParity_20260708/pie_initial_open_no_focus.png`

## Character customization category Designer parity - 2026-07-08

Scope:

- Fixed only runtime C++ overrides that broke `WBP_CharacterCustomizationRoot_V2` category menu parity in `C:\unrealEngine\avariika_UE58_sandbox`.
- No design changes, no new menu, no Apply/save/reopen changes, no `WorkerAppearance`, materials, preview framing, Modular Police, or inventory changes.
- No manual `.uasset` / `.umap` edits.

Changed files:

- `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp`
- `Source/Avaryo/Public/UI/AvCharacterCustomizationRootWidget.h`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Implemented:

- Removed the old C++ category visual map and refresh path from `UAvCharacterCustomizationRootWidget`.
- `BindCategoryButtons()` now only binds category click handlers; it no longer normalizes button style, hover/press delegates, padding, or render transform.
- `SelectCategory()` now only stores `SelectedCategory`; it no longer changes category text/icon colors, active background/line visibility, opacity, geometry, render transform, or padding.
- Removed the unused responsive Canvas-slot rewrite functions from the root widget so `LeftCategoryPanel`, center preview, right panel, and other Designer-authored slots cannot be resized by this C++ path.
- Confirmed `NativeTick` is not present in `UAvCharacterCustomizationRootWidget`.

Build:

- UBT build succeeded:
  - `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject" -WaitMutex`
  - Warnings only: existing compiler-version/deprecated API warnings.

PIE verification:

- Unreal Editor was saved and closed normally before UBT build, then reopened.
- PIE was started through the real editor Play flow; `AvCustomize` was executed on the PIE `BP_FirstPersonPlayerController_C_0`.
- Runtime opened `WBP_CharacterCustomizationRoot_V2_C`.
- Screenshot shows the Designer-authored 8-category left menu:
  - `ПРЕСЕТЫ`
  - `ГОЛОВА`
  - `ВОЛОСЫ И ГОЛОВНЫЕ УБОРЫ` with the intended line wrap
  - `ЛИЦО И ЗАЩИТА`
  - `РУКИ И АКСЕССУАРЫ`
  - `ВЕРХ И СНАРЯЖЕНИЕ`
  - `НИЗ И НАБЕДРЕННОЕ`
  - `ЦЕЛЬНЫЕ КОМПЛЕКТЫ`
- PIE was stopped after verification.

Artifacts:

- Runtime screenshot: `Saved/CodexScreenshots/CustomizeCategoryParity_20260708/pie_avcustomize_runtime_open_pc.png`
- Supporting diagnostics:
  - `Saved/CodexDiagnostics/CustomizeCategoryParity_20260708/pie_state_after_alt_p.json`
  - `Saved/CodexDiagnostics/CustomizeCategoryParity_20260708/avcustomize_execute_on_pie_pc_second.json`

## Modular Police neutral candidates MCP validation - 2026-07-08

Scope:

- Read-only validation attempt for the 10 neutral Modular Police candidates in `C:\unrealEngine\avariika_UE58_sandbox`.
- No UI connection, no Worker customization code changes, no Apply/save/reopen/randomize/framing changes, no asset copies, no `.uasset` / `.umap` edits, no build.

Changed files:

- `Docs/AI/MODULAR_POLICE_NEUTRAL_CANDIDATES_VALIDATION.md`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Result:

- Unreal Editor was already open and MCP was reachable on `127.0.0.1:13579`.
- Direct MCP load of `/Game/Modular_Police/...` failed because the bundle under `плагины\Modular_Police\Content\Modular_Police` is not mounted as `/Game/Modular_Police`.
- Asset Registry file scan also rejected the external `.uasset` path as not in a mounted path; `PackageTools.filename_to_package_name` returned empty.
- Control check on mounted `/Game/Modular_Workers/.../SKM_Glasses_Classic` loaded successfully, so the blocker is Police bundle mountability, not MCP itself.
- Physical files for all 10 scoped candidates exist. String fallback found expected Police Quantum skeleton refs, physics/material refs, and no strict item-level police/SWAT/badge/sheriff/patch/emblem branding tokens.
- Validation counts: SAFE V1 `0`, NEEDS VISUAL CHECK `0`, BLOCKED `10`.

Next:

- Do not integrate any Police neutral candidate yet.
- If requested later, first make the bundle loadable through a proper mounted content source, then rerun MCP load validation on the same 10 assets.
- If that later validation passes, the first low-identity candidates should be `SKM_Glasses_Classic`, `SKM_Glasses_SunGlass`, and `SKM_Glasses_Combat_Orange`.

## Modular Police neutral candidate refinement - 2026-07-08

Scope:

- Documentation-only refinement of the Modular Police audit.
- No UI connection, no Apply/save/reopen changes, no Worker customization code changes, no asset copies, no `.uasset` / `.umap` edits, no build.

Changed files:

- `Docs/AI/MODULAR_POLICE_CUSTOMIZATION_AUDIT.md`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Findings:

- The audit is now neutral Worker-focused. Police vest, police caps/hats/berets with badge refs, police clothing, full police outfits, weapons/props, and anything that visually makes the Worker read as police/SWAT/security are excluded from recommended V1.
- Read-only string probes found `MI_Police_Badge` references in `Beret_Police` and `Cap_02_Police`, so they are not neutral V1 candidates despite being cap/beret shapes.
- No standalone male boots/shoes were found. Male bundle has base feet; female `Police_Pants_Inboots` is a merged pants asset and outside current male Worker flow.
- Neutral candidate families now emphasized: classic/combat glasses, goggles, headset, hip bag, tactical belt, MOLLE backpack, neutral gloves/watch families, with gloves/backpack/watch kept behind later conflict validation.
- Neutral recommended validation set is 10 SKM assets: `SKM_Glasses_Classic`, `SKM_Glasses_SunGlass`, `SKM_Glasses_Combat_Glass`, `SKM_Glasses_Combat_Orange`, `SKM_Goggles_Black`, `SKM_Goggles_Bege`, `SKM_Headphones_Micro_Black`, `SKM_Headphones_Micro_Gray`, `SKM_Bag_Hip_Black`, `SKM_Belt_Tactical_Black`.

Next:

- If requested later, run a read-only editor/MCP validation pass on only the neutral recommended SKM assets.
- Keep Police-branded assets out of customization v1 unless a separate, explicit police mode is requested.

## Modular Police customization audit - 2026-07-08

Scope:

- Read-only audit only in `C:\unrealEngine\avariika_UE58_sandbox`.
- Bundle audited: `C:\unrealEngine\avariika_UE58_sandbox\плагины\Modular_Police`.
- No UI connection, no Apply/save/reopen changes, no Worker customization code changes, no asset copies, no `.uasset` / `.umap` edits.

Changed files:

- `Docs/AI/MODULAR_POLICE_CUSTOMIZATION_AUDIT.md`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Findings:

- Bundle exists and contains 2015 `.uasset` files plus 3 `.umap` demo/preview maps.
- Mesh inventory includes 373 `SKM_*`, 173 `SM_*`, 135 physics-named assets, and 4 skeleton-like assets.
- Male Worker-relevant inventory includes 174 male `SKM_*` assets; after excluding base body modules there are 118 normal modular male SKM candidates plus 32 merged/full preset SKMs.
- Police male skeleton uses the same named Quantum skeleton as Modular Workers; a read-only binary probe found 23/23 common body bone names in both skeleton files. Skeleton files are not byte-identical, so compatibility is expected but still needs editor/MCP load validation before integration.
- Superseded by the neutral refinement above: police cap, badge cap/beret variants, police vest, police clothing, and full police presets are not recommended for ordinary Worker customization.

Next:

- If requested later, run a read-only editor/MCP validation pass on the neutral recommended SKM assets listed in `Docs/AI/MODULAR_POLICE_CUSTOMIZATION_AUDIT.md`.
- Do not integrate Police Bundle assets into customization until that validation is approved.

## Character customization preview tighter framing - 2026-07-08

Scope:

- Narrow preview-only framing adjustment in `C:\unrealEngine\avariika_UE58_sandbox`.
- No UMG layout, central preview area sizing, mesh scale, Apply/save/reopen, material, randomize, left/right panel, footer, Police Bundle, skin pass, or beard pass changes.

Changed files:

- `Source/Avaryo/Private/UI/AvCustomizePreviewActor.cpp`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Implemented:

- Reduced `AAvCustomizePreviewActor` canonical scene-capture camera distance from `560.f` to `445.f` after two follow-up +5% tightening requests (`490.f -> 467.f -> 445.f`).
- Kept canonical focus at `(0, 0, 88)` and kept stable canonical framing for all presets, so preset changes should remain free of X/Y/scale jumps.
- This is camera/framing only; mesh scale and widget layout are unchanged.

Verification:

- UBT build succeeded after the final `445.f` distance change:
  - `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject" -WaitMutex`
  - Recompiled `AvCustomizePreviewActor.cpp`; warnings only for existing compiler-version preference.
- PIE visual check left for the user, per request. Check Worker 01 -> 02 -> 01 -> 06 -> 18 -> 25 for full helmet/boots and stable framing.

## Character customization high-risk review fixes - 2026-07-08

Scope:

- Fixed only the three high-risk review items for character customization stabilization in `C:\unrealEngine\avariika_UE58_sandbox`.
- No Police Bundle, skin pass, beard pass, UI redesign, marketplace asset edits, or manual `.uasset` / `.umap` edits.

Changed files:

- `Source/Avaryo/Public/UI/AvCharacterCustomizationRootWidget.h`
- `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp`
- `Source/Avaryo/Private/UI/AvCustomizePreviewActor.cpp`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Implemented:

- Apply binding ambiguity:
  - Removed the early Save-only bind from `Btn_SaveCharacter`.
  - `Btn_SaveCharacter` is now treated as the legacy visible Apply button in this WBP.
  - `BindApplyButton()` clears both known apply/save delegates before binding one visible Apply+Save target.
  - `ResolveApplyButton()` first chooses a visible/enabled button containing `ПРИМЕНИТЬ`, then visible/enabled `Btn_ApplyCharacter`, then visible/enabled legacy `Btn_SaveCharacter`.
  - `HandleSaveCharacterClicked()` now routes to `HandleApplyCharacterClicked()` so stale legacy calls cannot perform Save-only.
  - Apply click log now records `Apply clicked -> RequestApplyWorkerAppearance (Apply+Save)`.
- Tick-time visual/layout overrides:
  - Removed `NativeTick` override from `UAvCharacterCustomizationRootWidget`.
  - Category visuals are refreshed only through explicit state changes such as `SelectCategory`.
  - `RefreshFullscreenLayout()` no longer calls `UpdateResponsiveLayout()` / Canvas slot rewriting; it only sizes the root widget to the viewport.
  - Randomize remains outside category selected-state logic.
- Preview framing jumps:
  - `AAvCustomizePreviewActor::UpdateCaptureFraming()` now uses stable canonical focus/distance for all presets instead of recalculating camera focus and distance from the current preset bounds.
  - Mesh scale, UMG layout, and material/apply logic were not changed.

Build:

- UBT succeeded:
  - `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject" -WaitMutex`
  - Warnings only: existing compiler-version and deprecated API warnings.

PIE verification:

- PIE started in the real editor, customization opened through the real `AvCustomize` console path on the PIE PlayerController.
- Logs confirmed the visible legacy button binding:
  - `[AvCustomizeApply] Bound visible Apply+Save button: Btn_SaveCharacter`
- Logs confirmed Apply+Save flow:
  - `[AvCustomizeApply] Apply clicked -> RequestApplyWorkerAppearance (Apply+Save).`
  - `[AvCustomize] Applied worker appearance saved as default.`
- Randomize handler executed without moving the control into category selected-state.
- Preview sequence was captured: Worker 01 -> 02 -> 01 -> 06 -> 18 -> 25. Canonical framing kept X/Y/scale stable enough for manual inspection; helmets and boots are visible in checked screenshots.
- Apply/reopen was checked for Worker 01, Worker 06, and Worker 25. After Apply there were no customization widgets in viewport; reopen created a new widget and the active body matched the target preset.
- PIE stopped after verification. Dirty content/maps were empty.

Fresh screenshots:

- `Saved/CodexScreenshots/CustomizeHighRisk_20260708/fresh_open.png`
- `Saved/CodexScreenshots/CustomizeHighRisk_20260708/random_after_click.png`
- `Saved/CodexScreenshots/CustomizeHighRisk_20260708/preview_seq_01_worker_01.png`
- `Saved/CodexScreenshots/CustomizeHighRisk_20260708/preview_seq_02_worker_02.png`
- `Saved/CodexScreenshots/CustomizeHighRisk_20260708/preview_seq_03_worker_01.png`
- `Saved/CodexScreenshots/CustomizeHighRisk_20260708/preview_seq_04_worker_06.png`
- `Saved/CodexScreenshots/CustomizeHighRisk_20260708/preview_seq_05_worker_18.png`
- `Saved/CodexScreenshots/CustomizeHighRisk_20260708/preview_seq_06_worker_25.png`
- `Saved/CodexScreenshots/CustomizeHighRisk_20260708/reopen_after_worker_01_apply_retry.png`
- `Saved/CodexScreenshots/CustomizeHighRisk_20260708/reopen_after_worker_06_apply_retry.png`
- `Saved/CodexScreenshots/CustomizeHighRisk_20260708/reopen_after_worker_25_apply_retry.png`

Diagnostics:

- `Saved/CodexDiagnostics/CustomizeHighRisk_20260708/apply_reopen_worker_1_retry.json`
- `Saved/CodexDiagnostics/CustomizeHighRisk_20260708/apply_reopen_worker_6_retry.json`
- `Saved/CodexDiagnostics/CustomizeHighRisk_20260708/apply_reopen_worker_25_retry.json`
- `Saved/CodexDiagnostics/CustomizeHighRisk_20260708/pie_stop_state_final.json`

## Character customization stabilization - 2026-07-08

Scope:

- Stabilized the current V2 character customization flow in `C:\unrealEngine\avariika_UE58_sandbox`.
- No Police Bundle, skin material quality pass, beard pass, large UI redesign, marketplace/demo asset edits, or manual `.uasset` / `.umap` edits.
- C++/runtime only, plus this handoff update.

Changed files in this pass:

- `Source/Avaryo/Public/UI/AvCharacterCustomizationRootWidget.h`
- `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp`
- `Source/Avaryo/Private/UI/AvCustomizePreviewActor.cpp`
- `Source/Avaryo/Private/Components/WorkerAppearanceComponent.cpp`
- `Source/Avaryo/Public/AvaryoCharacter.h`
- `Source/Avaryo/Private/AvaryoCharacter.cpp`
- `Source/Avaryo/Private/UI/AvCustomizeWidgets.cpp`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Diagnosis and fixes:

- Random Appearance shift:
  - The old risk was layout/style drift from inherited category-button behavior plus runtime bottom-panel reconstruction.
  - `RebuildBottomEquipmentPanel()` is no longer called from `NativeConstruct`, so Designer remains the source of the visible bottom/randomize layout.
  - `Btn_RandomAppearance` is normalized once at construct: `PressedPadding = NormalPadding`, zero render translation, scale 1, filled content slot, no hover/pressed delegates, and no category selected-state participation.
  - Runtime category active-state still applies only to the seven real category buttons.
- Preview framing:
  - Previous focus/padding still allowed tall presets to sit too low and let boots run into the footer.
  - `AAvCustomizePreviewActor::UpdateCaptureFraming()` now uses `PreviewVerticalFitPadding = 1.08`, `PreviewHorizontalFitPadding = 1.08`, and a small negative focus height bias (`Extent.Z * -0.015`, clamped `-4..-2`) so the worker is slightly smaller and higher in frame without UMG layout or mesh scale changes.
- Apply/save/reopen mismatch:
  - Root cause: `HandleApplyCharacterClicked()` applied `BuildSelectedAppearance()` but did not persist it to `CompanyLedgerSubsystem`; next menu open initialized from default `CurrentPresetIndex = 0`, `SelectedSkinToneIndex = 0`, not from the applied/saved/current worker.
  - `AAvaryoCharacter::RequestApplyWorkerAppearance()` now applies and saves the same `FWorkerAppearance` in one server-authoritative path.
  - The V2 widget initializes selection from the active `WorkerAppearance` first, then saved ledger appearance, so right panel name/counter/skin/preview match the current state on reopen.
  - Apply closes immediately only on local authority success; remote clients close through `ClientWorkerAppearanceApplySucceeded()`.
  - Current WBP's visible apply button is legacy-named `Btn_SaveCharacter`; `ResolveApplyButton()` now intentionally uses `Btn_ApplyCharacter` if present, otherwise `Btn_SaveCharacter` as the legacy visible apply button. Final smoke log confirmed: `[AvCustomizeApply] Bound Apply button: Btn_SaveCharacter`.
- Material state after Apply:
  - Root cause for delayed/stale-looking materials was stale transient material overrides surviving a full appearance replacement.
  - `UWorkerAppearanceComponent::ApplyAppearance()`, `ApplyLocalPreviewAppearance()`, and `ClearAll()` now reset `SlotColors` before rebuilding.
  - `RebuildVisuals()` continues to call `EmptyOverrideMaterials()` before mesh assignment and before whitelisted skin MID application.
  - Diagnostic after random apply showed only skin slots became MIDs on `SKM_Worker_Male_23`; gloves/clothes/boots/hat stayed authored material instances.

Build status:

- Final UBT build succeeded:
  - `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject" -WaitMutex`
  - Warnings only: existing compiler-version and deprecated API warnings.

PIE verification:

- PIE was run through `LevelEditorSubsystem.editor_request_begin_play()`.
- Customization was opened through the real `AvCustomize` console path on the PIE PlayerController.
- Background screenshots were captured with `PrintWindow` to avoid stealing focus from the user's second monitor.
- Final smoke confirmed the visible apply button binding path after the last build.

Screenshot artifacts:

- Fresh open: `Saved/CodexScreenshots/CustomizeStabilization_20260708/fresh_open_after_framing_rebuild.png`
- Random click-after: `Saved/CodexScreenshots/CustomizeStabilization_20260708/random_click_after.png`
- Preview Worker 01: `Saved/CodexScreenshots/CustomizeStabilization_20260708/preview_worker_01_final.png`
- Preview Worker 06: `Saved/CodexScreenshots/CustomizeStabilization_20260708/preview_worker_06_final.png`
- Preview Worker 18: `Saved/CodexScreenshots/CustomizeStabilization_20260708/preview_worker_18_final.png`
- Preview Worker 25: `Saved/CodexScreenshots/CustomizeStabilization_20260708/preview_worker_25_final.png`
- Worker 25 apply world: `Saved/CodexScreenshots/CustomizeStabilization_20260708/apply_worker25_world_thirdperson_back.png`
- Worker 25 reopen: `Saved/CodexScreenshots/CustomizeStabilization_20260708/reopen_after_worker25_apply.png`
- Worker 01 apply world: `Saved/CodexScreenshots/CustomizeStabilization_20260708/apply_worker01_world.png`
- Worker 01 reopen: `Saved/CodexScreenshots/CustomizeStabilization_20260708/reopen_after_worker01_apply.png`
- Worker 06 apply world: `Saved/CodexScreenshots/CustomizeStabilization_20260708/apply_worker06_world.png`
- Worker 06 reopen: `Saved/CodexScreenshots/CustomizeStabilization_20260708/reopen_after_worker06_apply.png`
- Random selected before apply: `Saved/CodexScreenshots/CustomizeStabilization_20260708/random_selected_before_apply.png`
- Random apply world: `Saved/CodexScreenshots/CustomizeStabilization_20260708/apply_random_world.png`
- Random reopen: `Saved/CodexScreenshots/CustomizeStabilization_20260708/reopen_after_random_apply.png`

Diagnostic artifact:

- `Saved/CodexDiagnostics/CustomizeStabilization_20260708/pie_apply_material_summary.json`
  - Random apply runtime body: `/Game/Modular_Workers/Mesh/Male/Presets/SKM_Worker_Male_23.SKM_Worker_Male_23`
  - Runtime body visible: `true`
  - Skin slots were dynamic material instances; non-skin gear/clothes/boots remained source materials.

Notes:

- The project worktree was already heavily dirty before this pass. No unrelated changes were reverted.
- The initial full requested front/back world-angle matrix was partially constrained by the current third-person camera/level obstruction setup. Real PIE world screenshots were still captured for Worker 25, Worker 01, Worker 06, and random apply, and reopen screenshots verify selection persistence.

## Apply Worker appearance overlap fix - 2026-07-07

Scope:

- Critical Apply bug only: active in-world Worker appearance after `APPLY`.
- No UMG layout, panel layout, randomize, preview framing, skin quality pass, marketplace/demo asset, Blueprint, map, or asset edits.
- Project-owned C++ visibility/appearance path only.

Changed files in this pass:

- `Source/Avaryo/Public/Components/WorkerAppearanceComponent.h`
- `Source/Avaryo/Private/Components/WorkerAppearanceComponent.cpp`
- `Source/Avaryo/Private/AvaryoCharacter.cpp`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Latest continuation note:

- Added explicit Apply-path visibility logging in `AAvaryoCharacter::RequestApplyWorkerAppearance()`:
  - `BeforeApply`
  - `AfterApplyBeforeCameraView`
  - `AfterApplyCameraView`
- After server-side `WorkerAppearance->ApplyAppearance(NewAppearance)`, the local pawn now immediately calls `ApplyCameraView()` before the final visibility log. This keeps the active local third-person state consistent in the same Apply transaction.
- The log records `CharacterMesh0` mesh/visibility/OwnerNoSee and the managed runtime WorkerAppearance body mesh/visibility/OwnerNoSee.
- Latest PIE smoke was run through the real V2 customization widget path:
  - `AvCustomize` executed on the PIE PlayerController with `SystemLibrary.execute_console_command(..., PlayerController)`.
  - Presets were switched through runtime `Btn_PresetNext.OnClicked.broadcast()`.
  - Apply was invoked through reflected `WBP_CharacterCustomizationRoot_V2_C.call_method("HandleApplyCharacterClicked")`.

Exact root cause:

- `RequestApplyWorkerAppearance -> ServerApplyWorkerAppearanceData -> WorkerAppearanceComponent::ApplyAppearance/RebuildVisuals` correctly created one runtime managed `USkeletalMeshComponent` for the merged preset body and hid the base `CharacterMesh0`.
- After that, the old camera/visibility pass in `AAvaryoCharacter::ApplyCameraView()` iterated all `USkeletalMeshComponent`s and re-enabled `CharacterMesh0` in third person.
- Result after Apply was two visible worker bodies on the same active pawn: old/base `CharacterMesh0` plus the applied merged preset body. From the back this produced the "character inside character" look and dark/brown clothing fragments on pants/back.
- Material inspection did not show broad stale material override leakage. Worker 06 after Apply had dynamic materials only on whitelisted Quantum skin slots; gloves/clothes/pants remained source materials. The visible fragments were caused by doubled visible meshes, not by applying skin MIDs to every slot.

Implemented:

- Added `UWorkerAppearanceComponent::HasActiveBodyMesh()` and `IsManagedVisualComponent(...)` so `AAvaryoCharacter` can distinguish the new runtime appearance body from old/base skeletal parts.
- `ApplyCameraView()` now keeps `CharacterMesh0` hidden/owner-no-see whenever WorkerAppearance has an active body mesh, including after first/third-person toggles.
- `ApplyCameraView()` hides legacy worker modular/body parts when a managed merged WorkerAppearance body is active, but leaves the managed runtime component visible in third person.
- Runtime managed appearance components inherit the local owner visibility intent (`OwnerNoSee`) when rebuilt/attached.
- `WorkerAppearanceComponent::RebuildVisuals()` still clears old override materials before assigning a new mesh and still applies skin only through the existing material-slot whitelist.

Verification:

- UE 5.8 UBT build succeeded after the C++ changes:
  - `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject" -WaitMutex`
  - Warnings only: existing compiler-version/deprecated API warnings.
- Continuation build after adding Apply visibility logs also succeeded with the same UE 5.8 command.
- PIE component smoke covered Worker 01, Worker 06, Worker 25, and random Worker 12.
- For every checked preset after Apply and after first-person -> third-person toggle:
  - exactly one visible worker preset mesh remained;
  - `CharacterMesh0` stayed hidden;
  - total skeletal component count stayed stable at 3 (`CharacterMesh0`, `FirstPersonMesh`, one managed WorkerAppearance body);
  - no stale extra WorkerAppearance components accumulated;
  - no material slot issues were reported by the diagnostic script.
- Repeated customization-open attempt through direct Python API was not authoritative because that method is not exposed on the PlayerController; post-attempt component count still stayed 3 with only the expected visible random Worker 12 body.
- PIE was stopped after verification.
- Latest continuation PIE smoke summary: `Saved/CodexDiagnostics/ApplyBug/apply_bug_smoke_summary_latest.json`
  - Worker 01, 06, 25, and random12 all passed.
  - Each latest check had component count 3.
  - Each latest check had exactly one visible `SKM_Worker_Male_*` runtime body.
  - `CharacterMesh0` stayed hidden for all four latest checks.
- Latest post-PIE editor state: `Saved/CodexDiagnostics/ApplyBug/editor_state_after_latest_pie.json`
  - PIE false.
  - dirty content/maps empty.

Artifacts:

- Component summary: `Saved/CodexDiagnostics/ApplyBug/apply_bug_smoke_summary.json`
- Latest component summary: `Saved/CodexDiagnostics/ApplyBug/apply_bug_smoke_summary_latest.json`
- Before-fix evidence: `Saved/CodexDiagnostics/ApplyBug/after_toggle_third_person.json`
- Per-preset component dumps: `Saved/CodexDiagnostics/ApplyBug/worker01_after_apply_third.json`, `worker06_after_apply_third.json`, `worker25_after_apply_third.json`, `random12_after_apply_third.json`
- Latest per-preset component dumps: `Saved/CodexDiagnostics/ApplyBug/worker01_after_apply_third_latest.json`, `worker06_after_apply_third_latest.json`, `worker25_after_apply_third_latest.json`, `random12_after_apply_third_latest.json`
- Clean player-camera back screenshots: `Saved/CodexScreenshots/ApplyBug_20260707/worker01_back.png`, `worker06_back.png`, `worker25_back.png`, `random12_back.png`
- Player-camera front screenshots were generated too, but some are camera-clipped by the close third-person camera and should not be used as visual proof of mesh correctness.
- Additional clean viewport proof for the random Worker 12 applied body: `Saved/CodexScreenshots/ApplyBug_20260707/random12_front_viewport_window.png` and `random12_front_viewport_automation.png`. These are clean side/viewport proofs, not a replacement for per-preset close front screenshots.
- A fresh continuation screenshot probe did not complete because the Python command string for `HighResShot filename=...` hit a quoting error before writing output. No assets were modified. Use the existing `Saved/CodexScreenshots/ApplyBug_20260707/` PIE screenshots plus the latest component dumps/logs above for this pass.

Log note:

- Final UBT log was clean for errors and the target was up to date.
- The latest editor log contains a shutdown-time Fatal in `python311.dll` / `PythonScriptPlugin` after normal `CloseEditor()` shutdown. This happened after PIE verification and after dirty content/map lists were already empty. It is not an Apply path or build failure, but it remains a real editor shutdown crash in the log.

## AvCustomize Designer/PIE visual parity - 2026-07-07

Scope:

- Narrow parity pass for `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2`.
- Designer is the source of truth for randomize button text, color, padding, size, and backdrop.
- No central preview layout, left/right panel layout, footer layout, preview maps, skin/material, or old equipment-row restoration.

Changed files:

- `Content/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2.uasset`
- `Source/Avaryo/Public/UI/AvCharacterCustomizationRootWidget.h`
- `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp`
- `Source/Avaryo/Private/UI/AvCustomizePreviewActor.cpp`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Diagnosis:

- The previous runtime `RefreshRandomAppearanceVisual()` still overrode Designer every tick: `SB_BottomArea` width/height and overlay padding, randomize visibility/opacity, button content padding, backdrop brush color, icon tint/brush size, label tint/font size, and hover orange foreground.
- `UpdatePresetLabels()` wrote `Worker XX`, while the WBP Designer text uses the Russian `Рабочий XX` pattern.
- `Txt_RandomAppearanceLabel` had a double space in Designer between `ВНЕШНИЙ` and `ВИД`; a direct MCP text call can corrupt Cyrillic to `?`, so the final text was written through Unreal Python from Unicode codepoints.
- Preview worker sat slightly too low because `UpdateCaptureFraming()` aimed above the bounds center with a `0.095 * Extent.Z` focus bias.

Implemented:

- Removed `RefreshRandomAppearanceVisual()` and all randomize style/layout overrides from C++.
- `Btn_RandomAppearance` now only gets a C++ click binding to `HandleRandomAppearanceClicked()`; it no longer receives runtime color, brush, padding, font, icon-size, SizeBox, opacity, or slot overrides.
- Kept randomize outside `SelectedCategory` and category active/inactive refresh.
- Changed runtime preset name from `Worker %02d` to `Рабочий %02d` to match the WBP language.
- Fixed `Txt_RandomAppearanceLabel` and `Txt_RandomizeHint` to exact text `СЛУЧАЙНЫЙ ВНЕШНИЙ ВИД`.
- Raised the worker in preview by reducing capture focus height bias from `Clamp(Extent.Z * 0.095, 10, 18)` to `Clamp(Extent.Z * 0.035, 4, 7)`.

Runtime overrides intentionally still present:

- Category active-state still toggles category background/line visibility and icon/text active/inactive colors for the seven real categories only.
- Runtime skin swatches are still generated and colored in C++ because the WBP has fewer authored swatches than the current seven-tone runtime.
- Preview image still receives the render-target UI material at runtime.

Verification:

- WBP compile succeeded after the text update.
- UE 5.8 UBT build succeeded:
  - `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject" -WaitMutex`
  - Only existing compiler-version/deprecated API warnings were reported.
- PIE was started through the editor; customization opened through the real `AvCustomize` console path on the PIE PlayerController.
- Fresh-open screenshot verified:
  - randomize is compact, dark, bottom-left in preview;
  - text/icon are not orange;
  - label text is `СЛУЧАЙНЫЙ ВНЕШНИЙ ВИД`;
  - footer hint text matches;
  - worker is slightly higher and remains fully in frame.
- After random click screenshot verified:
  - randomize did not move, resize, or become persistent orange;
  - button stayed inside its backdrop;
  - selected category remained `ПРЕСЕТЫ`;
  - preset changed to `Рабочий 21`, proving random behavior executed.
- PIE was stopped after verification; dirty packages were `[]`.

Artifacts:

- Fresh open: `Saved/CodexScreenshots/DesignerParity_20260707/customization_open_designer_parity_final.png`
- After random click: `Saved/CodexScreenshots/DesignerParity_20260707/customization_after_random_designer_parity_final.png`

## AvCustomize random appearance button semantics/visual - 2026-07-07

Scope:

- Narrow WBP/C++ fix for `/Game/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2`.
- No manual `.uasset`/`.umap` edits.
- No central preview, left panel, right panel, footer, or old equipment-row restoration.

Changed files:

- `Content/Avariika/UI/CharacterCustomization/WBP_CharacterCustomizationRoot_V2.uasset`
- `Source/Avaryo/Public/UI/AvCharacterCustomizationRootWidget.h`
- `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Diagnosis:

- Designer vs PIE diverged because the randomize control was a copied category button (`Btn_CategoryHead_7`) with a Blueprint `OnClicked` event and `SelectCategoryVisual` branch still treating it as an eighth category.
- A previous runtime stabilizer also forced the copied widgets to active orange category colors and `SB_BottomArea` to `286x58`, which made the PIE button smaller than the target and visually active.
- The WBP had been renamed to semantic randomize names, but stale graph nodes still referenced old `CategoryHead_7` widget GUIDs and broke compile until those graph nodes were removed.

Implemented:

- Renamed the copied WBP widgets to semantic names:
  - `VB_RandomAppearance`
  - `Btn_RandomAppearance`
  - `Overlay_RandomAppearance`
  - `Border_RandomAppearanceBackdrop`
  - `HB_RandomAppearanceContent`
  - `SB_RandomAppearanceContent`
  - `Img_RandomAppearanceIcon`
  - `Txt_RandomAppearanceLabel`
- Removed stale Blueprint graph nodes for `On Clicked (Btn_CategoryHead_7)` and the old `SelectCategoryVisual` randomize branch.
- Replaced `StabilizeRandomizeButtonVisual()` with `BindRandomAppearanceButton()` and `RefreshRandomAppearanceVisual()`.
- `Btn_RandomAppearance` now binds to `HandleRandomAppearanceClicked()` in C++ and is no longer part of `SelectedCategory` or active/inactive category visual refresh.
- Default randomize visual is a compact dark backdrop with light gray icon/text; hover can shift to orange accent. It uses a `356x64` bottom overlay box with internal padding and does not create a wide bottom strip.
- Click behavior randomizes current preset and skin tone, refreshes the preview and swatches, and leaves the selected category unchanged.

Verification:

- WBP compile succeeded after graph cleanup and again after the C++ rebuild.
- UE 5.8 UBT build succeeded:
  - `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject" -WaitMutex`
  - Only existing compiler-version/deprecated API warnings were reported.
- PIE was started on `/Game/Avariika/Maps/Lvl_FirstPerson`.
- `AvCustomize` was executed through the PIE PlayerController console path; `AvaryoHUD` created `WBP_CharacterCustomizationRoot_V2_C`.
- Final visual check confirms the randomize button has a compact dark backing, light gray default icon/text, normal padding, no wide bottom strip, and no footer overlap.
- Runtime click-path was verified on the live PIE widget: `HandleRandomAppearanceClicked()` changed Worker 01 to Worker 04 and changed skin tone while `ПРЕСЕТЫ` remained the selected category.
- PIE was stopped after verification.

Artifacts:

- After visual fix: `Saved/CodexScreenshots/RandomAppearanceButton_20260707/random_appearance_after_pie_printwindow.png`
- After random click-path verification: `Saved/CodexScreenshots/RandomAppearanceButton_20260707/random_appearance_after_random_call_printwindow.png`
- Previous before/old-result reference: `Saved/CodexScreenshots/RandomizeButtonBG_20260707/randomize_bg_after_final_pie_printwindow.png`

Note:

- During a normal editor shutdown through `QUIT_EDITOR`, UE reported an access violation in `python311` / `PythonScriptPlugin`. The WBP had already been saved and compiled before this. The remaining editor window was closed with normal `CloseMainWindow`, not `taskkill`, then UBT build completed successfully.

## AvCustomize living skin, framing bump, Save/Apply split - 2026-07-07

Scope:

- Project-owned runtime/C++ only.
- No marketplace/demo asset edits and no manual `.uasset`/`.umap` edits.

Changed files in this pass:

- `Source/Avaryo/Private/Components/WorkerAppearanceComponent.cpp`
- `Source/Avaryo/Public/AvaryoCharacter.h`
- `Source/Avaryo/Private/AvaryoCharacter.cpp`
- `Source/Avaryo/Public/UI/AvCharacterCustomizationRootWidget.h`
- `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp`
- `Source/Avaryo/Private/UI/AvCustomizeWidgets.cpp`
- `Source/Avaryo/Private/UI/AvCustomizePreviewActor.cpp`

Implemented:

- Skin runtime now uses a softer luminance-dependent Quantum `Color Correction Value`, warmer correction target, and mild `Brightness` / `Saturation` / `Contrast` nudges.
- Skin MID creation remains restricted to confirmed Quantum head/body/arms slots; gloves, gear, helmets, glasses, boots, beards, and facial-detail materials remain authored.
- `M_Quantum_Head*` slot variants, including Worker 18 `M_Quantum_Head1`, are covered by the whitelist.
- Preview fit is about 10% closer and biased lower in frame through camera/framing constants, not mesh transform hacks.
- V2 customization Save/Apply handlers are split:
  - Save persists the selected preview appearance through `CompanyLedgerSubsystem` and keeps the UI open.
  - Apply applies the selected preview appearance to the active worker through `WorkerAppearanceComponent`; the V2 UI handler then closes customization.

Verification:

- UE 5.8 UBT build succeeded:
  - `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject" -WaitMutex`
  - Only existing deprecated API / compiler-version warnings were reported.
- PIE runtime inspection covered Worker 01, 02, 06, 18, and 25, including `01 -> 02 -> 01`.
- Skin tone clicks did not move preview actor or SceneCapture framing.
- Runtime material inspection confirmed Worker 02/06 gloves, Worker 18 beard/boots/facial details, helmets, glasses, and boots stayed on source materials while skin slots received MIDs.
- `HandleSaveCharacterClicked` and `HandleApplyCharacterClicked` were invoked through reflected UFUNCTIONs in PIE. The automated widget-count close check is not authoritative because the smoke script created several test widgets, but the C++ V2 Apply path calls `HandleCloseClicked()` after applying.

Artifacts:

- `Saved/CodexScreenshots/AvCustomizeSkinFramingSaveApply_20260707/pie_verification.json`
- `Saved/Screenshots/WindowsEditor/avcust_20260707_worker01_tone01.png`
- `Saved/Screenshots/WindowsEditor/avcust_20260707_worker01_tone07.png`
- `Saved/Screenshots/WindowsEditor/avcust_20260707_worker02.png`
- `Saved/Screenshots/WindowsEditor/avcust_20260707_worker01_return.png`
- `Saved/Screenshots/WindowsEditor/avcust_20260707_worker06_tone04.png`
- `Saved/Screenshots/WindowsEditor/avcust_20260707_worker18_tone04.png`
- `Saved/Screenshots/WindowsEditor/avcust_20260707_worker25_tone07.png`

## AvCustomize preview framing and living skin - 2026-07-06

Scope:

- Project-owned C++ runtime only.
- No `.uasset`/`.umap`, config, `.uproject`, plugin, marketplace, demo map/widget, or WBP layout edits.
- Existing V2 layout structure was left untouched.

Changed files:

- `Source/Avaryo/Private/UI/AvCustomizePreviewActor.cpp`
- `Source/Avaryo/Public/UI/AvCustomizePreviewActor.h`
- `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp`
- `Source/Avaryo/Private/Components/WorkerAppearanceComponent.cpp`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Diagnosis:

- Framing instability came from recalculating capture framing immediately after mesh/animation changes while component pose/bounds could still be stale. The old fit also used vertical bounds only, so width/yaw/accessory variation could shift the perceived framing.
- Dead/uneven skin came from using the Worker Bundle `Color Correction` parameter as a strong flat tint (`Color Correction Value` fixed at `0.42`) and from repeatedly creating MIDs on top of existing MIDs. One confirmed source of uneven tone was Worker 18 using slot `M_Quantum_Head1`, while the whitelist accepted only exact `M_Quantum_Head`, so arms changed but the head stayed authored.

Implemented:

- Preview meshes now force-refresh component transforms, bone transforms, and bounds before camera fitting.
- Capture framing now uses projected visible-component bounds against the actual view right/up vectors, with vertical and horizontal padding, lower visual placement, and a smaller minimum distance.
- SceneCapture ShowOnly now includes only visible skeletal mesh components with active meshes, preventing old hidden/removed components from contributing to capture state.
- Skin tone clicks still do not rebuild preset/camera/mesh; they only update existing eligible skin MIDs and request a capture.
- Skin MID application now reuses an existing dynamic material instead of nesting a new MID on every swatch click.
- Skin correction strength now scales by tone luminance instead of using one strong value for every tone.
- Seven runtime tones were warmed and made more distinguishable.
- Skin whitelist still excludes clothes, gloves, watch, helmet, hair, beard, glasses, accessories, boots, eyes/teeth, and facial-detail slots; it now accepts confirmed Quantum head slot variants such as `M_Quantum_Head1`.

Verification:

- UBT build succeeded twice with UE 5.8:
  - `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject" -WaitMutex`
  - Only existing compiler/deprecated API warnings were reported.
- Editor was closed normally through MCP/Python Save All before each build; no force-close was used.
- PIE opened `WBP_CharacterCustomizationRoot_V2`; preset sequence checked through runtime widget handlers:
  - Worker 01 tone 1;
  - Worker 01 tone 7;
  - Worker 01 -> Worker 02;
  - Worker 02 -> Worker 01;
  - Worker 06 tone 4;
  - Worker 18 tone 4;
  - Worker 25 tone 4.
- Visual checks: workers are larger and lower in the central preview, centered on X, helmet/head safe, boots visible, no observed left drift after switching.
- Runtime material inspection confirmed:
  - Worker 02 glove slot `M_Gloves_Print` retained authored `MI_Gloves_Worker_Print_Gray`;
  - Worker 06 glove slot retained authored `MI_Gloves_Worker_Print_Yellow`;
  - Worker 18 now applies skin MID to `M_Quantum_Head1` as well as exposed arms;
  - Worker 25 applies skin MID only to Quantum head/arms;
  - helmet, glasses, boots, facial details, eye/teeth, clothes, gloves, and accessories stayed authored materials.

Screenshots:

- `Saved/CodexScreenshots/AvCustomizeFramingLivingSkin_20260706/Worker01_framing_skin_tone1.png`
- `Saved/CodexScreenshots/AvCustomizeFramingLivingSkin_20260706/Worker01_framing_skin_tone7.png`
- `Saved/CodexScreenshots/AvCustomizeFramingLivingSkin_20260706/Worker02_after_switch.png`
- `Saved/CodexScreenshots/AvCustomizeFramingLivingSkin_20260706/Worker06_preview.png`
- `Saved/CodexScreenshots/AvCustomizeFramingLivingSkin_20260706/Worker18_preview.png`
- `Saved/CodexScreenshots/AvCustomizeFramingLivingSkin_20260706/Worker25_preview.png`

Remaining asset-level limits:

- The Worker Bundle exposes `Color Correction` / `Color Correction Value`, not a physically based skin/melanin material stack or masks. Runtime now uses those authored controls conservatively on confirmed skin slots only. More realistic skin would require project-owned skin materials or source material masks.
- Some red/colored hand details on Worker 02 are authored glove texture/material content, not a skin override.

## AvCustomize runtime preview framing/look - 2026-07-06

Scope:

- Project-owned runtime/C++ preview only.
- No UI layout changes, no `SB_PreviewArea` resizing, no demo/package asset edits, no `.uasset`/`.umap` edits.

Changed files:

- `Source/Avaryo/Private/UI/AvCustomizePreviewActor.cpp`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Implemented:

- Runtime preview camera now frames the worker closer:
  - FOV adjusted from `26` to `25`;
  - vertical fit padding reduced from `1.24` to `1.06`;
  - minimum camera distance reduced from `520` to `430`;
  - focus is slightly lowered to keep boots in frame while reducing empty headroom.
- Preview lighting/exposure was retuned toward a warmer in-game look:
  - manual exposure bias increased from `0.55` to `0.85`;
  - key/fill/soft lights warmed and intensified;
  - cold rim light reduced and warmed;
  - ambient sky light increased and warmed.
- Added mild SceneCapture color grading:
  - saturation, contrast, and gamma overrides reduce the previous grey/pale look without touching source materials.

Verification:

- UBT build succeeded with UE 5.8:
  - `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject" -WaitMutex`
  - Only the existing non-preferred compiler-version warning was reported.
- Unreal Editor was closed normally before build after a Save All attempt; no force-close was used.
- PIE was opened after build, customization was opened with `B`, and Worker 01, Worker 06, and Worker 25 were checked.
- Visual result:
  - workers are noticeably larger and are the main object in the preview;
  - boots/feet remain fully visible for Worker 01, 06, and 25;
  - empty space around the worker is reduced;
  - preview appears warmer, brighter, and more saturated than before.
- PIE was stopped after verification. No content assets were saved or modified for this task.

Screenshots:

- Before Worker 01: `Saved/CodexScreenshots/AvCustomizeRuntimePreviewFraming_20260706/before_worker_01.png`
- Before Worker 06: `Saved/CodexScreenshots/AvCustomizeRuntimePreviewFraming_20260706/before_worker_06.png`
- Before Worker 25: `Saved/CodexScreenshots/AvCustomizeRuntimePreviewFraming_20260706/before_worker_25.png`
- After Worker 01: `Saved/CodexScreenshots/AvCustomizeRuntimePreviewFraming_20260706/after_worker_01_clean.png`
- After Worker 06: `Saved/CodexScreenshots/AvCustomizeRuntimePreviewFraming_20260706/after_worker_06_clean.png`
- After Worker 25: `Saved/CodexScreenshots/AvCustomizeRuntimePreviewFraming_20260706/after_worker_25_clean.png`

## AvCustomize Worker 06 and skin runtime fix - 2026-07-06

Scope:

- Project-owned C++ runtime only.
- No UI design, lighting, camera, layout, preset catalog, Save/Apply, config, plugin, map, demo, or marketplace asset changes.

Diagnosis:

- Worker 06 source mesh is `/Game/Modular_Workers/Mesh/Male/Presets/SKM_Worker_Male_6`.
- The source preset and Skeletal Mesh LOD0 retain separate glove, skin, clothing, helmet, glasses, accessory, and face-detail material slots.
- The defect was introduced in `UWorkerAppearanceComponent::RebuildVisuals`, not authored into Worker 06.
- Reusing the preview body component across presets left its old `OverrideMaterials` array intact. Because Worker presets have different numeric material-slot layouts, skin MIDs created for Worker 01 were reused at the same indices on Worker 06. This replaced unrelated Worker 06 sections with head/arms MIDs and allowed later skin updates to affect gloves/accessories.
- A skin click called `RebuildVisuals`, then `ConfigurePreviewMeshes` and `UpdateCaptureFraming`; this reapplied the preset, animation, and framing instead of only updating skin material parameters.
- The existing path check inspected the current override material. Once a stale MID occupied the wrong index, that MID still matched the skin path even though the underlying Worker 06 mesh slot was not skin.

Implemented:

- Clear component override materials before assigning every new skeletal mesh.
- Resolve skin eligibility from the active skeletal mesh's authored `FSkeletalMaterial`, not from the current override.
- Use an exact slot/material-pair whitelist:
  - head: `M_Quantum_Head` with the supported Quantum head instances;
  - body: `M_Quantum_Body` with the supported Quantum body instances;
  - exposed arms: `M_Quantum_Arms`, `M_Quantum_Arms2`, or `M_Quantum_Arms3` with the supported Quantum arms instances.
- `SetSkinColor` now updates only already-created preview components and never rebuilds appearance.
- `SetPreviewSkinColor` no longer configures meshes, recalculates framing, captures explicitly, or touches animation/transforms.

Changed files:

- `Source/Avaryo/Private/Components/WorkerAppearanceComponent.cpp`
- `Source/Avaryo/Private/UI/AvCustomizePreviewActor.cpp`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Verification status:

- Full UE 5.8 UBT build succeeded after the editor was closed normally.
- PIE was driven through background MCP calls without taking keyboard or mouse focus.
- Worker 01, Worker 06, and Worker 25 were each checked with skin tones 1, 4, and 7.
- On all nine checks, the preview actor transform and SceneCapture transform remained identical across skin clicks.
- Runtime material inspection confirmed:
  - Worker 06 slot 0 `M_Gloves_Print` retained `MI_Gloves_Worker_Print_Yellow`;
  - only Worker 06 `M_Quantum_Head` and `M_Quantum_Arms2` used skin MIDs;
  - Worker 01 used skin MIDs only for `M_Quantum_Head` and `M_Quantum_Arms3`;
  - Worker 25 used skin MIDs only for `M_Quantum_Head`, `M_Quantum_Arms2`, and `M_Quantum_Arms3`;
  - clothing, vest, helmet, glasses, pants, boots, gloves, watch, face-detail, eye, teeth, and accessory materials remained source materials.
- The three tested tone values reached every eligible MID exactly:
  - tone 1: `(0.98, 0.82, 0.68)`;
  - tone 4: `(0.62, 0.40, 0.27)`;
  - tone 7: `(0.22, 0.14, 0.10)`.
- Visual PIE checks confirmed visible face/forearm changes, unchanged gloves/watch, intact idle, working preset switching, full-height framing, and no Worker 06 material/texture replacement artifact.

Screenshots:

- Worker 06 / tone 1: `Saved/CodexScreenshots/AvCustomizeSkinRuntimeFix_20260706/worker06_tone01.png`
- Worker 06 / tone 7: `Saved/CodexScreenshots/AvCustomizeSkinRuntimeFix_20260706/worker06_tone07.png`
- Worker 06 glove/watch comparison: `Saved/CodexScreenshots/AvCustomizeSkinRuntimeFix_20260706/worker06_hands_gloves_watch_comparison.png`
- Worker 25 / tone 4: `Saved/CodexScreenshots/AvCustomizeSkinRuntimeFix_20260706/worker25_tone04.png`

Asset-level limitation:

- The Worker Bundle exposes `Color Correction` plus `Color Correction Value`, not a dedicated melanin/skin-tone parameter. The runtime now applies that authored correction only to exact skin slots. A physically based skin system would require project-owned skin materials or source material masks, which was outside this fix.

## AvCustomize skin tones and preview lighting - 2026-07-06

Current task:

- Make exactly seven clickable skin-tone swatches visible in one row.
- Improve the runtime preview lighting without changing the workshop background, transparency, idle animation, rotation, preset switching, or marketplace assets.

Changed files:

- `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp`
- `Source/Avaryo/Private/UI/AvCustomizePreviewActor.cpp`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Audit:

- `WBP_CharacterCustomizationRoot_V2` contains only four authored swatches under `HB_SkinSwatches`.
- The four authored `SB_Skin01..04` widgets were visible, enabled, unclipped, and in automatic `HorizontalBoxSlot`s.
- Runtime C++ looked for `SkinSwatches` or `Skin01`, while the actual names are `HB_SkinSwatches` and `SB_Skin01`. `BuildSkinToneSwatches` therefore returned before creating or binding any runtime buttons.
- All seven C++ skin handlers existed, but none were bound before the fix.
- `M_PreviewWorker_UI` is an unlit translucent UI material. It passes render-target RGB through and uses inverted render-target alpha for opacity; it was not the source of the dark preview.
- The preview used manual exposure, but its key/fill/rim light intensities were too low for the source sizes and distances.
- Skin tint is restricted by material path to `MI_Quantum_Head`, `MI_Quantum_Arms`, and `MI_Quantum_Body`. Preset clothing, helmet, vest, gloves, pants, footwear, glasses, and accessories do not match this allowlist.

Implemented:

- Runtime swatch construction now resolves the actual `HB_SkinSwatches` container, with compatibility fallbacks for older names.
- The four authored children are replaced at runtime by exactly seven `34x34` swatches in one horizontal row with the existing gap and orange active outline.
- Runtime button content fills the complete tile and the default grey button background is transparent.
- `SelectedSkinToneIndex` remains initialized to zero, so the first swatch is selected on startup.
- Existing seven handlers are bound to the seven runtime buttons.
- Preview lighting was retuned as a neutral soft key/fill/rim setup while retaining manual exposure and the HDR transparent capture path.
- Existing bounds-based full-body framing, workshop background, idle, yaw rotation, preset switching, and render-target material were preserved.

Verification:

- UBT build succeeded with UE 5.8:
  - `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject" -WaitMutex`
  - Only the existing non-preferred compiler-version warning was reported.
- PIE was controlled through background MCP/API calls without taking keyboard or mouse focus.
- Runtime inspection found exactly seven `SkinToneRuntime_01..07` buttons; all were visible and enabled.
- All seven `OnClicked` delegates were invoked successfully.
- After clicking tones 1 through 7, only `SkinToneRuntimeOutline_07` had the orange active color; the other six had the inactive outline.
- Worker 01, Worker 06, and Worker 25 were checked. Each remained centered, full-height, and visible down to the boots.
- Worker 06 material inspection after skin changes:
  - dynamic: head and two exposed-arm skin slots;
  - unchanged source materials: vest, T-shirt, helmet, glasses, hip bag, jeans, gloves, eyes, teeth, and facial-detail slots.
- PIE was stopped after verification. No `.uasset` or `.umap` asset was modified or saved.

Screenshots:

- Seven swatches / Worker 01: `Saved/CodexScreenshots/AvCustomizePreviewLightingSkin_20260706/skin_7_final.png`
- Worker 06: `Saved/CodexScreenshots/AvCustomizePreviewLightingSkin_20260706/worker_06_final.png`
- Worker 25: `Saved/CodexScreenshots/AvCustomizePreviewLightingSkin_20260706/worker_25_final.png`

Runtime logic versus source-asset limits:

- Runtime logic fixed the missing swatches, handler binding, exclusive active state, skin-material allowlist application, preview exposure/light balance, and full-body presentation.
- Source-asset limitation: the preset meshes expose skin only through the authored Quantum head/arms/body material slots. Covered hands under glove materials and skin hidden by clothing cannot be recolored independently without different source meshes/material masks.

## AvCustomize preview framing and quality - 2026-07-06

Current task:

- Fix project-owned runtime/C++ preview framing and quality for `WBP_CharacterCustomizationRoot_V2`.
- Do not edit demo assets, maps, widgets, package assets, plugins, `.uproject`, or config.

Changed files:

- `Source/Avaryo/Public/UI/AvCustomizePreviewActor.h`
- `Source/Avaryo/Private/UI/AvCustomizePreviewActor.cpp`
- `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Backups:

- `Docs/AI/Backups/AvCustomizePreviewQuality_20260706/AvCustomizePreviewActor.h.bak`
- `Docs/AI/Backups/AvCustomizePreviewQuality_20260706/AvCustomizePreviewActor.cpp.bak`
- `Docs/AI/Backups/AvCustomizePreviewQuality_20260706/AvCharacterCustomizationRootWidget.cpp.bak`

Implemented:

- Replaced fixed preview camera framing with bounds-based full-body framing from all visible preview `USkeletalMeshComponent`s.
- The scene capture now computes camera distance from vertical bounds, FOV, and safety padding, so boots/feet fit for short/tall/full preset meshes.
- Preview skeletal meshes are configured for quality at runtime:
  - `SetForcedLOD(1)` to force LOD0.
  - `AlwaysTickPoseAndRefreshBones` to keep preview pose/bounds current.
  - update-rate optimizations disabled for the preview actor components.
- Render target resolution for the root customization preview was increased from `1024x1024` to `2048x2048`.
- SceneCapture anti-aliasing show flag is explicitly enabled; existing HDR scene-color capture, manual exposure, lights, idle animation, mouse yaw, and UI layout were preserved.

Diagnosis:

- Cropped feet/boots were caused by hard-coded capture coordinates (`Focus(0,0,92)`, camera at `(360,-35,100)`) that did not adapt to the active preset mesh bounds.
- The visible clothing/body artifacts in this preset preview path were not caused by stacked modular body and clothing components: `RefreshPresetPreview` applies each worker preset as a single full preset skeletal mesh in the `Body` slot. Runtime overlap was therefore not found for the tested preset flow.
- The runtime-side contributor to rough/uneven preview appearance was auto LOD plus a 1024 render target; forcing LOD0 and using a 2048 RT made the preview cleaner. Any remaining small garment wrinkles/seams on preset meshes are asset-authored mesh/material/normal detail, not a C++ slot-visibility conflict.

Verification status:

- UBT build succeeded with UE 5.8:
  - `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject" -WaitMutex`
  - Warnings only: existing deprecated API/compiler-version warnings.
- PIE verified in the editor window after build:
  - Opened customization with virtual-key `B`.
  - Checked preset 01 bright jacket, preset 06 vest, and preset 25 late preset.
  - Full body, legs, and boots were visible in all checked presets.
  - Idle animation and preset switching remained functional.
  - No runtime body/clothing overlap was observed on the vest/torso/sleeves in preset 06.
- Screenshots:
  - `Saved/CodexScreenshots/AvCustomizePreviewQuality_20260706/pie_preset_01_bright_jacket_verified.png`
  - `Saved/CodexScreenshots/AvCustomizePreviewQuality_20260706/pie_preset_06_scan.png`
  - `Saved/CodexScreenshots/AvCustomizePreviewQuality_20260706/pie_preset_25_late.png`
  - Contact sheet: `Saved/CodexScreenshots/AvCustomizePreviewQuality_20260706/preset_01_10_contact_sheet.png`
- PIE was stopped after verification. No `.uasset` or `.umap` files were edited or saved for this task.

## AvCustomize category runtime fix - 2026-07-06

Current task:

- Fix runtime active-state bug in the left category panel of `WBP_CharacterCustomizationRoot_V2`.

Changed files:

- `Source/Avaryo/Public/UI/AvCharacterCustomizationRootWidget.h`
- `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp`
- `Docs/AI/CODEX_HANDOFF_CURRENT.md`

Backups:

- `Docs/AI/Backups/AvCustomizeCategoryFix_20260706/AvCharacterCustomizationRootWidget.h.bak`
- `Docs/AI/Backups/AvCustomizeCategoryFix_20260706/AvCharacterCustomizationRootWidget.cpp.bak`
- `Docs/AI/Backups/AvCustomizeCategoryFix_20260706/CODEX_HANDOFF_CURRENT.md.bak`

Implemented:

- Added one C++ source of truth: `SelectedCategory`.
- Added explicit handlers for the seven allowed categories: `Preset`, `Head`, `Hair`, `Headwear`, `Beard`, `Balaclava`, `Hands`.
- `NativeConstruct` now binds those category buttons after Blueprint `Construct`, removes existing `OnClicked` bindings on the same widget instance, and initializes `Preset` as the only active category.
- Category selection now resets all seven rows inactive first, then enables active visuals only for `SelectedCategory`.
- `NativeTick` reapplies category visuals after Blueprint `Tick`, so old Blueprint runtime graph state cannot accumulate extra active rows.
- Active line/background widgets are toggled with `Hidden`/`HitTestInvisible` to avoid layout participation changes while removing inactive orange visuals.
- Category button pressed padding is normalized to normal padding; category widgets get zero render translation and scale 1 to prevent click/hover geometry drift.

Diagnosis:

- Double active state was caused by the Blueprint graph in `WBP_CharacterCustomizationRoot_V2` (`ApplyCategoryVisual`, `SelectCategoryVisual`, and seven `OnClicked` bindings) keeping visual state per row instead of deriving all rows from one selected category.
- X-shift risk came from runtime button/visual state changes: pressed padding and render transform changes could affect perceived icon/text position after hover/press/click.

Verification status:

- UBT build succeeded with UE 5.8:
  - `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject" -WaitMutex`
  - Warnings only: existing deprecated API/compiler-version warnings.
- PIE verified in the editor window:
  - Opened customization with `B`; log confirmed `WBP_CharacterCustomizationRoot_V2_C`.
  - Click sequence completed: `ПРЕСЕТЫ -> ГОЛОВА -> ВОЛОСЫ -> ГОЛОВНЫЕ УБОРЫ -> БОРОДА -> БАЛАКЛАВА -> РУКИ -> ПРЕСЕТЫ`.
  - Visual check: exactly one active row after each tested click; other rows stayed grey; no accumulated orange text/icons/left bars; icon/text X positions remained stable.
- Required screenshots:
  - Start on `ПРЕСЕТЫ`: `Saved/CodexScreenshots/AvCustomizeCategoryFix/pie_start_presets.png`
  - `ВОЛОСЫ` selected: `Saved/CodexScreenshots/AvCustomizeCategoryFix/pie_hair_selected.png`
  - Extra final check after full sequence: `Saved/CodexScreenshots/AvCustomizeCategoryFix/pie_final_presets_after_sequence.png`
- PIE was stopped after verification. No project-owned assets were modified during PIE verification, so no content assets were saved.

## AvCustomize responsive UI

Current task:

- Continue only responsive UI fixes for AvCustomize.
- Do not work on levels, Dungeon Architect, Menu System Pro, ModuFusion, Steam, SaveGame, or SceneCapture.

Status:

- Responsive layout code change has been made.
- Responsive UI changes have been built and partially checked manually.
- Automated screenshot checks through MCP/editor viewport are not valid for UMG verification in this project: `take_screenshot`, `HighResShot`, and editor viewport captures can miss or misrepresent the UMG layer.
- Further visual verification must be performed manually by the user through New Editor Window (PIE), not through Simulate or MCP screenshots.

Changed files:

- `Source/Avaryo/Private/UI/AvCharacterCustomizationRootWidget.cpp`
- `Source/Avaryo/Private/UI/AvaryoHUD.cpp`

Implemented changes:

- `UAvCharacterCustomizationRootWidget::NativeTick` now reacts to the widget `Geometry` size, so windowed resize can trigger responsive layout recalculation.
- Removed the `MaxSafeWidth = 2200` cap; the safe content width now uses the full viewport width.
- Increased the right options panel target width to roughly the 620 px content width instead of the previous too-small width.
- `AAvaryoHUD::SetCustomize` now adds the widget with `AddToViewport(100)` instead of `AddToPlayerScreen(...)`.

Risks / follow-up checks:

- `NativeTick` may be unnecessary or more expensive than needed; consider replacing with a lower-frequency or explicit viewport resize path if it causes overhead.
- Increasing `RightPanel` width may hurt Full HD layouts or force narrow layout sooner than intended.
- Warning: CommonUI is running without `CommonGameViewportClient`; input routing may behave incorrectly. Do not fix this yet and do not change `GameViewportClient` unless explicitly requested.
- Required visual checks:
  - 1920x1080 fullscreen.
  - 2560x1440 fullscreen.
  - 2K windowed resize, including resizing after the widget is already open.

Do not continue with new AvCustomize changes until these checks are performed or explicitly requested.

## Dungeon Architect / UE 5.8

- Dungeon Architect установлен как project plugin: `Plugins/DungeonArchitect`
- В `avariika.uproject` включён `"DungeonArchitect": true`
- Descriptor был адаптирован под UE 5.8.0
- Были внесены минимальные compatibility-патчи; ниже перечислены точные файлы и суть изменений
- `avariikaEditor Win64 Development` успешно собран
- Smoke-запуск Editor прошёл, DungeonArchitect загрузился
- Старые ошибки `AnimBP_Dog_GH` и `LogAutomationTest` не относятся к Dungeon Architect

### Точные compatibility-патчи

> Примечание: основной набор патчей лежит в рабочем дереве плагина; `git diff` по корню проекта показывает изменение `avariika.uproject`, а изменения внутри `Plugins/DungeonArchitect` находятся в копии плагина, подключенной как project plugin.

- `Plugins/DungeonArchitect/Source/DungeonArchitectRuntime/Public/Frameworks/PCG/Data/PCGSGFTextureData.h`
  - `UPCGDungeonSGFTextureData` переведён с `UPCGBaseTextureData` на `UPCGTexture2DSingleBaseData`
  - `IsValid()` больше не помечен `override`
  - Добавлены UE 5.8 overrides: `GetTexture()`, `GetTextureRHI()`, `GetTextureResourceType()`, `RequestCPUReadback()`
- `Plugins/DungeonArchitect/Source/DungeonArchitectRuntime/Private/Frameworks/PCG/Data/PCGSGFTextureData.cpp`
  - helper sampler обновлён под `UPCGTexture2DSingleBaseData*`
- `Plugins/DungeonArchitect/Source/DungeonArchitectRuntime/Public/Frameworks/Canvas/UI/Viewport/DungeonCanvasViewportClient.h`
- `Plugins/DungeonArchitect/Source/DungeonArchitectRuntime/Private/Frameworks/Canvas/UI/Viewport/DungeonCanvasViewportClient.cpp`
  - `InputGesture` обновлён под UE 5.8, добавлен параметр `EGesturePhase GesturePhase`
- `Plugins/DungeonArchitect/Source/DungeonArchitectGameplay/Public/Interaction/DAGPlayerInteractionScannerComponent.h`
  - `InteractionScanRate = 0.100f` вместо неявного `0.100`
- `Plugins/DungeonArchitect/Source/DungeonArchitectRuntime/DungeonArchitectRuntime.Build.cs`
  - `bWarningsAsErrors = false`
- `Plugins/DungeonArchitect/Source/DungeonArchitectEditor/DungeonArchitectEditor.Build.cs`
  - `bWarningsAsErrors = false`
  - добавлен `PCG` в `PrivateDependencyModuleNames`
- `Plugins/DungeonArchitect/Source/DungeonArchitectGameplay/DungeonArchitectGameplay.Build.cs`
  - `bWarningsAsErrors = false`
- `Plugins/DungeonArchitect/Source/DungeonArchitectGameplaySupport/DungeonArchitectGameplaySupport.Build.cs`
  - `bWarningsAsErrors = false`
- `Plugins/DungeonArchitect/Source/DungeonArchitectEditor/Private/Core/Editors/SnapMapEditor/AppModes/DebugAppMode.cpp`
- `Plugins/DungeonArchitect/Source/DungeonArchitectEditor/Private/Core/Editors/SnapMapEditor/AppModes/GraphDesignAppMode.cpp`
- `Plugins/DungeonArchitect/Source/DungeonArchitectEditor/Private/Core/Editors/SnapMapEditor/AppModes/VisualizeAppMode.cpp`
- `Plugins/DungeonArchitect/Source/DungeonArchitectEditor/Private/Core/Editors/ThemeEditor/AppModes/MarkerGenerator/MarkerGeneratorAppMode.cpp`
- `Plugins/DungeonArchitect/Source/DungeonArchitectEditor/Private/Core/Editors/ThemeEditor/AppModes/ThemeGraph/ThemeGraphAppMode.cpp`
  - private `FApplicationMode::RegisterTabFactories(...)` заменён на protected `RegisterTabFactoriesWithAppAndManager(...)`

## Созданный Snap Grid Flow POC

Точные созданные ассеты:

- `Content/Avariika/LevelGeneration/DA_Test/Assets/SC_Avariika_Test_Door_240.uasset`
- `Content/Avariika/LevelGeneration/DA_Test/Assets/SGF_MB_Avariika_Test_1200x600x300.uasset`
- `Content/Avariika/LevelGeneration/DA_Test/Assets/SGF_DB_Avariika_Test_EntryHallGarage.uasset`
- `Content/Avariika/LevelGeneration/DA_Test/Assets/SGF_Avariika_EntryHallGarage.uasset`
- `Content/Avariika/LevelGeneration/DA_Test/Modules/M_Test_Entry.umap`
- `Content/Avariika/LevelGeneration/DA_Test/Modules/M_Test_Hall.umap`
- `Content/Avariika/LevelGeneration/DA_Test/Modules/M_Test_Garage.umap`
- `Content/Avariika/LevelGeneration/DA_Test/Lvl_DA_SnapGrid_Test.umap`

## Проверенный статус POC

- `M_Test_Entry`: 1 bounds actor, 1 snap connection, 8 greybox mesh actors
- `M_Test_Hall`: 1 bounds actor, 2 snap connections, 10 greybox mesh actors
- `M_Test_Garage`: 1 bounds actor, 1 snap connection, 8 greybox mesh actors
- Door locations проверены по сетке:
  - `(1200,300,0)`
  - `(2400,300,0)`
- `Lvl_DA_SnapGrid_Test` сохранена и содержит три тестовые секции и два маркера проверки стыков

## Что НЕ завершено

- Полный editor-time `Build Dungeon` через Snap Grid Flow не был автоматически завершён
- Причина: cached поля `FSnapGridFlowModuleDatabaseConnectionInfo.Transform` и часть module database cache в Dungeon Architect read-only / `VisibleAnywhere` и не выставляются Unreal Python

## Следующий ручной шаг в Unreal Editor

1. Открыть `SGF_DB_Avariika_Test_EntryHallGarage`
2. Нажать штатную кнопку `Build / Rebuild / Update Module Database Cache`
3. Проверить, что база видит:
   - `M_Test_Entry`
   - `M_Test_Hall`
   - `M_Test_Garage`
4. Открыть `SGF_Avariika_EntryHallGarage`
5. Проверить flow: `Entry -> Hall -> Garage`
6. Открыть `Lvl_DA_SnapGrid_Test`
7. Выбрать Dungeon Architect actor и нажать `Build Dungeon`
8. Проверить, что проходы совпадают и стены не перекрывают двери

## Принятые решения

- Dungeon Architect используем только editor-time
- Runtime generation не использовать
- `Lvl_FirstPerson` не трогать
- Сначала доказать POC из трёх модулей
- Только после успешного ручного `Build Dungeon` переходить к настоящему модулю `MainHall_Stairs`
- Цель будущего уровня: компактная миссия Avariika на 15–20 минут, а не большой процедурный лабиринт

## Важно для следующего Codex

- Не форсировать закрытие `UnrealEditor.exe`
- Не запускать PowerShell `Invoke-WebRequest` к localhost MCP без `-UseBasicParsing`
- Не делать новые уровни, пока POC не собран вручную
- Не делать commit без отдельной команды пользователя
