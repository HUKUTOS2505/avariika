# Modular Police Neutral Candidates Validation

## Scope

Read-only validation target: only these 10 neutral candidates from `C:\unrealEngine\avariika_UE58_sandbox\плагины\Modular_Police`.

| # | Asset Name | Expected Unreal Asset Path |
|---:|---|---|
| 1 | `SKM_Glasses_Classic` | `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Classic/SKM_Glasses_Classic` |
| 2 | `SKM_Glasses_SunGlass` | `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Classic/SKM_Glasses_SunGlass` |
| 3 | `SKM_Glasses_Combat_Glass` | `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Combat/SKM_Glasses_Combat_Glass` |
| 4 | `SKM_Glasses_Combat_Orange` | `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Combat/SKM_Glasses_Combat_Orange` |
| 5 | `SKM_Goggles_Black` | `/Game/Modular_Police/Mesh/Male/Face_Modules/Goggles/SKM_Goggles_Black` |
| 6 | `SKM_Goggles_Bege` | `/Game/Modular_Police/Mesh/Male/Face_Modules/Goggles/SKM_Goggles_Bege` |
| 7 | `SKM_Headphones_Micro_Black` | `/Game/Modular_Police/Mesh/Male/Head_Modules/Headphones_Micro/SKM_Headphones_Micro_Black` |
| 8 | `SKM_Headphones_Micro_Gray` | `/Game/Modular_Police/Mesh/Male/Head_Modules/Headphones_Micro/SKM_Headphones_Micro_Gray` |
| 9 | `SKM_Bag_Hip_Black` | `/Game/Modular_Police/Mesh/Male/Hips_Modules/Bag_Hip/SKM_Bag_Hip_Black` |
| 10 | `SKM_Belt_Tactical_Black` | `/Game/Modular_Police/Mesh/Male/Belts_Modules/Belt_Tactical/SKM_Belt_Tactical_Black` |

This pass did not connect anything to UI, did not touch Worker customization code, did not touch Apply/save/reopen/randomize/framing, did not copy assets, did not edit `.uasset`/`.umap`, did not save Unreal assets, and did not run a build.

## Validation Method

Editor/MCP validation attempted:

- Unreal Editor was already open on the sandbox project and MCP port `127.0.0.1:13579` responded.
- Direct MCP `get_skeleton_info` on `/Game/Modular_Police/.../SKM_Glasses_Classic` failed: the asset could not be loaded.
- MCP `execute_python` checked all 10 `/Game/Modular_Police/...` paths through `EditorAssetLibrary` / Asset Registry; all returned not found because `/Game/Modular_Police` is not mounted in the current project.
- `AssetRegistry.scan_files_synchronous` against the physical `.uasset` path logged that the file is not in a mounted path and would not scan.
- `PackageTools.filename_to_package_name` returned an empty package name for the external `плагины\Modular_Police\Content\...` file path.
- Control check: MCP successfully loaded `/Game/Modular_Workers/Mesh/Male/Face_Modules/Glasses_Classic/SKM_Glasses_Classic`, confirming MCP/editor asset loading works for mounted project content.

Filesystem fallback used only for non-authoritative evidence:

- Verified all 10 physical `.uasset` files exist in `плагины\Modular_Police\Content\Modular_Police`.
- String-scanned those files for skeleton, physics asset, material references, and strict branding tokens.
- Rechecked Police and Worker male skeleton files by string probe for required common bones: `24/24` matched (`root`, `pelvis`, spine, neck, head, hands, thighs, feet, etc.).

Important limitation: because the Police bundle is outside mounted Unreal content and has no loadable mount in the current Editor session, material slot counts, exact loaded class, actual skeleton object equality, sockets, imported bounds, and visual scale could not be validated through loaded `USkeletalMesh` objects. No candidate is promoted to SAFE V1 in this pass.

## Summary

| Readiness | Count | Meaning |
|---|---:|---|
| SAFE V1 | 0 | No asset completed required Editor/MCP load validation. |
| NEEDS VISUAL CHECK | 0 | No asset reached the post-load visual-check stage. |
| BLOCKED | 10 | All 10 are blocked by missing `/Game/Modular_Police` mount/loadability. |

Branding result:

