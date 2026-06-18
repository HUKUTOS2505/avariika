# -*- coding: utf-8 -*-
# Подробно по обоим скелетным мешам: тени, аним-инстанс, габариты, ассет — кто рисуется/кто кидает тень.
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
            for p in ("visible","hidden_in_game","owner_no_see","only_owner_see",
                      "cast_shadow","cast_hidden_shadow","render_in_main_pass",
                      "render_in_depth_pass","visible_in_ray_tracing","affect_distance_field_lighting"):
                try: m[p] = bool(c.get_editor_property(p))
                except Exception: pass
            try:
                sm = c.get_editor_property("skeletal_mesh_asset")
                m["mesh"] = sm.get_name() if sm else None
            except Exception:
                try:
                    sm = c.get_skeletal_mesh_asset(); m["mesh"] = sm.get_name() if sm else None
                except Exception as e: m["mesh_err"] = str(e)
            try:
                ai = c.get_anim_instance()
                m["anim_instance"] = ai.get_class().get_name() if ai else None
            except Exception as e: m["anim_err"] = str(e)
            try:
                m["anim_mode"] = str(c.get_editor_property("animation_mode"))
            except Exception: pass
            try:
                lb = c.get_local_bounds()
                # get_local_bounds returns (min, max)
                m["local_bounds"] = str(lb)
            except Exception as e:
                m["bounds_err"] = str(e)
            try:
                wl = c.get_world_location(); m["world"] = [round(wl.x),round(wl.y),round(wl.z)]
            except Exception: pass
            try:
                comp_world_scale = c.get_world_scale(); m["scale"] = [round(comp_world_scale.x,2),round(comp_world_scale.y,2),round(comp_world_scale.z,2)]
            except Exception: pass
            R["meshes"].append(m)
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_mesh_detail.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("MESH_DETAIL %s" % json.dumps(R, default=str))
