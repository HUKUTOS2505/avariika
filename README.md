# Avariika

## Start Here

- [PROJECT_BRAIN.md](PROJECT_BRAIN.md) - canonical project source of truth.
- [AGENTS.md](AGENTS.md) - operational manual for AI and human engineering agents.
- [PROJECT_STATE.md](PROJECT_STATE.md) - current subsystem status.
- [ROADMAP.md](ROADMAP.md) - milestone plan.
- [TASKS.md](TASKS.md) - prioritized backlog.
- [KNOWN_ISSUES.md](KNOWN_ISSUES.md) - known risks and limitations.
- [DECISIONS.md](DECISIONS.md) - architectural decisions.
- [CHANGELOG.md](CHANGELOG.md) - project history.

## Documentation

- [Docs/Architecture/CODE_MAP.md](Docs/Architecture/CODE_MAP.md)
- [Docs/Architecture/CLASS_INDEX.md](Docs/Architecture/CLASS_INDEX.md)
- [Docs/Architecture/MODULE_DEPENDENCIES.md](Docs/Architecture/MODULE_DEPENDENCIES.md)
- [Docs/Architecture/SUBSYSTEMS.md](Docs/Architecture/SUBSYSTEMS.md)
- [Docs/Architecture/COMPONENTS.md](Docs/Architecture/COMPONENTS.md)
- [Docs/Architecture/GAME_FLOW.md](Docs/Architecture/GAME_FLOW.md)
- [Docs/Architecture/BUILD_PIPELINE.md](Docs/Architecture/BUILD_PIPELINE.md)
- [Docs/Architecture/MCP_CAPABILITIES.md](Docs/Architecture/MCP_CAPABILITIES.md)
- [Docs/Architecture/REPOSITORY_STRUCTURE.md](Docs/Architecture/REPOSITORY_STRUCTURE.md)
- [Docs/Gameplay/GAMEPLAY_SYSTEMS.md](Docs/Gameplay/GAMEPLAY_SYSTEMS.md)
- [Docs/Animation/ANIMATION_AND_LOCOMOTION.md](Docs/Animation/ANIMATION_AND_LOCOMOTION.md)
- [Docs/Audio/AUDIO_VFX_KNOWLEDGE.md](Docs/Audio/AUDIO_VFX_KNOWLEDGE.md)
- [Docs/World/WORLD_AND_MAPS.md](Docs/World/WORLD_AND_MAPS.md)
- [Docs/Concepts/GAME_CONCEPTS.md](Docs/Concepts/GAME_CONCEPTS.md)
- [Docs/Audits/CODE_AUDITS.md](Docs/Audits/CODE_AUDITS.md)
- [Docs/Audits/ASSET_AND_PACK_AUDITS.md](Docs/Audits/ASSET_AND_PACK_AUDITS.md)
- [Docs/Audits/REPOSITORY_ANALYSIS.md](Docs/Audits/REPOSITORY_ANALYSIS.md)
- [Docs/Worklogs/DEVELOPMENT_WORKLOG.md](Docs/Worklogs/DEVELOPMENT_WORKLOG.md)
- [Docs/Worklogs/WORKLOG.md](Docs/Worklogs/WORKLOG.md)
- [Docs/REORGANIZATION_REPORT.md](Docs/REORGANIZATION_REPORT.md)

## AI Agent Guides

- [Docs/AI/BuildRules.md](Docs/AI/BuildRules.md)
- [Docs/AI/CodingRules.md](Docs/AI/CodingRules.md)
- [Docs/AI/Workflow.md](Docs/AI/Workflow.md)
- [Docs/AI/CurrentMission.md](Docs/AI/CurrentMission.md)
- [Docs/AI/CurrentFocus.md](Docs/AI/CurrentFocus.md)
- [Docs/AI/NamingConvention.md](Docs/AI/NamingConvention.md)
- [Docs/AI/ReplicationRules.md](Docs/AI/ReplicationRules.md)
- [Docs/AI/AnimationRules.md](Docs/AI/AnimationRules.md)
- [Docs/AI/MCPGuide.md](Docs/AI/MCPGuide.md)

## Developer Workflow Prompts

- [Prompts/Continue.md](Prompts/Continue.md)
- [Prompts/Feature.md](Prompts/Feature.md)
- [Prompts/BugFix.md](Prompts/BugFix.md)
- [Prompts/Review.md](Prompts/Review.md)
- [Prompts/Build.md](Prompts/Build.md)
- [Prompts/SmokeTest.md](Prompts/SmokeTest.md)
- [Prompts/DocsUpdate.md](Prompts/DocsUpdate.md)
- [Prompts/Commit.md](Prompts/Commit.md)

Original pre-reorganization notes are preserved under `Docs/Archive/Originals/`.

## Unreal Project

- Project file: `avariika.uproject`
- Runtime module: `Source/Avaryo`
- Main content root: `Content/Avariika`
- Active map observed through MCP: `/Game/Avariika/Maps/Lvl_FirstPerson`

## Build

Close Unreal Editor before a full C++ build when Live Coding may interfere:

```powershell
"C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" avariikaEditor Win64 Development -project="C:\unrealEngine\avariika\avariika.uproject" -WaitMutex
```
