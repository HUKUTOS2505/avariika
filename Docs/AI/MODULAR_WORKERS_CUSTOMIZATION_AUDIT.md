# Modular Workers Customization Audit

Read-only audit for future Avariika character customization.

This pass did not intentionally change C++, Blueprints, WBP, maps, materials, assets, config, `.uproject`, or plugins; did not build; did not run PIE; and did not save `.uasset`/`.umap`. The only created file is this document.

## A. Источники аудита

Primary source requested by the task:

- `/Script/Engine.World'/Game/Modular_Workers/Maps/Preview_Assets.Preview_Assets'`

Cross-check sources:

- `/Game/Modular_Workers/Demo/Blueprint/CBP_Male_Quantum_Character`
- `/Game/Modular_Workers/Demo/Blueprint/WB_Male_Modular_Character_Builder`
- `/Game/Modular_Workers/Demo/Blueprint/WB_Male_Character_Presets`
- `/Game/Modular_Workers/Maps/MegaBundle_Character_Builder`
- `/Game/Modular_Workers/Maps/Preview_Presets`
- `/Game/Modular_Workers/Mesh/Male/`
- Existing read-only audit: `Docs/AI/WORKER_BUILDER_MAP_AUDIT.md`

Read-only methods used:

- MCP `list_actors` on `Preview_Assets`.
- Filesystem inventory of `Content/Modular_Workers/Mesh/Male`.
- Binary string scan of requested `.uasset` / `.umap` files for slot labels and `SKM_*` references.
- Existing previous read-only Blueprint audit for exact `CBP_Male_Quantum_Character` component names.

`Preview_Assets` facts:

- The map loaded as `/Game/Modular_Workers/Maps/Preview_Assets.Preview_Assets`.
- MCP reported `315` actors.
- It contains standalone `SkeletalMeshActor` and `StaticMeshActor` placements for modular assets, not a single character-builder-only preview.
- Confirmed representative placed actors include:
  - `SKM_Quantum_Feet`, `SKM_Quantum_Hands_Afro`, `SKM_Quantum_FullHands`, `SKM_Quantum_FullHands_Tatto`;
  - `SKM_Arms_Gloves_Print_Gray`, `SKM_Watches_Military_L_Bege`, `SKM_Watches_Military_R_Bege`;
  - `SKM_TShirt_Black`, `SKM_Tshirt_Tucked_Green`, `SKM_Shirt_RolledUp_Red`;
  - `SKM_Jacket_M65_Bege`, `SKM_Jacket_Worker_Green`, `SKM_Jacket_Worker_Hood_2_Gray`;
  - `SKM_Vest_Worker_Yellow`, `SKM_Vest_Worker_LimeGreen`, `SKM_Bag_Banana_Black`;
  - `SKM_Pants_Worker_Bege`, `SKM_Pants_Worker_Gray`, `SKM_Jumpsuit_DarkBlue`;
  - `SKM_Worker_Shirt_Jumpsuit_2`, `SKM_Worker_Shirt_Jumpsuit_3`, `SKM_Worker_Tshirt_Jumpsuit_1`, `SKM_Worker_Tshirt_Pants_7`;
  - `SKM_Overalls_Tshirt_Boots_3`;
  - `SKM_Bag_Hip_Bege`;
  - `SKM_Glasses_Aviator`, `SKM_Glasses_Aviator_Gold`, `SKM_Glasses_Classic`, `SKM_Glasses_Worker_Orange`;
  - `SKM_Beard_Long`;
  - `SKM_Hair_Mohawk`;
  - `SKM_Cap_Blue`, `SKM_Hat_Cowboy`, `SKM_Hat_Flat_Brown`, `SKM_Hat_Up`, `SKM_Helmet_Worker_Orange`, `SKM_Helmet_Worker_Yellow`;
  - static previews such as `SM_Cap_Street`, `SM_Helmet_Worker_Yellow`, `SM_Headphones_Worker_Orange`, `SM_Cowboy_Hat`.

Important limitation: this audit did not visually test every possible combination. Conflict findings are based on actual asset grouping, component/slot evidence, merged mesh names, and known shared physical zones. Combinations marked “needs visual validation” should be tested in a later non-read-only pass.

## B. Таблица всех реальных слотов

Exact `CBP_Male_Quantum_Character` components found by prior read-only Blueprint audit:

