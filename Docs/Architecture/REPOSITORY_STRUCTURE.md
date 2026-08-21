# Repository Structure

Target structure after documentation reorganization.

```text
/
  AGENTS.md
  PROJECT_BRAIN.md
  PROJECT_STATE.md
  ROADMAP.md
  TASKS.md
  CHANGELOG.md
  DECISIONS.md
  KNOWN_ISSUES.md
  README.md

  Docs/
    Architecture/
    Gameplay/
    Systems/
    AI/
    Animation/
    UI/
    Audio/
    Networking/
    Multiplayer/
    Economy/
    World/
    Concepts/
    Audits/
    BuildSheets/
    Worklogs/
    Archive/
      Originals/
      CrawlData/
      Misc/

  Reference/
    Images/
      UI/
      Screenshots/
      Concepts/
      Mocap/
    Mocap/

  Content/        # Unreal assets, do not move manually
  Source/         # C++ source
  Config/         # Unreal configuration
  Plugins/        # Unreal plugins
  Binaries/       # generated/build output
  Intermediate/   # generated/build output
  Saved/          # generated/editor output
  Scripts/        # editor automation and diagnostics
```

## Rules

- Root is for entry points and source-of-truth documents only.
- Domain docs live under `Docs/`.
- Historical source material lives under `Docs/Archive/Originals/`.
- Loose reference images live under `Reference/Images/`.
- Unreal asset folders are left exactly where Unreal expects them.

