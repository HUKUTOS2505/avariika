# -*- coding: utf-8 -*-
# Тень есть, тела нет. Проверяем материалы и флаги рендера у CharacterMesh0 (hazmat) на CDO BP.
import unreal, json, traceback
R = {"err": None}
try:
    gen = unreal.load_object(None, "/Game/Avariika/Blueprints/BP_AvaryoCharacter.BP_AvaryoCharacter_C")
    cdo = unreal.get_default_object(gen) if gen else None
    R["cdo"] = cdo.get_name() if cdo else None
    if cdo:
        comp = None
        for c in cdo.get_components_by_class(unreal.SkeletalMeshComponent):
            comp = c; break
        if comp:
            R["comp"] = comp.get_name()
            for p in ("render_in_main_pass", "render_in_depth_pass", "cast_shadow",
                      "cast_hidden_shadow", "visible", "hidden_in_game", "owner_no_see",
                      "only_owner_see", "render_custom_depth"):
                try: R[p] = bool(comp.get_editor_property(p))
                except Exception as e: R[p] = "ERR:%s" % e
            # Материалы на компоненте
            try:
                mats = comp.get_materials()
                R["comp_materials"] = [m.get_name() if m else None for m in mats]
            except Exception as e:
                R["comp_mat_err"] = str(e)
            # Материалы на самом скелетном меше (ассете)
            try:
                sm = comp.get_editor_property("skeletal_mesh_asset")
                R["mesh_asset"] = sm.get_name() if sm else None
                if sm:
                    sk_mats = sm.get_editor_property("materials")
                    out = []
                    for sec in sk_mats:
                        mi = sec.get_editor_property("material_interface")
                        out.append(mi.get_path_name() if mi else None)
                    R["asset_materials"] = out
            except Exception as e:
                R["asset_mat_err"] = str(e)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_hazmat_render.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("HAZMAT_RENDER %s" % json.dumps(R, default=str))
