import unreal, json
OUT = r"D:/unrealEngine/avariika/Scripts/fix_operator_color.json"
mel = unreal.MaterialEditingLibrary
eal = unreal.EditorAssetLibrary
R = {"steps": [], "err": None}


def step(s):
    R["steps"].append(s)


try:
    # 1) Use the NATIVE imported material (shows the Meshy texture reliably),
    #    kill the false emissive=basecolor that self-lit/washed it.
    mic = unreal.load_asset("/Game/Characters/Operator/Material_1.Material_1")
    if mic:
        try:
            mel.set_material_instance_scalar_parameter_value(mic, "EmissiveColorMapWeight", 0.0)
            step("emissive_off")
        except Exception as e:
            step("emissive_err:" + str(e))
        eal.save_loaded_asset(mic, False)

    sk = unreal.load_asset("/Game/Characters/Operator/SK_Operator.SK_Operator")
    if sk and mic:
        slot = unreal.SkeletalMaterial()
        slot.set_editor_property("material_interface", mic)
        slot.set_editor_property("material_slot_name", "Material_1")
        sk.set_editor_property("materials", [slot])
        eal.save_loaded_asset(sk, False)
        step("native_mat_assigned")

    # 2) Brighten L_AnimTest so colors are clearly visible
    sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    for a in sub.get_all_level_actors():
        cn = a.get_class().get_name()
        if cn == "DirectionalLight":
            c = a.get_component_by_class(unreal.DirectionalLightComponent)
            if c:
                c.set_intensity(10.0)
        elif cn == "SkyLight":
            c = a.get_component_by_class(unreal.SkyLightComponent)
            if c:
                c.set_editor_property("intensity", 3.0)
                try:
                    c.set_editor_property("real_time_capture", True)
                    c.recapture_sky()
                except Exception:
                    pass
    try:
        unreal.EditorLoadingAndSavingUtils.save_current_level()
        step("level_saved")
    except Exception as e:
        step("save_err:" + str(e))
except Exception as e:
    R["err"] = str(e)

with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("FIX_COLOR: " + json.dumps(R))