- Body/customization components: inherited `Mesh`, `Preset`, `Arms`, `Clothes`, `Pants`, `Gloves`, `Hair`, `Hat`, `Glasses`, `Headphones`, `Balaclava`, `Scarf`, `Gasmask`, `Watch`.
- Equipment/module components: `Backpack`, `Armor`, `Bulletproof`, `Bulletproof_Drops`, `Tactical_Belt`, `Hip_Left`, `Hip_Right`, `Arm_Module_Right`, `Helmet`, `Helmet_Module`, `Helmet_Drops`, `Helmet_Headphones`, `Patch_Back`, `Patch_L_Arm`, `Patch_R_Arm`, `Patch_Cap`, `Patch_Helmetn`.
- `Rifle` exists as a `StaticMeshComponent`.
- Construction Script uses Leader Pose / Master Pose style modular following from the main mesh.

| Logical slot for Avariika | Exact CBP component evidence | Real folders | Variants | None/Empty | Separate or merged | Notes |
|---|---|---|---:|---|---|---|
| Head Type | inherited `Mesh`, `Preset`; builder label `Head Type` | `Body/European`, `Body/Afro`, `Body/Quantum_FaceRig` | 24 body/head/face-rig meshes total | No safe None for base body/head | Separate base meshes and full/body pieces | European/Afro head/body variants exist. FaceRig variants also exist. |
| Hair | `Hair` | `Head_Modules/Hair` | 5 | Yes by clearing component/index | Separate | `Hair_Skin` is effectively bald/covered-safe option. |
| Hats / Hair | `Hat`, `Helmet`, `Helmet_Module`, `Helmet_Drops`, `Helmet_Headphones` | `Head_Modules/Helmet_Worker`, `Cap`, `Hat_Cowboy`, `Hat_Farmer`, `Hat_Flat`, `Hat_Up`, `Warm_Hat` | 22 SKM headwear variants | Yes | Separate | Includes both skeletal and static preview versions for many hats. |
| Glasses | `Glasses` | `Face_Modules/Glasses_Aviator`, `Glasses_Classic`, `Glasses_Worker` | 7 SKM | Yes | Separate | Worker safety glasses, classic glasses, aviators. |
| Headphones | `Headphones`, `Helmet_Headphones` | `Head_Modules/Headphones_Workers`, `Headphones_Workers_Micro` | 8 SKM | Yes | Separate | Ear protection/headset, with and without microphone. |
| Balaclava / Beard | `Balaclava`; builder strings include `Beard` | `Face_Modules/Beard` | 7 SKM | Yes | Separate beard/moustache meshes | No confirmed balaclava skeletal mesh folder was found under `Mesh/Male`; this slot name is misleading for current assets. |
| Gasmask | `Gasmask` | `Face_Modules/Respirator` | 2 SKM | Yes | Separate | The actual assets are respirators. UI may call this “Gasmask”. |
| Arms | `Arms` | `Arms_Modules/Arms`; also body arms/full hands in `Body/European`, `Body/Afro` | 4 dedicated arms + body hand variants | No for body baseline; yes for optional module | Separate, except glove-print merged arms | Contains hands/forearms, tattoo and ornament variants. Not gloves. |
| Gloves | `Gloves` | `Arms_Modules/Gloves_Worker`, `Gloves_Worker_Print` | 6 SKM | Yes | Mixed | `Gloves_Worker` are gloves; `Gloves_Worker_Print` are `SKM_Arms_Gloves_Print_*`, i.e. arms+gloves merged. |
| Watch | `Watch` | `Arms_Modules/Watches`, `Arms_Modules/Wrist` | 6 SKM | Yes | Separate | Watches exist left/right, beige/black; wrist filler meshes also exist. |
| Clothes | `Clothes` | `Clothes_Modules/Tshirt`, `Tshirt_Tucked`, `Shirt_RolledUp`, `Jacket_M65`, `Jacket_Worker`, `Jacket_Worker_Hood_1`, `Jacket_Worker_Hood_2` | 42 SKM | Yes | Separate upper-body meshes | Includes T-shirts, tucked T-shirts, shirts, jackets, hooded jackets. |
| Chest Module | Builder/preset strings `Chest Module`; CBP equipment includes `Bulletproof`, `Armor` | `Chest_Modules/Vest_Worker`, `Chest_Modules/Bag_Banana` | 6 SKM | Yes | Separate chest overlays | In this package this means hi-vis vest and banana/chest bag. Treat as one mutually exclusive chest-module slot. |
| Overalls | Builder string `Overalls`; no standalone CBP component named `Overalls` in prior audit | `Overalls_Modules/*` | 30 SKM | Yes by leaving overalls mode off | Merged outfit meshes | Merged top+bottom and sometimes boots. Blocks separate upper/lower choices. |
| Pants | `Pants` | `Pants_Modules/Jeans`, `Jumpsuit`, `Pants_Worker`, `Pants_Worker_Full` | 21 SKM | Yes | Separate lower-body meshes; some full lower coverage | No standalone boots folder found. |
| Left Hip Drops | `Hip_Left`; builder strings `Left Hip Drops` | `Hips_Modules/Bag_Hip` | 3 SKM | Yes | Separate | Hip bag/pouches: beige, black, green. |
| Additional equipment | `Backpack`, `Armor`, `Bulletproof`, `Bulletproof_Drops`, `Tactical_Belt`, `Hip_Right`, `Arm_Module_Right`, patches | Not all represented in `Mesh/Male` inventory above | Unknown in this pass | Yes by clearing | Separate if present | These are in CBP but outside the specific user-facing first-pass list. Need separate equipment audit before exposing. |
| Presets | `Preset` | `Presets` | 25 full + 25 body-only | No for selected preset; yes as “not using preset mode” | Merged full worker meshes | `SKM_Worker_Male_1..25` are not modular data records. |

