# floor1_map.py — read-only ASCII-карта 1 этажа L_Dom1 (пол/стены/двери/окна/лестницы).
# Заполняем по bounds (clip клетки), не точечно — тонкие стены иначе проскакивают.
# Слой пола: SM_Floor* c z-центром в [150,280]. Слой стен: bounds пересекает z=300, центр z<500.
import unreal, json
OUT_TXT = r"D:\unrealEngine\avariika\Saved\floor1_map.txt"
OUT_JSON = r"D:\unrealEngine\avariika\Saved\floor1_map.json"

CELL = 100.0
X0, X1 = -2700.0, 5100.0
Y0, Y1 = -2700.0, 2600.0
NX = int((X1 - X0) / CELL)
NY = int((Y1 - Y0) / CELL)

res = {"ok": False}
try:
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = eas.get_all_level_actors()
    wname = actors[0].get_world().get_name() if actors else ""
    if "L_Dom1" not in wname:
        res["error"] = "world guard: " + wname
        raise Exception("guard")

    grid = [[" "] * NX for _ in range(NY)]

    def cells_of(origin, ext):
        ax0 = origin.x - ext.x; ax1 = origin.x + ext.x
        ay0 = origin.y - ext.y; ay1 = origin.y + ext.y
        ix0 = max(0, int((ax0 - X0) / CELL)); ix1 = min(NX - 1, int((ax1 - X0) / CELL))
        iy0 = max(0, int((ay0 - Y0) / CELL)); iy1 = min(NY - 1, int((ay1 - Y0) / CELL))
        return ix0, ix1, iy0, iy1

    def put(origin, ext, ch):
        ix0, ix1, iy0, iy1 = cells_of(origin, ext)
        for iy in range(iy0, iy1 + 1):
            for ix in range(ix0, ix1 + 1):
                grid[iy][ix] = ch

    PRIO = {" ": 0, ".": 1, "#": 2, "G": 2, "S": 3, "o": 4, "D": 5, "@": 6}
    def put_p(origin, ext, ch):
        ix0, ix1, iy0, iy1 = cells_of(origin, ext)
        for iy in range(iy0, iy1 + 1):
            for ix in range(ix0, ix1 + 1):
                if PRIO[ch] >= PRIO.get(grid[iy][ix], 0):
                    grid[iy][ix] = ch

    for a in actors:
        cn = a.get_class().get_name()
        loc = a.get_actor_location()
        try: label = a.get_actor_label()
        except: label = a.get_name()
        try: origin, ext = a.get_actor_bounds(False)
        except: continue
        zc = origin.z
        name = (label + " " + cn)
        nl = name.lower()
        # пол
        if "SM_Floor" in label and 150 <= zc <= 300:
            put_p(origin, ext, ".")
            continue
        # лестницы
        if "stair" in nl and zc < 600:
            put_p(origin, ext, "S"); continue
        # двери (ADoor)
        if cn == "Door":
            put_p(origin, ext, "D"); continue
        # окна
        if "window" in nl and zc < 500:
            put_p(origin, ext, "o"); continue
        # PlayerStart
        if cn == "PlayerStart":
            put_p(origin, ext, "@"); continue
        # стены: bounds пересекает z=300, не крыша
        zmin = origin.z - ext.z; zmax = origin.z + ext.z
        if zmin < 300 < zmax and zc < 520:
            ch = "G" if "garage" in nl else "#"
            put_p(origin, ext, ch)

    # рендер: север (макс Y) сверху
    lines = []
    header = "     " + "".join([("%d" % ((X0 + i*CELL)/100 % 10)) if i % 5 == 0 else " " for i in range(NX)])
    lines.append("Y\\X north(top)=+Y  west(left)=-X   1cell=1m   .=floor #=wall G=garage D=door o=window S=stairs @=start")
    for iy in range(NY - 1, -1, -1):
        ycoord = (Y0 + iy * CELL) / 100
        lines.append("%5d%s" % (ycoord, "".join(grid[iy])))
    txt = "\n".join(lines)
    with open(OUT_TXT, "w") as f:
        f.write(txt)
    res["ok"] = True
    res["nx"] = NX; res["ny"] = NY
    res["legend"] = ". floor / # wall / G garage / D door / o window / S stairs / @ playerstart"
    with open(OUT_JSON, "w") as f:
        json.dump(res, f, indent=1)
except Exception as e:
    res["exc"] = str(e)
    with open(OUT_JSON, "w") as f:
        json.dump(res, f, indent=1)
