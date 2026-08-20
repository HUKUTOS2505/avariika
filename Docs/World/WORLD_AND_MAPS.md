# World and Maps

Canonical project context: [PROJECT_BRAIN.md](../../PROJECT_BRAIN.md). Historical world-design material is preserved under [Docs/Archive/Originals/World](../Archive/Originals/World/).

## Current Configuration Snapshot

- Project engine association: UE 5.8.
- Editor startup map: `/Game/Avariika/Maps/Lvl_FirstPerson`.
- Game default map: `/Game/Avariika/Maps/MainMenu/L_MainMenu`.
- Global default game mode: `/Game/Avariika/blueprinsTest/BP_Gamemode`.
- The last recorded editor inspection describes `Lvl_FirstPerson` as the active gameplay prototype using `BP_AvaryoGameMode_C`; that is historical inspection evidence, not a claim about the current open editor session.

## World Direction

The near-term playable-object direction is a compact repair incident: diagnosis, a correctly ordered repair chain, hazards, and team extraction. The project uses darkness, rain/fog, power tension, sound, and system-driven scares before any monster work. The Listener Monster remains explicitly deferred.

House/Dom is the main prototype direction. Factory and hospital remain documented candidate or parallel locations; they are not asserted as production-ready maps by this document.

## Working Rules

- Treat `Lvl_FirstPerson` as the gameplay prototype and `L_MainMenu` as the game entry map until an editor-side change is verified.
- Use Unreal MCP/editor workflows for inspecting or changing maps, actors, materials, Blueprints, and assets.
- Never edit `.umap` or `.uasset` files through the filesystem.
- Keep detailed historical layouts and unverified proposals in the archive; update `PROJECT_STATE.md` when a map gains verified production status.
