# Mobility Retarget V2 Validation

Date: 2026-06-28

## Scope

Validated the clean v2 retarget output under:

`/Game/Avariika/Anim/Locomotion/Mobility_v2/`

No gameplay wiring, `ABP_Worker` edits, C++ edits, montages, or commits were made.

## Created Pipeline Assets

- Source IK Rig copy: `/Game/Avariika/Anim/Locomotion/Mobility_v2/Rig/IK_MobilityUE4_v2.IK_MobilityUE4_v2`
- Retargeter: `/Game/Avariika/Anim/Locomotion/Mobility_v2/Rig/RTG_MobilityPro_To_Worker_v2.RTG_MobilityPro_To_Worker_v2`

The source IK Rig copy uses `Root` as the retarget root. The v2 retargeter targets `/Game/_Packs/WorkAnimations/Demo/Mannequins/Meshes/SK_Mannequin.SK_Mannequin`.

## Asset Validation

- Retargeted manifest count: 76 sequences
- Retarget failures: 0
- Missing source assets: 0
- Skeleton on representative outputs: `/Game/_Packs/WorkAnimations/Demo/Mannequins/Meshes/SK_Mannequin.SK_Mannequin`
- `validate_assets` reported no issues in its scan.

## Representative Clip Results

| Purpose | Source asset | Retargeted asset | Length | Root-motion result | Foot-slide / visual result | Result |
| --- | --- | --- | ---: | --- | --- | --- |
| Idle | `/Game/Mobility_01/Animation/In-Place/MOB1_M1_Stand_Relaxed_Idle_IP` | `/Game/Avariika/Anim/Locomotion/Mobility_v2/MPv2_MOB1_M1_Stand_Relaxed_Idle_IP` | 4.1000s | Preserved as disabled | Viewport screenshot attempted; no obvious asset/skeleton failure proven | Asset pass, visual inconclusive |
| Forward locomotion | `/Game/Mobility_01/Animation/In-Place/MOB1_M1_Walk_F_IP` | `/Game/Avariika/Anim/Locomotion/Mobility_v2/MPv2_MOB1_M1_Walk_F_IP` | 1.2000s | Preserved as disabled | Viewport screenshot attempted; no obvious asset/skeleton failure proven | Asset pass, visual inconclusive |
| Crouch locomotion | `/Game/Mobility_01/Animation/In-Place/MOB1_M1_CrouchWalk_F_IP` | `/Game/Avariika/Anim/Locomotion/Mobility_v2/MPv2_MOB1_M1_CrouchWalk_F_IP` | 1.2000s | Preserved as disabled | Viewport screenshot attempted; no obvious asset/skeleton failure proven | Asset pass, visual inconclusive |
| L90 turn | `/Game/Mobility_01/Animation/Root_Motion/MOB1_M1_Stand_Relaxed_L90` | `/Game/Avariika/Anim/Locomotion/Mobility_v2/MPv2_MOB1_M1_Stand_Relaxed_L90` | 2.8000s | Source and target root motion disabled; source root track measured 0 yaw and 0 XY | Could not confirm foot lift/no slide; viewport capture remained clipped | Fail for gameplay integration |
| R90 turn | `/Game/Mobility_01/Animation/Root_Motion/MOB1_M1_Stand_Relaxed_R_90` | `/Game/Avariika/Anim/Locomotion/Mobility_v2/MPv2_MOB1_M1_Stand_Relaxed_R_90` | 3.1667s | Source and target root motion disabled; source root track measured 0 yaw and 0 XY | Could not confirm foot lift/no slide; viewport capture remained clipped | Fail for gameplay integration |
| L180 turn | `/Game/Mobility_01/Animation/Root_Motion/MOB1_M1_Stand_Relaxed_L180` | `/Game/Avariika/Anim/Locomotion/Mobility_v2/MPv2_MOB1_M1_Stand_Relaxed_L180` | 3.7667s | Source and target root motion disabled; source root track measured 0 yaw and 0 XY | Could not confirm foot lift/no slide; viewport capture remained clipped | Fail for gameplay integration |
| R180 turn | `/Game/Mobility_01/Animation/Root_Motion/MOB1_M1_Stand_Relaxed_R_180` | `/Game/Avariika/Anim/Locomotion/Mobility_v2/MPv2_MOB1_M1_Stand_Relaxed_R_180` | 3.6667s | Source and target root motion disabled; source root track measured 0 yaw and 0 XY | Could not confirm foot lift/no slide; viewport capture remained clipped | Fail for gameplay integration |

## Turn Clip Decision

Do not integrate the v2 turn clips into `ABP_Worker` yet.

The v2 retargeted turn assets are structurally valid and retargeted to the correct skeleton, but they do not satisfy the requested turn validation gate because:

1. the source `Root_Motion` turn clips measured no root-track turn motion;
2. root-motion enable state is false on source and v2 target turns;
3. viewport visual proof of foot lift/no slide was not obtained in this session.

## Reconfiguration Needed

- Find or import a Mobility Pro source variant whose root bone actually rotates/translates for turn-in-place, or accept these clips as non-root-motion pose turns and design the gameplay/AnimBP around that.
- Re-open the v2 retargeter in the editor and manually inspect source/target retarget poses, especially legs and feet, because the target Manny pose assets logged stale-pose warnings during audit.
- Re-run visual validation in the Animation Editor for `L90`, `R90`, `L180`, and `R180` before using them in gameplay.

## Safe Next Plan

1. Do not wire these turn clips into `ABP_Worker` until visual preview passes.
2. In the Animation Editor, compare source Mobility turns against v2 turns at start/mid/end frames.
3. If the turn clips are intended to be non-root-motion, integrate them later as state-machine clips with explicit actor yaw handling, not as root-motion montages.
4. If true root-motion turns are required, locate/import true root-motion turn source clips first, then retarget through `RTG_MobilityPro_To_Worker_v2`.
