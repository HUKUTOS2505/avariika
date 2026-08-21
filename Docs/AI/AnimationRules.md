# Animation Rules

Primary context: [PROJECT_BRAIN.md](../../PROJECT_BRAIN.md). Animation details live in [ANIMATION_AND_LOCOMOTION.md](../Animation/ANIMATION_AND_LOCOMOTION.md).

- Do not change animation assets manually on disk.
- Use Unreal MCP/editor automation for Animation Blueprint, retargeting, montage, and asset edits.
- Keep C++ animation state minimal and stable; avoid moving animation graph logic into gameplay code.
- Foot IK is a current priority but not complete.
- Work montage support is documented as broken/incomplete; verify graph slots before relying on montages.
