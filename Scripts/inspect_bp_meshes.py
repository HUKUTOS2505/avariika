# -*- coding: utf-8 -*-
# Смотрим СКЕЛЕТНЫЕ меши на дефолте BP_AvaryoCharacter (без PIE): у кого назначен меш,
# кто скрыт (hidden_in_game / visible) — чтобы понять, почему в 3-м лице тела не видно.
import unreal, json, traceback
R = {"err": None, "skeletals": []}
try:
    bp_path = "/Game/Avariika/Blueprints/BP_AvaryoCharacter"
    gen = unreal.load_object(None, bp_path + ".BP_AvaryoCharacter_C")
    cdo = unreal.get_default_object(gen) if gen else None
    R["cdo"] = cdo.get_name() if cdo else None
    if cdo:
        sks = cdo.get_components_by_class(unreal.SkeletalMeshComponent)
        for c in sks:
            info = {"name": c.get_name()}
            try:
                sm = c.get_editor_property("skeletal_mesh_asset")
                info["mesh"] = sm.get_name() if sm else None
            except Exception as e:
                info["mesh_err"] = str(e)
            for prop in ("owner_no_see", "hidden_in_game", "visible", "only_owner_see", "cast_shadow"):
                try: info[prop] = bool(c.get_editor_property(prop))
                except Exception: pass
            try:
                loc = c.get_relative_location(); info["rel_z"] = round(loc.z)
            except Exception: pass
            R["skeletals"].append(info)
        # Заодно — все примитивы с мешем (static тоже)
        prims = cdo.get_components_by_class(unreal.PrimitiveComponent)
        R["prim_count"] = len(prims)
        R["prim_names"] = [p.get_name() for p in prims]
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_inspect_bp_meshes.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("INSPECT_BP_MESHES %s" % json.dumps(R, default=str))
