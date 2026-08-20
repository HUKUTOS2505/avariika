# Build Workflow

Use this prompt when compiling the project.

## Preflight

1. Check `git status --short`.
2. If Unreal Editor is open, warn that Live Coding or file locks may affect a full build.
3. Close Unreal Editor only if needed for the build and only within the project workflow.

## Command

Run:

```powershell
"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika_UE58_sandbox\avariika.uproject" -WaitMutex
```

## Failure Policy

- Do not stop after the first compile error.
- Read the first meaningful compiler or UBT error.
- Fix the cause.
- Rebuild.
- Repeat until clean or genuinely blocked.

## Report

Include:

- Build command used.
- Success/failure.
- Key errors fixed.
- Remaining blockers.
