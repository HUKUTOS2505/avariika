# StraySpark Unreal MCP Server v2 inventory

Sandbox project: `C:\unrealEngine\avariika_UE58_sandbox`

Source project was not used for build or conversion.

## Verified runtime

- Plugin folder: `Plugins\UnrealMCPServer57`
- Plugin descriptor version: `Unreal MCP Server` v2.0.2 by StraySpark.
- JSON-RPC server info: `unreal-mcp-server` v2.0.0.
- Protocol version: `2025-06-18`.
- Transport: Streamable HTTP.
- Endpoint: `http://localhost:13579/mcp`.
- Legacy SSE endpoint: `http://localhost:13579/sse`.
- Bind address observed: `127.0.0.1:13579`.
- Tool preset: `Full`.
- `tools/list` count: 304 tools.
- Resources registered in log: 15.
- Prompts registered in log: 11.

## Build and editor status

- UE 5.8 build target `avariikaEditor Win64 Development` succeeded after source compatibility fixes.
- Editor opened through UE 5.8 and loaded `UnrealEditor-UnrealMCPServer.dll` from the sandbox plugin binaries.
- Startup log reported: `MCP server ready with 304 tools on port 13579`.
- Active Codex profile was configured with MCP server `strayspark-ue58` using `http://localhost:13579/mcp`.

## Read-only checks performed

- `initialize` succeeded and returned MCP session id.
- `tools/list` succeeded and returned 304 tools.
- `get_project_info` succeeded and reported project `avariika`, engine `5.8.0-55116800+++UE5+Release-5.8`.
- `list_assets` succeeded under `/Game`; it reported 30004 assets and returned the first 5 entries.
- `get_level_info` succeeded; current map is `Lvl_FirstPerson`, package `/Game/Avariika/Maps/Lvl_FirstPerson`, with 292 actors.
- `list_assets` with `name_filter=ABP_Worker` found `/Game/Avariika/Anim/Locomotion/ABP_Worker.ABP_Worker`.
- `get_blueprint_info` on `ABP_Worker` succeeded and returned parent class `AnimInstance`, variables, and event graph structure.

No MCP tool that creates, deletes, saves, compiles, moves, imports, or edits assets was executed during this inventory.

## Project / Editor status

Observed tools include:

`get_project_info`, `list_project_modules`, `get_build_configuration`, `get_selection`, `focus_viewport`, `set_viewport_camera`, `undo`, `redo`, `run_console_command`, `execute_python`, `get_memory_report`, `get_render_stats`, `get_map_check_errors`, `search_project`, `get_engine_header`, `search_engine_api`, `search_engine_class`.

Useful now: project metadata, current selection, viewport focus/camera checks, logs through resources, memory/render readouts, and project search. `execute_python` and console execution are powerful and should stay approval-gated.

## Asset Browser and asset search

Observed tools include:

`list_assets`, `get_asset_info`, `get_asset_references`, `get_asset_size_report`, `find_unused_assets`, `validate_assets`, `import_asset`, `import_asset_with_settings`, `delete_asset`, `duplicate_asset`, `rename_asset`, `move_assets_to_folder`, `list_animation_assets`, `list_anim_assets_by_skeleton`, `list_metasound_assets`, `list_ai_assets`, `list_datatables`, `list_user_structs`, `list_3d_models`, `rebuild_search_index`.

Useful now: asset listing, reference checks, size reports, validation, and searches. Import/delete/rename/move/duplicate operations are destructive or state-changing and should not be used without a separate task.

## Actors / Level / Transform

Observed tools include:

`list_actors`, `find_actors`, `get_actor_properties`, `get_actor_bounds`, `get_actor_hierarchy`, `create_actor`, `destroy_actors`, `duplicate_actors`, `select_actors`, `set_actor_transform`, `set_actor_property`, `set_actor_mobility`, `set_actor_hidden`, `set_actor_tags`, `attach_actor`, `detach_actor`, `batch_transform`, `batch_set_property`, `align_actors`, `stack_actors`, `place_actor_on_ground`, `find_placement_position`, `measure_distance`, `line_trace`, `overlap_test`.

Level and world tools include:

`get_level_info`, `new_level`, `open_level`, `save_level`, `create_basic_level`, `get_world_settings`, `set_world_settings`, `get_world_partition_info`, `load_world_partition_region`, `get_spatial_context`.

Useful now: read actor lists/properties, level info, spatial measurements, and bounds. Actor creation, deletion, transforms, batch edits, level open/save/new-level operations are state-changing.

