# Mobility Retarget V2 Audit

Date: 2026-06-28

## Scope

This audit covers a clean Mobility Pro locomotion retarget pipeline for the active Worker animation target. It is read-only for assets; no `.uasset` files were created or modified during this phase.

## Source And Target

- Mobility Pro source skeletal mesh: `/Game/Mobility_01/Character/Mesh/SK_Mannequin.SK_Mannequin`
- Mobility Pro source skeleton: `/Game/Mobility_01/Character/Mesh/UE4_Mannequin_Skeleton.UE4_Mannequin_Skeleton`
- Worker game mesh checked: `/Game/Modular_Workers/Mesh/Male/Presets/SKM_Worker_Male_1.SKM_Worker_Male_1`
- Worker game mesh skeleton: `/Game/Modular_Workers/Mesh/Male/SK_Male_Quantum_Character_Skeleton.SK_Male_Quantum_Character_Skeleton`
- Active `ABP_Worker` target skeleton: `/Game/_Packs/WorkAnimations/Demo/Mannequins/Meshes/SK_Mannequin.SK_Mannequin`
- Retarget target preview mesh: `/Game/_Packs/WorkAnimations/Demo/Mannequins/Meshes/SKM_Manny.SKM_Manny`

The clean v2 retarget should target the WorkAnimations UE5 mannequin skeleton used by `ABP_Worker`, not the modular Quantum skeleton directly.

## Existing IK Assets

- Source IK Rig: `/Game/Avariika/Anim/Rig/IK_MobilityUE4.IK_MobilityUE4`
- Target IK Rig: `/Game/_Packs/WorkAnimations/Demo/Mannequins/Rigs/IK_Mannequin.IK_Mannequin`
- Existing retargeter: `/Game/Avariika/Anim/Rig/RTG_MobilityUE4_to_Mannequin.RTG_MobilityUE4_to_Mannequin`

Existing retargeter setup:

- Source IK Rig: `IK_MobilityUE4`
- Target IK Rig: `IK_Mannequin`
- Source preview mesh: Mobility Pro `SK_Mannequin`
- Target preview mesh: WorkAnimations `SKM_Manny`
- Source pose: `Default Pose`
- Target pose: `Default Pose`
- Enabled ops: `Pelvis Motion`, `FK Chains`, `Root Motion`, `Remap Curves`
- Disabled ops: `Retarget IK Goals`, `Run IK Rig`

## IK Rig Findings

`IK_MobilityUE4`:

- Retarget root: `pelvis`
- Chains: `Root`, `Spine`, `Neck`, `Head`, `LeftClavicle`, `LeftArm`, `RightClavicle`, `RightArm`, `LeftLeg`, `RightLeg`
- Leg chains end at `ball_l` and `ball_r`
- No IK goals assigned to the chains

`IK_Mannequin`:

- Retarget root: `pelvis`
- Chains include spine, arms, legs, twist chains, fingers, IK helper bones, `Root`, and `Head`
- Arm and leg chains have IK goals (`hand_l_Goal`, `hand_r_Goal`, `foot_l_Goal`, `foot_r_Goal`)
- Editor log reports stale Manny retarget pose assets while loading this rig; the target pose data should not be trusted blindly.

## Current Pipeline Defects

1. The existing source IK rig uses `pelvis` as retarget root. That can work for floor alignment, but it is not a clean root-motion turn setup when the source pack has separate root-motion assets.
2. The existing retargeter uses only `Default Pose` on both source and target. There is no confirmed Mobility-to-Manny stance correction pose.
3. Target IK goal retargeting and target IK rig execution are disabled.
4. The target IK rig loaded with stale retarget pose warnings, so the existing target pose state needs revalidation.
5. The source `Root_Motion` stand turn assets measured below do not contain meaningful animated root-track yaw or root XY translation through `AnimationLibrary.extract_root_track_transform`.

