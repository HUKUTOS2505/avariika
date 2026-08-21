# MCP Guide

See [MCP_CAPABILITIES.md](../Architecture/MCP_CAPABILITIES.md) and [AGENTS.md](../../AGENTS.md).

- Use Unreal MCP whenever the editor, Blueprint, asset, material, level, or map state must change.
- If the editor is closed, launch the project, wait for load, reconnect, then continue.
- Discover tools in the active session before assuming a tool exists.
- If a named MCP tool is unavailable, search for equivalent tools and inspect schemas.
- Do not edit binary `.uasset` or `.umap` files directly.
- Save modified assets through MCP/editor workflows.
