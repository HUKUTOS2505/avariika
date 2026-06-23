# -*- coding: utf-8 -*-
# Проверка механики «живой провод»: спавним ARepairable (сломан + bLiveWireWhenBroken),
# проверяем IsLiveWireHot при питании, снимаем питание (b_electrically_powered=False) — должен потухнуть.
# Временные акторы удаляем, карту НЕ сохраняем.
import unreal, json, traceback
R = {"err": None, "checks": []}
def chk(name, cond, extra=None):
    R["checks"].append({"name": name, "pass": bool(cond), "extra": extra})
    unreal.log("LWCHK %s = %s (%s)" % (name, cond, extra))
try:
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    rep_cls = unreal.load_class(None, "/Script/Avaryo.Repairable")
    if not rep_cls:
        R["err"] = "ARepairable class not found"; raise RuntimeError(R["err"])
    rep = eas.spawn_actor_from_class(rep_cls, unreal.Vector(0, 0, 100000), unreal.Rotator(0, 0, 0))
    if not rep:
        R["err"] = "spawn failed"; raise RuntimeError(R["err"])
    rep.set_editor_property("broken", True)
    rep.set_editor_property("live_wire_when_broken", True)

    # питание по умолчанию подано → провод "горячий", чинить нельзя
    hot1 = rep.is_live_wire_hot()
    lw = rep.is_live_wire()
    pwr1 = rep.is_powered()
    chk("broken+live -> IsLiveWire", lw)
    chk("powered by default", pwr1)
    chk("HOT when powered (IsLiveWireHot)", hot1)

    # снять питание
    powered_off_ok = False
    try:
        rep.set_editor_property("electrically_powered", False)
        powered_off_ok = True
    except Exception as pe:
        R["set_powered_err"] = str(pe)
    hot2 = rep.is_live_wire_hot()
    pwr2 = rep.is_powered()
    chk("could set b_electrically_powered=False", powered_off_ok)
    chk("NOT hot after power off", (not hot2), {"is_powered": pwr2, "is_live_wire_hot": hot2})

    # IsLiveWire всё ещё True (сломан+флаг), но не горячий
    chk("still IsLiveWire after power off (just safe)", rep.is_live_wire())

    eas.destroy_actor(rep)
    R["all_pass"] = all(c["pass"] for c in R["checks"])
except Exception as e:
    R["err"] = str(e); R["tb"] = traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_verify_livewire.json", "w", encoding="utf-8") as f:
    json.dump(R, f, indent=1, default=str, ensure_ascii=False)
unreal.log("LIVEWIRE VERIFY %s" % json.dumps(R, default=str))
