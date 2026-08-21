# Replication Rules

See [PROJECT_BRAIN.md](../../PROJECT_BRAIN.md), [CLASS_INDEX.md](../Architecture/CLASS_INDEX.md), and [MODULE_DEPENDENCIES.md](../Architecture/MODULE_DEPENDENCIES.md).

- Server owns gameplay truth.
- Clients request actions through server RPCs; do not trust client-side state for repairs, inventory, vitals, scoring, or economy.
- Replicated actors/components must declare state in `GetLifetimeReplicatedProps`.
- Cosmetic feedback can use multicast/client RPCs, but authoritative results must be replicated state.
- `UCompanyLedgerSubsystem` is host/server authority; clients read replicated summaries from `ARunState`.
- Validate multiplayer changes in PIE-2 or a listen-server test.
