# PROJECT_STATE

Current implementation status. For the full project explanation, see `PROJECT_BRAIN.md`.

## Legend

- Implemented: usable and expected to work, though still subject to tuning.
- Partially Implemented: present but incomplete, unpolished, or needs validation.
- Planned: designed or prioritized, not yet built.
- Experimental: exploratory, prototype, imported, or not production-settled.
- Broken: documented as not working or unsafe.
- Unknown: not enough verified information in current docs.

## Subsystem Status

| Subsystem | Status | Notes |
|---|---|---|
| Project documentation | Implemented | Root knowledge base and domain docs exist. |
| Repository structure | Implemented | `Docs/` and `Reference/` organized; Unreal folders left in place. |
| Core C++ module `Avaryo` | Implemented | Runtime module exists with character, world, item, game, UI classes. |
| Active map `Lvl_FirstPerson` | Partially Implemented | MCP reported `/Game/Avariika/Maps/Lvl_FirstPerson`, 291 actors, `BP_AvaryoGameMode_C`. |
| Repairable objects | Partially Implemented | `ARepairable` exists and is central; audits show many fixes but interaction polish remains. |
| Power/electricity | Partially Implemented | `APowerSwitch` exists; previous replication issues documented as fixed but needs co-op validation. |
| Water/flooding | Partially Implemented | Present in repair-chain design and code references; final gameplay validation unknown. |
| Gas/explosion | Partially Implemented | Gas leak/explosion gameplay and feedback exist in docs/code; needs co-op and balance validation. |
| Fire | Experimental | Intended mainly as emergent consequence; exact production status unclear. |
| Doors | Partially Implemented | `ADoor` exists; debounce/audit fixes documented. |
| Exit/extraction | Partially Implemented | `AExitZone` exists; full run loop validation unknown. |
| Dispatch/job board | Partially Implemented | `ACallBoard` and `DispatchSubsystem` exist; voice/radio depth planned. |
| Tool case/loadout | Partially Implemented | `AToolCase` exists; complete prep loop unknown. |
| Inventory | Partially Implemented | Character and item systems exist; heavy/light slot polish remains. |
| Pickup items | Partially Implemented | `APickupItem` and `ABioPickup` exist. |
| Vitals | Partially Implemented | HP/panic/stamina/toilet states exist; balance and edge cases remain. |
| Wounded/dragging | Partially Implemented | Dragging and wounded flow exist; animation and co-op feel need validation. |
| Player locomotion | Partially Implemented | Mobility/WorkAnim path active. Minimal idle L90/R90 Turn-In-Place exists and was PIE-verified; starts/stops, 45/135/180, crouch turns, sprint turns, movement-start turns, and Foot IK remain. |
| Crouch | Partially Implemented | Assets and work exist; final animation graph quality unknown. |
| Foot IK | Planned | Priority item, not documented as complete. |
| Animation pipeline | Partially Implemented | Retarget path and assets exist; needs disciplined validation. |
| Work montages | Broken | Docs say `ABP_Worker` lacks slot/state-machine support for work montages. |
| Head aim/free look | Experimental | Aim-offset assets exist. Current TIP behavior still uses `bTurning` to suppress AimYaw to zero; `TurnInPlaceAimAlpha` exists in C++ but is not yet wired into the `ABP_Worker` AimYaw/spine/neck graph. |
| Multiplayer/session | Partially Implemented | Online subsystem code and plugins exist; Null/LAN likely current baseline; co-op validation is top priority. |
| Save/economy | Partially Implemented | `AvariikaSaveGame` and `CompanyLedgerSubsystem` exist. |
| Shop/progression | Planned | Detailed spec exists; implementation status incomplete. |
| Loot economy | Partially Implemented | Design and item value direction exist; production balance unknown. |
| UI/HUD | Partially Implemented | Canvas HUD remains the fallback. The Menu System Pro main-menu flow is production-routed for Continue, New Game/customization, Settings, Credits, Exit confirmation and Multiplayer Coming Soon. Required mouse/keyboard acceptance has passed; physical-gamepad acceptance is not a release blocker. |
| Audio | Partially Implemented | Many core SFX exist; male vocal effort library missing. |
| VFX | Partially Implemented | Niagara coverage exists; incident-specific polish remains. |
| AI dispatch | Planned | Dispatch voice/intent features specified but not complete. |
| Civilian NPCs | Planned | Candidate assets/design exist. |
| Listener Monster | Planned | Explicitly deferred and forbidden without instruction. |
| Gameplay Ability System | Planned | Priority exists; no completed GAS architecture documented. |
| Optimization | Partially Implemented | Audits exist; TickOptToolkit enabled; further work needed. |
| Testing | Partially Implemented | `TESTING` docs and some C++ tests exist; automated coverage unknown. |
| Scripts/tooling | Experimental | Many useful scripts mixed with one-off outputs. |
| Asset licensing/readiness | Partially Implemented | Many packs are placeholders or candidates; production license status must be verified. |

## Current Main Menu State

