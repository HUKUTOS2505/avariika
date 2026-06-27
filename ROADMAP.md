# ROADMAP

Milestone roadmap. See `PROJECT_BRAIN.md` for project architecture and priorities.

## Pre-Alpha

Goal: prove the core co-op repair loop is stable and fun on one playable object.

Exit criteria:

- Clean build from current branch.
- One playable map with start, diagnosis, repair chain, extraction, report/reward.
- Host/client smoke test passes for core interactions.
- Locomotion is readable and not immersion-breaking.
- HUD communicates objectives, vitals, interaction prompts, and hazards.
- Known P0 issues are fixed or explicitly accepted.

Primary focus:

- Multiplayer stability.
- Interaction targeting.
- Repair chain reliability.
- `ABP_Worker` slot/state-machine foundation.
- Build and smoke-test discipline.

## Alpha

Goal: complete the vertical slice and make the game loop repeatable.

Exit criteria:

- Multiple incident types work: electric, water, gas, generator, emergent fire.
- Economy/save loop works across sessions.
- Tool/loadout preparation works.
- Wounded/drag/revive flow is usable in co-op.
- Audio/VFX feedback covers all core incidents.
- First-pass Foot IK or equivalent grounding quality is in.
- Settings/menu path is usable.

Primary focus:

- Character movement quality.
- Foot IK.
- Economy and save.
- Incident feedback.
- Co-op regression testing.

## Beta

Goal: content breadth, optimization, and production hardening.

Exit criteria:

- Additional object/map content is playable or production-planned.
- Performance is acceptable on target hardware.
- Asset licensing status is clear.
- Automated or scripted smoke checks exist for critical systems.
- Networking edge cases are reduced: late join, disconnect, re-host, session flow.
- UI, audio, VFX, and animation have consistent production direction.

Primary focus:

- Optimization.
- Content validation.
- Multiplayer edge cases.
- Animation polish.
- Asset replacement/licensing.

## Release

Goal: shippable, stable co-op build.

Exit criteria:

- No known build-breaking issues.
- No unlicensed placeholder assets in the shipping build.
- Save/progression migration/reset behavior is defined.
- Multiplayer sessions are stable on the chosen online backend.
- Core loop is understandable without developer guidance.
- Documentation reflects shipped architecture.

Primary focus:

- Reliability.
- Compliance/licensing.
- Final performance.
- Player onboarding.
- Release packaging and QA.

