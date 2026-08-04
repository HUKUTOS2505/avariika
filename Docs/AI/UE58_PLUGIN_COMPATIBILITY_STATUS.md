# UE 5.8 Plugin Compatibility Status

Workspace: `C:\unrealEngine\avariika_UE58_sandbox`  
Source project `C:\unrealEngine\avariika` is out of scope and must not be modified.

Status values:

- `KEEP_ENABLED` - keep enabled in sandbox.
- `FIX_AND_ENABLE` - has source and should be adapted/verified before keeping enabled.
- `DISABLE_SANDBOX` - disable only in sandbox; do not delete plugin folder.
- `WAIT_FOR_OFFICIAL_UE58_PACKAGE` - leave disabled until a UE 5.8 vendor package is available.
- `BLOCKED_NEEDS_USER_DECISION` - risk/content/runtime dependency needs explicit user decision.

## Current Summary

- Confirmed working and protected: `BlueprintAssist`, `LDA2`, `GraphMinimap`, `ElectronicNodes`, `UnrealMCPServer` / StraySpark v2.
- StraySpark remains the primary MCP endpoint: `http://127.0.0.1:13579/mcp`.
- Do not connect/use: `NwiroIntegrationKit`, `Claudius`, old NVIDIA DLSS/NIS/Streamline stack, Epic Unreal MCP Codex endpoint.
- `meshy` was enabled only by sandbox `.uproject`, has no detected project C++/Config dependency, and is disabled in sandbox to remove the UE 5.7 build dialog.

## Plugin Matrix

