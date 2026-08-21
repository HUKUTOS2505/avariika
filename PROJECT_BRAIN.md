# PROJECT_BRAIN

Canonical project knowledge for "Avariika". New agents should read this first, then use linked domain docs only when deeper detail is needed.

## 1. Project Identity

`Avariika` is an Unreal Engine 5.7 cooperative horror-comedy about a night emergency repair crew. Players are ordinary utility workers, not soldiers. The fantasy is tense, physical, messy repair work: diagnose the real failure, carry awkward tools, make noise, panic, help injured teammates, argue with dispatch, and get the crew back to the van.

Primary design pillars:

- Co-op first: problems should be easier, funnier, or safer with communication.
- Repair chains, not single buttons: the visible symptom often hides a deeper cause.
- Physical vulnerability: darkness, electricity, gas, water, fatigue, panic, wounds, equipment limits.
- Horror from systems and atmosphere before monsters.
- Comedy from workplace chaos, bad decisions, bureaucracy, and slapstick.
- Progression through gear, tools, money, and crew identity.

Hard creative constraint: do not implement the Listener Monster without explicit instruction.

## 2. Core Game Loop

1. Start at the crew base or dispatch context.
2. Receive a job from dispatch. The job describes a symptom, not necessarily the root cause.
3. Load or find required tools and protective gear.
4. Travel to the object.
5. Diagnose the actual chain of failures.
6. Repair in the correct order. Wrong order can cause shock, flood, explosion, fire, panic, or injury.
7. Extract the whole crew to the van/exit zone.
8. Receive an act/report, money, penalties, titles, and progression.
9. Upgrade gear, buy consumables, and take the next job.

MVP incident families:

- Electricity.
- Water/flooding.
- Gas.
- Generator/power supply.
- Fire as an emergent result of mistakes.

Deep gameplay source: `Docs/Gameplay/GAMEPLAY_SYSTEMS.md`.

## 3. Progression And Economy

Progression is intended to be practical, not power-fantasy combat progression.

- Money is earned from completed repair work, loot extraction, and job results.
- Money is spent on gear, consumables, tools, upgrades, services, and cosmetics.
- Tools and protective equipment unlock safer or faster options.
- Heavy equipment can require pre-planning: bring it in the van, find it on-site, or pay for help.
- Loot economy exists as a risk/reward layer: carry valuables out, sell them, fund upgrades.

Current code includes `CompanyLedgerSubsystem` and `AvariikaSaveGame`, indicating a persistent company/economy/save direction. Treat economy values as tunable and gameplay-facing.

## 4. Player Systems

Primary player class: `Source/Avaryo/Public/AvaryoCharacter.h`.

Known player responsibilities:

- Movement, sprint, crouch, jumping, and camera behavior.
- Inventory and item slots.
- Heavy item carrying.
- Interaction through `E`.
- Item drop/throw/transfer.
- Use/cast items such as medkit, cigarette, coffee, extinguisher, radio.
- Dragging wounded teammates.
- Pushing, stumbling, noise generation, and operator monitor entry points.

Vitals component: `Source/Avaryo/Public/Components/VitalsComponent.h`.

Tracked or designed player states include:

- HP.
- Panic.
- Stamina.
- Toilet/bladder pressure.
- Wounded/downed/unconscious flow.
- Wet, shocked, gassed, burned, winded, soiled, dragged.

Flashlight component: `Source/Avaryo/Public/Components/UFlashlightComponent.h`.

Worker appearance component: `Source/Avaryo/Public/Components/WorkerAppearanceComponent.h`.

## 5. Inventory And Items

Inventory direction:

- One heavy item in hands.
- Several light slots.
- Heavy item means hands are occupied and movement/interaction options are constrained.
- Van/tool case acts as a preparation and storage layer.

Item classes:

- `APickupItem`: general pickup/tool/consumable.
- `ABioPickup`: throwable bio pickup.
- Gameplay items include welder, gas detector, rubber boots, medkit, cigarette, coffee, extinguisher, fuses, pipes, cables, batteries, and repair consumables.

## 6. Interaction And World Systems

World gameplay classes live under `Source/Avaryo/Public/World/`.

Core classes:

- `ARepairable`: central repair object with stages, minigames, gas, water, electricity, generator, and repair completion.
- `APowerSwitch`: power/lighting/electricity control.
- `ADoor`: open/close interaction.
- `AExitZone`: extraction and run completion.
- `ACallBoard`: dispatch/job board.
- `AToolCase`: tool preparation.
- `AToilet`: toilet state/minigame.
- `ATrap`: trap feedback and hazards.
- `AFloodlight`: portable or placed lighting.
- `AFoamPatch`: foam/slip hazard.
- `ABioProjectile`: thrown bio hazard.