## C. Таблица содержимого слотов и asset paths

| Folder under `/Game/Modular_Workers/Mesh/Male` | Count | Examples |
|---|---:|---|
| `Arms_Modules/Arms` | 4 | `SKM_Quantum_Hands`, `SKM_Quantum_Hands_Afro`, `SKM_Quantum_Hands_Ornament`, `SKM_Quantum_Hands_Tatto` |
| `Arms_Modules/Gloves_Worker` | 2 | `SKM_Gloves_Worker`, `SKM_Gloves_Worker_Brown` |
| `Arms_Modules/Gloves_Worker_Print` | 4 | `SKM_Arms_Gloves_Print_Gray`, `SKM_Arms_Gloves_Print_Green`, `SKM_Arms_Gloves_Print_Orange`, `SKM_Arms_Gloves_Print_Yellow` |
| `Arms_Modules/Watches` | 4 | `SKM_Watches_Military_L_Bege`, `SKM_Watches_Military_L_Black`, `SKM_Watches_Military_R_Bege`, `SKM_Watches_Military_R_Black` |
| `Arms_Modules/Wrist` | 2 | `SKM_Quantum_Wrist`, `SKM_Quantum_Wrist_Afro` |
| `Body/Afro` | 8 | `SKM_Quantum_Body_Bottom_Afro`, `SKM_Quantum_Body_Full_Afro`, `SKM_Quantum_Feet_Afro`, `SKM_Quantum_FullHands_Afro`, `SKM_Quantum_Head_Afro` |
| `Body/European` | 10 | `SKM_Quantum_Body_Bottom`, `SKM_Quantum_Body_Full`, `SKM_Quantum_Feet`, `SKM_Quantum_FullHands`, `SKM_Quantum_FullHands_Tatto` |
| `Body/Quantum_FaceRig` | 6 | `SKM_Quantum_FaceRig`, `SKM_Quantum_FaceRig_Afro`, `SKM_Quantum_FaceRig_Beard`, `SKM_Quantum_FaceRig_Beard_Afro`, `SKM_Qunatum_FaceRig_Body` |
| `Chest_Modules/Bag_Banana` | 2 | `SKM_Bag_Banana_Bege`, `SKM_Bag_Banana_Black` |
| `Chest_Modules/Vest_Worker` | 4 | `SKM_Vest_Worker_Blue`, `SKM_Vest_Worker_LimeGreen`, `SKM_Vest_Worker_Orange`, `SKM_Vest_Worker_Yellow` |
| `Clothes_Modules/Jacket_M65` | 3 | `SKM_Jacket_M65_Bege`, `SKM_Jacket_M65_Black`, `SKM_Jacket_M65_Green` |
| `Clothes_Modules/Jacket_Worker` | 6 | `SKM_Jacket_Worker_Bege`, `SKM_Jacket_Worker_Blue`, `SKM_Jacket_Worker_Gray`, `SKM_Jacket_Worker_Green`, `SKM_Jacket_Worker_LightGreen` |
| `Clothes_Modules/Jacket_Worker_Hood_1` | 6 | `SKM_Jacket_Worker_Hood_1_Bege`, `SKM_Jacket_Worker_Hood_1_Blue`, `SKM_Jacket_Worker_Hood_1_Gray`, `SKM_Jacket_Worker_Hood_1_Green`, `SKM_Jacket_Worker_Hood_1_LightGr` |
| `Clothes_Modules/Jacket_Worker_Hood_2` | 6 | `SKM_Jacket_Worker_Hood_2_Bege`, `SKM_Jacket_Worker_Hood_2_Blue`, `SKM_Jacket_Worker_Hood_2_Gray`, `SKM_Jacket_Worker_Hood_2_Green`, `SKM_Jacket_Worker_Hood_2_LightGr` |
| `Clothes_Modules/Shirt_RolledUp` | 8 | `SKM_Shirt_RolledUp_Bege`, `SKM_Shirt_RolledUp_Black`, `SKM_Shirt_RolledUp_Blue`, `SKM_Shirt_RolledUp_Green`, `SKM_Shirt_RolledUp_Red` |
| `Clothes_Modules/Tshirt` | 6 | `SKM_TShirt_Bege`, `SKM_TShirt_Black`, `SKM_TShirt_Blue`, `SKM_TShirt_Gray`, `SKM_TShirt_Green` |
| `Clothes_Modules/Tshirt_Tucked` | 7 | `SKM_Tshirt_Tucked_Bege`, `SKM_Tshirt_Tucked_Black`, `SKM_Tshirt_Tucked_Blue`, `SKM_Tshirt_Tucked_Green`, `SKM_Tshirt_Tucked_Grey` |
| `Face_Modules/Beard` | 7 | `SKM_Beard`, `SKM_Beard_02`, `SKM_Beard_03`, `SKM_Beard_4`, `SKM_Beard_Long`, plus long/moustache variants |
| `Face_Modules/Glasses_Aviator` | 3 | `SKM_Glasses_Aviator`, `SKM_Glasses_Aviator_Brown`, `SKM_Glasses_Aviator_Gold` |
| `Face_Modules/Glasses_Classic` | 2 | `SKM_Glasses_Classic`, `SKM_Glasses_SunGlass` |
| `Face_Modules/Glasses_Worker` | 2 | `SKM_Glasses_Worker_Orange`, `SKM_Glasses_Worker_White` |
| `Face_Modules/Respirator` | 2 | `SKM_Respirator`, `SKM_Respirator_Black` |
| `Head_Modules/Cap` | 6 | `SKM_Cap_Bege`, `SKM_Cap_Black`, `SKM_Cap_Blue`, `SKM_Cap_Color`, `SKM_Cap_Red` |
| `Head_Modules/Hair` | 5 | `SKM_Hair_Back`, `SKM_Hair_Long`, `SKM_Hair_Mohawk`, `SKM_Hair_Short`, `SKM_Hair_Skin` |
| `Head_Modules/Hat_Cowboy` | 1 | `SKM_Hat_Cowboy` |
| `Head_Modules/Hat_Farmer` | 1 | `SKM_Hat_Cowboy` |
| `Head_Modules/Hat_Flat` | 3 | `SKM_Hat_Flat_Blue`, `SKM_Hat_Flat_Brown`, `SKM_Hat_Flat_Grey` |
| `Head_Modules/Hat_Up` | 1 | `SKM_Hat_Up` |
| `Head_Modules/Headphones_Workers` | 4 | `SKM_Headphones_Worker_Green`, `SKM_Headphones_Worker_Orange`, `SKM_Headphones_Worker_White`, `SKM_Headphones_Worker_Yellow` |
| `Head_Modules/Headphones_Workers_Micro` | 4 | `SKM_Headph_Worker_Mic_Gr`, `SKM_Headph_Worker_Mic_Orn`, `SKM_Headph_Worker_Mic_Wht`, `SKM_Headph_Worker_Mic_Yel` |
| `Head_Modules/Helmet_Worker` | 6 | `SKM_Helmet_Worker_Blue`, `SKM_Helmet_Worker_Green`, `SKM_Helmet_Worker_Orange`, `SKM_Helmet_Worker_Red`, `SKM_Helmet_Worker_White` |
| `Head_Modules/Warm_Hat` | 4 | `SKM_Warm_Hat_Bege`, `SKM_Warm_Hat_Black`, `SKM_Warm_Hat_Gray`, `SKM_Warm_Hat_Green` |
| `Hips_Modules/Bag_Hip` | 3 | `SKM_Bag_Hip_Bege`, `SKM_Bag_Hip_Black`, `SKM_Bag_Hip_Green` |
| `Overalls_Modules/Overalls_Shirt_Boots` | 5 | `SKM_Shirt_Jumpsuit_Boots_1` ... `_5` |
| `Overalls_Modules/Overalls_Shirt_Pants_Worker` | 3 | `SKM_Overalls_Shirt_Pants_Worker_1` ... `_3` |
| `Overalls_Modules/Overalls_Tshirt_Boots` | 5 | `SKM_Overalls_Tshirt_Boots_1` ... `_5` |
| `Overalls_Modules/Overalls_Worker_Shirt` | 5 | `SKM_Worker_Shirt_Jumpsuit_1` ... `_5` |
| `Overalls_Modules/Overalls_Worker_Tshirt` | 5 | `SKM_Worker_Tshirt_Jumpsuit_1` ... `_5` |
| `Overalls_Modules/Overalls_Worker_Tshirt_Pants` | 7 | `SKM_Worker_Tshirt_Pants_1` ... `_7` |
| `Pants_Modules/Jeans` | 5 | `SKM_Jeans`, `SKM_Jeans_Bege`, `SKM_Jeans_Black`, `SKM_Jeans_Blue`, `SKM_Jeans_Turn` |
| `Pants_Modules/Jumpsuit` | 4 | `SKM_Jumpsuit_Bege`, `SKM_Jumpsuit_Black`, `SKM_Jumpsuit_Blue`, `SKM_Jumpsuit_DarkBlue` |
| `Pants_Modules/Pants_Worker` | 6 | `SKM_Pants_Worker_Bege`, `SKM_Pants_Worker_Blue`, `SKM_Pants_Worker_Gray`, `SKM_Pants_Worker_Green`, `SKM_Pants_Worker_LightGreen` |
| `Pants_Modules/Pants_Worker_Full` | 6 | `SKM_Pants_Worker_Full_Bege`, `SKM_Pants_Worker_Full_Blue`, `SKM_Pants_Worker_Full_Gray`, `SKM_Pants_Worker_Full_Green`, `SKM_Pants_Worker_Full_LightGrn` |
| `Presets` | 50 | `SKM_Worker_Male_1..25`, `SKM_Worker_Male_Body_1..25` |

