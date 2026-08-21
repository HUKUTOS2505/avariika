# Coding Rules

See [PROJECT_BRAIN.md](../../PROJECT_BRAIN.md), [CLASS_INDEX.md](../Architecture/CLASS_INDEX.md), and [MODULE_DEPENDENCIES.md](../Architecture/MODULE_DEPENDENCIES.md).

- Follow UE5 C++ and Epic conventions.
- Preserve the existing `Avaryo` architecture unless a task explicitly asks for refactoring.
- Keep replication server-authoritative.
- Keep Blueprint/content edits in Unreal MCP/editor workflows.
- Avoid introducing GAS, new modules, or major abstractions without an implementation plan.
- Do not implement the Listener Monster without explicit instruction.
