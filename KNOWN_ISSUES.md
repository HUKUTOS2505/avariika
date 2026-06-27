# KNOWN_ISSUES

Known problems and risks collected from the documentation corpus. See `PROJECT_BRAIN.md` for system context.

## Gameplay

- Interaction targeting can choose the wrong object when overlap zones are large or priorities beat aim direction.
- Repair chains need full PIE/co-op validation after recent changes.
- Some incident systems are designed more deeply than they are verified in gameplay.
- Fire is intended as emergent but production completeness is unclear.
- Wounded/drag flow exists but needs animation and co-op feel validation.

## Code

- Current dirty worktree build health is unknown.
- Historical audits list many fixed issues, but fixes still need regression validation against current code.
- Full-world scans in HUD/tick paths were repeatedly flagged as performance risks.
- Session handling and online backend behavior need validation beyond Null/LAN.
- Save/economy reset and migration behavior need explicit tests.

## Assets

- Many imported packs are placeholders, references, or license-uncertain.
- Large external/reference folders remain at root and need future storage policy.
- Production asset ownership is not fully resolved.
- Some old docs mention paths that may be stale.

## Animation

- `ABP_Worker` needs a robust state machine and work montage slot.
- Foot IK is not complete.
- Turn-in-place, starts/stops, idle fidgets, and final head aim/free-look need validation.
- Retargeting custom rigs can produce bad poses without visual checks.
- FreeAnimationLibrary dynamic locomotion is documented as unsafe for active locomotion.

## Networking

- Multiplayer stability remains top priority.
- Co-op visual feedback can desync if cosmetic state is not replicated or multicast correctly.
- Late joiners need validation for monitor/camera/visual states.
- Host/client fairness for timing minigames needs validation.

## Performance

- Heavy imported packs and large textures can exceed practical VRAM budgets.
- Per-frame world iteration in UI/gameplay is a known risk.
- Tick optimization is still an active priority.
- Weather/framework plugins should be cherry-picked, not blindly integrated.

## Pipeline

- `Scripts/` mixes reusable tools, diagnostics, generated outputs, and one-off experiments.
- Documentation archive contains valuable history but may contradict current `PROJECT_BRAIN.md`.
- Some PowerShell reads display older Russian text as mojibake; preserve files but verify encoding when editing.
- Markdown links are currently verified, but no permanent automated check is installed yet.