## Blueprint and Graph

Observed tools include:

`get_blueprint_info`, `validate_blueprint`, `compile_blueprint`, `create_blueprint`, `spawn_blueprint`, `create_blueprint_interface`, `get_blueprint_interfaces`, `add_interface_to_blueprint`, `add_component`, `set_component_property`, `add_variable`, `add_local_variable`, `add_function_graph`, `add_function_pin`, `get_function_signature`, `list_class_functions`.

Graph tools include:

`connect_pins`, `disconnect_pin`, `get_node_pins`, `remove_node`, `set_pin_default_value`, `add_event_node`, `add_custom_event`, `add_function_call_node`, `add_branch_node`, `add_cast_node`, `add_sequence_node`, `add_delay_node`, `add_for_each_loop_node`, `add_while_loop_node`, `add_select_node`, `add_switch_on_enum_node`, `add_switch_on_int_node`, `add_switch_on_string_node`, `add_make_array_node`, `add_make_struct_node`, `add_break_struct_node`, `add_set_struct_fields_node`, `add_timeline_node`, `add_event_dispatcher`, `add_bind_dispatcher_node`, `add_call_dispatcher_node`, `add_function_return_node`, `add_get_all_actors_of_class_node`, `add_spawn_actor_node`.

Widget and UI tools include:

`create_widget_blueprint`, `create_common_ui_widget`, `list_widget_blueprints`, `list_common_ui_widgets`, `get_widget_tree`, `get_widget_properties`, `add_widget`, `remove_widget`, `move_widget`, `set_widget_properties`, `set_widget_slot`, `set_widget_image`, `batch_add_widgets`, `batch_set_widget_properties`, `bind_widget_event`, `spawn_widget_component`, `set_widget_component_property`, `configure_common_button`, `set_common_ui_input_mode`, `generate_ui_image`.

Useful now: read Blueprint/AnimBP structure and validate. Compilation and graph/widget edits should be treated as asset modifications.

## Animation / AnimBP / Retarget

Observed tools include:

`list_animation_assets`, `list_anim_assets_by_skeleton`, `list_anim_notifies`, `get_anim_blueprint_info`, `get_anim_montage_info`, `get_anim_state_machine_info`, `get_skeleton_info`, `create_anim_blueprint`, `create_anim_montage`, `create_anim_state_machine`, `add_anim_state`, `add_anim_transition`, `add_anim_notify`, `create_blend_space`, `add_blend_space_sample`, `create_aim_offset`, `set_animation_blueprint`, `set_skeletal_mesh`, `play_animation`, `create_control_rig`, `get_control_rig_info`.

Useful now: inspect animation assets, AnimBP info, skeleton info, and montage/state machine metadata. Creation, assignment, montage/state edits, and playback are stateful and need a separate task.

## Materials / VFX / Lighting

Observed material and mesh tools include:

`create_material`, `create_material_instance`, `compile_material`, `assign_material`, `get_material_expressions`, `add_material_expression`, `add_material_parameter_expression`, `add_texture_sample_expression`, `connect_material_expression`, `remove_material_expression`, `set_material_expression_value`, `set_material_scalar`, `set_material_vector`, `get_static_mesh_info`, `get_mesh_asset_bounds`, `get_mesh_complexity_report`, `set_static_mesh`, `create_static_mesh_actor`, `configure_mesh_lod`, `set_mesh_material_slots`, `enable_nanite`, `set_texture_settings`.

VFX, lighting, audio, and physics tools include:

`spawn_niagara_system`, `get_niagara_parameters`, `set_niagara_parameter`, `create_light_rig`, `set_light_properties`, `set_sky_atmosphere`, `set_fog_settings`, `set_post_process_settings`, `build_lighting`, `create_metasound_source`, `duplicate_metasound`, `get_metasound_info`, `set_metasound_parameter`, `set_metasound_quality`, `spawn_sound`, `set_audio_properties`, `get_sound_info`, `create_physics_material`, `assign_physics_material`, `get_physics_material_info`, `set_physics_simulation`, `set_collision_profile`, `set_collision_response`, `add_physics_constraint`, `list_collision_channels`, `get_physics_info`.

Useful now: inspect mesh/material/VFX/audio/physics metadata. Assignments, creation, compilation, lighting build, and parameter changes are state-changing.

## Build / Compile / Save

Observed tools include:

