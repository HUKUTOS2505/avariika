import unreal
_OUT=[]
def line(s=""):
    unreal.log("RETAG| "+str(s)); _OUT.append(str(s))

WORK = unreal.load_asset("/Game/_Packs/WorkAnimations/Demo/Mannequins/Meshes/SK_Mannequin")
line("WorkAnim skel: %s" % (WORK.get_path_name() if WORK else None))

# 1) BS_Locomotion: FBL -> WorkAnim (samples are already RT_MOB1 WorkAnim clips)
bs = unreal.load_asset("/Game/Avariika/Anim/Locomotion/BS_Locomotion")
try:
    before = bs.get_editor_property("skeleton")
    bs.set_editor_property("skeleton", WORK)
    after = bs.get_editor_property("skeleton")
    line("BS_Locomotion skeleton: %s -> %s" % (before.get_name() if before else None, after.get_name() if after else None))
except Exception as e:
    line("BS retag err: %s" % e)

# 2) ABP_Worker: target_skeleton FBL -> WorkAnim
abp = unreal.load_asset("/Game/Avariika/Anim/Locomotion/ABP_Worker")
try:
    before = abp.get_editor_property("target_skeleton")
    abp.set_editor_property("target_skeleton", WORK)
    after = abp.get_editor_property("target_skeleton")
    line("ABP_Worker target_skeleton: %s -> %s" % (
        before.get_path_name() if before else None, after.get_path_name() if after else None))
except Exception as e:
    line("ABP retag err: %s" % e)

# recompile + save
for p in ("/Game/Avariika/Anim/Locomotion/BS_Locomotion",
          "/Game/Avariika/Anim/Locomotion/ABP_Worker",
          "/Game/Avariika/Blueprints/BP_AvaryoCharacter"):
    a = unreal.load_asset(p)
    if a and a.get_class().get_name() in ("AnimBlueprint","Blueprint"):
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(a); line("compiled %s" % p)
        except Exception as e:
            line("compile err %s: %s" % (p, e))
    try:
        line("save %s -> %s" % (p, unreal.EditorAssetLibrary.save_asset(p, only_if_is_dirty=False)))
    except Exception as e:
        line("save err %s: %s" % (p, e))

# verify
line("--- verify ---")
line("BS_Locomotion.skeleton = %s" % unreal.load_asset("/Game/Avariika/Anim/Locomotion/BS_Locomotion").get_editor_property("skeleton").get_name())
line("ABP_Worker.target_skeleton = %s" % unreal.load_asset("/Game/Avariika/Anim/Locomotion/ABP_Worker").get_editor_property("target_skeleton").get_path_name())

with open("C:/unrealEngine/avariika/Scripts/fix_skeleton_retag_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