Clarifications for disputed names:

- **Balaclava / Beard:** actual mesh folder found is `Face_Modules/Beard`; it contains beards and moustache. A `Balaclava` component/label exists in the demo character/widget strings, but no confirmed balaclava skeletal mesh folder was found under `Mesh/Male`.
- **Arms:** actual arms assets are hands/forearms skin variants: normal, Afro, ornament, tattoo. Gloves are separate except `Gloves_Worker_Print`, which is arms+gloves merged.
- **Chest Module:** in the audited mesh folders this means `Vest_Worker` and `Bag_Banana`. It is not a backpack; it is chest-worn overlay equipment.
- **Left Hip Drops:** `Bag_Hip` variants: hip bag/pouch, not tools as separate objects.
- **Boots:** no standalone boots folder was found. Boots appear inside presets, `Overalls_*_Boots`, and likely full lower meshes/material slots.
- **Overalls:** merged outfit meshes. Names confirm combinations such as shirt+jumpsuit+boots, T-shirt+boots, shirt+pants worker, worker shirt jumpsuit, worker T-shirt jumpsuit, worker T-shirt+pants.

## D. Таблица конфликтов/зависимостей

| Combination | Status | Factual basis |
|---|---|---|
| Overalls vs Clothes | Hard conflict | Overalls mesh names include shirt/T-shirt and jumpsuit/pants combinations. They replace the ordinary upper-body clothing selection. |
| Overalls vs Pants | Hard conflict | Overalls include lower-body geometry; several folders are explicitly jumpsuit/pants/boots combinations. |
| Overalls with Boots in name vs footwear | Hard conflict | `Overalls_Shirt_Boots` and `Overalls_Tshirt_Boots` confirm boots are part of those meshes. There is no standalone boots slot to combine. |
| Clothes + Chest Module | Supported in principle | `Clothes_Modules` and `Chest_Modules` are separate folders and demo labels. Chest module is designed as overlay over upper clothing. |
| Vest_Worker + Bag_Banana | Conflict / one-of | Both are `Chest_Modules`; both occupy the torso/chest overlay zone. Treat as one slot, not two simultaneous slots. |
| Known “vest + card/map/chest module” style issue | Block until authored combo exists | No literal `card` mesh was found under `Mesh/Male`; the confirmed chest bag/map-like chest prop is `Bag_Banana`. It shares the chest-module zone with `Vest_Worker`, so vest + chest bag/card should be mutually exclusive or require a combined mesh. |
| Left Hip Drops + normal pants | Likely supported | Hip bag is its own `Hips_Modules/Bag_Hip`; normal pants are separate. Needs visual validation for clipping per pants family. |
| Left Hip Drops + Overalls/full lowers | Risk / block initially | Overalls/full lower meshes cover hip/waist zones. Hip bag may clip. |
| Watch + simple gloves | Conditional | Watches are separate wrist meshes; simple `SKM_Gloves_Worker*` may leave wrist space. Needs visual validation. |
| Watch + `Gloves_Worker_Print` | Conflict | Print gloves are named `SKM_Arms_Gloves_Print_*`, so they already include arms/gloves and can replace the wrist/arm area. |
| Arms tattoo/ornament + simple gloves | Likely supported | Arms variants are separate and simple gloves are separate. Validate material/skin continuity. |
| Arms tattoo/ornament + print gloves | Conflict | Print gloves are arms+gloves merged and will override/cover the separate arm variant. |
| Beard/moustache + respirator/gasmask | Conflict risk | Beard/moustache and respirator occupy lower-face/mouth zone. Long beard should be blocked with respirator. |
| Balaclava + beard/gasmask | Not implementable yet | No confirmed separate balaclava mesh in `Mesh/Male`. |
| Hair + helmets/caps/hats | Conditional clipping | Hair and headwear are separate; bulky hair (`Hair_Long`, `Hair_Mohawk`, `Hair_Back`) can occupy the same volume as hats/helmet. Use `Hair_Skin`/None when headwear is selected until validated. |
| Headphones + helmet/hat | Conditional/hard by item | Headphones occupy ears/sides; helmets/hats occupy top/sides. Do not freely combine until specific item pairs are validated. |
| Glasses + respirator | Conditional | Glasses and respirator are separate face meshes. Worker safety glasses may combine better than aviators/classic glasses; validate before allowing all. |
| Preset full mesh + modular slots | Mode conflict | Presets are full worker skeletal meshes, not modular state records. Do not layer modular parts on top of full preset mesh unless explicitly using it as base body and hiding conflicting components. |