Conclusion: a new dedicated retargeter is required. Reusing the existing IK rigs is acceptable only if the v2 retargeter explicitly corrects/validates pose, root handling, and leg/foot mapping.

## Required Manifest

Use source assets from `/Game/Mobility_01/Animation/In-Place/` for locomotion loops and start/stop clips, and source assets from `/Game/Mobility_01/Animation/Root_Motion/` for turn clips.

Idle:

- `MOB1_M1_Stand_Relaxed_Idle_IP`
- `MOB1_M1_Stand_Relaxed_Idle_v2_IP`

Walk:

- `MOB1_M1_Walk_F_IP`
- `MOB1_M1_Walk_B_IP`
- `MOB1_M1_Walk_L_IP`
- `MOB1_M1_Walk_R_IP`
- `MOB1_M1_Walk_FL_Loop_IP`
- `MOB1_M1_Walk_FR_Loop_IP`
- `MOB1_M1_Walk_BL_BkPd_Loop_IP`
- `MOB1_M1_Walk_BR_BkPd_Loop_IP`

Jog:

- `MOB1_M1_Jog_F_IP`
- `MOB1_M1_Jog_B_IP`
- `MOB1_M1_Jog_L_IP`
- `MOB1_M1_Jog_R_IP`
- `MOB1_M1_Jog_FL_Loop_IP`
- `MOB1_M1_Jog_FR_Loop_IP`
- `MOB1_M1_Jog_BL_BkPd_Loop_IP`
- `MOB1_M1_Jog_BR_BkPd_Loop_IP`

Run:

- `MOB1_M1_Run_F_IP`
- `MOB1_M1_Run_L_IP`
- `MOB1_M1_Run_R_IP`
- `MOB1_M1_Run_FL_Loop_IP`
- `MOB1_M1_Run_FR_Loop_IP`

Crouch:

- `MOB1_M1_Crouch_Idle_IP`
- `MOB1_M1_Crouch_Idle_V2_IP`
- `MOB1_M1_CrouchWalk_F_IP`
- `MOB1_M1_CrouchWalk_B_IP`
- `MOB1_M1_CrouchWalk_L_IP`
- `MOB1_M1_CrouchWalk_R_IP`
- `MOB1_M1_CrouchWalk_FL_Loop_IP`
- `MOB1_M1_CrouchWalk_FR_Loop_IP`
- `MOB1_M1_CrouchWalk_BL_BkPd_Loop_IP`
- `MOB1_M1_CrouchWalk_BR_BkPd_Loop_IP`

Start/stop/pivot coverage:

- `MOB1_M1_Stand_Relaxed_to_Walk_F_IP`
- `MOB1_M1_Stand_Relaxed_to_Walk_B_IP`
- `MOB1_M1_Stand_Relaxed_to_Walk_L_IP`
- `MOB1_M1_Stand_Relaxed_to_Walk_R_IP`
- `MOB1_M1_Walk_F_to_Stand_Relaxed_IP`
- `MOB1_M1_Walk_B_to_Stand_Relaxed_IP`
- `MOB1_M1_Walk_L_to_Stand_Relaxed_IP`
- `MOB1_M1_Walk_R_to_Stand_Relaxed_IP`
- `MOB1_M1_Stand_Relaxed_to_Jog_F_IP`
- `MOB1_M1_Stand_Relaxed_to_Jog_B_IP`
- `MOB1_M1_Stand_Relaxed_to_Jog_L_IP`
- `MOB1_M1_Stand_Relaxed_to_Jog_R_IP`
- `MOB1_M1_Jog_F_to_Stand_Relaxed_IP`
- `MOB1_M1_Jog_B_to_Stand_Relaxed_IP`
- `MOB1_M1_Jog_L_to_Stand_Relaxed_IP`
- `MOB1_M1_Jog_R_to_Stand_Relaxed_IP`
- `MOB1_M1_Stand_Relaxed_to_Run_F_IP`
- `MOB1_M1_Stand_Relaxed_to_Run_L_IP`
- `MOB1_M1_Stand_Relaxed_to_Run_R_IP`
- `MOB1_M1_Run_F_to_Stand_Relaxed_IP`
- `MOB1_M1_Run_L_to_Stand_Relaxed_IP`
- `MOB1_M1_Run_R_to_Stand_Relaxed_IP`
- `MOB1_M1_Crouch_to_CrouchWalk_F_IP`
- `MOB1_M1_Crouch_to_CrouchWalk_B_IP`
- `MOB1_M1_Crouch_to_CrouchWalk_L_IP`
- `MOB1_M1_Crouch_to_CrouchWalk_R_IP`
- `MOB1_M1_Crouch_to_Stand_Relaxed_IP`

