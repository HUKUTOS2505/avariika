import unreal, json
OUT = r"C:/unrealEngine/avariika/Scripts/diag_color.json"
mel = unreal.MaterialEditingLibrary
info = {}

tex = unreal.load_asset("/Game/Characters/Operator/texture_0.texture_0")
info["texture_0_loaded"] = tex is not None
info["texture_0_class"] = tex.get_class().get_name() if tex else None

mat = unreal.load_asset("/Game/Characters/Operator/M_Operator.M_Operator")
info["M_Operator_loaded"] = mat is not None
if mat:
    for prop, key in [(unreal.MaterialProperty.MP_BASE_COLOR, "basecolor"),
                      (unreal.MaterialProperty.MP_NORMAL, "normal"),
                      (unreal.MaterialProperty.MP_ROUGHNESS, "roughness")]:
        try:
            node = mel.get_material_property_input_node(mat, prop)
            info[key + "_connected"] = node is not None
            info[key + "_node"] = node.get_class().get_name() if node else None
        except Exception as e:
            info[key + "_err"] = str(e)

sk = unreal.load_asset("/Game/Characters/Operator/SK_Operator.SK_Operator")
if sk:
    mats = sk.get_editor_property("materials")
    info["mesh_slots"] = [(m.get_editor_property("material_interface").get_path_name()
                           if m.get_editor_property("material_interface") else None) for m in mats]

# test map lighting
try:
    sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    lights = []
    op_mat = None
    for a in sub.get_all_level_actors():
        cn = a.get_class().get_name()
        if cn in ("DirectionalLight", "SkyLight", "SkyAtmosphere", "PointLight"):
            lights.append(a.get_actor_label() + ":" + cn)
        if a.get_actor_label() == "Operator":
            comp = a.skeletal_mesh_component
            mm = comp.get_material(0)
            op_mat = mm.get_path_name() if mm else None
    info["level"] = str(unreal.EditorLevelLibrary.get_editor_world().get_name())
    info["level_lights"] = lights
    info["operator_actor_mat0"] = op_mat
except Exception as e:
    info["level_err"] = str(e)

with open(OUT, "w") as f:
    json.dump(info, f, indent=1)
unreal.log("DIAG_COLOR done")