## E. Таблица 25 готовых preset workers

Facts:

- Full presets are `SKM_Worker_Male_1` through `SKM_Worker_Male_25`.
- Body-only matching meshes are `SKM_Worker_Male_Body_1` through `SKM_Worker_Male_Body_25`.
- These are merged skeletal meshes. They can be used as ready-made workers.
- They are not exact modular recipes. A preset can only be approximately reconstructed by matching visible/material slot names to modular assets.

| # | Full preset | Body-only preset | Merged? | Can decompose exactly? | Suggested use |
|---:|---|---|---|---|---|
| 1 | `SKM_Worker_Male_1` | `SKM_Worker_Male_Body_1` | Yes | No | Ready preset or visual reference |
| 2 | `SKM_Worker_Male_2` | `SKM_Worker_Male_Body_2` | Yes | No | Ready preset or visual reference |
| 3 | `SKM_Worker_Male_3` | `SKM_Worker_Male_Body_3` | Yes | No | Ready preset or visual reference |
| 4 | `SKM_Worker_Male_4` | `SKM_Worker_Male_Body_4` | Yes | No | Ready preset or visual reference |
| 5 | `SKM_Worker_Male_5` | `SKM_Worker_Male_Body_5` | Yes | No | Ready preset or visual reference |
| 6 | `SKM_Worker_Male_6` | `SKM_Worker_Male_Body_6` | Yes | No | Ready preset or visual reference |
| 7 | `SKM_Worker_Male_7` | `SKM_Worker_Male_Body_7` | Yes | No | Ready preset or visual reference |
| 8 | `SKM_Worker_Male_8` | `SKM_Worker_Male_Body_8` | Yes | No | Ready preset or visual reference |
| 9 | `SKM_Worker_Male_9` | `SKM_Worker_Male_Body_9` | Yes | No | Ready preset or visual reference |
| 10 | `SKM_Worker_Male_10` | `SKM_Worker_Male_Body_10` | Yes | No | Ready preset or visual reference |
| 11 | `SKM_Worker_Male_11` | `SKM_Worker_Male_Body_11` | Yes | No | Ready preset or visual reference |
| 12 | `SKM_Worker_Male_12` | `SKM_Worker_Male_Body_12` | Yes | No | Ready preset or visual reference |
| 13 | `SKM_Worker_Male_13` | `SKM_Worker_Male_Body_13` | Yes | No | Ready preset or visual reference |
| 14 | `SKM_Worker_Male_14` | `SKM_Worker_Male_Body_14` | Yes | No | Ready preset or visual reference |
| 15 | `SKM_Worker_Male_15` | `SKM_Worker_Male_Body_15` | Yes | No | Ready preset or visual reference |
| 16 | `SKM_Worker_Male_16` | `SKM_Worker_Male_Body_16` | Yes | No | Ready preset or visual reference |
| 17 | `SKM_Worker_Male_17` | `SKM_Worker_Male_Body_17` | Yes | No | Ready preset or visual reference |
| 18 | `SKM_Worker_Male_18` | `SKM_Worker_Male_Body_18` | Yes | No | Ready preset or visual reference |
| 19 | `SKM_Worker_Male_19` | `SKM_Worker_Male_Body_19` | Yes | No | Ready preset or visual reference |
| 20 | `SKM_Worker_Male_20` | `SKM_Worker_Male_Body_20` | Yes | No | Ready preset or visual reference |
| 21 | `SKM_Worker_Male_21` | `SKM_Worker_Male_Body_21` | Yes | No | Ready preset or visual reference |
| 22 | `SKM_Worker_Male_22` | `SKM_Worker_Male_Body_22` | Yes | No | Ready preset or visual reference |
| 23 | `SKM_Worker_Male_23` | `SKM_Worker_Male_Body_23` | Yes | No | Ready preset or visual reference |
| 24 | `SKM_Worker_Male_24` | `SKM_Worker_Male_Body_24` | Yes | No | Ready preset or visual reference |
| 25 | `SKM_Worker_Male_25` | `SKM_Worker_Male_Body_25` | Yes | No | Ready preset or visual reference |

