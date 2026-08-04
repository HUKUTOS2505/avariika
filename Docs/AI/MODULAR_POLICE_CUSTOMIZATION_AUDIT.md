# Modular Police Customization Audit

## Source

Bundle path:

`C:\unrealEngine\avariika_UE58_sandbox\плагины\Modular_Police`

Content root:

`C:\unrealEngine\avariika_UE58_sandbox\плагины\Modular_Police\Content\Modular_Police`

Unreal paths use `/Game/Modular_Police/...` if the bundle content is mounted under the project content root. The audit is read-only: no assets were copied, edited, saved, connected to UI, or added to Worker customization.

## Summary

Bundle found: yes.

Inventory:

| Asset group | Count | Notes |
|---|---:|---|
| All `.uasset` under bundle `Content` | 2015 | Full marketplace bundle payload. |
| `.umap` demo/preview maps | 3 | `MegaBundle_Character_Builder`, `Preview_Assets`, `Preview_Presets`. |
| `Mesh` folder `.uasset` | 687 | Male, female, static meshes, skeletons, physics assets. |
| `SKM_*` assets under `Mesh` | 373 | Skeletal mesh naming convention. |
| `SM_*` assets under `Mesh` | 173 | Static mesh naming convention. |
| Physics assets by name | 135 | `PA_*` or `*PhysicsAsset*`. |
| Skeleton-like assets | 4 | Male/Female Quantum skeletons plus face rig skeletons. |
| Materials folder `.uasset` | 207 | 36 `M_*`, 144 `MI_*`, 27 other material-related assets. |
| Textures folder `.uasset` | 417 | Police bundle texture payload. |
| Demo folder `.uasset` | 39 | Demo character builder BPs/widgets/animations/backdrop. |

Male Worker-relevant inventory:

| Male category | `.uasset` | `SKM_*` | `SM_*` |
|---|---:|---:|---:|
| Arms modules | 23 | 16 | 0 |
| Backpacks modules | 11 | 7 | 0 |
| Belts modules | 12 | 8 | 0 |
| Body modules | 35 | 24 | 0 |
| Chest modules | 5 | 3 | 0 |
| Clothes modules | 12 | 10 | 0 |
| Drops modules | 12 | 8 | 0 |
| Face modules | 39 | 20 | 11 |
| Head modules | 54 | 24 | 18 |
| Helmet modules | 29 | 12 | 12 |
| Hips modules | 6 | 4 | 0 |
| Pants modules | 8 | 6 | 0 |
| Presets | 34 | 32 | 0 |

Candidate count for customization planning:

- 118 male modular `SKM_*` assets outside body modules and merged presets.
- 32 male merged/full preset `SKM_*` assets.
- 41 male-module `SM_*` accessory variants plus 29 shared `Static_Meshes/Male` static meshes. Static meshes should be treated as socket/attachment candidates, not direct Worker SKM modules.
- Weapons/props are present, but are audit-only and should not enter customization v1.
- Neutral Avariika-compatible candidates are narrower than the full Police inventory: glasses/goggles, headset, hip bag, tactical belt, some backpack/pouch families, and neutral gloves/watch families. No standalone male boots/shoes were found.
- `Beret_Police` and `Cap_02_Police` are not recommended as neutral items: a read-only string probe found `MI_Police_Badge` references in those SKM assets.

Compatibility summary:

- The bundle uses the same named male Quantum skeleton asset, `SK_Male_Quantum_Character_Skeleton`, as Modular Workers.
- A read-only binary probe found 23/23 common reference bone names in both the Police and Worker male skeleton files: root, pelvis, spine, neck/head, clavicles, arms, hands, thighs, calves, feet, and ball bones.
- The skeleton files are not byte-identical, so treat compatibility as expected but not proven until an editor/MCP load check confirms actual skeleton object, retarget base pose, bounds, and material slots.
- Most standalone SKM accessories look like the best candidates. Full outfits, body modules, clothes, pants, arms/gloves, and face-rig assets require a separate validation pass.

## Skeleton / Scale Compatibility

