# TASKS

Prioritized backlog. For context and architecture, see `PROJECT_BRAIN.md`.

## P0

### P0-1 Validate Current Build Health

- Description: Establish whether the current dirty worktree compiles and opens cleanly.
- Dependencies: Unreal Engine 5.7 installed; editor closed for full UBT build.
- Complexity: Medium.
- Expected files: none if validation-only; possible follow-up in `Source/` if build breaks.
- Verification: UBT build succeeds; editor loads project; active map opens.

### P0-2 Multiplayer Smoke Test Matrix

- Description: Verify host/client behavior for repairables, power switch, traps, dragging, item use, HUD warnings, and extraction.
- Dependencies: Clean build; two-player listen-server test path.
- Complexity: High.
- Expected files: `Docs/Worklogs/DEVELOPMENT_WORKLOG.md`, possible fixes in `Source/Avaryo`.
- Verification: Host and client observe matching gameplay state and feedback.

### P0-3 Interaction Targeting Fix/Validation

- Description: Resolve or validate `E` targeting ambiguity where overlap fallback may choose the wrong object.
- Dependencies: PIE scene with overlapping interactables.
- Complexity: Medium.
- Expected files: `Source/Avaryo/Private/AvaryoCharacter.cpp`, possibly interactable classes.
- Verification: Aim/facing tests in PIE; no regression for call board, tool case, doors, repairables, pickups.

### P0-4 `ABP_Worker` Work Montage Slot

- Description: Add or validate animation graph slot support so repair/work montages can play over locomotion.
- Dependencies: Unreal MCP/editor Blueprint tooling; current `ABP_Worker` state.
- Complexity: Medium.
- Expected files: `Content/Avariika/Anim/Locomotion/ABP_Worker.uasset`.
- Verification: Compile Blueprint; play a repair montage in PIE or animation preview.

## P1

### P1-1 Locomotion State Machine

- Description: Build production locomotion state machine: idle, move, crouch, starts, stops, turn-in-place, aim layer.
- Dependencies: Stable `ABP_Worker`, Mobility assets, visual validation.
- Complexity: High.
- Expected files: `ABP_Worker.uasset`, locomotion blendspaces/aim offsets.
- Verification: Walk/jog/crouch/turn/head aim tested in editor and PIE.

### P1-2 Foot IK Foundation

- Description: Implement or integrate stable foot placement for worker locomotion.
- Dependencies: Locomotion graph structure.
- Complexity: High.
- Expected files: Animation Blueprint, IK rigs/control rig assets if used.
- Verification: Feet stay planted on flat/slope/stairs; no network-visible artifacts.

### P1-3 Economy And Save Audit

- Description: Verify company ledger, save game, upgrades, quota windows, and reset behavior.
- Dependencies: Current `CompanyLedgerSubsystem` and `AvariikaSaveGame`.
- Complexity: Medium.
- Expected files: `Source/Avaryo/Private/Game/CompanyLedgerSubsystem.cpp`, save classes, tests.
- Verification: Unit/PIE flow for earn, spend, save, reload, reset.

### P1-4 Audio/VFX Incident Pass

- Description: Connect core incident sounds and Niagara feedback to gas, electric, water, fire, repair, and UI events.
- Dependencies: Stable gameplay triggers.
- Complexity: Medium.
- Expected files: Content assets through MCP, possibly `ARepairable`, `AvaryoHUD`, audio cues.
- Verification: PIE event checklist; no missing cue spam in logs.

### P1-5 Documentation Maintenance Script

- Description: Add a markdown link checker and documentation audit helper.
- Dependencies: Current `Docs/` layout.
- Complexity: Low.
- Expected files: `Scripts/maintenance/`.
- Verification: Script reports no broken links.

## P2

### P2-1 GAS Architecture Plan

- Description: Decide how Gameplay Ability System fits existing vitals, items, repair actions, and effects.
- Dependencies: Current systems mapped in `PROJECT_BRAIN.md`.
- Complexity: Medium.
- Expected files: `DECISIONS.md`, `Docs/Systems/`.
- Verification: ADR accepted before implementation.

### P2-2 Civilian/NPC Prototype

- Description: Prototype non-combat NPCs for hospital/house atmosphere.
- Dependencies: Map target and animation source decision.
- Complexity: Medium.
- Expected files: Content Blueprints/assets through MCP, possible AI classes.
- Verification: NPC spawns, idles/moves, and does not impact core performance.

### P2-3 Scripts Cleanup

- Description: Separate reusable editor tools from diagnostics and generated outputs.
- Dependencies: Inventory of script references.
- Complexity: Medium.
- Expected files: `Scripts/` organization and docs only unless tools are actively changed.
- Verification: No referenced automation breaks; docs explain entry points.

### P2-4 Asset Licensing And Production Readiness

- Description: Classify packs as production-owned, placeholder, reference-only, or remove-before-release.
- Dependencies: Asset inventory and license records.
- Complexity: High.
- Expected files: `Docs/Audits/ASSET_AND_PACK_AUDITS.md`, `PROJECT_STATE.md`.
- Verification: Every production asset category has an ownership decision.

