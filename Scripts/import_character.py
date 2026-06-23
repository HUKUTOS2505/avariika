# Headless import of the operator character: skeletal mesh (from Idle withSkin) +
# all animations onto its skeleton, with optimization (texture cap, LODs), then
# place a test actor on Lvl_FirstPerson. Run via UnrealEditor-Cmd -run=pythonscript.
import unreal, os, json

STAGE_MESH = r"C:/unrealEngine/avariika/RawAssets/_char_stage/mesh"
STAGE_ANIM = r"C:/unrealEngine/avariika/RawAssets/_char_stage/anims"
DEST = "/Game/Characters/Operator"
DEST_ANIM = DEST + "/Anims"
OUT = r"C:/unrealEngine/avariika/Scripts/import_character_result.json"

DO_PLACE = False  # placement uses the editor world/Slate -> crashes in -unattended commandlet; do it in GUI later

tools = unreal.AssetToolsHelpers.get_asset_tools()
R = {"mesh": None, "skeleton": None, "anims_ok": 0, "anims_fail": [], "tex_capped": 0,
     "lod": None, "placed": None, "errors": []}


def err(s):
    R["errors"].append(s)
    unreal.log_error("IMPORT_CHAR: " + s)


def flush():
    try:
        with open(OUT, "w") as f:
            json.dump(R, f, indent=1)
    except Exception:
        pass


# Legacy FBX importer: Interchange touches Slate UI and asserts in headless commandlets.
try:
    unreal.SystemLibrary.execute_console_command(None, "Interchange.FeatureFlags.Import.FBX false")
except Exception as e:
    err("disable interchange: " + str(e))


# ---------- 1. Skeletal mesh ----------
mesh_fbx = None
for f in os.listdir(STAGE_MESH):
    if f.lower().endswith(".fbx"):
        mesh_fbx = os.path.join(STAGE_MESH, f)
        break

if not mesh_fbx:
    err("no mesh fbx in stage")
else:
    ui = unreal.FbxImportUI()
    ui.set_editor_property("import_mesh", True)
    ui.set_editor_property("import_as_skeletal", True)
    ui.set_editor_property("import_animations", False)
    ui.set_editor_property("import_materials", True)
    ui.set_editor_property("import_textures", True)
    ui.set_editor_property("create_physics_asset", True)
    ui.set_editor_property("mesh_type_to_import", unreal.FBXImportType.FBXIT_SKELETAL_MESH)
    try:
        smid = ui.get_editor_property("skeletal_mesh_import_data")
        smid.set_editor_property("import_morph_targets", False)
        smid.set_editor_property("convert_scene", True)
        smid.set_editor_property("normal_import_method",
                                 unreal.FBXNormalImportMethod.FBXNIM_IMPORT_NORMALS_AND_TANGENTS)
    except Exception as e:
        err("smid: " + str(e))

    t = unreal.AssetImportTask()
    t.set_editor_property("filename", mesh_fbx)
    t.set_editor_property("destination_path", DEST)
    t.set_editor_property("destination_name", "SK_Operator")
    t.set_editor_property("automated", True)
    t.set_editor_property("replace_existing", True)
    t.set_editor_property("save", True)
    t.set_editor_property("options", ui)
    try:
        tools.import_asset_tasks([t])
    except Exception as e:
        err("mesh import: " + str(e))

sk_mesh = unreal.load_asset(DEST + "/SK_Operator.SK_Operator")
R["mesh"] = sk_mesh is not None
skeleton = sk_mesh.get_editor_property("skeleton") if sk_mesh else None
R["skeleton"] = skeleton.get_path_name() if skeleton else None