- No strict item/material branding token was found among the 10 candidates after excluding the bundle root name `Modular_Police`.
- Strict checked tokens: `POLICE`, `SWAT`, `badge`, `sheriff`, `patch`, `emblem`, `law enforcement` / `law`.
- This does not replace visual review; it only means no obvious hidden branding was found in asset/material names for these 10 files.

Compatibility result:

- Filesystem string refs show all 10 candidates reference `/Game/Modular_Police/Mesh/Male/SK_Male_Quantum_Character_Skeleton`.
- Worker uses `/Game/Modular_Workers/Mesh/Male/SK_Male_Quantum_Character_Skeleton`.
- These are different object paths, so object identity is not proven.
- Skeleton-file string probe found the required common bones in both skeleton files, so compatibility is probably compatible if the Police bundle is mounted and the meshes load correctly.
- Current validation status remains BLOCKED until actual loaded asset inspection succeeds.

## Candidate Validation Table

| Asset Name | Asset Path | Type | Slot | Skeleton | Materials | Police Branding Check | Conflicts | V1 Readiness | Risk | Notes |
|---|---|---|---|---|---|---|---|---|---|---|
| `SKM_Glasses_Classic` | `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Classic/SKM_Glasses_Classic` | Expected SKM; not loaded | Face / Glasses | String ref: Police Quantum male skeleton; Worker object path differs; 24/24 common skeleton bone strings matched | String refs: `MI_Classic_Glasses`, `M_Classic_Glasses_Glass`; slot count not loaded | No strict branding token hit | Other glasses/goggles/masks | BLOCKED | high | Good neutral candidate by names, but Editor/MCP cannot load it from current mount state. |
| `SKM_Glasses_SunGlass` | `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Classic/SKM_Glasses_SunGlass` | Expected SKM; not loaded | Face / Glasses | Same as above | String refs: `MI_Classic_Glasses`, `M_Classic_Glasses_SunGlass` | No strict branding token hit | Other glasses/goggles/masks | BLOCKED | high | Neutral sunglasses by names; cannot validate slots/bounds/skeleton object until mounted. |
| `SKM_Glasses_Combat_Glass` | `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Combat/SKM_Glasses_Combat_Glass` | Expected SKM; not loaded | Face / Glasses | Same as above | String refs: `MI_Glasses_Combat`, `M_Glasses_Combat_Glass` | No strict branding token hit | Other glasses/goggles/masks/helmet glass | BLOCKED | high | Utility eyewear; likely candidate after mount, but not SAFE now. |
| `SKM_Glasses_Combat_Orange` | `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Combat/SKM_Glasses_Combat_Orange` | Expected SKM; not loaded | Face / Glasses | Same as above | String refs: `MI_Glasses_Combat`, `M_Glasses_Combat_Orange` | No strict branding token hit | Other glasses/goggles/masks/helmet glass | BLOCKED | high | Orange utility eyewear fits Avariika accent direction; still blocked by loadability. |
| `SKM_Goggles_Black` | `/Game/Modular_Police/Mesh/Male/Face_Modules/Goggles/SKM_Goggles_Black` | Expected SKM; not loaded | Face / Goggles | Same as above | String refs: `MI_Goggles_Black`, `M_Goggles_Glass` | No strict branding token hit | Glasses/masks/helmet glass/some headwear | BLOCKED | high | Useful worker protection item, but needs loaded bounds and head clipping check. |
| `SKM_Goggles_Bege` | `/Game/Modular_Police/Mesh/Male/Face_Modules/Goggles/SKM_Goggles_Bege` | Expected SKM; not loaded | Face / Goggles | Same as above | String refs: `MI_Goggles_Bege`, `M_Goggles_Glass` | No strict branding token hit | Glasses/masks/helmet glass/some headwear | BLOCKED | high | Same family as black goggles; visual check required after mount. |
| `SKM_Headphones_Micro_Black` | `/Game/Modular_Police/Mesh/Male/Head_Modules/Headphones_Micro/SKM_Headphones_Micro_Black` | Expected SKM; not loaded | Head accessory / Headphones | Same as above | String refs: `MI_HeadPhonesMicro_Black`; physics ref: `PA_Headphones_Micro_Bege_Physics` | No strict branding token hit | Hats/helmets/some goggles/hair | BLOCKED | high | Neutral headset by names; needs ear/head clipping and socket/bone validation. |
| `SKM_Headphones_Micro_Gray` | `/Game/Modular_Police/Mesh/Male/Head_Modules/Headphones_Micro/SKM_Headphones_Micro_Gray` | Expected SKM; not loaded | Head accessory / Headphones | Same as above | String refs: `MI_HeadPhonesMicro_Gray`; physics ref: `PA_Headphones_Micro_Bege_Physics` | No strict branding token hit | Hats/helmets/some goggles/hair | BLOCKED | high | Neutral headset by names; same load blocker. |
| `SKM_Bag_Hip_Black` | `/Game/Modular_Police/Mesh/Male/Hips_Modules/Bag_Hip/SKM_Bag_Hip_Black` | Expected SKM; not loaded | Hip gear / Bag | Same as above | String refs: `MI_Bag_Hip_Black`; physics ref: `PA_Bag_Hip` | No strict branding token hit | Other hip gear, Worker banana bag/chest bag if shared rules, overalls/full outfits | BLOCKED | high | Neutral utility bag by names; likely needs visual check against pants/overalls. |
| `SKM_Belt_Tactical_Black` | `/Game/Modular_Police/Mesh/Male/Belts_Modules/Belt_Tactical/SKM_Belt_Tactical_Black` | Expected SKM; not loaded | Belt / Hip gear | Same as above | String refs: `MI_Belt_Tactical_Black`; physics ref: `PA_Belt_Tactical_PhysicsAsset` | No strict branding token hit | Other belt/waist gear, hip bag, overalls/full outfits | BLOCKED | high | Neutral utility belt by material names; cannot validate bounds/clipping until mounted. |

