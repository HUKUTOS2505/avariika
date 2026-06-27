# Repository Analysis

Date: 2026-06-27

## Scope

Read-only analysis covered root markdown/txt/json/docx files, loose images, top-level folders, script inventory, project configuration summary, and Git worktree state.

## Findings

### Worktree

The repository was already dirty before the reorganization. Existing changes included `Content/`, `Source/`, `Config/`, `Claudius/`, `SHOPPING.md`, `WORKLOG.md`, and a staged empty `AGENTS.md`. These pre-existing changes were treated as user/agent work and were not reverted.

### Documentation

The root contained many overlapping documents:

- Code audits: `CODE_AUDIT.md`, `CODE_AUDIT2.md`, `CODE_AUDIT3.md`.
- Animation and locomotion: `ANIM_IMPORT_PLAN.md`, `ANIM_PACKS_SURVEY.md`, `LOCOMOTION_PLAN.md`, `LOCO_BUILD_SHEET.md`, `STATE_MACHINE_BUILD.md`, `TURN_IN_PLACE_BUILD_SHEET.md`, `MODULAR_WORKER_PLAN.md`.
- Audio/VFX: `AUDIO_INVENTORY.md`, `SOUND_MAP.md`, `SOUND_VFX_TZ.md`, `INCIDENT_FX_MAP.md`, `EFFECTS_INVENTORY.md`.
- Concepts: `CONCEPT.md`, `КОНЦЕПТ_2.0.md`, `KONTSEPT_Avariika.md`, `IDEAS.md`, `avariika_ideas_bank.md`, `PLAYER_APPEAL_PLAN.md`, `PROJECT_BRIEF_FOR_WEB.md`.
- World/maps: house, factory, hospital, atmosphere, and floor planning docs.
- Asset audits and references: asset inventory, pack references, raw asset triage, crawl findings, optimization reports.
- Worklogs/testing: `WORKLOG.md`, `AUTONOMOUS_TASKS.md`, `TESTING.md`.

No byte-identical duplicate root markdown/txt/json files were found. Duplication was thematic and structural.

### Images

The root contained loose PNG/JPG files. They were a mix of UI screenshots, animation/mocap references, concept images, and general screenshots. Filenames were preserved during organization.

### Scripts

`Scripts/` contained hundreds of Python utilities and many generated outputs:

- 439 `.py`
- 156 `.json`
- 91 `.txt`
- 3 `.ps1`
- 1 `.pyc`

Scripts were not moved because they may be referenced by active Unreal/editor workflows. Future work should split reusable scripts from one-off diagnostics and output files.

### Temporary / Backup / Reference Folders

Top-level non-Unreal support folders included:

- `.claude`
- `.idea`
- `.venv`
- `blendspace`
- `Claudius`
- `GameAnimationSample`
- `Scripts`
- `_KeptPacks`
- `_mapmove_backup`
- `анимации`

These were not moved during this task. `_mapmove_backup`, `GameAnimationSample`, `blendspace`, and raw animation folders are candidates for future archival or external storage after validation.

### Unreal Safety

Unreal-required folders were not reorganized:

- `Content/`
- `Source/`
- `Config/`
- `Plugins/`
- `Binaries/`
- `Intermediate/`
- `Saved/`

## Quality Assessment

The documentation contained valuable project-specific information but lacked a stable entry point. The main quality problems were root clutter, overlapping planning documents, stale references, and unclear status boundaries between current truth and historical notes.