`compile_blueprint`, `validate_blueprint`, `compile_material`, `validate_assets`, `build_lighting`, `build_navigation`, `get_build_configuration`, `get_lighting_build_info`, `save_level`, `rebuild_search_index`.

Useful now: read build configuration and lighting build info. Compile/save/build tools can modify generated or asset state and should be run only under an explicit task.

## PIE / Screenshot / Logs

Observed tools include:

`take_screenshot`, `play_animation`, `play_sequence`, `open_sequence`, `add_actor_to_sequence`, `add_sequence_track`, `add_keyframe`, `add_camera_cut_track`, `add_fade_track`, `add_audio_track`, `set_sequence_range`, `get_sequence_info`.

No dedicated `start_pie` or `stop_pie` tool was present in `tools/list`. Screenshots and Sequencer playback are available; PIE was not launched during this check.

Log/status data is available through registered MCP resources including:

`unreal://project/info`, `unreal://level/current`, `unreal://assets/summary`, `unreal://editor/log`, `unreal://editor/selection`, `unreal://editor/performance`, `unreal://project/settings`, `unreal://project/plugins`, `unreal://editor/viewport`, `unreal://level/lighting`, `unreal://assets/recent`, `unreal://level/bounds`, `unreal://level/analysis`, `unreal://project/capabilities`, `unreal://editor/history`.

## Dangerous or destructive operations

Treat these tool families as dangerous or state-changing:

- Asset modification: `import_asset`, `import_asset_with_settings`, `delete_asset`, `duplicate_asset`, `rename_asset`, `move_assets_to_folder`, `rebuild_search_index`.
- Actor and level modification: `create_actor`, `destroy_actors`, `duplicate_actors`, `set_actor_transform`, `set_actor_property`, `attach_actor`, `detach_actor`, `new_level`, `open_level`, `save_level`.
- Blueprint and graph modification: `create_blueprint`, `spawn_blueprint`, `add_component`, `set_component_property`, `add_variable`, `connect_pins`, `disconnect_pin`, `remove_node`, `compile_blueprint`, widget creation/edit tools.
- Animation modification: `create_anim_blueprint`, `create_anim_montage`, `add_anim_state`, `add_anim_transition`, `add_anim_notify`, `set_animation_blueprint`, `set_skeletal_mesh`, `play_animation`.
- Rendering and content generation: material creation/edit tools, Niagara spawn/edit tools, lighting build, mesh LOD/Nanite tools, `generate_3d_model`, `image_to_3d_model`, `generate_ui_image`, `remove_background`.
- Project/system execution: `execute_python`, `run_console_command`, build/navigation/lighting tools, any save tool.

## Full tool name inventory

