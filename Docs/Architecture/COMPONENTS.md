# Components

Canonical project context: [PROJECT_BRAIN.md](../../PROJECT_BRAIN.md).

## Explicit Gameplay Components

| Component | Owner | Purpose | Replication | Notes |
|---|---|---|---|---|
| `UVitalsComponent` | `AAvaryoCharacter` | Health, panic, stamina, bladder, wounded/unconscious, wet/burning/smell/smoking status. | Replicates core values and status timers. Mutations should be server-authoritative. | This is the strongest standalone gameplay component and should remain the owner of player vitals logic. |
| `UFlashlightComponent` | `AAvaryoCharacter` | Headlamp power state, battery, flicker, forced blackout, cheap unit behavior. | Replicates power state, battery, cheap flag. | Light component is assigned in Blueprint or discovered from the owner. |
| `UWorkerAppearanceComponent` | `AAvaryoCharacter` | Modular worker outfit assembly from soft mesh references. | Replicates `FWorkerAppearance`; clients rebuild visuals locally. | Uses `AssetRegistry` for option discovery and transient component maps for visuals. |

## Conceptual Systems Not Yet Split Into Components

| System | Current Location | Recommendation |
|---|---|---|
| Interaction | Mostly `AAvaryoCharacter` plus interactable actors | Extract later only if interaction branching keeps growing. A lightweight interface would reduce character/world actor coupling. |
| Inventory | `AAvaryoCharacter` and `APickupItem` | Consider a replicated inventory component before adding more item categories or GAS integration. |
| Foot IK | `AAvaryoCharacter` variables and animation Blueprint work | Keep C++ minimal. Productionize in Animation Blueprint/control rig once locomotion settles. |
| Repair tool usage | `AAvaryoCharacter` and `ARepairable` | Avoid moving until repair minigame rules stabilize. |
| Ability system | Not implemented as GAS components yet | Planned priority; do not introduce partial GAS without a migration plan. |
| Replication helpers | Per-class `GetLifetimeReplicatedProps` and RPCs | Keep close to owning class until repeated patterns become obvious. |

## Engine Components In Use

- `ACharacter`/`UCharacterMovementComponent`: core movement and prediction.
- `ULightComponent`: flashlight/floodlight light surface.
- `UAudioComponent`/`USoundBase`: incident, repair, dispatcher, and item feedback.
- Collision primitives on world actors: overlap/interaction volumes for doors, traps, exits, toilets, foam patches, and projectiles.

## Component Boundaries

- Vitals, flashlight, and appearance are good component boundaries because they have clear state ownership and Blueprint-facing APIs.
- Inventory and interaction are not yet cleanly isolated; adding features there should include coupling checks against [MODULE_DEPENDENCIES.md](MODULE_DEPENDENCIES.md).