# ---------- 2. Animations ----------
if skeleton:
    aui = unreal.FbxImportUI()
    aui.set_editor_property("import_mesh", False)
    aui.set_editor_property("import_as_skeletal", True)
    aui.set_editor_property("import_animations", True)
    aui.set_editor_property("import_materials", False)
    aui.set_editor_property("import_textures", False)
    aui.set_editor_property("create_physics_asset", False)
    aui.set_editor_property("mesh_type_to_import", unreal.FBXImportType.FBXIT_ANIMATION)
    aui.set_editor_property("skeleton", skeleton)

    tasks = []
    for f in sorted(os.listdir(STAGE_ANIM)):
        if not f.lower().endswith(".fbx"):
            continue
        base = f[:-4]  # A_<Name>
        t = unreal.AssetImportTask()
        t.set_editor_property("filename", os.path.join(STAGE_ANIM, f))
        t.set_editor_property("destination_path", DEST_ANIM)
        t.set_editor_property("destination_name", base)
        t.set_editor_property("automated", True)
        t.set_editor_property("replace_existing", True)
        t.set_editor_property("save", True)
        t.set_editor_property("options", aui)
        tasks.append((base, t))

    for base, t in tasks:
        try:
            tools.import_asset_tasks([t])
            a = unreal.load_asset(DEST_ANIM + "/" + base + "." + base)
            if a is not None:
                R["anims_ok"] += 1
            else:
                R["anims_fail"].append(base)
        except Exception as e:
            R["anims_fail"].append(base + ":" + str(e))
else:
    err("no skeleton -> skipped anims")

flush()  # persist import status before the riskier optimization steps

# ---------- 3. Optimization ----------
# 3a. Cap texture in-game size (Meshy textures are big; 2K is plenty for a character)
try:
    ar = unreal.AssetRegistryHelpers.get_asset_registry()
    ar.scan_paths_synchronous([DEST], True, False)
    for ad in ar.get_assets_by_path(DEST, recursive=True):
        obj_path = str(ad.package_name) + "." + str(ad.asset_name)
        obj = unreal.load_asset(obj_path)
        if isinstance(obj, unreal.Texture2D):
            try:
                obj.set_editor_property("max_texture_size", 2048)
                obj.set_editor_property("lod_group", unreal.TextureGroup.TEXTUREGROUP_CHARACTER)
                unreal.EditorAssetLibrary.save_loaded_asset(obj, False)
                R["tex_capped"] += 1
            except Exception as e:
                err("tex cap " + obj_path + ": " + str(e))
except Exception as e:
    err("tex scan: " + str(e))

# 3b. Skeletal mesh LODs (auto-reduce). API varies by version; try the known paths.
if sk_mesh:
    done = False
    for attempt in ("EditorSkeletalMeshLibrary", "SkeletalMeshEditorSubsystem"):
        if done:
            break
        try:
            if attempt == "EditorSkeletalMeshLibrary":
                unreal.EditorSkeletalMeshLibrary.regenerate_lod(sk_mesh, 4)
            else:
                sub = unreal.get_editor_subsystem(unreal.SkeletalMeshEditorSubsystem)
                sub.regenerate_lod(sk_mesh, 4)
            unreal.EditorAssetLibrary.save_loaded_asset(sk_mesh, False)
            R["lod"] = attempt + ":4"
            done = True
        except Exception as e:
            err("lod " + attempt + ": " + str(e))
    if not done:
        R["lod"] = "FAILED"

flush()

# ---------- 4. Test actor on the level (only when DO_PLACE; needs editor world/Slate) ----------
def place_test_actor():
    LEVEL = "/Game/FirstPerson/Lvl_FirstPerson"
    unreal.EditorLoadingAndSavingUtils.load_map(LEVEL)
    idle = unreal.load_asset(DEST_ANIM + "/A_Idle_11.A_Idle_11")
    loc = unreal.Vector(0.0, 300.0, -15.0)  # near PlayerStart platform, on the floor
    rot = unreal.Rotator(0.0, 0.0, 180.0)
    actor = None
    try:
        sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
        actor = sub.spawn_actor_from_class(unreal.SkeletalMeshActor, loc, rot)
    except Exception:
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.SkeletalMeshActor, loc, rot)
    if not (actor and sk_mesh):
        err("spawn/actor null")
        return
    actor.set_actor_label("TestOperator")
    comp = actor.skeletal_mesh_component
    try:
        comp.set_skeletal_mesh_asset(sk_mesh)
    except Exception:
        comp.set_editor_property("skeletal_mesh", sk_mesh)
    if idle:
        comp.set_editor_property("animation_mode", unreal.AnimationMode.ANIMATION_SINGLE_NODE)
        comp.set_animation(idle)
        comp.play(True)
    unreal.EditorLoadingAndSavingUtils.save_current_level()
    R["placed"] = True

if DO_PLACE:
    try:
        place_test_actor()
    except Exception as e:
        err("place: " + str(e))

flush()
unreal.log("IMPORT_CHAR DONE: " + json.dumps(R))