```text
add_ability_component
add_action_mapping
add_actor_to_sequence
add_anim_notify
add_anim_state
add_anim_transition
add_audio_track
add_bind_dispatcher_node
add_blackboard_key
add_blend_space_sample
add_branch_node
add_break_struct_node
add_call_dispatcher_node
add_camera_cut_track
add_cast_node
add_component
add_create_widget_node
add_custom_event
add_datatable_row
add_delay_node
add_event_dispatcher
add_event_node
add_fade_track
add_flow_control_node
add_foliage_type
add_for_each_loop_node
add_function_call_node
add_function_graph
add_function_pin
add_function_return_node
add_gameplay_tags
add_get_all_actors_of_class_node
add_input_action_event
add_interface_to_blueprint
add_keyframe
add_local_variable
add_make_array_node
add_make_struct_node
add_material_expression
add_material_parameter_expression
add_pcg_node
add_physics_constraint
add_select_node
add_sequence_node
add_sequence_track
add_set_struct_fields_node
add_spawn_actor_node
add_spline_point
add_state_tree_state
add_sub_sequence
add_switch_on_enum_node
add_switch_on_int_node
add_switch_on_string_node
add_texture_sample_expression
add_timeline_node
add_variable
add_variable_get_node
add_variable_set_node
add_while_loop_node
add_widget
align_actors
assign_material
assign_physics_material
attach_actor
batch_add_widgets
batch_set_property
batch_set_widget_properties
batch_transform
bind_widget_event
build_lighting
build_navigation
compile_blueprint
compile_material
configure_common_button
configure_mesh_lod
connect_material_expression
connect_pcg_nodes
connect_pins
create_actor
create_aim_offset
create_anim_blueprint
create_anim_montage
create_anim_state_machine
create_attribute_set
create_basic_level
create_behavior_tree
create_blackboard
create_blend_space
create_blueprint
create_blueprint_interface
create_common_ui_widget
create_control_rig
create_enum
create_eqs_query
create_folder
create_game_mode
create_game_state
create_gameplay_ability
create_gameplay_effect
create_grid_layout
create_hud
create_input_action
create_input_mapping_context
create_landscape
create_level_sequence
create_light_rig
create_material
create_material_instance
create_metasound_source
create_pcg_graph
create_physics_material
create_player_controller
create_player_state
create_ring_layout
create_scene_from_template
create_spline_actor
create_staircase
create_state_tree
create_static_mesh_actor
create_trigger_volume
create_user_struct
create_widget_blueprint
delete_asset
destroy_actors
detach_actor
disconnect_pin
duplicate_actors
duplicate_asset
duplicate_metasound
enable_nanite
erase_foliage
execute_pcg
execute_python
find_actors
find_placement_position
find_unused_assets
focus_viewport
generate_3d_model
generate_ui_image
get_actor_bounds
get_actor_hierarchy
get_actor_properties
get_anim_blueprint_info
get_anim_montage_info
get_anim_state_machine_info
get_asset_info
get_asset_references
get_asset_size_report
get_behavior_tree_info
get_blackboard_info
get_blueprint_info
get_blueprint_interfaces
get_build_configuration
get_component_replication
get_control_rig_info
get_datatable_rows
get_engine_header
get_foliage_stats
get_function_signature
get_game_framework_info
get_gas_info
get_input_mapping_info
get_landscape_info
get_level_info
get_lighting_build_info
get_map_check_errors
get_material_expressions
get_memory_report
get_mesh_asset_bounds
get_mesh_complexity_report
get_metasound_info
get_navigation_info
get_niagara_parameters
get_node_pins
get_pcg_graph_nodes
get_pcg_info
get_physics_info
get_physics_material_info
get_project_info
get_render_stats
get_replication_info
get_selection
get_sequence_info
get_skeleton_info
get_sound_info
get_spatial_context
get_spline_info
get_state_tree_info
get_static_mesh_info
get_struct_info
get_widget_properties
get_widget_tree
get_world_partition_info
get_world_settings
image_to_3d_model
import_asset
import_asset_with_settings
line_trace
list_3d_models
list_actors
list_ai_assets
list_anim_assets_by_skeleton
list_anim_notifies
list_animation_assets
list_assets
list_attribute_sets
list_class_functions
list_collision_channels
list_common_ui_widgets
list_datatables
list_gameplay_abilities
list_gameplay_effects
list_gameplay_tags
list_input_actions
list_input_mapping_contexts
list_metasound_assets
list_pcg_graphs
list_project_modules
list_state_trees
list_user_structs
list_widget_blueprints
load_world_partition_region
measure_distance
move_assets_to_folder
move_widget
new_level
open_level
open_sequence
overlap_test
paint_foliage
place_actor_on_ground
play_animation
play_sequence
profile_actors_in_view
query_navigation_path
rebuild_search_index
redo
remove_background
remove_material_expression
remove_node
remove_spline_point
remove_widget
rename_asset
run_console_command
save_level
search_engine_api
search_engine_class
search_project
select_actors
set_actor_gameplay_tags
set_actor_hidden
set_actor_mobility
set_actor_property
set_actor_tags
set_actor_transform
set_animation_blueprint
set_audio_properties
set_bt_blackboard
set_collision_profile
set_collision_response
set_common_ui_input_mode
set_component_property
set_component_replication
set_fog_settings
set_landscape_material
set_light_properties
set_material_expression_value
set_material_scalar
set_material_vector
set_mesh_material_slots
set_metasound_parameter
set_metasound_quality
set_net_dormancy
set_niagara_parameter
set_pcg_static_mesh_spawner_meshes
set_physics_simulation
set_pin_default_value
set_post_process_settings
set_replication_settings
set_sequence_range
set_skeletal_mesh
set_sky_atmosphere
set_spline_closed
set_spline_point
set_spline_type
set_state_tree_evaluator
set_static_mesh
set_texture_settings
set_viewport_camera
set_widget_component_property
set_widget_image
set_widget_properties
set_widget_slot
set_world_settings
spawn_blueprint
spawn_niagara_system
spawn_pcg_actor
spawn_sound
spawn_widget_component
stack_actors
take_screenshot
undo
validate_assets
validate_blueprint
```
