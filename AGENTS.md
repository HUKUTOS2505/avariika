# AGENTS

Operational manual for AI agents. Read `PROJECT_BRAIN.md` first; it is the canonical project source.

## Work Style

- Work autonomously and end-to-end.
- Inspect before editing.
- Prefer small, verifiable changes.
- Preserve architecture and avoid broad refactors unless the task requires them.
- Never revert user or other-agent work unless explicitly asked.

## Safety Rules

- Do not implement the Listener Monster without explicit instruction.
- Do not introduce purple UI. Use the established orange accent direction.
- Do not manually edit `.uasset` or `.umap` files.
- Do not move Unreal-required folders: `Content/`, `Source/`, `Config/`, `Plugins/`, `Binaries/`, `Intermediate/`, `Saved/`.
- For documentation-only tasks, do not modify gameplay, C++, Blueprints, config, plugins, or assets.
- Archive uncertain information; do not delete it.

## Unreal MCP

- Use Unreal MCP for editor, level, Blueprint, material, animation, actor, and content operations.
- Prefer purpose-built MCP tools over `execute_python`.
- Use `execute_python` only when no safer MCP tool exists.
- If a needed MCP tool is missing, use discovery/search and choose an equivalent.
- For read-only editor checks, use read-only MCP calls such as level info, actor lists, selections, bounds, asset info, or compile status.

## Unreal Editor Coexistence

The user may keep Unreal Editor open while manually building maps, placing assets, adjusting lighting, or editing level geometry.

Agents may open Unreal Editor when it is genuinely needed for the task, including Blueprint, AnimBP, retargeting, map, asset, PIE, or Unreal MCP work.

When Unreal Editor is open, agents may:

- Read project files, documentation, logs, and source code.
- Inspect assets without saving changes.
- Prepare plans, reports, task lists, and code changes.
- Modify text-only project files only when the task explicitly allows it and does not require a build.
- Work on files unrelated to the currently open level, but must not save or modify `.uasset` or `.umap` assets.

Agents must never kill Unreal Editor processes with `taskkill`, process termination, or forced closing.

When an operation requires Unreal Editor to be closed, agents may close it only through the normal editor workflow:

1. Stop PIE first, if PIE is running.
2. Run `Save All`.
3. Verify saving completed without errors and without open confirmation dialogs.
4. Only after successful saving, close Unreal Editor normally.
5. Never force-close `UnrealEditor.exe`.

If `Save All` fails, a dialog appears, a conflict occurs, a save error appears, or an unknown unsaved asset is present:

- Do not close Unreal Editor.
- Report in Russian what exactly could not be saved.
- Wait for the user's decision.

Before closing Unreal Editor, briefly report:

- What was saved.
- Why Unreal Editor is being closed.
- What operation will run after closing.

Operations requiring the editor to be closed:

- Full C++/UBT build.
- `UnrealEditor-Cmd` or headless commandlets.
- Python scripts that create, modify, retarget, migrate, compile, or save Unreal assets.
- Asset migration.
- Batch retargeting.
- Plugin or `.uproject`/config changes.
- Modifying or saving Blueprints, AnimBPs, maps, levels, or other `.uasset`/`.umap` files.
- Operations that could conflict with the user editing the same assets.

If Unreal Editor is open and the task does not require closing it, continue with safe read-only, planning, or text-only work instead of asking the user to close it.

## Build Rules

- C++ changes require an Unreal Build Tool build.
- Do not stop after one failed compile. Read errors, fix, rebuild.
- Follow the Unreal Editor coexistence rule when full rebuilds require the editor or Live Coding state to be cleared.
- Standard build command:

```powershell
"C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika\avariika.uproject" -WaitMutex
```

## Commit Rules

- Commit only complete, verified work.
- Never commit a broken build.
- Do not mix unrelated user changes into a commit.
- Documentation-only commits should not include source, config, or asset changes.

## Documentation Rules

- `PROJECT_BRAIN.md` is the single source of truth for the complete project description.
- `PROJECT_STATE.md` classifies current implementation status.
- `TASKS.md` tracks prioritized backlog.
- `ROADMAP.md` tracks milestones.
- `KNOWN_ISSUES.md` tracks known problems.
- `DECISIONS.md` tracks architectural decisions.
- Domain docs under `Docs/` are deep references, not primary status.

## Developer Workflow Prompts

Use prompt templates under `Prompts/` for repeatable engineering workflows:

- `Prompts/Continue.md` - resume orientation and next-step analysis.
- `Prompts/Feature.md` - feature planning and implementation workflow.
- `Prompts/BugFix.md` - defect reproduction, root-cause analysis, and minimal fix workflow.
- `Prompts/Review.md` - read-only review workflow.
- `Prompts/Build.md` - Unreal Build Tool workflow.
- `Prompts/SmokeTest.md` - headless runtime smoke-test workflow.
- `Prompts/DocsUpdate.md` - documentation update workflow.
- `Prompts/Commit.md` - safe commit workflow.

## Project Conventions

- UE5 C++ with Epic conventions.
- Server-authoritative multiplayer gameplay.
- Orange UI accent.
- Mobility/WorkAnim/UE mannequin-compatible animation path for player locomotion unless a decision changes it.
- Use root knowledge-base docs for current truth; use `Docs/Archive/Originals/` only for history.
