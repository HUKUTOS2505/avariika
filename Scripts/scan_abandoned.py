# -*- coding: utf-8 -*-
"""Скан реестра + сводка по импортированному AbandonedHospital."""
import unreal
out = []
ar = unreal.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous(['/Game/AbandonedHospital'], True, True)
datas = ar.get_assets_by_path('/Game/AbandonedHospital', recursive=True)
out.append('total assets under /Game/AbandonedHospital: %d' % len(datas))

by_class = {}
for d in datas:
    cn = str(d.asset_class_path.asset_name) if hasattr(d, 'asset_class_path') else str(d.get_class().get_name())
    by_class[cn] = by_class.get(cn, 0) + 1
for cn in sorted(by_class, key=lambda k: -by_class[k]):
    out.append('  %-28s %d' % (cn, by_class[cn]))

# уровни в паке
maps = [str(d.package_name) for d in datas if str(getattr(d, 'asset_class_path', d).asset_name if hasattr(d, 'asset_class_path') else '') == 'World']
out.append('maps: ' + (', '.join(maps) if maps else 'none'))

open(r'D:\unrealEngine\avariika\Saved\scan_abandoned.txt', 'w', encoding='utf-8').write('\n'.join(out))
