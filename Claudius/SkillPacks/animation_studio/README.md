# animation_studio (skill pack)

Studio helpers for shot-based animation work in Unreal: technical explainers,
forensic reconstructions, historical recreations.

## Skills

- **batch_ingest_fbx** — import a folder of FBX/USD/OBJ into a single
  destination path with consistent settings. Returns a list of imported asset
  paths and any files that were skipped.
- **setup_camera_rig** — spawn a `CineCameraActor` aimed at a target actor or
  location with sensible defaults (35 mm focal length, manual focus on the
  target). Tunable radius / height / focal length.
- **exploded_view** — push a list of actors outward along their pivot-to-center
  vector by a configurable factor. Returns from/to transforms so a Sequencer
  step can keyframe them.
- **render_shot** — queue a Level Sequence in the Movie Render Queue, optionally
  with a named `MoviePipelinePrimaryConfig` preset. Falls back to PNG sequence
  with 2× spatial samples if no preset is given. Output lands in
  `{Project}/Saved/MovieRenders/<sequence>/`.
- **material_swap_reveal** — swap a material on a list of `StaticMeshActor` /
  `SkeletalMeshActor` actors at a given material index. Useful for cross-section
  reveals or branded material switches mid-shot.

## Status

This pack is shipped at v0.1.0 — useful as scaffolding, but **not yet
production-hardened**. Expect to wrap each skill in a thin per-studio variant
once you've used it on a real shot.

The `animation_studio` workflow preset enables this pack along with the
Sequencer, Recording, Viewport, Modeling, Control Rig, Live Link, and
SourceControl categories.
