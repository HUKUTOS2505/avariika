import unreal, json
OUT = r"C:/unrealEngine/avariika/Scripts/light_operator.json"
R = {"ok": False, "op_loc": None, "err": None}
try:
    sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    op = None
    for a in sub.get_all_level_actors():
        if a.get_actor_label() == "TestOperator":
            op = a
        if a.get_actor_label() == "TestOperatorLight":
            sub.destroy_actor(a)
    loc = op.get_actor_location() if op else unreal.Vector(300.0, 0.0, -13.0)
    R["op_loc"] = [loc.x, loc.y, loc.z]

    light = sub.spawn_actor_from_class(unreal.PointLight, unreal.Vector(loc.x, loc.y + 70.0, loc.z + 180.0))
    light.set_actor_label("TestOperatorLight")
    lc = light.get_component_by_class(unreal.PointLightComponent)
    if lc:
        lc.set_editor_property("intensity", 30000.0)
        lc.set_editor_property("attenuation_radius", 1200.0)
        lc.set_editor_property("light_color", unreal.Color(255, 250, 240, 255))
        lc.set_editor_property("cast_shadows", False)
    unreal.EditorLoadingAndSavingUtils.save_current_level()
    R["ok"] = True
except Exception as e:
    R["err"] = str(e)
with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("LIGHT_OPERATOR: " + json.dumps(R))
