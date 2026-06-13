# -*- coding: utf-8 -*-
"""Проверить пак Horror_Hospital: типы ассетов + валидность модульной стены."""
import unreal
out = []
ar = unreal.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous(['/Game/Horror_Hospital'], True)
datas = ar.get_assets_by_path('/Game/Horror_Hospital', recursive=True)
out.append('assets in /Game/Horror_Hospital: %d' % len(datas))
counts = {}
wall = None
for d in datas:
    try:
        cn = str(d.asset_class_path.asset_name)
    except Exception:
        cn = '?'
    counts[cn] = counts.get(cn, 0) + 1
    if cn == 'StaticMesh' and wall is None and 'Wall' in str(d.asset_name):
        wall = d
for k in sorted(counts, key=lambda x: -counts[x]):
    out.append('  %s: %d' % (k, counts[k]))
if wall:
    m = wall.get_asset()
    if m:
        b = m.get_bounds().box_extent
        m0 = m.get_material(0)
        out.append('sample wall %s: %.0fx%.0fx%.0f tris=%d mat0=%s' % (
            m.get_name(), b.x*2, b.y*2, b.z*2, m.get_num_triangles(0), m0.get_name() if m0 else 'None'))
# уровень
out.append('L_Horror_Hospital exists=%s' % unreal.EditorAssetLibrary.does_asset_exist('/Game/Horror_Hospital/Levels/L_Horror_Hospital'))
open(r'D:\unrealEngine\avariika\Saved\verify_horror.txt', 'w', encoding='utf-8').write('\n'.join(out))