## F. Что пользователь реально сможет менять отдельно

| User-facing item | Status | Evidence |
|---|---|---|
| голова | доступно отдельно | `Body/European`, `Body/Afro`, `Body/Quantum_FaceRig` include head/body variants. |
| волосы | доступно отдельно, но конфликтует | `Head_Modules/Hair`; clipping risk with headwear. |
| головной убор | доступно отдельно | Helmet/cap/hat/warm hat folders. |
| очки | доступно отдельно | Aviator/classic/worker glasses folders. |
| наушники | доступно отдельно, но конфликтует | Headphones/headphones with mic folders; headwear conflict risk. |
| борода | доступно отдельно, но конфликтует | Beard/moustache meshes; lower-face conflict with respirator. |
| балаклава | отсутствует в пакете как подтверждённый mesh | Label/component exists, but no `Balaclava` mesh folder found under `Mesh/Male`. |
| противогаз | доступно отдельно | Actual folder is `Face_Modules/Respirator`; UI can call it respirator/gasmask. |
| руки / татуировки | доступно отдельно | `Arms_Modules/Arms` includes tattoo/ornament variants. |
| перчатки | доступно отдельно, но конфликтует | Simple gloves are separate; print gloves are arms+gloves merged. |
| часы | доступно отдельно, но конфликтует | Watches exist L/R; conflict with merged print gloves. |
| футболка / базовый верх | доступно отдельно | T-shirt and tucked T-shirt folders. |
| верхняя одежда | доступно отдельно | Rolled shirts and jackets. |
| жилет / нагрудный модуль | доступно отдельно, но конфликтует | Vest and banana/chest bag share chest module zone. |
| комбинезон | доступно только частью merged mesh | Overalls folders are merged combinations. |
| штаны | доступно отдельно | Jeans, jumpsuit, worker pants, full worker pants. |
| обувь | доступно только частью merged mesh / preset | No standalone boots folder found. |
| набедренная сумка / подсумки | доступно отдельно, но конфликтует | `Hips_Modules/Bag_Hip`; validate against overalls/full lower meshes. |