| Check | Modular Workers | Modular Police | Result | Risk / Notes |
|---|---|---|---|---|
| Male skeleton asset name | `/Game/Modular_Workers/Mesh/Male/SK_Male_Quantum_Character_Skeleton` | `/Game/Modular_Police/Mesh/Male/SK_Male_Quantum_Character_Skeleton` | Same name/convention | Medium confidence. Same Quantum naming strongly suggests intended compatibility. |
| Male skeleton file size | 189785 bytes | 181474 bytes | Different | Not byte-identical. Do not assume full identity without editor load validation. |
| Common bone-name probe | 23/23 common bones present | 23/23 common bones present | Matches probe | Probe covers the main mannequin/Quantum body chain only, not every auxiliary/face bone. |
| Scale convention | Modular Workers male Quantum body | Modular Police male Quantum body/presets | Expected same scale | Needs editor bounds check before integration. Filesystem audit cannot measure actual imported bounds reliably. |
| Worker body compatibility | Current Worker customization is Modular Workers male SKM slot based | Police standalone SKM modules appear built for the same male Quantum skeleton | Expected for standalone SKM accessories | Must validate clipping on Worker body, especially chest/hip/arms/clothes. |
| Static mesh compatibility | Worker flow is SKM modular slots | Police SM variants exist for head/face/helmet/weapons | Socket-only, not skeleton-bound | Needs socket placement and attachment rules. Not v1 unless a socket pass is explicitly requested. |
| Face rig/body assets | Worker body uses current Modular Workers path | Police has separate Quantum face rig/body modules | Not v1 | Face rig and full body replacement must be handled separately from simple modular clothing. |

## Candidate Items

Rows below are item families found in the bundle. `Asset Path` is the exact folder/prefix pattern for the family; variants are individual assets under that path. Use exact individual asset paths from the `Recommended V1 Additions` section for the first safe pass.

