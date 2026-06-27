# Subsystems

Canonical project context: [PROJECT_BRAIN.md](../../PROJECT_BRAIN.md).

## Project Subsystems

| Subsystem | Type | Purpose | Responsibilities | Dependencies | Status |
|---|---|---|---|---|---|
| `UDispatchSubsystem` | `UGameInstanceSubsystem` | Preserve dispatch/job context across server travel. | Stores home hub map, active call id/title, and whether the kit was loaded. | `ACallBoard`, `AToolCase`, `ARunState`, server travel. | Partially Implemented |
| `UCompanyLedgerSubsystem` | `UGameInstanceSubsystem` | Host-authoritative company economy and career state. | Loads/saves company balance, shifts, reputation, equipment upgrades, stock, career stats, and quota. | `UAvariikaSaveGame`, `ARunState`, future shop/progression UI. | Partially Implemented |
| `UAvariikaOnlineSubsystem` | `UGameInstanceSubsystem` | Thin wrapper over UE OnlineSubsystem sessions. | Host, find, join, leave session; manages session delegates and map travel. | `OnlineSubsystem`, `OnlineSubsystemUtils`, `DefaultEngine.ini` service selection. | Partially Implemented |
| `URunStateSubsystem` | `UWorldSubsystem` | World-local run-state ownership. | Ensures or locates `ARunState` for active world/map. | `ARunState`, map lifecycle. | Partially Implemented |

## Non-Subsystem Persistent Objects

| Object | Type | Purpose |
|---|---|---|
| `UAvariikaSaveGame` | `USaveGame` | Disk persistence container for company ledger data in slot `AvariikaCompany`. |
| `ARunState` | `AInfo` | Replicated shift state actor, spawned/owned through `URunStateSubsystem`. |

## Not Present

- No custom `UEngineSubsystem` in project source.
- No custom `UEditorSubsystem` in project source.
- No GAS-specific subsystem is implemented.
- Plugin subsystems may exist inside third-party plugin source, but they are not Avariika gameplay ownership boundaries.