## G. Рекомендованная структура левого меню

The proposed structure in the prompt is mostly correct, with one correction: “Балаклава” should not be shown as a first-version item until a real mesh is found.

Recommended left menu:

- **ПРЕСЕТЫ**: full `SKM_Worker_Male_1..25` looks.
- **ГОЛОВА**: head type, skin tone, base face/body type.
- **ВОЛОСЫ И ГОЛОВНОЙ УБОР**: hair, helmet/cap/hat/warm hat, headphones with conflict rules.
- **ЛИЦО И МАСКИ**: glasses, beard/moustache, respirator/gasmask. Hide balaclava for v1.
- **РУКИ И АКСЕССУАРЫ**: arms/tattoos, gloves, watches.
- **ВЕРХ**: T-shirt/shirt/jacket and chest module.
- **НИЗ И ОБУВЬ**: pants families; show footwear as included/locked, not separately selectable.
- **НАБЕДРЕННОЕ СНАРЯЖЕНИЕ**: hip bag/pouches.
- **ЦЕЛЬНЫЕ КОМПЛЕКТЫ**: overalls and other merged outfit modes.

## H. Рекомендованное контекстное содержимое правой панели

| Left category | Right panel content |
|---|---|
| ПРЕСЕТЫ | Grid Worker 01-25; action “Выбрать комплект”. Optional future action “использовать как основу”, but only after modular recipe mapping exists. |
| ГОЛОВА | Head type: European/Afro/FaceRig-compatible; skin tone; reset. |
| ВОЛОСЫ И ГОЛОВНОЙ УБОР | Hair style; no hair / `Hair_Skin`; headwear family tabs: каска, кепка, шляпа, тёплая шапка; headphones toggle/family with conflict warnings. |
| ЛИЦО И МАСКИ | Glasses family; beard/moustache; respirator/gasmask. Do not expose balaclava in v1. |
| РУКИ И АКСЕССУАРЫ | Arm skin/tattoo/ornament; glove family; watch side/color. Disable watch and arm tattoo controls when selected gloves mesh is `SKM_Arms_Gloves_Print_*`. |
| ВЕРХ | Top family: T-shirt, tucked T-shirt, rolled shirt, jackets. Chest module subslot: none, vest, chest bag. |
| НИЗ И ОБУВЬ | Jeans, jumpsuit, worker pants, full worker pants. Footwear state: included / not separately editable. |
| НАБЕДРЕННОЕ СНАРЯЖЕНИЕ | Hip bag: none, beige, black, green. Warn/block with overalls/full lower until validated. |
| ЦЕЛЬНЫЕ КОМПЛЕКТЫ | Overalls grouped by family: shirt+boots, T-shirt+boots, shirt+pants worker, worker shirt jumpsuit, worker T-shirt jumpsuit, worker T-shirt+pants. Selecting locks upper/lower/footwear. |

