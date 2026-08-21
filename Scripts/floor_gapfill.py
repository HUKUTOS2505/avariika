import json, math, collections

plan = json.load(open(r"C:/unrealEngine/avariika/Saved/floor_plan2.json"))
walls = json.load(open(r"C:/unrealEngine/avariika/Saved/walls_dump.json"))["walls"]
ox, oy = plan["ox"], plan["oy"]
sh = {"SM_Floor_3m": (3, 3, 300, 300), "SM_Floor_2m_a": (3, 2, 300, 200),
      "SM_Floor_2m_b": (2, 3, 200, 300), "SM_Floor_1x1m": (1, 1, 100, 100)}

# floor cells -> Z
floor = {}   # (i,j) -> z
for t in plan["tiles"]:
    dw, dh, W, D = sh[t["mesh"].split("/")[-1]]
    px, py, z = t["loc"]
    X0 = px - W; Y0 = py
    i0 = round((X0 - ox) / 100); j0 = round((Y0 - oy) / 100)
    for a in range(dw):
        for b in range(dh):
            floor[(i0 + a, j0 + b)] = z

# wall cells (expand wall bbox by 35cm so thin walls catch adjacent cells)
wallcells = set()
for w in walls:
    x0, x1, y0, y1 = w["x0"] - 35, w["x1"] + 35, w["y0"] - 35, w["y1"] + 35
    i0 = int(math.floor((x0 - ox) / 100)); i1 = int(math.ceil((x1 - ox) / 100))
    j0 = int(math.floor((y0 - oy) / 100)); j1 = int(math.ceil((y1 - oy) / 100))
    for i in range(i0, i1):
        for j in range(j0, j1):
            cx = ox + i * 100 + 50; cy = oy + j * 100 + 50
            if x0 <= cx <= x1 and y0 <= cy <= y1:
                wallcells.add((i, j))

# gap = empty cell, 4-adjacent to floor, and (itself or a 4-neighbor) is a wall cell
N4 = [(1, 0), (-1, 0), (0, 1), (0, -1)]
gaps = {}
cand = set()
for (i, j) in floor:
    for di, dj in N4:
        c = (i + di, j + dj)
        if c not in floor:
            cand.add(c)
for c in cand:
    i, j = c
    adj_floor = [floor[(i + di, j + dj)] for di, dj in N4 if (i + di, j + dj) in floor]
    if not adj_floor:
        continue
    near_wall = (c in wallcells) or any((i + di, j + dj) in wallcells for di, dj in N4)
    if not near_wall:
        continue
    # Z = majority of adjacent floor cells
    z = collections.Counter(adj_floor).most_common(1)[0][0]
    gaps[c] = z

# emit fill tiles (1m), pivot at (Xmax,Ymin) = (ox+i*100+100, oy+j*100)
F1 = "/Game/ResidentialHouses/Meshes/Architecture/Floors/SM_Floor_1x1m"
fills = []
for (i, j), z in sorted(gaps.items()):
    X0 = ox + i * 100; Y0 = oy + j * 100
    fills.append({"mesh": F1, "x": X0 + 100, "y": Y0, "z": z})
json.dump({"n": len(fills), "fills": fills}, open(r"C:/unrealEngine/avariika/Saved/floor_fills.json", "w"))
zc = collections.Counter(f["z"] for f in fills)
print("gap cells to fill:", len(fills), " z:", dict(zc))
for f in fills[:12]:
    print("  ", f["x"], f["y"], f["z"])
