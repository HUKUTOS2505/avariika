# -*- coding: utf-8 -*-
# Выключаем "Use Less CPU when in Background" — чтобы редактор рендерил вьюпорт даже без фокуса
# (тогда HighResShot/take_screenshot снимут настоящий кадр, а не пустой). Сохраняем в конфиг.
import unreal, json
R = {"err": None}
try:
    s = unreal.get_default_object(unreal.EditorPerformanceSettings)
    before = s.get_editor_property("throttle_cpu_when_not_foreground")
    s.set_editor_property("throttle_cpu_when_not_foreground", False)
    after = s.get_editor_property("throttle_cpu_when_not_foreground")
    try: s.save_config()
    except Exception as se: R["save_err"] = str(se)
    R["throttle_before"] = before
    R["throttle_after"] = after
    R["ok"] = (after == False)
except Exception as e:
    R["err"] = str(e)
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_throttle.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, default=str, ensure_ascii=False)
unreal.log("THROTTLE %s" % json.dumps(R, default=str))