Stand turn-in-place from source root-motion folder:

- `MOB1_M1_Stand_Relaxed_L45`
- `MOB1_M1_Stand_Relaxed_L90`
- `MOB1_M1_Stand_Relaxed_L135`
- `MOB1_M1_Stand_Relaxed_L180`
- `MOB1_M1_Stand_Relaxed_R_45`
- `MOB1_M1_Stand_Relaxed_R_90`
- `MOB1_M1_Stand_Relaxed_R_135`
- `MOB1_M1_Stand_Relaxed_R_180`

Crouch turns exist in the source pack:

- `MOB1_M1_Crouch_L_45`
- `MOB1_M1_Crouch_L_90`
- `MOB1_M1_Crouch_L_135`
- `MOB1_M1_Crouch_L_180`
- `MOB1_M1_Crouch_R_45`
- `MOB1_M1_Crouch_R_90`
- `MOB1_M1_Crouch_R_135`
- `MOB1_M1_Crouch_R_180`

Jump/fall/land:

- Excluded from v2 manifest. `ABP_Worker` currently depends on idle, Mobility locomotion blendspace, Mobility crouch blendspace, and head aim assets; no jump/fall/land animation assets are referenced by the active graph.

## Source Turn Measurements

Measured with `AnimationLibrary.get_sequence_length`, `is_root_motion_enabled`, and `extract_root_track_transform`.

| Source asset | Duration | Root-motion flag | Root yaw delta | Root XY delta | Pelvis XY range | Foot lift check |
| --- | ---: | --- | ---: | ---: | ---: | --- |
| `MOB1_M1_Stand_Relaxed_L45` | 2.3667s | false | 0.0 | 0.0 | 12.392 | component-space visual check still required |
| `MOB1_M1_Stand_Relaxed_L90` | 2.8000s | false | 0.0 | 0.0 | 16.242 | component-space visual check still required |
| `MOB1_M1_Stand_Relaxed_L135` | 3.2000s | false | 0.0 | 0.0 | 23.133 | component-space visual check still required |
| `MOB1_M1_Stand_Relaxed_L180` | 3.7667s | false | 0.0 | 0.0 | 22.634 | component-space visual check still required |
| `MOB1_M1_Stand_Relaxed_R_45` | 2.7000s | false | 0.0 | 0.0 | 10.852 | component-space visual check still required |
| `MOB1_M1_Stand_Relaxed_R_90` | 3.1667s | false | 0.0 | 0.0 | 14.943 | component-space visual check still required |
| `MOB1_M1_Stand_Relaxed_R_135` | 3.5333s | false | 0.0 | 0.0 | 25.519 | component-space visual check still required |
| `MOB1_M1_Stand_Relaxed_R_180` | 3.6667s | false | 0.0 | 0.0 | 26.254 | component-space visual check still required |

These are real left/right source variants by naming and paired source coverage, not generated mirrors in the current project. However, the measured root track is not a meaningful root-motion turn track. Treat the source turns as pose/stepping turns unless a visual preview proves otherwise.

## Required Decision

Create a dedicated v2 retargeter and retarget into `/Game/Avariika/Anim/Locomotion/Mobility_v2/`. Do not overwrite current Mobility assets, do not create montages, and do not wire gameplay until v2 turn clips pass validation.