Disable/hide rules:

- If **Overalls** is selected: disable/hide Clothes, Pants, footwear, and risky chest/hip overlays by default.
- If **Chest Module = Vest**: disable chest bag/map/card-style chest module.
- If **Chest Module = Bag_Banana**: disable vest and any future chest overlay.
- If **Gloves = `SKM_Arms_Gloves_Print_*`**: disable separate Arms tattoo/ornament and Watch.
- If **Respirator/Gasmask** is active: disable long beard; allow moustache/short beard only after validation.
- If **Helmet/Hat/Headphones** is active: prefer `Hair_Skin`/None or block bulky hair until item-pair validation.

Russian UI naming:

- `Chest Module` -> **Нагрудное снаряжение**
- `Bag_Banana` -> **Нагрудная сумка**
- `Left Hip Drops` / `Bag_Hip` -> **Набедренная сумка**
- `Respirator` / `Gasmask` -> **Респиратор**
- `Overalls` -> **Цельный комплект** or **Комбинезон**
- `Gloves_Worker_Print` -> **Перчатки с рукавами**
- `Hair_Skin` -> **Без волос / под каску**

## I. Какие элементы первой версии кастомизации делать сразу

- Presets Worker 01-25 as full ready looks.
- Head type and skin tone.
- Hair with safe none/`Hair_Skin` state.
- One headwear slot: helmet/cap/hat/warm hat.
- Glasses as one face accessory slot.
- Respirator/gasmask as lower-face slot with beard conflict lock.
- Beard/moustache, but block with respirator initially.
- Arms variants, including tattoo/ornament.
- Simple gloves and print gloves, with watch/arms locks for print gloves.
- Watch variants, only when not using merged arms+gloves.
- Upper clothing families.
- One chest module selector: none/vest/chest bag.
- Pants families.
- Hip bag variants.
- Overalls/full outfit mode as mutually exclusive with top/lower/footwear.

## J. Какие элементы отложить из-за merged meshes или конфликтов

- Standalone shoes/boots: no independent boots folder found under `Mesh/Male`.
- Balaclava: label exists, but no confirmed mesh asset was found in the audited mesh folders.
- Exact decomposition of 25 presets into modular choices: presets are merged meshes, not data recipes.
- Free mixing of vest + chest bag/map/card: same chest-module zone; needs authored combined meshes or mutual exclusion.
- Watch with print gloves / merged arms+gloves: block or validate per item.
- Long/bulky hair under helmets/hats/headphones: needs visual validation.
- Respirator with long beard: block until visually validated.
- Overalls with hip/chest overlays: block by default until individual combinations are checked.
- Additional CBP equipment slots (`Backpack`, `Armor`, `Bulletproof`, `Tactical_Belt`, patches, rifle): present in CBP but not part of the requested first customization pass; audit separately before exposing.
