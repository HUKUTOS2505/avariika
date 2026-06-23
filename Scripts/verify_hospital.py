# -*- coding: utf-8 -*-
"""Проверить пак HospitalCombo: типы ассетов, валидность примера меша."""
import unreal
out = []
ar = unreal.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous(['/Game/HospitalCombo'], True)
datas = ar.get_assets_by_path('/Game/HospitalCombo', recursive=True)
out.append('assets in /Game/HospitalCombo: %d' % len(datas))

# по классам
counts = {}
sample_mesh = None
for d in datas:
    try:
        cn = str(d.asset_class_path.asset_name)
    except Exception:
        cn = str(getattr(d, 'asset_class', '?'))
    counts[cn] = counts.get(cn, 0) + 1
    if cn == 'StaticMesh' and sample_mesh is None:
        sample_mesh = d
for k in sorted(counts, key=lambda x: -counts[x]):
    out.append('  %s: %d' % (k, counts[k]))

# валидность примера
if sample_mesh:
    m = sample_mesh.get_asset()
    if m:
        b = m.get_bounds().box_extent
        out.append('sample mesh %s: %.0fx%.0fx%.0f tris=%d sections=%d' % (
            m.get_name(), b.x*2, b.y*2, b.z*2, m.get_num_triangles(0), m.get_num_sections(0)))
        m0 = m.get_material(0)
        out.append('  mat0=%s' % (m0.get_name() if m0 else 'None'))

open(r'C:\unrealEngine\avariika\Saved\verify_hospital.txt', 'w', encoding='utf-8').write('\n'.join(out))
