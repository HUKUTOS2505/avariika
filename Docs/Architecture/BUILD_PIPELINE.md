# Build Pipeline

Canonical project context: [PROJECT_BRAIN.md](../../PROJECT_BRAIN.md).

## Project Files

- Project: `avariika.uproject`
- Runtime target: `Source/avariika.Target.cs`
- Editor target: `Source/avariikaEditor.Target.cs`
- Runtime module: `Source/Avaryo/Avaryo.Build.cs`

## Build Inputs

- C++ source: `Source/Avaryo`
- Config: `Config`
- Plugins: enabled in `avariika.uproject`
- Content/assets: loaded by Unreal, not edited by build tooling.

## Expected Build Workflow

1. Open or launch Unreal Editor when editor interaction is required.
2. For C++ changes, regenerate project files only when module/target/plugin structure changes.
3. Build the editor target with Unreal Build Tool.
4. Fix compile errors and rebuild until clean.
5. Run a focused smoke test:
   - Editor load.
   - Map load.
   - PIE single player for basic spawn.
   - PIE two-player/listen server for multiplayer-sensitive changes.
6. Save modified assets only when asset work was requested.

## Documentation-Only Workflow

For this knowledge-base task:

- Do not compile unless source/config changed.
- Do not regenerate project files.
- Do not open/save assets.
- Validate markdown links and run `python Scripts/update_project_docs.py --check`.

## Automation Tests

`Source/Avaryo/Private/Tests/AvaryoTests.cpp` exists, so selected C++ automation tests are present. The current documentation pass did not execute Unreal automation tests because gameplay source and project configuration were not modified.

## Failure Policy

For feature work, never stop at one failed compile. Read the errors, fix the cause, rebuild, and repeat. For documentation-only work, compile failures in pre-existing code should be reported rather than fixed unless the user asks for code changes.
