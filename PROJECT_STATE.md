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
| Player locomotion | Partially Implemented | Mobility/WorkAnim path active; state machine, starts/stops, TIP, Foot IK remain. |
| Crouch | Partially Implemented | Assets and work exist; final animation graph quality unknown. |
| Foot IK | Planned | Priority item, not documented as complete. |
| Animation pipeline | Partially Implemented | Retarget path and assets exist; needs disciplined validation. |
| Work montages | Broken | Docs say `ABP_Worker` lacks slot/state-machine support for work montages. |
| Head aim/free look | Experimental | Aim-offset assets exist; final graph/property wiring may need validation. |
| Multiplayer/session | Partially Implemented | Online subsystem code and plugins exist; Null/LAN likely current baseline; co-op validation is top priority. |
| Save/economy | Partially Implemented | `AvariikaSaveGame` and `CompanyLedgerSubsystem` exist. |
| Shop/progression | Planned | Detailed spec exists; implementation status incomplete. |
| Loot economy | Partially Implemented | Design and item value direction exist; production balance unknown. |
| UI/HUD | Partially Implemented | Canvas HUD and menu exist; settings/menu polish and link to systems remain. |
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

## Source Inventory

<!-- AUTO-GENERATED:SOURCE_INVENTORY:START -->
Generated manually from the source inventory on 2026-06-27.

- Runtime modules: `Avaryo`
- Public C++ classes indexed: 30
- Project GameInstance subsystems: `UDispatchSubsystem`, `UCompanyLedgerSubsystem`, `UAvariikaOnlineSubsystem`
- Project World subsystems: `URunStateSubsystem`
- Explicit gameplay components: `UVitalsComponent`, `UFlashlightComponent`, `UWorkerAppearanceComponent`
- Save object: `UAvariikaSaveGame`
- Primary replicated runtime actor: `ARunState`
<!-- AUTO-GENERATED:SOURCE_INVENTORY:END -->

## Current Worktree Note

At the time the documentation pass began, the repository already had dirty changes in `Content/`, `Source/`, `Config/`, and other files. This status document does not assert those changes are complete or buildable.
