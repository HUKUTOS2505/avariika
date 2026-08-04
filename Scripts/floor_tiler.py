import json, math, collections

bk = json.load(open(r"C:/unrealEngine/avariika/Saved/floor_backup.json"))
main = [t for t in bk if t["z"] <= 235]
east = [t for t in bk if t["z"] > 235]
allf = main + east
minx = min(t["x0"] for t in allf); maxx = max(t["x1"] for t in allf)
miny = min(t["y0"] for t in allf); maxy = max(t["y1"] for t in allf)
ox = math.floor(minx / 100) * 100
oy = math.floor(miny / 100) * 100

def rasterize(tiles):
    cells = set()
    for t in tiles:
        i0 = int(math.floor((t["x0"] - ox) / 100)); i1 = int(math.ceil((t["x1"] - ox) / 100))
        j0 = int(math.floor((t["y0"] - oy) / 100)); j1 = int(math.ceil((t["y1"] - oy) / 100))
        for i in range(i0, i1):
            for j in range(j0, j1):
                cx = ox + i * 100 + 50; cy = oy + j * 100 + 50
                if t["x0"] <= cx <= t["x1"] and t["y0"] <= cy <= t["y1"]:
                    cells.add((i, j))
    return cells

mc = rasterize(main)
ec = rasterize(east) - mc   # main has priority on overlap

F3  = "/Game/ResidentialHouses/Meshes/Architecture/Floors/SM_Floor_3m"
F2a = "/Game/ResidentialHouses/Meshes/Architecture/Floors/SM_Floor_2m_a"
F2b = "/Game/ResidentialHouses/Meshes/Architecture/Floors/SM_Floor_2m_b"
F1  = "/Game/ResidentialHouses/Meshes/Architecture/Floors/SM_Floor_1x1m"
# (mesh, dw_cells, dh_cells, width, depth)
PIECES = [(F3, 3, 3, 300, 300), (F2a, 3, 2, 300, 200), (F2b, 2, 3, 200, 300), (F1, 1, 1, 100, 100)]

def fits(cells, used, i, j, dw, dh):
    for a in range(dw):
        for b in range(dh):
            c = (i + a, j + b)
            if c not in cells or c in used:
                return False
    return True

def tile(cells, z):
    used = set(); out = []
    for (i, j) in sorted(cells):
        if (i, j) in used:
            continue
        for (mesh, dw, dh, W, D) in PIECES:
            if fits(cells, used, i, j, dw, dh):
                X0 = ox + i * 100; Y0 = oy + j * 100
                out.append({"mesh": mesh, "loc": [X0 + W, Y0, z]})
                for a in range(dw):
                    for b in range(dh):
                        used.add((i + a, j + b))
                break
    return out

tm = tile(mc, 221.3)
te = tile(ec, 259.3)
plan = {"tiles": tm + te, "n_main": len(tm), "n_east": len(te),
        "total": len(tm) + len(te), "main_cells": len(mc), "east_cells": len(ec),
        "ox": ox, "oy": oy}
json.dump(plan, open(r"C:/unrealEngine/avariika/Saved/floor_plan2.json", "w"))
cnt = collections.Counter(t["mesh"].split("/")[-1] for t in plan["tiles"])
print("total=%d main=%d east=%d  cells m/e=%d/%d" % (plan["total"], plan["n_main"], plan["n_east"], len(mc), len(ec)))
print(dict(cnt))
