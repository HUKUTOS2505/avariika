# -*- coding: utf-8 -*-
"""Скан реестра по новым пакам из общее2 + сводка."""
import unreal
out = []
ar = unreal.AssetRegistryHelpers.get_asset_registry()
PATHS = ['/Game/Modern_Hospital_Leartes', '/Game/Madrid_Street', '/Game/Backrooms_TheLobby',
         '/Game/FoggyStreet', '/Game/LightHouse', '/Game/Shoothouse',
         '/Game/YI_Luggage', '/Game/FencesVOL2', '/Game/oldBenchPack']
for p in PATHS:
    ar.scan_paths_synchronous([p], True, True)
    datas = ar.get_assets_by_path(p, recursive=True)
    sm = sum(1 for d in datas if str(getattr(d, 'asset_class_path', d).asset_name) == 'StaticMesh')
    out.append('%s -> %d ассетов (мешей: %d)' % (p, len(datas), sm))
open(r'C:\unrealEngine\avariika\Saved\scan_imports2.txt', 'w', encoding='utf-8').write('\n'.join(out))
