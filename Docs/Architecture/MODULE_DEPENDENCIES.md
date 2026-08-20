# Module Dependencies

Canonical project context: [PROJECT_BRAIN.md](../../PROJECT_BRAIN.md).

## Unreal Module Dependencies

<!-- AUTO-GENERATED:MODULE_DEPENDENCIES:START -->
`Avaryo.Build.cs` declares one runtime module, `Avaryo`.

Public dependency modules:

- `Core`
- `CoreUObject`
- `Engine`
- `ApplicationCore`
- `InputCore`
- `EnhancedInput`
- `NetCore`
- `EngineCameras`
- `OnlineSubsystem`
- `OnlineSubsystemUtils`
- `UMG`
- `Slate`
- `SlateCore`
- `Niagara`
- `AssetRegistry`
<!-- AUTO-GENERATED:MODULE_DEPENDENCIES:END -->

## Current Conceptual Graph

```text
Core Unreal
  -> Avaryo runtime module
      -> Game framework: GameMode, PlayerController, HUD
      -> Character: movement, interaction, inventory, vitals, flashlight, appearance
      -> World actors: repairables, doors, power, traps, exits, toilets, call board, tool case
      -> Items: pickup items, bio pickups/projectiles
      -> Run state: objectives, scoring, dispatcher, phase
      -> Persistence: company ledger, save game
      -> Networking: online session subsystem and replicated actors
      -> UI: Canvas HUD, menu HUD, camera shakes
```

## Requested Production Graph Compared To Actual Code

```text
Core
  -> Gameplay
      -> Interaction
          -> Inventory
              -> Items
      -> Animation
      -> Networking
      -> UI
      -> Save System
```

Actual implementation is flatter: almost all systems live in the single `Avaryo` runtime module and use class-level dependencies rather than module boundaries.

## Class-Level Coupling Hotspots

| Hotspot | Why It Matters | Suggested Simplification |
|---|---|---|
| `AAvaryoCharacter` <-> `ARepairable` | Character drives repair use; repairable needs actor identity and feedback. | Keep stable for now. Later introduce an interaction/repair interface if more repair actor types appear. |
| `AAvaryoCharacter` -> `APickupItem` | Character owns inventory slots and item use/drop/throw. | Consider `UInventoryComponent` when item count and UI grow. |
| `ARepairable` -> player/world feedback | Repairable owns hazards, effects, and multiplayer feedback. | Split only after repair stages are production-proven. |
| `ARunState` -> characters, repairables, exits, ledger | Run state aggregates objective and scoring data across systems. | Keep as orchestrator; avoid putting low-level gameplay rules here. |
| `AAvaryoHUD` -> character/run state/world data | HUD pulls state directly for Canvas rendering. | Move to view models only if UI becomes UMG-heavy. |

## Circular Dependencies

No separate C++ module cycles exist because there is only one runtime module. At class level there are bidirectional gameplay relationships, mainly character/repairable/run-state references. These are acceptable for prototype-stage production code but should be watched as systems expand.

## Plugin Dependency Surface

The `.uproject` enables many content/tool plugins. The C++ module directly depends only on the modules listed above. Plugin availability should be treated as project environment state, not as implicit gameplay architecture.
