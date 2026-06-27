# Smoke Test Workflow

Use this prompt for a basic headless runtime check.

## Command

Run:

```powershell
"C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "C:\unrealEngine\avariika\avariika.uproject" -game -nullrhi -nosound -unattended -log
```

## Log Check

Inspect:

```text
Saved\Logs\avariika.log
```

Search for:

- `Fatal`
- `Ensure`
- `: Error:`

## Report

Include:

- Whether the command launched successfully.
- Whether the log contains fatal/ensure/error entries.
- Relevant log excerpts.
- Whether a deeper editor/PIE test is still needed.