## Safe V1 Candidates

None.

Reason: SAFE V1 requires successful Editor/MCP load, material-slot inspection, skeleton object/path validation, bounds/scale inspection, and at least a minimal visual/clipping check plan. The current bundle path is not mounted, so none of the 10 candidates completed validation.

## Needs Visual Check

None counted in this validation pass.

After the bundle becomes loadable in the Editor, these should be the first visual-check group:

- `SKM_Glasses_Classic`
- `SKM_Glasses_SunGlass`
- `SKM_Glasses_Combat_Orange`
- `SKM_Goggles_Black`
- `SKM_Headphones_Micro_Black`
- `SKM_Bag_Hip_Black`
- `SKM_Belt_Tactical_Black`

They are not classified as NEEDS VISUAL CHECK yet because the required loaded asset inspection did not complete.

## Blocked / Excluded

Blocked by mount/loadability:

| Asset | Reason |
|---|---|
| All 10 scoped assets | Physical files exist, but `/Game/Modular_Police` is not mounted in the current project/editor. MCP cannot load them as Unreal assets. |

Excluded by branding:

| Asset | Reason |
|---|---|
| None among the scoped 10 | Strict asset/material string scan found no item-level `POLICE`, `SWAT`, `badge`, `sheriff`, `patch`, `emblem`, or law-enforcement token. |

Still excluded from the broader Police bundle:

- Police caps/hats/berets with badge references.
- Police vest and bulletproof/tactical vest silhouettes.
- Police shirts/pants.
- Full police outfits/presets.
- Weapons/props.
- Patch/back gear with police patch risk.

## Proposed Conflict Rules

- Glasses and goggles use one mutually exclusive face accessory slot.
- Goggles conflict with glasses, full masks, helmet glass, and unvalidated headwear overlaps.
- Headphones/headset conflict with helmets and any headwear that intersects ears/sides of head until pair-specific visual validation passes.
- Hip bag conflicts with other hip gear, holsters, belt drop/pouch bundles, and overalls/full outfits until validated.
- Tactical belt conflicts with other belt/waist gear and likely with overalls/full outfits.
- No Police-branded asset may be allowed through neutral Worker customization even if its mesh technically loads.

## Minimal Next Integration Step

Do not integrate any of the 10 now.

Minimal safe next step, only after explicit command:

1. Make the Modular Police bundle loadable in the Editor as a proper mounted content source without changing Worker UI/code.
2. Re-run MCP load validation on the same 10 assets and capture actual `USkeletalMesh` class, skeleton object path, physics asset object path, material slots, material asset paths, sockets, and bounds.
3. If validation passes, first integration candidates should be only 1-3 low-identity face items: `SKM_Glasses_Classic`, `SKM_Glasses_SunGlass`, and `SKM_Glasses_Combat_Orange`.

No build is required for this validation document because no C++ was changed.
