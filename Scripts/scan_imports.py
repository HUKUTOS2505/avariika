# -*- coding: utf-8 -*-
"""Скан реестра по списку новых путей + сводка классов (проверка загрузки старых версий)."""
import unreal
out = []
ar = unreal.AssetRegistryHelpers.get_asset_registry()
PATHS = ['/Game/AmericanDrive', '/Game/Construction_VOL2', '/Game/Construction_Pit',
         '/Game/FootstepSystem', '/Game/ResourcePack']
for p in PATHS:
    ar.scan_paths_synchronous([p], True, True)
    datas = ar.get_assets_by_path(p, recursive=True)
    by = {}
    for d in datas:
        try:
            cn = str(d.asset_class_path.asset_name)
        except Exception:
            cn = 'unknown'
        by[cn] = by.get(cn, 0) + 1
    summary = ', '.join('%s:%d' % (k, by[k]) for k in sorted(by, key=lambda k: -by[k])[:8])
    out.append('%s -> %d assets [%s]' % (p, len(datas), summary))
open(r'D:\unrealEngine\avariika\Saved\scan_imports.txt', 'w', encoding='utf-8').write('\n'.join(out))
