# Commit Workflow

Use this prompt before creating a Git commit.

## Preflight

1. Show `git status --short`.
2. Show a diff summary.
3. Identify unrelated dirty changes and exclude them.
4. Confirm build/smoke/doc checks are appropriate for the change type.

## Rules

- Commit only complete, verified work.
- Never commit a broken build.
- Do not mix unrelated dirty changes.
- Do not include generated asset changes unless they belong to the completed task.
- Push only after successful build/smoke verification when code, Blueprint, asset, config, or gameplay behavior changed.

## Report

Include:

- Files included.
- Files deliberately excluded.
- Verification performed.
- Commit hash after committing.
