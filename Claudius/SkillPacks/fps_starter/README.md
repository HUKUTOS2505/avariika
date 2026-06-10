# fps_starter (CLAUDIUS skill pack)

Starter helpers for first-person shooter projects. Ships with CLAUDIUS v3.0+ as a reference pack.

## Install

```json
{
  "category": "pack",
  "command": "install",
  "parameters": {
    "path": "<path to this folder>"
  }
}
```

Or enable directly if it was already copied to `{Project}/Claudius/SkillPacks/fps_starter/`:

```json
{ "category": "pack", "command": "enable", "parameters": { "pack": "fps_starter" } }
```

## Commands

- `fps_starter.spawn_target_dummies` - grid of cylinder "targets"
- `fps_starter.spawn_weapon_pickup` - labelled weapon pickup placeholder

## Authoring a new pack

Copy this folder, rename it, update `manifest.json`, and drop Python files in. Each `.py` becomes a command; add `# @claudius category/description/workflow` headers so it shows up correctly in the registry and CLAUDE.md index.
