# Game Flow

Canonical project context: [PROJECT_BRAIN.md](../../PROJECT_BRAIN.md).

This file describes the runtime flow implied by the current code.

## Boot

1. Unreal loads `avariika.uproject` with EngineAssociation `5.7`.
2. The `Avaryo` runtime module is loaded.
3. Enabled plugins are initialized by Unreal before gameplay startup.
4. GameInstance subsystems initialize:
   - `UCompanyLedgerSubsystem` loads company state from `AvariikaCompany`.
   - `UAvariikaOnlineSubsystem` binds online session interfaces.
   - `UDispatchSubsystem` is available as cross-map memory.

## Menu / Hub

1. Menu maps can use `AMenuGameMode`, `AMenuPlayerController`, and `AMenuHUD`.
2. Hub job selection is represented by `ACallBoard`.
3. Tool prep is represented by `AToolCase`.
4. Accepting a call stores hub/call data in `UDispatchSubsystem`.
5. Hosting uses `UAvariikaOnlineSubsystem::HostGame`, then server travels to a listen map.

## Map Load

1. A world is created for the selected map.
2. `URunStateSubsystem` ensures an `ARunState` exists.
3. `ARunState` scans/owns runtime objective state.
4. World actors begin play: repairables, exits, doors, traps, toilets, power switches, items.

## Player Spawn

1. GameMode spawns/possesses `AAvaryoCharacter`.
2. Character components initialize:
   - `UVitalsComponent`
   - `UFlashlightComponent`
   - `UWorkerAppearanceComponent`
3. Player controller and HUD read replicated state and local pawn state.
4. Animation Blueprint consumes character movement and state data.

## Gameplay Loop

```text
Player explores
  -> detects/uses interactable world actor
  -> character sends server-authoritative action
  -> target actor updates replicated state
  -> run state observes objectives/incidents/stats
  -> HUD renders character/run feedback
  -> company ledger receives final shift outcome
```

## Repair Flow

1. `AAvaryoCharacter` interacts with `ARepairable`.
2. `ARepairable` validates stages, required actions, and minigame state.
3. Success updates objective progress.
4. Failure can trigger hazards: gas, water/electric, fire, panic, explosions, or dispatcher commentary.
5. `ARunState` updates repair counts and win/loss conditions.

## Save / Economy Flow

1. During or after shift completion, `ARunState` computes player/company financial results.
2. `UCompanyLedgerSubsystem` commits balance, reputation, shift number, quota, stock, equipment, and career stats.
3. `UAvariikaSaveGame` persists the ledger to disk.
4. Clients receive live replicated snapshots from `ARunState`, not directly from the ledger subsystem.

## Shutdown / Restart

1. Restart requests are handled server-side through `ARunState`.
2. Return-to-hub flow uses `UDispatchSubsystem` home hub state when available.
3. GameInstance subsystems survive travel; world actors and `ARunState` are recreated per world.