- Production map: `/Game/Avariika/Maps/MainMenu/L_MainMenu`.
- `MenuHUD` mounts the project-owned Menu System Pro root only on the main-menu map; the Canvas fallback is preserved.
- Confirmed PIE flows:
  - Continue validates the real save-presence adapter and reaches `/Game/Avariika/Maps/Lvl_FirstPerson`.
  - New Game reaches the same gameplay map and opens the existing character-customization root without deleting the company save.
  - Settings, Credits, Multiplayer Coming Soon and Exit confirmation open and return focus to Home.
- Confirmed staged Win64 flow: packaged startup opens the production menu map, real foreground-verified `Down` + `Enter` routes from Continue to New Game, loads `/Game/Avariika/Maps/Lvl_FirstPerson?AvStartCustomize`, and creates `WBP_CharacterCustomizationRoot_V2_C`.
- Packaging includes the two production maps, project MSP roots/materials, the `AvariikaUI.csv` UFS directory and the code-addressed customization assets. The production customization root is a soft runtime reference; the native cook delegate explicitly includes it and its runtime-loaded preview material without pulling in the legacy root.
- Final isolated staged startup and New Game checks had zero failed or missing String Table entries, missing preview material, Blueprint runtime errors, `Accessed None`, `SourceMenu`, fatal errors or assertions. New Game created `WBP_CharacterCustomizationRoot_V2_C`.
- Isolated staged SaveGame acceptance passed: an empty UserDir keeps Continue disabled, focuses New Game and creates no company slot; a protected copy of the valid company save enables and focuses Continue. The production SaveGame SHA remained unchanged.
- RU 1920x1080, RU 1280x720 and EN 1920x1080 Home states plus Settings, Credits, Exit confirmation and valid-save Continue were captured from PIE.
- Final UE 5.8 UBT succeeded. Win64 Development build/cook/stage/pak also completed with AutomationTool exit code 0.
- Mouse and keyboard are the primary target input. Their acceptance is a required release criterion and has passed for the production Menu System Pro flow.
- Existing gamepad/focus support must not be removed or broken. A physical-controller run may be performed later as an additional compatibility check, but it does not block Menu System Pro completion.
- The earlier startup-only String Table warnings are resolved, and the earlier vendor `SourceMenu` diagnostics did not reproduce in the final isolated packaged checks.
- Evidence: `Saved/CodexDiagnostics/MenuSystemProProductionClosure_20260727` and `Saved/CodexDiagnostics/MenuSystemProStartupCleanup_20260727`.

## Source Inventory

<!-- AUTO-GENERATED:SOURCE_INVENTORY:START -->
Generated by `Scripts/update_project_docs.py` from `Source/Avaryo`.

- Runtime modules: `Avaryo`
- Public C++ classes indexed: 52
- Actor classes indexed: 21
- Project subsystems: `URunStateSubsystem`, `UAvariikaOnlineSubsystem`, `UCompanyLedgerSubsystem`, `UDispatchSubsystem`
- Explicit gameplay components: `UFlashlightComponent`, `UVitalsComponent`, `UWorkerAppearanceComponent`
<!-- AUTO-GENERATED:SOURCE_INVENTORY:END -->

## Current Worktree Note

Documentation audit on 2026-08-20 began from a clean Git worktree. This is a source-control snapshot, not a new editor or runtime acceptance run; use the dated evidence below and the current handoff for feature-specific validation.

Current cross-account Codex handoff: `Docs/AI/CODEX_HANDOFF_CURRENT.md`.

## Current Turn-In-Place State

- Minimal idle Turn-In-Place implementation for L90/R90 exists.
- Modified files:
  - `Source/Avaryo/Public/AvaryoCharacter.h`
  - `Source/Avaryo/Private/AvaryoCharacter.cpp`
  - `Content/Avariika/Anim/Locomotion/ABP_Worker.uasset`
- UBT build succeeded.
- PIE verified both L90 and R90:
  - Montage plays.
  - Final actor yaw reaches `-90/+90`.
  - `bTurning` prevents retrigger.
  - Walking and crouching work afterward.
- Current tuning:
  - Threshold: `78` degrees.
  - Intent delay: `0.13` seconds.
  - Camera farther-away input scale while turning: `0.45`.
  - Soft camera/body clamp: `+/-105` degrees.
  - Aim fade-out: `0.18` seconds.
  - Aim fade-in: `0.15` seconds.
  - Yaw montage window: `0.08 -> 0.72`.
- Important unresolved item:
  - `TurnInPlaceAimAlpha` exists in C++, but `ABP_Worker` graph does not yet multiply/gate AimYaw with it.
  - Current behavior still uses `bTurning` to suppress AimYaw to zero.
  - Manual AnimBP graph review is required before claiming visually smooth head-to-body transition.
- Do not add 45/135/180 or crouch turns yet.
- Next task: manually inspect the current visual result in PIE, then safely wire `TurnInPlaceAimAlpha` into the existing AimYaw/spine/neck path in `ABP_Worker`, without altering unrelated graph logic.
