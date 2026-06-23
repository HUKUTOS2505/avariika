# -*- coding: utf-8 -*-
import unreal, json
R = {}
for p in ["/Engine/EngineFonts/Roboto", "/Engine/EngineFonts/RobotoDistanceField",
          "/Game/Avariika/UI/Fonts/arial", "/Game/Avariika/UI/Fonts/AvCyrFont"]:
    f = unreal.load_asset(p)
    if f:
        info = {"class": f.get_class().get_name()}
        try: info["cache"] = str(f.get_editor_property("font_cache_type"))
        except Exception as e: info["cache_err"] = str(e)
        R[p] = info
    else:
        R[p] = "NOT FOUND"
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_font_check.json", "w") as f:
    json.dump(R, f, indent=1, default=str)
unreal.log("FONT_CHECK %s" % json.dumps(R, default=str))