| Plugin | Path | Type | EngineVersion | Enabled in sandbox | Source | Old Binaries/Intermediate | Detected dependency | Status | Notes |
|---|---|---:|---:|---:|---:|---:|---|---|---|
| AudioToolkitPro | `Plugins/AudioToolkitPro` | Editor | empty | yes | yes | yes | `.uproject` only | `KEEP_ENABLED` | Editor audio tool; keep unless launch/build reports module error. |
| BlockoutToolsPlugin | `Plugins/BlockoutToolsPlugin` | Mixed Runtime+Editor | 5.8.0 | yes | yes | yes | `.uproject`; plugin self-content references | `KEEP_ENABLED` | Descriptor updated after successful UE 5.8 build and observed 5.7 dialog. |
| BlueprintAssist | `Plugins/BlueprintAssist` | Editor | 5.8.0 | yes | yes | yes | `.uproject` | `KEEP_ENABLED` | Confirmed working. |
| DLSS | `Plugins/DLSS` | Mixed Runtime+Editor | 5.7.0 | no | yes | yes | `.uproject` disabled; UI/content mentions DLSS options | `WAIT_FOR_OFFICIAL_UE58_PACKAGE` | Keep disabled until official UE 5.8 NVIDIA package. |
| ElectronicNodes | `Plugins/ElectronicNodes` | Editor | 5.8.0 | yes | yes | yes | `.uproject` | `KEEP_ENABLED` | Confirmed working. |
| EOSCore | `Plugins/EOSCore` | Runtime | 5.8.0 | yes | yes | yes | `.uproject`; `Config/DefaultEngine.ini` EOSCore sections | `KEEP_ENABLED` | Descriptor updated after successful UE 5.8 build and observed 5.7 dialog. |
| GraphMinimap | `Plugins/GraphMinimap` | Editor | 5.8.0 | yes | yes | yes | `.uproject` | `KEEP_ENABLED` | Confirmed working. |
| Imperfecter | `Plugins/Imperfecter` | Mixed Runtime+Editor | 5.8.0 | yes | yes | yes | `.uproject`; plugin content | `KEEP_ENABLED` | Descriptor updated after successful UE 5.8 build and observed 5.7 dialog. |
| LDA2 | `Plugins/LDA2` | Editor | 5.8.0 | yes | yes | yes | `.uproject`; plugin content | `KEEP_ENABLED` | Confirmed working. |
| meshy | `Plugins/meshy` | Editor | 5.7.0 | no | yes | yes | `.uproject` only; StraySpark `meshy-v6` strings are not this plugin | `DISABLE_SANDBOX` | Disabled in sandbox on 2026-06-30; plugin folder preserved. |
| n00dEmotes | `Plugins/n00dEmotes` | Runtime | 5.8.0 | yes | yes | yes | `.uproject`; plugin content | `KEEP_ENABLED` | Descriptor updated after successful UE 5.8 build and observed 5.7 dialog. |
| Narrative | `Plugins/Narrative` | Mixed Runtime+UncookedOnly | 5.8.0 | yes | yes | yes | `.uproject`; plugin content; known editor widget compile mismatch | `KEEP_ENABLED` | Descriptor updated after successful UE 5.8 build; known `WBP_DefaultQuestNode` editor-widget compiler issue is asset-side and not edited here. |
| NIS | `Plugins/NIS` | Runtime | 5.7.0 | no | yes | yes | `.uproject` disabled; UI/content mentions options | `WAIT_FOR_OFFICIAL_UE58_PACKAGE` | Keep disabled with NVIDIA stack. |
| NwiroIntegrationKit | `Plugins/NwiroIntegrationKit` | Editor | 5.7.0 | no `.uproject` entry | yes | yes | no active dependency found | `DISABLE_SANDBOX` | Do not use. |
| RuntimeAudioImporter | `Plugins/RuntimeAudioImporter` | Mixed Runtime+Editor | 5.8.0 | yes | yes | yes | `.uproject` | `KEEP_ENABLED` | Descriptor updated after successful UE 5.8 build and observed 5.7 dialog. |
| RuntimeSpeechRecognizer | `Plugins/RuntimeSpeechRecognizer` | Mixed Runtime+Editor | 5.8.0 | yes | yes | yes | `.uproject`; plugin content | `KEEP_ENABLED` | Descriptor updated after successful UE 5.8 build and observed 5.7 dialog. |
| RuntimeTextToSpeech | `Plugins/RuntimeTextToSpeech` | Mixed Runtime+Editor | 5.8.0 | yes | yes | yes | `.uproject`; `Config/DefaultGame.ini` cook dirs | `KEEP_ENABLED` | Descriptor updated after successful UE 5.8 build; has explicit cook directory dependency. |
| StageZero | `Plugins/StageZero` | Mixed Runtime+Editor | 5.8.0 | yes | yes | yes | `.uproject`; plugin content | `KEEP_ENABLED` | Descriptor updated after successful UE 5.8 build and observed 5.7 dialog. |
| StreamlineCore | `Plugins/StreamlineCore` | Runtime | 5.7.0 | no | yes | yes | `.uproject` disabled; scalability comments only | `WAIT_FOR_OFFICIAL_UE58_PACKAGE` | Keep disabled with NVIDIA stack. |
| StreamlineDeepDVC | `Plugins/StreamlineDeepDVC` | Runtime | 5.7.0 | no | yes | yes | `.uproject` disabled | `WAIT_FOR_OFFICIAL_UE58_PACKAGE` | Keep disabled with NVIDIA stack. |
| StreamlineDLSSG | `Plugins/StreamlineDLSSG` | Runtime | 5.7.0 | no | yes | yes | `.uproject` disabled | `WAIT_FOR_OFFICIAL_UE58_PACKAGE` | Keep disabled with NVIDIA stack. |
| StreamlineNGXCommon | `Plugins/StreamlineNGXCommon` | Runtime | 5.7.0 | no | yes | yes | `.uproject` disabled | `WAIT_FOR_OFFICIAL_UE58_PACKAGE` | Keep disabled with NVIDIA stack. |
| StreamlineReflex | `Plugins/StreamlineReflex` | Runtime | 5.7.0 | no | yes | yes | `.uproject` disabled | `WAIT_FOR_OFFICIAL_UE58_PACKAGE` | Keep disabled with NVIDIA stack. |
| TickOptToolkit | `Plugins/TickOptToolkit` | Mixed Runtime+Editor | 5.8.0 | yes | yes | yes | `.uproject`; runtime Blueprint classes possible | `KEEP_ENABLED` | Descriptor updated after successful UE 5.8 build and observed 5.7 dialog. |
| TriangleCountViewMode | `Plugins/TriangleCountViewMode` | Editor | 5.8.0 | yes | yes | yes | `.uproject`; plugin content | `KEEP_ENABLED` | Descriptor updated after successful UE 5.8 build and observed 5.7 dialog. |
| UltimateLevelArtTool | `Plugins/UltimateLevelArtTool` | Mixed Editor+Runtime | 5.8.0 | yes | yes | yes | `.uproject`; many plugin self-content soft paths | `KEEP_ENABLED` | Descriptor updated after successful UE 5.8 build and observed 5.7 dialog. |
| UniversalVoiceChatPro | `Plugins/UniversalVoiceChatPro` | Runtime | 5.8.0 | yes | yes | yes | `.uproject` | `KEEP_ENABLED` | Descriptor updated after successful UE 5.8 build and observed 5.7 dialog. |
| UnrealMCPServer | `Plugins/UnrealMCPServer57` | Editor | 5.8.0 | yes | yes | yes | `.uproject` | `KEEP_ENABLED` | StraySpark v2 primary MCP server. |
| UnrealMonsterAssetInstaller | `Plugins/UnrealMonsterAssetInstaller` | Editor | empty | yes | yes | yes | `.uproject` only | `KEEP_ENABLED` | Editor installer; do not implement Listener Monster. |
| WorldBLD | `Plugins/WorldBLD` | Mixed Runtime+Editor | 5.8.0 | yes | yes | yes | `.uproject`; plugin content | `KEEP_ENABLED` | Descriptor updated after successful UE 5.8 build and observed 5.7 dialog. |

## Read-Only Audit Notes

- Scanned sandbox `.uproject`, `Config`, local plugin descriptors, plugin `Source`, `Binaries`, and `Intermediate` presence.
- Text/binary Content search was used only as a dependency signal. A targeted read-only search for explicit mount-path strings such as `/meshy/`, `/Narrative/`, `/RuntimeTextToSpeech/`, `/DLSS/`, `/StreamlineCore/` in `Content/` returned no direct matches; binary `.uasset` class dependencies can still require Asset Registry validation before disabling runtime/content plugins.
- No source project files under `C:\unrealEngine\avariika` were edited.