Interaction conventions:

- Prefer authoritative server state for gameplay outcomes.
- Client-visible feedback must replicate or be triggered by multicast/client RPC as appropriate.
- Avoid large overlap zones stealing `E` from the intended target; aim/facing priority needs careful PIE validation.

## 7. Locomotion And Animation

Current direction:

- Main worker body uses Modular Workers / Quantum visual identity.
- Active locomotion work uses Mobility Pro / WorkAnim / UE mannequin-compatible clips.
- `ABP_Worker` is the key animation Blueprint.
- Current active assets include `BS_Loco_Mobility`, crouch blendspaces, aim offsets, and many retargeted Mobility clips under `Content/Avariika/Anim/Locomotion/Mobility/`.

Animation rules:

- Target player animation skeleton is the UE mannequin-compatible `SK_Mannequin` / WorkAnim path documented in `Docs/Animation/ANIMATION_AND_LOCOMOTION.md`.
- New player animations should end on the same compatible skeleton path before use in `ABP_Worker`.
- Do not bulk-import giant packs blindly.
- Retarget Motifect/custom rigs only with visual validation.
- FreeAnimationLibrary dynamic locomotion is documented as unsafe/incorrect for active locomotion.

Known locomotion status:

- T-pose issue was addressed by aligning the graph/skeleton path.
- Walk/jog speeds were tuned to blendspace rows.
- Sprint is intended to be forward-gated.
- Full state machine, high quality idle fidgets, starts/stops, turn-in-place, work montage slot, and Foot IK remain active priorities.

Deep source: `Docs/Animation/ANIMATION_AND_LOCOMOTION.md`.

## 8. Networking And Multiplayer

Multiplayer stability is the top engineering priority.

Current networking direction:

- Listen-server / Null subsystem appears to be current local baseline.
- Online integration includes `OnlineSubsystem`, `OnlineSubsystemNull`, and `OnlineSubsystemUtils`.
- `.uproject` currently also enables `EOSCore`, voice, and other online-related plugins. Verify actual runtime path before production work.

Replication rules:

- Gameplay authority belongs on the server.
- Cosmetic feedback still matters in co-op; if clients cannot see it, the mechanic can feel broken.
- Late joiners must initialize replicated visual and monitor/camera states.
- Avoid full-world scans every frame in HUD or tick paths.
- Test host and at least one client for repair, light, trap, item, wounded, and extraction features.

Audit source: `Docs/Audits/CODE_AUDITS.md`.

## 9. AI

Current AI status is limited/planned.

Planned AI areas:

- Dispatch as systemic voice/UI guidance.
- Civilian or patient NPCs for future maps.
- Monster AI is deferred by explicit project rule.
- Gameplay Ability System and StateTree may become relevant to AI/abilities, but do not assume completed integration.

Plugin note: `GameplayStateTree` is enabled.

## 10. UI

Current UI direction:

- In-game HUD is C++ Canvas-based: `AvaryoHUD`.
- Menu HUD exists: `MenuHUD`.
- UI accent should be orange.
- Do not introduce purple UI.
- Easy Options Menu exists as an asset/plugin candidate for settings, but integration must respect project input/audio architecture.

HUD responsibilities include vitals, repair prompts, crosshair, warnings, gas/repair feedback, minimap/objective information, and operator monitor concepts.

## 11. Audio And VFX

Audio/VFX are gameplay feedback, not decoration.

Current coverage:

- Existing SFX include explosion, repair done, radio, ambient boiler, heartbeat, flash click, gas hiss, electric zap, weld buzz, drink, and Survival SFX UI/craft/movement assets.
- Niagara/VFX sources include explosion, fire, smoke, sparks, gas, rain, weather, and incident effects.
- Major missing area: male vocal efforts, pain, breathing, panic, coughing, dragging, and exertion.

Deep source: `Docs/Audio/AUDIO_VFX_KNOWLEDGE.md`.

## 12. Maps And World

Active map observed through Unreal MCP:

- Map: `Lvl_FirstPerson`
- Package: `/Game/Avariika/Maps/Lvl_FirstPerson`
- Actor count at last read-only check: `291`
- Game mode: `BP_AvaryoGameMode_C`

Map/world direction:

- House/Dom is a major prototype/object direction.
- Factory and hospital are documented future or parallel object concepts.
- Hospital is a flagship environment candidate.
- Atmosphere targets night, fog, rain, power/light tension, room tone, creaks, and system-driven scares.