| Asset Path | Type | Slot | Standalone/Merged | Skeleton Compatible | Conflicts | V1 Candidate | Risk | Notes |
|---|---|---|---|---|---|---|---|---|
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Beret_Police/SKM_Beret_Police_*` | SKM | Head / headwear | Standalone | Expected yes | Hair, helmets, hats | No | Medium | 5 variants. Read-only string probe found `MI_Police_Badge`; exclude from neutral V1. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Beret_Police/SM/SM_Beret_Police_*` | SM | Head / headwear | Standalone socket | N/A | Hair, helmets, hats | No | Medium | Static duplicate variants. Needs socket/attachment pass. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Cap_02_Police/SKM_Cap_02_Police_*` | SKM | Head / cap | Standalone | Expected yes | Hair, helmets, hats | No | Medium | 4 variants. Read-only string probe found `MI_Police_Badge`; exclude from neutral V1. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Cap_02_Police/SM/SM_Cap_02_Police_*` | SM | Head / cap | Standalone socket | N/A | Hair, helmets, hats | No | Medium | Static duplicate variants. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Police_Cap/SKM_Police_Cap` | SKM | Head / cap | Standalone | Expected yes | Hair, helmets, hats | No | High | Police identity item. Exclude from neutral Worker list. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Police_Cap/SM/SM_Police_Cap` | SM | Head / cap | Standalone socket | N/A | Hair, helmets, hats | No | Medium | Static duplicate. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Police_Cowboy_Hat/SKM_Police_Hat_Cowboy` | SKM | Head / hat | Standalone | Expected yes | Hair, helmets, hats | No | High | Police-themed hat. Exclude from neutral Worker list. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Police_Hat/SKM_Police_Hat*` | SKM | Head / hat | Standalone | Expected yes | Hair, helmets, hats | No | High | Police hat family; string probe found `MI_Police_Badge` on base hat. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Police_Warm_Hat/SKM_Police_Warm_Hat` | SKM | Head / hat | Standalone | Expected yes | Hair, helmets, hats | No | High | Uses `MI_Police_Hat`; exclude from neutral Worker list. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Hairs/SKM_Hair_*` | SKM | Hair | Standalone | Expected yes | Headwear, balaclava, helmets | No | High | 5 hair variants. Hairline/head clipping requires a dedicated hair pass. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Headphones_Micro/SKM_Headphones_Micro_*` | SKM | Head / headset | Standalone | Expected yes | Hats, helmets, some goggles | Yes | Medium | 3 variants. Neutral headset accessory, but needs ear/head clipping check. |
| `/Game/Modular_Police/Mesh/Male/Helmet_Modules/Helmet_Basic/SKM_Helmet_Basic*` | SKM | Helmet | Standalone | Expected yes | Hair, hats, headset, goggles | Later | Medium | 4 variants including plus variants. Needs helmet-specific conflict rules. |
| `/Game/Modular_Police/Mesh/Male/Helmet_Modules/Helmet_Glass/SKM_Helmet_Glass*` | SKM | Helmet | Standalone | Expected yes | Hair, hats, headset, goggles, glasses | Later | Medium | 4 variants: closed/open glass in beige/black. |
| `/Game/Modular_Police/Mesh/Male/Helmet_Modules/Goggles/SKM_Goggles_OnHelmet_*` | SKM | Helmet accessory | Standalone | Expected yes | Helmet variants, goggles, glasses | No | Medium | Use only with helmet-specific validation. |
| `/Game/Modular_Police/Mesh/Male/Helmet_Modules/Goggles_Cover/SKM_Goggles_Cover_*` | SKM | Helmet accessory | Standalone | Expected yes | Helmet variants, goggles, glasses | No | Medium | Helmet attachment variant. Not generic face goggles. |
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Aviator/SKM_Glasses_Aviator*` | SKM | Glasses / face | Standalone | Expected yes | Glasses, goggles, masks, helmets with glass | Later | Medium | 3 variants. Needs face/nose fit check. |
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Classic/SKM_Glasses_*` | SKM | Glasses / face | Standalone | Expected yes | Glasses, goggles, masks | Later | Medium | 2 variants: classic and sunglass. |
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Combat/SKM_Glasses_Combat_*` | SKM | Glasses / face protection | Standalone | Expected yes | Glasses, goggles, masks, helmet glass | Yes | Low | 3 variants. Good first-pass face accessory. |
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Goggles/SKM_Goggles_*` | SKM | Goggles / face protection | Standalone | Expected yes | Glasses, masks, helmet glass, some hats | Yes | Medium | 2 variants. Neutral worker protection item; needs head/headwear fit check. |
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Gas_Mask_Glass/SKM_Gas_Mask_Glass` | SKM | Mask / face protection | Standalone | Expected yes | Beard, glasses, goggles, respirator, headwear | No | High | Not v1. Needs face/mask conflict and clipping pass. |
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Balaclava/SKM_Balaclava_*` | SKM | Mask / face | Standalone | Expected yes | Hair, beard, glasses, masks, headwear | No | High | 2 variants. Face/head occlusion and skin coverage need separate pass. |
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Balaclava_Mouth/SKM_Balaclava_Mouth_*` | SKM | Mask / mouth | Standalone | Expected yes | Beard, respirator, masks | No | High | 2 variants. Mouth/beard conflicts. |
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Beard/SKM_*` | SKM | Beard / moustache | Standalone | Expected yes | Masks, respirator, balaclava, some glasses | No | High | 5 variants. Existing Worker beard/mask rules need extension first. |
| `/Game/Modular_Police/Mesh/Male/Clothes_Modules/Police_Shirt/SKM_Police_Shirt*` | SKM | Upper | Body clothing | Expected yes | Current top, chest vest clipping, overalls, merged preset | No | High | 5 variants. Do not treat as simple accessory; requires top replacement pass. |
| `/Game/Modular_Police/Mesh/Male/Clothes_Modules/Police_Shirt_Short/SKM_Police_Shirt_Short*` | SKM | Upper | Body clothing | Expected yes | Current top, arms/gloves/wrist, chest vest, overalls | No | High | 5 variants. Short sleeves interact with arms/tattoos/watch. |
| `/Game/Modular_Police/Mesh/Male/Chest_Modules/Bulletproof_Vest/SKM_Bulletproof_Vest_*` | SKM | Vest / chest gear | Standalone over torso | Expected yes | Worker vest, bag banana, backpacks, chest gear, overalls | No | High | Tactical vest silhouette reads police/SWAT/security; exclude from neutral Worker V1. |
| `/Game/Modular_Police/Mesh/Male/Chest_Modules/Police_Vest/SKM_Police_Vest` | SKM | Vest / chest gear | Standalone over torso | Expected yes | Worker vest, bag banana, backpacks, chest gear, overalls | No | High | Explicit Police vest; excluded. |
| `/Game/Modular_Police/Mesh/Male/Backpacks_Modules/BackDrops/SKM_BackDrops_02_*` | SKM | Back gear | Standalone | Expected yes | Vests, backpacks, overalls, rear props | No | Medium | 2 variants. Needs back overlap and camera silhouette check. |
| `/Game/Modular_Police/Mesh/Male/Backpacks_Modules/Bacpack_Molly/SKM_Backpack*_Molly*` | SKM | Backpack | Standalone | Expected yes | Vests, back gear, overalls | No | Medium | 2 variants. Typo in folder name is in source bundle. |
| `/Game/Modular_Police/Mesh/Male/Backpacks_Modules/PatchBackNew/SKM_PatchBackNew` | SKM | Back patch | Standalone | Expected yes | Backpacks, vests, overalls | No | Medium | Patch/back item. Needs slot semantics before use. |
| `/Game/Modular_Police/Mesh/Male/Backpacks_Modules/Police_Back/SKM_Police_Back*` | SKM | Back gear | Standalone | Expected yes | Backpacks, vests, overalls | No | Medium | 2 variants. Needs back conflict rules. |
| `/Game/Modular_Police/Mesh/Male/Arms_Modules/Arms/SKM_Quantum_Hands*` | SKM | Hands / arms | Body part | Expected yes | Gloves, wrist, watch, tattoos, short sleeves | No | High | 4 variants. Body part replacement, not simple accessory. |
| `/Game/Modular_Police/Mesh/Male/Arms_Modules/Gloves/SKM_Arms_Gloves_*` | SKM | Gloves / arms | Body/hand module | Expected yes | Arms tattoo, watch, wrist, short sleeves | No | High | 2 long glove variants. Needs arms slot pass. |
| `/Game/Modular_Police/Mesh/Male/Arms_Modules/Gloves_Short/SKM_Gloves_Short_*` | SKM | Gloves / hands | Body/hand module | Expected yes | Arms tattoo, watch, wrist | No | Medium | 4 variants. Potential later after glove/watch conflict support. |
| `/Game/Modular_Police/Mesh/Male/Arms_Modules/Watches_Military/SKM_Watches_Military_*` | SKM | Watch / wrist | Standalone | Expected yes | Gloves, wrist, arms variants | Later | Medium | 4 variants, left/right beige/black. Avoid until existing watch/tattoo rules are extended. |
| `/Game/Modular_Police/Mesh/Male/Arms_Modules/Wrist/SKM_Quantum_Wrist*` | SKM | Wrist | Body part | Expected yes | Gloves, watches, arms | No | High | Body connector part, not a user-facing item by itself. |
| `/Game/Modular_Police/Mesh/Male/Pants_Modules/Police_Pants/SKM_Police_Pants*` | SKM | Pants / lower | Body clothing | Expected yes | Current pants, hip gear, belt, overalls, merged preset | No | High | 3 variants. Requires lower-body replacement pass. |
| `/Game/Modular_Police/Mesh/Male/Pants_Modules/Police_Pants_Tactic/SKM_Police_Pants_Tactic*` | SKM | Pants / lower | Body clothing | Expected yes | Current pants, hip gear, belt, overalls, merged preset | No | High | 3 variants. More gear overlap risk. |
| `/Game/Modular_Police/Mesh/Male/Belts_Modules/Belt_Tactical/SKM_Belt_Tactical_*` | SKM | Belt / hip gear | Standalone | Expected yes | Pants, hip bag, holster, drops, overalls | Later | Medium | 2 variants. Needs hip conflict rules and visual check. |
| `/Game/Modular_Police/Mesh/Male/Belts_Modules/Police_Belt_Drops_01/SKM_Police_Belt_Drops_01_*` | SKM | Belt / hip gear | Standalone | Expected yes | Pants, hip bag, holster, drops, overalls | Later | Medium | 2 variants. |
| `/Game/Modular_Police/Mesh/Male/Belts_Modules/Police_Belt_Drops_02/SKM_Police_Belt_Drops_02_*` | SKM | Belt / hip gear | Standalone | Expected yes | Pants, hip bag, holster, drops, overalls | Later | Medium | 2 variants. |
| `/Game/Modular_Police/Mesh/Male/Belts_Modules/Police_Belt_Drops_03/SKM_Police_Belt_Drops_03_*` | SKM | Belt / hip gear | Standalone | Expected yes | Pants, hip bag, holster, drops, overalls | Later | Medium | 2 variants. |
| `/Game/Modular_Police/Mesh/Male/Hips_Modules/Bag_Hip/SKM_Bag_Hip_*` | SKM | Hip gear / bag | Standalone | Expected yes | Worker banana bag, belt, holster, overalls | Later | Medium | 2 variants. Potential later, not first pass. |
| `/Game/Modular_Police/Mesh/Male/Hips_Modules/Holster_Hard/SKM_Holster_Hard_*` | SKM | Hip gear / holster | Standalone | Expected yes | Hip bag, belts, drops, weapons, overalls | No | Medium | 2 variants. Do not pair with weapons in customization v1. |
| `/Game/Modular_Police/Mesh/Male/Drops_Modules/Police_Drops_01/SKM_Police_Drops_01_*` | SKM | Belt drops / pouches | Standalone | Expected yes | Belts, hip bag, holster, pants, overalls | Later | Medium | 2 variants. |
| `/Game/Modular_Police/Mesh/Male/Drops_Modules/Police_Drops_02/SKM_Police_Drops_02_*` | SKM | Belt drops / pouches | Standalone | Expected yes | Belts, hip bag, holster, pants, overalls | Later | Medium | 2 variants. |
| `/Game/Modular_Police/Mesh/Male/Drops_Modules/Police_Drops_03/SKM_Police_Drops_*` | SKM | Belt drops / pouches | Standalone | Expected yes | Belts, hip bag, holster, pants, overalls | Later | Medium | Folder contains beige plus a black asset named `SKM_Police_Drops_04_Black`. Validate exact asset before any integration. |
| `/Game/Modular_Police/Mesh/Male/Drops_Modules/Police_Drops_04/SKM_Police_Drops_04_*` | SKM | Belt drops / pouches | Standalone | Expected yes | Belts, hip bag, holster, pants, overalls | Later | Medium | 2 variants. |
| `/Game/Modular_Police/Mesh/Male/Presets/SKM_Modular_Police_Preset_Male_*` | SKM | Full outfit / merged preset | Merged | Expected yes | All normal body/clothes/chest/hip/head modules depending preset | No | High | 16 full preset meshes. Separate mode only, like Overalls/full preset mode. |
| `/Game/Modular_Police/Mesh/Male/Presets/SKM_Modular_Police_Preset_Male_Body_*` | SKM | Full body / merged body preset | Merged | Expected yes | All normal body/clothes/chest/hip modules | No | High | 16 body preset meshes. Do not treat as upper or pants. |
| `/Game/Modular_Police/Mesh/Male/Body_Modules/*/SKM_Quantum_*` | SKM | Base body pieces | Body replacement | Expected yes | Current Worker body, skin, face rig, clothes, arms/legs | No | High | 24 male body module SKMs. Not police customization v1. |
| `/Game/Modular_Police/Mesh/Male/Body_Modules/Quantum_FaceRig/*` | SKM/Skeleton | Face rig / body | Face/body rig | No for v1 | Head, face, skin, animation/rig flow | No | High | Separate face-rig pipeline, not simple modular clothing. |
| `/Game/Modular_Police/Mesh/Static_Meshes/Male/SM_Glasses_*` | SM | Glasses / face | Standalone socket | N/A | Glasses, goggles, masks | No | Medium | Shared static variants exist; current Worker flow should prefer SKM versions first. |
| `/Game/Modular_Police/Mesh/Static_Meshes/Male/SM_Goggles_*` | SM | Goggles / face | Standalone socket | N/A | Glasses, masks, helmets | No | Medium | Socket-only candidate. |
| `/Game/Modular_Police/Mesh/Static_Meshes/Male/SM_*Hat*`, `/SM_*Cap*`, `/SM_Beret_*`, `/SM_Headphones_*` | SM | Headwear / headset | Standalone socket | N/A | Hair, helmets, hats | No | Medium | Socket-only duplicates of SKM head accessories. |
| `/Game/Modular_Police/Mesh/Static_Meshes/Weapons/SM_*` | SM | Weapons / props | Standalone prop | N/A | Holsters, hands, gameplay inventory | No | High | Pistol, rifles, grenades, knife/holster props. Audit-only. Do not connect to customization v1. |
| `/Game/Modular_Police/Mesh/Female/**` | SKM/SM | Female modules | Female body flow | No for current Worker | Current male Worker body/skeleton flow | No | High | Present in bundle, but outside current male Worker customization scope. |

## Neutral Avariika-Compatible Candidates

This refined list excludes anything that is explicitly police-branded, references badge/police materials in a read-only string probe, or visually pushes the Worker toward a police/SWAT/security identity. It is still a filesystem/string audit, not an editor visual QA pass.

| Asset Path | Type | Slot | Why Neutral | Conflicts | V1 Candidate | Risk | Notes |
|---|---|---|---|---|---|---|---|
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Aviator/SKM_Glasses_Aviator*` | SKM | Glasses / face | Civilian sunglasses/aviator styling; no police/badge material reference found in probe. | Other glasses, goggles, masks, helmet glass | Later | Medium | 3 variants. Needs nose/face fit check. |
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Classic/SKM_Glasses_Classic` | SKM | Glasses / face | Neutral classic glasses. | Other glasses, goggles, masks | Yes | Low | Good low-identity Worker accessory. |
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Classic/SKM_Glasses_SunGlass` | SKM | Glasses / face | Neutral sunglasses. | Other glasses, goggles, masks | Yes | Low | Good low-identity Worker accessory. |
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Combat/SKM_Glasses_Combat_*` | SKM | Glasses / face protection | Utility eye protection; no police/badge material reference found in probe. | Other glasses, goggles, masks, helmet glass | Yes | Low | 3 variants. `Orange` variant fits Avariika orange accent direction. |
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Goggles/SKM_Goggles_*` | SKM | Goggles / face protection | Work/utility goggles without police naming in asset basename. | Other glasses, masks, helmet glass, some headwear | Yes | Medium | 2 variants. Needs Worker head and headwear clipping check. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Headphones_Micro/SKM_Headphones_Micro_*` | SKM | Headset / headphones | Neutral headset/communications accessory; no police/badge material reference found in probe. | Hats, helmets, some goggles | Yes | Medium | 3 variants: beige, black, gray. |
| `/Game/Modular_Police/Mesh/Male/Hips_Modules/Bag_Hip/SKM_Bag_Hip_*` | SKM | Hip bag / utility pouch | Generic hip bag with `MI_Bag_Hip_*`; no police/badge material reference found in probe. | Worker banana bag, belt gear, holster, overalls/full presets | Yes | Medium | 2 variants. Good neutral utility candidate after hip conflict validation. |
| `/Game/Modular_Police/Mesh/Male/Belts_Modules/Belt_Tactical/SKM_Belt_Tactical_*` | SKM | Belt / hip gear | Tactical belt with neutral `MI_Belt_Tactical_*`; no police/badge material reference found in probe. | Pants, hip bag, holster, pouch drops, overalls/full presets | Yes | Medium | 2 variants. Utility, not police-branded by strings. |
| `/Game/Modular_Police/Mesh/Male/Backpacks_Modules/Bacpack_Molly/SKM_Backpack*_Molly*` | SKM | Backpack / back gear | Generic MOLLE backpack materials; no police/badge material reference found in probe. | Chest gear, back gear, overalls/full presets | Later | Medium | 2 variants. Tactical look; validate silhouette before v1. |
| `/Game/Modular_Police/Mesh/Male/Backpacks_Modules/BackDrops/SKM_BackDrops_02_*` | SKM | Back drops / utility pouches | Uses tactical bags/drops materials rather than police badge materials. | Backpack, vest, overalls/full presets | No | High | Probe also found a grenade material reference; keep out of first neutral pass. |
| `/Game/Modular_Police/Mesh/Male/Arms_Modules/Gloves_Short/SKM_Gloves_Short_*` | SKM | Gloves / hands | Neutral gloves; no police/badge material reference found in probe. | Arms tattoo, watch, wrist, arm modules, short sleeves | Later | Medium | 4 variants. Needs arms/gloves/watch conflict pass. |
| `/Game/Modular_Police/Mesh/Male/Arms_Modules/Gloves/SKM_Arms_Gloves_*` | SKM | Gloves / arms | Neutral long gloves; no police/badge material reference found in probe. | Arms tattoo, watch, wrist, arm modules, short sleeves | Later | High | 2 variants. Larger arm-body replacement risk. |
| `/Game/Modular_Police/Mesh/Male/Arms_Modules/Watches_Military/SKM_Watches_Military_*` | SKM | Watch / wrist | Utility watch; no police/badge material reference found in name/material family. | Gloves, wrist, arms variants, current Worker watch rules | Later | Medium | 4 variants, left/right beige/black. |
| `/Game/Modular_Police/Mesh/Male/Helmet_Modules/Helmet_Basic/SKM_Helmet_Basic*` | SKM | Helmet | No police/badge material reference found in probe. | Hair, hats, headset, helmet goggles, masks | No | High | Tactical/SWAT-like silhouette; not a first neutral Worker item. |
| `/Game/Modular_Police/Mesh/Male/Hips_Modules/Holster_Hard/SKM_Holster_Hard_*` | SKM | Holster / hip gear | Not police-branded by material strings. | Hip bag, belt, pouches, weapons, overalls/full presets | No | High | Probe found `MI_Pistol`; weapon association makes it unsuitable for neutral v1. |
| `N/A - no standalone male boots/shoes found` | N/A | Boots / shoes | No separate neutral male footwear candidate was found. | N/A | No | N/A | Search found male base feet only: `SKM_Quantum_Feet`; female `Police_Pants_Inboots` is merged pants, not male standalone footwear. |
| `N/A - no verified neutral cap/beret/beanie found` | N/A | Headwear | Candidate cap/beret families reference police/badge materials. | Hair, hats, helmets | No | N/A | `Beret_Police` and `Cap_02_Police` both referenced `MI_Police_Badge` in string probes. |

## Excluded Police-Branded Items

| Asset Path | Reason |
|---|---|
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Beret_Police/SKM_Beret_Police_*` | Excluded from neutral V1: string probe found `MI_Police_Badge`. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Beret_Police/SM/SM_Beret_Police_*` and `/Game/Modular_Police/Mesh/Static_Meshes/Male/Beret_Police/SM_Beret_Police_*` | Static duplicates of the badge-linked beret family; do not use as socket bypass for neutral v1. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Cap_02_Police/SKM_Cap_02_Police_*` | Excluded from neutral V1: string probe found `MI_Police_Badge`. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Cap_02_Police/SM/SM_Cap_02_Police_*` and `/Game/Modular_Police/Mesh/Static_Meshes/Male/Cap_02_Police/SM_Cap_02_Police_*` | Static duplicates of the badge-linked cap family; do not use as socket bypass for neutral v1. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Police_Cap/SKM_Police_Cap` | Explicit police identity item; uses `MI_Police_Cap`. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Police_Cap/SM/SM_Police_Cap` and `/Game/Modular_Police/Mesh/Static_Meshes/Male/Police_Cap/SM_Police_Cap` | Static duplicate of explicit police cap. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Police_Hat/SKM_Police_Hat*` | Police hat family; string probe found `MI_Police_Badge` on base variant. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Police_Hat/SM/SM_Police_Hat*` and `/Game/Modular_Police/Mesh/Static_Meshes/Male/Police_Hat/SM_Police_Hat*` | Static duplicates of police hat family. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Police_Cowboy_Hat/SKM_Police_Hat_Cowboy` | Police-themed hat; not suitable for ordinary Workers. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Police_Cowboy_Hat/SM/SM_Police_Hat_Cowboy` and `/Game/Modular_Police/Mesh/Static_Meshes/Male/Police_Cowboy_Hat/SM_Police_Hat_Cowboy` | Static duplicate of police-themed cowboy hat. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Police_Warm_Hat/SKM_Police_Warm_Hat` | Uses police hat material family; not a neutral beanie candidate. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Police_Warm_Hat/SM/SM_Police_Warm_Hat` and `/Game/Modular_Police/Mesh/Static_Meshes/Male/Police_Warm_Hat/SM_Police_Warm_Hat` | Static duplicate of police warm hat. |
| `/Game/Modular_Police/Mesh/Male/Chest_Modules/Police_Vest/SKM_Police_Vest` | Explicit police vest; excluded by request. |
| `/Game/Modular_Police/Mesh/Male/Chest_Modules/Bulletproof_Vest/SKM_Bulletproof_Vest_*` | Tactical/SWAT/security silhouette; visually makes Worker look like law enforcement/security. |
| `/Game/Modular_Police/Mesh/Male/Clothes_Modules/Police_Shirt*/SKM_Police_Shirt*` | Police shirt clothing family; not neutral Worker clothing. |
| `/Game/Modular_Police/Mesh/Male/Pants_Modules/Police_Pants*/SKM_Police_Pants*` | Police pants clothing family; not neutral Worker clothing. |
| `/Game/Modular_Police/Mesh/Male/Presets/SKM_Modular_Police_Preset_Male_*` | Full police outfits/merged presets; never use as normal Worker top/bottom. |
| `/Game/Modular_Police/Mesh/Male/Presets/SKM_Modular_Police_Preset_Male_Body_*` | Merged police body presets; separate mode only, not neutral v1. |
| `/Game/Modular_Police/Mesh/Male/Backpacks_Modules/Police_Back/SKM_Police_Back*` | Police back gear; string probe found `MI_Police_Props_*`. |
| `/Game/Modular_Police/Mesh/Male/Belts_Modules/Police_Belt_Drops_*/SKM_Police_Belt_Drops_*` | Police-named belt/drop bundles; probe found `MI_Police_Props_*`, `MI_Pistol`, and holster references. |
| `/Game/Modular_Police/Mesh/Male/Drops_Modules/Police_Drops_*/SKM_Police_Drops_*` | Police-named pouch/drop families; keep out of neutral v1 until visual logo/patch review. |
| `/Game/Modular_Police/Mesh/Male/Backpacks_Modules/PatchBackNew/SKM_PatchBackNew` | Patch item; bundle also contains `MI_Patch_Police`. Exclude until a no-police-patch variant is visually verified. |
| `/Game/Modular_Police/Mesh/Static_Meshes/Weapon/**/SM_*` | Weapons/props are explicitly out of customization v1. |
| `/Game/Modular_Police/Mesh/Female/**` | Female body/skeleton flow; outside current male Worker customization scope. |

## Recommended V1 Additions

Only consider these after a read-only editor/MCP validation confirms skeleton, bounds, material slots, and clipping against the current Worker body. The safest first pass is neutral SKM-only, male-only, standalone accessory modules.

All 10 exact asset paths in this table should be treated as the neutral validation set; none of them are police vest, police cap/hat/beret, full outfit, weapon, or explicit police clothing.

| Asset Path | Slot | Why first |
|---|---|---|
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Classic/SKM_Glasses_Classic` | Glasses / face | Neutral low-identity glasses. |
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Classic/SKM_Glasses_SunGlass` | Glasses / face | Neutral sunglasses. |
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Combat/SKM_Glasses_Combat_Glass` | Glasses / face protection | Utility eye protection without police/badge material reference in probe. |
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Glasses_Combat/SKM_Glasses_Combat_Orange` | Glasses / face protection | Utility eye protection; orange variant fits Avariika accent direction. |
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Goggles/SKM_Goggles_Black` | Goggles / face protection | Neutral utility goggles; needs headwear conflict validation. |
| `/Game/Modular_Police/Mesh/Male/Face_Modules/Goggles/SKM_Goggles_Bege` | Goggles / face protection | Neutral utility goggles; lighter worker-friendly variant. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Headphones_Micro/SKM_Headphones_Micro_Black` | Headset | Neutral communications/work headset. |
| `/Game/Modular_Police/Mesh/Male/Head_Modules/Headphones_Micro/SKM_Headphones_Micro_Gray` | Headset | Neutral communications/work headset. |
| `/Game/Modular_Police/Mesh/Male/Hips_Modules/Bag_Hip/SKM_Bag_Hip_Black` | Hip bag / utility pouch | Generic utility hip bag; conflicts can be contained to hip gear. |
| `/Game/Modular_Police/Mesh/Male/Belts_Modules/Belt_Tactical/SKM_Belt_Tactical_Black` | Belt / hip gear | Neutral tactical belt; useful Worker utility item after hip conflict validation. |

## Do Not Use Yet

- Do not use any asset that references `MI_Police_Badge`, `MI_Police_Cap`, `MI_Police_Hat`, `MI_Police_Vest`, or `MI_Police_Props` in the neutral Worker list.
- Do not use `/Game/Modular_Police/Mesh/Male/Presets/SKM_Modular_Police_Preset_Male_*` as normal tops, pants, or vests. They are merged/full presets and need a separate mode similar to Overalls.
- Do not use `/Game/Modular_Police/Mesh/Male/Presets/SKM_Modular_Police_Preset_Male_Body_*` in v1. They are full body preset meshes, not modular clothing.
- Do not use `Police_Shirt*` or `Police_Pants*`; they are police clothing, not neutral Worker clothing.
- Do not use police caps, police hats, police warm hats, police berets, or `Cap_02_Police` until a no-badge/no-logo material variant is explicitly verified.
- Do not use police or bulletproof vests for neutral Worker customization; they visually push the character toward police/SWAT/security.
- Do not use gloves, arms, wrist, or watch modules until arms tattoo/watch/glove conflicts are explicit.
- Do not use hair, beard, balaclava, gas mask, or face-rig assets until face/head clipping and mask/beard/respirator rules are expanded.
- Do not use backpacks, belt drops, holsters, or static weapons in customization v1. They need equipment/attachment semantics and can collide with Worker vest, bag banana, hip gear, and gameplay inventory.
- Do not use female assets for the current Worker path. They are a separate skeleton/body flow.
- Do not use static mesh duplicates as direct replacements for SKM modules. They need socket placement, attachment ownership, replication, and preview handling.

## Conflict Rules

Preliminary rules if this bundle is later integrated:

| Rule area | Conflict rule |
|---|---|
| Headwear | Exactly one hat/cap/beret/helmet at a time. Headwear should hide or block incompatible hair variants. |
| Helmets | Helmets conflict with hats, caps, berets, most hair, headset, helmet glass/goggle variants unless explicitly authored as a combo. |
| Glasses/goggles/masks | Use one face accessory group: glasses, goggles, gas mask, balaclava, respirator/mouth mask. |
| Beard/moustache | Beard and moustache conflict with respirator, balaclava mouth, gas mask, and any full-face mask. |
| Chest gear | Police/bulletproof vests are excluded from neutral v1. Any later neutral chest gear would conflict with Worker vest, bag banana/chest bag, backpacks/back gear, and overalls/full presets. |
| Upper/lower clothing | Police shirts and police pants are excluded from neutral v1. Any later neutral clothing replacement would conflict with current top/lower, overalls, and merged/full presets. |
| Arms/gloves/watch | Long gloves, short gloves, wrist parts, military watches, arms tattoo variants, and short-sleeve compatibility need one explicit arms conflict matrix. |
| Hip/belt gear | Belt, belt drops, hip bag, holster, and pouch modules are mutually exclusive unless a specific authored combination is validated. |
| Full presets | Full presets/merged body presets are a separate mode. They should disable normal top, bottom, chest, hip, arms, and possibly headwear depending the preset. |
| Weapons/props | Weapons and props stay out of customization v1. They belong to gameplay inventory/equipment or decorative preview-only logic after a separate decision. |

## Next Step

Minimal safe integration step, only after explicit command:

1. Run a read-only editor/MCP validation pass on the 10 neutral recommended SKM assets.
2. Confirm each candidate's actual skeleton reference, bounds/scale, material slot count, and visual clipping against the current Worker male body.
3. If validation passes, add a tiny data-only neutral candidate list behind existing conflict rules. Do not touch Apply/save/reopen, randomize, UI layout, or Worker customization behavior until that list is explicitly approved.

No build is required for this audit because only documentation was changed.
