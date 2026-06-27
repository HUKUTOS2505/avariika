# Feature Workflow

Use this prompt before implementing a new feature.

## Process

1. Read `AGENTS.md`, `PROJECT_BRAIN.md`, `PROJECT_STATE.md`, and relevant architecture docs under `Docs/Architecture/`.
2. Analyze the affected architecture before editing.
3. Identify affected classes, Blueprints, assets, maps, scripts, and docs.
4. Propose a short implementation plan.
5. Change one module or system at a time.
6. Prefer existing project patterns over new abstractions.
7. Use Unreal MCP for Blueprint, asset, level, material, animation, or editor work.

## Verification

After changes:

- Build when C++ or project settings changed.
- Run a focused smoke test when gameplay/editor behavior changed.
- Update docs only after actual implementation changes.
- Re-run failed verification after fixes.

## Boundaries

- Do not mix unrelated dirty changes.
- Do not implement the Listener Monster without explicit instruction.
- Do not introduce purple UI.
