# -*- coding: utf-8 -*-
# Гипотеза: CharacterMesh0 помечен FirstPersonPrimitiveType=WorldSpaceRepresentation (UE5.5 first-person
# рендеринг) — движок прячет его от владельца, но тень оставляет. Читаем и сбрасываем в None, снимаем кадр.
import unreal, json, traceback
R = {"err": None, "meshes": []}
try:
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    gw = ues.get_game_world()
    if not gw:
        R["err"] = "PIE не активен"
    else:
        pawn = unreal.GameplayStatics.get_player_pawn(gw, 0)
        for c in pawn.get_components_by_class(unreal.SkeletalMeshComponent):
            m = {"name": c.get_name()}
            for prop in ("first_person_primitive_type", "is_first_person", "first_person_field_of_view", "first_person_scale"):
                try: m[prop] = str(c.get_editor_property(prop))
                except Exception as e: m[prop+"_err"] = str(e)[:60]
            R["meshes"].append(m)
        # сбросить у CharacterMesh0 в None, чтобы оно рисовалось владельцу в 3-м лице
        for c in pawn.get_components_by_class(unreal.SkeletalMeshComponent):
            if c.get_name() == "CharacterMesh0":
                try:
                    c.set_editor_property("first_person_primitive_type", unreal.FirstPersonPrimitiveType.NONE)
                    R["set_cm0_none"] = str(c.get_editor_property("first_person_primitive_type"))
                except Exception as e:
                    R["set_err"] = str(e)
                break
        try:
            unreal.AutomationLibrary.take_high_res_screenshot(1280, 720, "fp_prim_shot")
            R["shot"] = "fp_prim_shot requested"
        except Exception as e:
            R["shot_err"] = str(e)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_fp_primitive.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("FP_PRIMITIVE %s" % json.dumps(R, default=str))
