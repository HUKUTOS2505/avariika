# Docs Update Workflow

Use this prompt after implementation changes that affect project knowledge.

## Rules

- Update docs only after factual implementation changes.
- Keep `PROJECT_BRAIN.md` as the single source of truth for broad project context.
- Prefer links to duplicated explanations.
- Do not edit gameplay, C++, Blueprints, assets, maps, config, or plugins for a docs-only task.

## Checks

Run:

```powershell
python Scripts\update_project_docs.py --check
```

If generated regions are stale and the implementation has actually changed, run:

```powershell
python Scripts\update_project_docs.py --write
python Scripts\update_project_docs.py --check
```

## Report

Include:

- Docs changed.
- Source facts reflected.
- Link/check results.
- Any stale or missing documentation.
