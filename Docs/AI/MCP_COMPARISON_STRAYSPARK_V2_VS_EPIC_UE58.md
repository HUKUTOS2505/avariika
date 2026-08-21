# MCP Comparison: StraySpark UnrealMCPServer v2 vs Epic Unreal MCP UE 5.8

Date: 2026-06-30

Project: `C:\unrealEngine\avariika_UE58_sandbox`

Scope: read-only comparison. No build, PIE, asset editing, Blueprint editing, map editing, `.uproject` editing, plugin editing, C++ editing, or config editing inside the project was performed.

## Runtime State Checked

| Item | Result |
|---|---|
| Unreal Editor | UE 5.8 sandbox restarted with `-ModelContextProtocolStartServer -ModelContextProtocolPort=8000` |
| StraySpark endpoint | `http://127.0.0.1:13579/mcp` |
| Epic endpoint | `http://127.0.0.1:8000/mcp` |
| Port listeners | Both `127.0.0.1:13579` and `127.0.0.1:8000` listened in the same `UnrealEditor.exe` process |
| Project plugin state | `UnrealMCPServer` enabled, `ModelContextProtocol` enabled, `Claudius` disabled |
| Nwiro | No enabled `Nwiro` entry found in `avariika.uproject` |
| Codex config | Temporary `[mcp_servers.epic-unreal-mcp]` entry was added for the comparison and must be removed after the test |

## Server And Transport

| Area | StraySpark `UnrealMCPServer` v2.0.2 | Epic `ModelContextProtocol` UE 5.8 |
|---|---|---|
| Descriptor/source location | Project plugin: `Plugins\UnrealMCPServer57\UnrealMCPServer.uplugin` | Engine plugin: `Engine\Plugins\Experimental\ModelContextProtocol\ModelContextProtocol.uplugin` |
| Transport | Streamable HTTP MCP | Streamable HTTP MCP |
| Endpoint | `http://127.0.0.1:13579/mcp` | `http://127.0.0.1:8000/mcp` |
| Legacy SSE | Present: `http://localhost:13579/sse` | Separate SSE GET endpoint not supported; GET `/mcp` returns bad method |
| `initialize` | Succeeded | Succeeded |
| Negotiated protocol | `2025-06-18` | `2025-06-18` |
| `serverInfo` observed | `unreal-mcp-server` v2.0.0 | Empty name/title/version in response |
| Autostart behavior | Starts from StraySpark plugin startup on configured port | Does not autostart by default; starts with `-ModelContextProtocolStartServer` or settings |
| Port conflict risk | Uses `13579` | Default `8000`; safe in parallel only when not set to `13579` |

## Tool Inventory Summary

| Area | StraySpark | Epic |
|---|---:|---:|
| Native `tools/list` count | 304 | 3 |
| Native tools | Full Unreal editor/content automation surface | `list_toolsets`, `describe_toolset`, `call_tool` |
| Discoverable toolsets | Built directly into native tool list | `ToolsetRegistry.AgentSkillToolset` only |
| Epic underlying toolset tools observed | N/A | 4 AgentSkill tools: `ListSkills`, `GetSkills`, `CreateSkill`, `UpdateSkill` |
| Unreal project/asset/level tools in current Epic setup | N/A | Not exposed |

StraySpark category counts from the captured `tools/list` inventory:

| Category | Count | Examples |
|---|---:|---|
| Actor, level, world, viewport, spatial | 44 | `list_actors`, `get_level_info`, `get_world_settings`, `line_trace`, `measure_distance` |
| Asset and project search/read | 27 | `list_assets`, `get_asset_info`, `get_asset_references`, `search_project` |
| Blueprint, graph, widgets | 73 | `get_blueprint_info`, `validate_blueprint`, `get_node_pins`, widget read/edit tools |
| Animation, AnimBP, skeleton, retarget-adjacent | 20 | `get_anim_blueprint_info`, `get_anim_state_machine_info`, `get_skeleton_info`, `create_anim_montage` |
| Material, mesh, texture | 28 | `get_material_expressions`, `get_static_mesh_info`, `get_mesh_asset_bounds` |
| Audio, VFX, MetaSound, Niagara | 13 | `list_metasound_assets`, `get_sound_info`, `get_niagara_parameters` |
| Build, save, compile, validation | 11 | `get_build_configuration`, `get_map_check_errors`, `validate_assets`, `save_level` |
| Screenshot/playback availability | 12 | `take_screenshot`, `play_sequence`, `play_animation` |
| Mutating or destructive by name heuristic | 200 | create/delete/set/add/remove/import/build/save/compile/open/execute families |

## Read-Only Probe Results

