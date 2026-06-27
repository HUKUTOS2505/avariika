# Code Map

Canonical project context: [PROJECT_BRAIN.md](../../PROJECT_BRAIN.md).

This file maps the actual C++ gameplay implementation in `Source/Avaryo`. It is intentionally implementation-facing; design intent belongs in the project brain and domain docs.

<!-- AUTO-GENERATED:CODE_MAP:START -->
Generated manually from the source inventory on 2026-06-27. Re-run `python Scripts/update_project_docs.py --write` after C++ changes to refresh this region.

| Area | Folder | Purpose | Main Classes | Dependencies | Status | Related Docs |
|---|---|---|---|---|---|---|
| Runtime module | `Source/Avaryo` | Single game runtime module. Registers project code and owns all gameplay C++ currently present. | `Avaryo` module | `Core`, `CoreUObject`, `Engine`, `InputCore`, `NetCore`, `EngineCameras`, `OnlineSubsystem`, `OnlineSubsystemUtils`, `UMG`, `Niagara`, `AssetRegistry` | Implemented | [MODULE_DEPENDENCIES.md](MODULE_DEPENDENCIES.md) |
| Player character | `Source/Avaryo/Public`, `Private` | Player pawn, interaction hub, inventory owner, movement/animation bridge, repair actor user, co-op action authority. | `AAvaryoCharacter` | Components, items, repairables, run state, camera, networking | Partially Implemented | [CLASS_INDEX.md](CLASS_INDEX.md), [COMPONENTS.md](COMPONENTS.md) |
| Components | `Source/Avaryo/Public/Components` | Reusable actor components for vitals, flashlight, and worker appearance. | `UVitalsComponent`, `UFlashlightComponent`, `UWorkerAppearanceComponent` | `ActorComponent`, replication, asset registry, light/audio types | Partially Implemented | [COMPONENTS.md](COMPONENTS.md) |
| World actors | `Source/Avaryo/Public/World` | Interactable level objects, repair targets, exit flow, traps, doors, power, environmental incidents. | `ARepairable`, `APowerSwitch`, `ADoor`, `AExitZone`, `AToolCase`, `AToilet`, `ATrap`, `ACallBoard`, `AFoamPatch`, `AFloodlight`, `ABioProjectile` | Character, run state, components, Niagara/audio, replication | Partially Implemented | [GAME_FLOW.md](GAME_FLOW.md), [Docs/Gameplay](../Gameplay/GAMEPLAY_SYSTEMS.md) |
| Items | `Source/Avaryo/Public/Items` | Pickup and usable item actors. | `APickupItem`, `ABioPickup` | Character inventory, world collision, item enums | Partially Implemented | [COMPONENTS.md](COMPONENTS.md) |
| Run state and persistence | `Source/Avaryo/Public/Game` | Shift lifecycle, objectives, company ledger, dispatch memory, save slot, online session wrapper. | `ARunState`, `URunStateSubsystem`, `UCompanyLedgerSubsystem`, `UDispatchSubsystem`, `UAvariikaSaveGame`, `UAvariikaOnlineSubsystem` | World subsystem, game instance subsystem, save game, online subsystem | Partially Implemented | [SUBSYSTEMS.md](SUBSYSTEMS.md), [GAME_FLOW.md](GAME_FLOW.md) |
| Player/controller/game mode | `Source/Avaryo/Public/Game` | Player controller and menu flow entry points. | `AAvaryoPlayerController`, `AMenuPlayerController`, `AMenuGameMode` | Engine gameplay framework, UI | Partially Implemented | [GAME_FLOW.md](GAME_FLOW.md) |
| UI/HUD | `Source/Avaryo/Public/UI` | Canvas HUD, menu HUD, and camera shake definitions. | `AAvaryoHUD`, `AMenuHUD`, `UPanicCameraShake`, `UExplosionCameraShake` | Character, run state, UMG, EngineCameras | Partially Implemented | [PROJECT_STATE.md](../../PROJECT_STATE.md) |
| Localization helper | `Source/Avaryo/Public/AvariikaLoc.h`, `Private/AvariikaStringTable.cpp` | String table/localization helper code. | `AvariikaLoc` helpers | Engine string table/localization | Partially Implemented | [PROJECT_BRAIN.md](../../PROJECT_BRAIN.md) |
| Tests | `Source/Avaryo/Private/Tests` | C++ automation tests for selected gameplay behavior. | `AvaryoTests.cpp` | Unreal automation framework, gameplay classes | Partially Implemented | [BUILD_PIPELINE.md](BUILD_PIPELINE.md) |
<!-- AUTO-GENERATED:CODE_MAP:END -->

## Notes

- The project currently has one game module, `Avaryo`; there are no separate gameplay, UI, or networking modules yet.
- Several conceptual systems are implemented inside `AAvaryoCharacter` rather than standalone components: interaction, inventory, foot IK variables, dragging, and some animation bridge state.
- Most gameplay actors are replication-aware. Co-op stability remains the highest validation priority.
