import unreal
try:
    s = unreal.get_default_object(unreal.EditorPerformanceSettings)
    s.set_editor_property("throttle_cpu_when_not_foreground", False)
    unreal.log("THROTTLE| throttle_cpu_when_not_foreground=False ok")
except Exception as e:
    unreal.log_error("THROTTLE| err %s" % e)
