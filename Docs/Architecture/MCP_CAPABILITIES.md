# MCP Capabilities

Canonical project context: [PROJECT_BRAIN.md](../../PROJECT_BRAIN.md).

This project uses Unreal MCP when work requires the running Unreal Editor.

## Catalog State

Earlier MCP inspection in this repository showed that the direct catalog tools requested by name, such as `list_tool_categories` and `search_tools`, were not always exposed as callable tools in the active Codex session. In that case, use available tool discovery and schema lookup equivalents before falling back to shell/editor automation.

Catalog mode should therefore be treated as environment-dependent: confirm it at the start of an editor-automation task.

## Useful Capability Areas

| Area | Typical MCP Use |
|---|---|
| Editor/world state | Get current level/world, selected actors, viewport state, screenshots. |
| Actors | Query, spawn, move, select, inspect components and transforms. |
| Blueprints | Inspect Blueprint metadata, compile Blueprints, create or adjust Blueprint assets when tools are available. |
| Materials | Inspect or edit material graphs when material tools are available. |
| Levels | Load/save levels, inspect actors, place test actors, validate maps. |
| Assets | Search assets, inspect metadata, check references. |
| Testing | Run console commands, PIE flows, screenshots, map smoke checks. |

## Recommended Workflow

1. Confirm the editor is running and connected.
2. If not connected, launch the project and reconnect MCP.
3. Discover available MCP tools for the active session.
4. Use MCP for editor, asset, Blueprint, and map changes.
5. Use filesystem tools only for text files and C++/script/doc changes.
6. Save assets through MCP/editor commands after asset edits.
7. Validate with a focused editor smoke test.

## Documentation-Only Boundary

For this knowledge-base task, MCP was not required because no editor state, Blueprint edits, asset edits, or map changes were requested. Content was analyzed only through file metadata and existing documentation.

## Known Limitations

- Tool availability can differ between sessions.
- Binary assets should not be edited manually.
- MCP should be preferred for Blueprint/content changes, but C++ and Markdown are still best handled through normal source editing.
- Always verify map/Blueprint changes with editor-side compile/save operations.
