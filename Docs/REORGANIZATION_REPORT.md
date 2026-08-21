# Reorganization Report

Date: 2026-06-27

## Summary

The repository root was reorganized into a maintainable Unreal production layout for documentation and references. Unreal project folders were left in place. No gameplay, C++, Blueprint, config, plugin, or Unreal asset work was intentionally performed.

## Documents Created

- `AGENTS.md`
- `PROJECT_BRAIN.md`
- `PROJECT_STATE.md`
- `ROADMAP.md`
- `TASKS.md`
- `CHANGELOG.md`
- `DECISIONS.md`
- `KNOWN_ISSUES.md`
- `Docs/Audits/REPOSITORY_ANALYSIS.md`
- `Docs/Architecture/REPOSITORY_STRUCTURE.md`
- `Docs/REORGANIZATION_REPORT.md`

## Documents Merged

- `CODE_AUDIT.md`, `CODE_AUDIT2.md`, `CODE_AUDIT3.md` -> `Docs/Audits/CODE_AUDITS.md`
- Animation and locomotion notes -> `Docs/Animation/ANIMATION_AND_LOCOMOTION.md`
- Audio, VFX, and incident feedback notes -> `Docs/Audio/AUDIO_VFX_KNOWLEDGE.md`
- Concept and player appeal notes -> `Docs/Concepts/GAME_CONCEPTS.md`
- World/map/level design notes -> `Docs/World/WORLD_AND_MAPS.md`
- Asset, pack, raw asset, and optimization audits -> `Docs/Audits/ASSET_AND_PACK_AUDITS.md`
- Gameplay, economy, localization, and feature specs -> `Docs/Gameplay/GAMEPLAY_SYSTEMS.md`
- Worklog, autonomous task notes, and testing notes -> `Docs/Worklogs/DEVELOPMENT_WORKLOG.md`
- Download/shopping/build-format notes -> `Docs/Archive/PLANNING_AND_DOWNLOADS.md`

## Files Moved

### Documentation Originals

Original root documents were moved into:

- `Docs/Archive/Originals/Architecture/`
- `Docs/Archive/Originals/Audits/`
- `Docs/Archive/Originals/Animation/`
- `Docs/Archive/Originals/Audio/`
- `Docs/Archive/Originals/Concepts/`
- `Docs/Archive/Originals/World/`
- `Docs/Archive/Originals/Assets/`
- `Docs/Archive/Originals/Planning/`

### Worklog

- `WORKLOG.md` -> `Docs/Worklogs/WORKLOG.md`

### Crawl Data And Misc

- `by_bucket.json`, `filtered_hits.json`, `hits.json`, `treasures.json` -> `Docs/Archive/CrawlData/`
- `промт.docx` -> `Docs/Archive/Misc/`

### Images

Loose root images were moved with filenames preserved:

- UI: `Reference/Images/UI/`
- Screenshots: `Reference/Images/Screenshots/`
- Concepts: `Reference/Images/Concepts/`
- Mocap/animation: `Reference/Images/Mocap/`

## Files Archived

No valuable source note was deleted. Historical documents were archived under `Docs/Archive/Originals/`.

## Duplicates Removed

No byte-identical duplicate markdown/txt/json files were found. Duplicates were thematic:

- Multiple code-audit passes.
- Multiple concept docs.
- Multiple animation/locomotion plans.
- Multiple audio/VFX maps and inventories.
- Multiple house/factory/hospital/world planning docs.
- Multiple asset/pack triage notes.

The root-level duplication was removed by consolidating each topic into a primary domain document and archiving originals.

## Verification

- Markdown relative links checked: pass.
- Root loose images checked: moved.
- Root loose docs checked: moved or replaced with source-of-truth docs.
- Unreal folders were not reorganized.
- `Content/`, `Source/`, `Config/`, `Plugins/`, `Binaries/`, `Intermediate/`, and `Saved/` were not intentionally modified.

## Potential Problems Discovered

- The repository had many pre-existing dirty changes in Unreal assets, C++ source, and config before this task.
- `Scripts/` contains many reusable tools mixed with one-off diagnostics and generated outputs.
- Large external/reference folders remain at root: `GameAnimationSample`, `blendspace`, `_KeptPacks`, `_mapmove_backup`, and `анимации`.
- Some legacy docs contain stale paths and old implementation assumptions; they are preserved for history but should not automatically override `PROJECT_STATE.md`.
- Some PowerShell reads displayed older Russian markdown as mojibake, but files were preserved and rewritten as UTF-8 during consolidation.

## Future Recommendations

- Add a lightweight markdown link-check script under `Scripts/maintenance/`.
- Split `Scripts/` into `Scripts/EditorTools/`, `Scripts/Diagnostics/`, and `Scripts/Archive/Outputs/`.
- Decide whether root reference folders such as `_KeptPacks`, `GameAnimationSample`, and `анимации` should move to external storage or a documented `External/` area.
- Keep `PROJECT_STATE.md`, `TASKS.md`, and `CHANGELOG.md` updated after every major development session.
- Stage documentation moves carefully so Git records them as renames where possible.

