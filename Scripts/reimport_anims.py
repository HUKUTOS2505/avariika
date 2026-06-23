import unreal, os, json
STAGE_ANIM = r"C:/unrealEngine/avariika/RawAssets/_char_stage/anims"
DEST_ANIM = "/Game/Characters/Operator/Anims"
OUT = r"C:/unrealEngine/avariika/Scripts/reimport_anims.json"
tools = unreal.AssetToolsHelpers.get_asset_tools()
R = {"by_class": {}, "ok_anim": 0, "fail": [], "errors": []}

try:
    unreal.SystemLibrary.execute_console_command(None, "Interchange.FeatureFlags.Import.FBX false")
except Exception as e:
    R["errors"].append("cvar: " + str(e))

skeleton = unreal.load_asset("/Game/Characters/Operator/SK_Operator_Skeleton.SK_Operator_Skeleton")
R["skeleton"] = skeleton is not None

files = sorted([f for f in os.listdir(STAGE_ANIM) if f.lower().endswith(".fbx")])
for f in files:
    base = f[:-4]
    ui = unreal.FbxImportUI()
    ui.set_editor_property("automated_import_should_detect_type", False)  # honor mesh_type below
    ui.set_editor_property("import_mesh", False)
    ui.set_editor_property("import_as_skeletal", True)
    ui.set_editor_property("import_animations", True)
    ui.set_editor_property("import_materials", False)
    ui.set_editor_property("import_textures", False)
    ui.set_editor_property("create_physics_asset", False)
    ui.set_editor_property("mesh_type_to_import", unreal.FBXImportType.FBXIT_ANIMATION)
    ui.set_editor_property("skeleton", skeleton)

    t = unreal.AssetImportTask()
    t.set_editor_property("filename", os.path.join(STAGE_ANIM, f))
    t.set_editor_property("destination_path", DEST_ANIM)
    t.set_editor_property("destination_name", base)
    t.set_editor_property("automated", True)
    t.set_editor_property("replace_existing", True)
    t.set_editor_property("save", True)
    t.set_editor_property("options", ui)
    try:
        tools.import_asset_tasks([t])
        obj = unreal.load_asset(DEST_ANIM + "/" + base + "." + base)
        cls = obj.get_class().get_name() if obj else "None"
        R["by_class"][cls] = R["by_class"].get(cls, 0) + 1
        if cls == "AnimSequence":
            R["ok_anim"] += 1
        else:
            R["fail"].append(base + ":" + cls)
    except Exception as e:
        R["fail"].append(base + ":EXC:" + str(e))

with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("REIMPORT_ANIMS: " + json.dumps(R["by_class"]))
