import unreal, json, traceback
p = "C:/unrealEngine/avariika/Saved/stair_walkmap.txt"
lines = []
try:
    ues = unreal.UnrealEditorSubsystem()
    world = ues.get_editor_world()
    stair_labels = {"SM_Staircase1", "SM_Staircase2"}
    obj_types = [unreal.ObjectTypeQuery.OBJECT_TYPE_QUERY1,
                 unreal.ObjectTypeQuery.OBJECT_TYPE_QUERY2,
                 unreal.ObjectTypeQuery.OBJECT_TYPE_QUERY3]
    R = 34.0; HH = 88.0; ZC = 90.0
    xs = [int(34 + i*55) for i in range(10)]   # 34..529 footprint X
    ys = [int(-77 + j*58) for j in range(10)]  # -77..445 footprint Y
    lines.append("Capsule r=%g hh=%g z=%g  |  '#'=stair blocks  '.'=clear  ' '=other-only" % (R,HH,ZC))
    header = "y\\x " + " ".join("%4d"%x for x in xs)
    lines.append(header)
    for y in ys:
        row = []
        for x in xs:
            loc = unreal.Vector(float(x), float(y), ZC)
            hits = unreal.SystemLibrary.capsule_overlap_actors(world, loc, R, HH, obj_types, None, [])
            if isinstance(hits, tuple):
                hits = hits[-1]
            if hits is None:
                hits = []
            labs = set()
            for a in hits:
                try: labs.add(a.get_actor_label())
                except: pass
            if labs & stair_labels:
                c = "#"
            elif labs:
                c = "o"
            else:
                c = "."
            row.append("   "+c)
        lines.append("%4d "%y + " ".join(row))
    lines.append("")
    lines.append("Legend: # stair-blocked, o other-actor-only, . fully clear")
except Exception:
    lines.append("FATAL\n"+traceback.format_exc())
f=open(p,"w"); f.write("\n".join(lines)); f.close()