| Probe | StraySpark | Epic |
|---|---|---|
| Project info | Works. Reported project `avariika`, engine `5.8.0-55116800+++UE5+Release-5.8`, sandbox paths, modules, plugins | Not available as current Epic tool |
| Asset list | Works. `/Game` reported `30004` assets | Not available as current Epic tool |
| Asset search | `list_assets` with `name_filter` works. `search_project` exists but current index returned no `ABP_Worker` result without rebuilding index | Not available as current Epic tool |
| Level info | Works. Current map `Lvl_FirstPerson`, package `/Game/Avariika/Maps/Lvl_FirstPerson`, actor count `292`, game mode `BP_AvaryoGameMode_C` | Not available as current Epic tool |
| Actor list | Works. `list_actors limit=5` returned current level actors | Not available as current Epic tool |
| Blueprint inspection | Works. `get_blueprint_info` on `ABP_Worker` returned parent class `AnimInstance` and variables | Not available as current Epic tool |
| AnimBP inspection | Works. `get_anim_blueprint_info` returned target skeleton `SK_Mannequin`, graphs, state machine, slot groups, sync groups, compiled state | Not available as current Epic tool |
| Anim state machine inspection | Works. `get_anim_state_machine_info` returned `Locomotion` state machine, states, transitions | Not available as current Epic tool |
| Logs/editor status | StraySpark exposes project log path in `get_project_info`; previous inventory showed MCP resources such as `unreal://editor/log`, but this run did not call resources/read | Epic current tools do not expose editor logs/status except generic MCP/session behavior |
| Screenshot availability | Tool exists: `take_screenshot`. It was not called | No screenshot tool exposed |
| PIE availability | No dedicated `start_pie`/`stop_pie` tool in StraySpark `tools/list`; play-like tools are Sequencer/animation/GAS naming, not PIE launch | No PIE tool exposed |

## Blueprint And AnimBP

| Capability | StraySpark | Epic |
|---|---|---|
| Blueprint read | Yes: `get_blueprint_info`, interfaces, function signatures, node pins | No Unreal Blueprint tools exposed |
| Blueprint validation | Yes: `validate_blueprint` | No |
| Blueprint mutation | Yes, many create/add/connect/remove/compile tools exist; avoid unless explicitly requested | No Unreal Blueprint mutation tools exposed |
| AnimBP read | Yes: `get_anim_blueprint_info`, `get_anim_state_machine_info`, `get_anim_montage_info`, skeleton info | No Unreal animation tools exposed |
| AnimBP mutation | Yes, create/set/add animation tools exist; avoid unless explicitly requested | No Unreal animation tools exposed |

## Level, Actor, Asset, Retarget, Logs

| Area | StraySpark | Epic |
|---|---|---|
| Level/actor read | Strong: level info, actor lists, actor properties, bounds, hierarchy, traces, overlaps | Not exposed |
| Level/actor mutation | Present and dangerous: create/destroy/set/attach/open/save tools | Not exposed |
| Asset search/read | Strong: list, info, references, size reports, validation, class-specific lists | Not exposed |
| Asset mutation | Present and dangerous: import/delete/duplicate/rename/move/rebuild index | Not exposed |
| Retarget/animation | Good inspection coverage for AnimBP/skeleton/montage/state machine; creation tools exist but should be gated | Not exposed |
| Build/save/logs | Read build config/map check exists; save/compile/build tools also exist and should be gated | Not exposed as Unreal workflow tools |
| Screenshot/PIE | Screenshot tool exists; no dedicated PIE start/stop in inventory | Not exposed |

## Destructive Tool Risk

StraySpark is much more useful but also much more dangerous. Its 304-tool surface includes many state-changing operations:

- Asset mutation: `import_asset`, `delete_asset`, `duplicate_asset`, `rename_asset`, `move_assets_to_folder`, `rebuild_search_index`.
- Actor/level mutation: `create_actor`, `destroy_actors`, `set_actor_transform`, `open_level`, `new_level`, `save_level`.
- Blueprint/graph mutation: `create_blueprint`, `add_component`, `connect_pins`, `remove_node`, `compile_blueprint`.
- Animation/material/VFX mutation: create/set/assign/play/build families.
- Execution escape hatches: `execute_python`, `run_console_command`.

Epic in this run exposed only AgentSkill tools. Two of those are mutating: `CreateSkill` and `UpdateSkill`; they are not useful for Avariika Unreal project inspection.

## What Is Useful For Avariika Now

| Need | Better MCP |
|---|---|
| Inspect active map, actors, bounds, world settings | StraySpark |
| Inspect `/Game` assets and references | StraySpark |
| Inspect `ABP_Worker`, AnimBP state machine, skeleton path | StraySpark |
| Read project metadata and plugin states | StraySpark |
| Compare tool availability without touching assets | StraySpark plus direct HTTP `tools/list` |
| Use Epic MCP for Avariika editor automation today | Not useful in current setup; it only exposes AgentSkill toolset |

## Missing Or Weak

| MCP | Missing / Weak |
|---|---|
| StraySpark | Large destructive surface requires strict tool gating; no dedicated PIE start/stop tool observed; `search_project` can be stale unless index is rebuilt, and rebuild is state-changing; no direct native logs tool in `tools/list`, though log resources were documented in prior inventory |
| Epic | No Unreal project info, level info, actor list, asset list/search, Blueprint/AnimBP inspection, animation/retarget, screenshot, PIE, logs, or build/save tools exposed in the current UE 5.8 setup; `serverInfo` response was empty |

## Recommendation

Keep StraySpark as the active Avariika MCP for now. Epic Unreal MCP can run in parallel on `8000`, but in this installed UE 5.8 sandbox it currently exposes only the ToolsetRegistry AgentSkill toolset, not the Unreal editor/content tool surface needed for Avariika work.

For future Epic MCP testing, only retry if Epic/UE exposes an actual Unreal editor toolset through ToolsetRegistry or if `bEnableToolSearch=false` plus a known Unreal tool library changes `tools/list`. Keep Epic on a non-StraySpark port such as `8000`.