Deep source: `Docs/World/WORLD_AND_MAPS.md`.

## 13. Save System

Save/economy classes:

- `AvariikaSaveGame`.
- `CompanyLedgerSubsystem`.
- `DispatchSubsystem`.

Known intended persistence:

- Company money.
- Equipment/upgrades.
- Quota/progression windows.
- Dispatch/job context.

Treat save schema changes carefully. Validate migration or reset behavior when changing persistent data.

## 14. Technical Architecture

Project:

- Engine: Unreal Engine 5.8 (`avariika.uproject` `EngineAssociation`).
- Root: `C:/unrealEngine/avariika_UE58_sandbox`.
- UProject: `avariika.uproject`.
- Runtime module: `Avaryo`.

C++ layout:

- `Source/Avaryo/Public` and `Source/Avaryo/Private`.
- `Components/`: vitals, flashlight, worker appearance.
- `Game/`: run state, dispatch, ledger, online subsystem, save/menu/player controller.
- `Items/`: pickup item types.
- `UI/`: HUD, menu, camera shakes.
- `World/`: interactable world gameplay.
- `Tests/`: project tests.

Important folders:

- `Content/Avariika`: project-owned Unreal content.
- `Scripts`: editor automation, diagnostics, one-off tooling.
- `Docs`: domain docs and archive.
- `Reference`: external/reference images and mocap references.

Do not move Unreal-required folders manually.

## 15. Coding Conventions

- Use UE5 C++ and Epic conventions.
- Keep code modular and avoid unnecessary coupling.
- Preserve current architecture unless a task explicitly requires refactoring.
- Use server-authoritative gameplay state for replicated systems.
- Prefer small, verifiable changes.
- Use `UPROPERTY`/replication metadata deliberately.
- Avoid hidden global scans in per-frame paths.
- No gameplay work should be hidden inside documentation tasks.

## 16. Plugins

Enabled plugins from `avariika.uproject` at the documentation pass include:

- Core/input/animation: `EnhancedInput`, `AnimationWarping`, `AnimationLocomotionLibrary`.
- Runtime/gameplay: `Niagara`, `GameplayStateTree`, `OnlineSubsystem`, `OnlineSubsystemNull`, `OnlineSubsystemUtils`, `AudioCapture`.
- Editor/utility: `EditorScriptingUtilities`, `ModelingToolsEditorMode`, `JsonBlueprintUtilities`.

Marketplace and optional plugins remain declared but disabled, including EOSCore, DLSS/Streamline, voice, Dungeon Architect, Narrative, TickOptToolkit, and UI/tooling packages. Treat a declared or installed plugin as inactive until its `Enabled` state and integration are verified.

Plugin status may differ between enabled, installed, actively used, and production-approved. Verify before relying on a plugin for core gameplay.

## 17. Unreal MCP Workflow

Use Unreal MCP for any editor/content interaction:

- Query current level, actors, selections, bounds, assets, materials, Blueprints, animations.
- Compile Blueprints and materials through MCP tools when available.
- Use `execute_python` only when no safer purpose-built MCP tool exists.
- Use `search_tools`/tool discovery if a desired MCP tool is not exposed.
- Do not edit `.uasset`/`.umap` files through raw filesystem operations.

For documentation-only tasks, Unreal MCP is usually unnecessary unless checking current editor state.

## 18. Build Workflow

For C++ tasks:

1. Inspect current Git status first.
2. Edit code.
3. Close or account for Unreal Editor/Live Coding when required.
4. Build:

```powershell
"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject" -WaitMutex
```

5. Read errors, fix, rebuild until clean.
6. Run a smoke test where possible.
7. Save modified assets if the editor/content changed.
8. Commit only after the feature is complete and clean.

Documentation-only tasks do not require UBT unless code/config/assets changed.

## 19. Current Implementation Status Summary

See `PROJECT_STATE.md` for the classified subsystem table. High-level state:

- Core repair gameplay: partially implemented.
- Player/vitals/inventory: partially implemented.
- Locomotion: partially implemented and actively evolving.
- Multiplayer: partially implemented, with stability as top priority.
- Economy/save: partially implemented.
- GAS: planned.
- Foot IK: planned/experimental.
- Listener Monster: explicitly deferred.

## 20. Project Priorities

1. Multiplayer stability.
2. Character movement.
3. Foot IK.
4. Interaction System.
5. Gameplay Ability System.
6. Optimization.

When in doubt, choose the action that protects these priorities and keeps the project buildable.
