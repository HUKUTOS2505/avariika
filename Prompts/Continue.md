# Continue Workflow

Use this prompt when resuming work on the Avariika project.

## Read First

Read these files before making decisions:

- `AGENTS.md`
- `PROJECT_BRAIN.md`
- `PROJECT_STATE.md`
- `TASKS.md`
- `CHANGELOG.md`

## Checks

1. Run `git status --short`.
2. Check whether Unreal MCP is available.
3. If MCP is unavailable, report that clearly; do not block indefinitely unless the task requires editor automation.
4. Do not modify files during this orientation pass unless explicitly asked.

## Report

State:

- Current project state.
- Current dirty worktree risks.
- Active priorities.
- Most likely next engineering task.
- Whether Unreal MCP is reachable.
- Whether build/smoke/docs checks are needed before continuing.
