# -*- coding: utf-8 -*-
"""Перф-фикс через AssetRegistry: ограничить max_texture_size всех текстур предметов.
Мелкие/носимые -> 1024, крупные пропсы/Hilux -> 2048."""
import unreal
out = []
ar = unreal.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous(['/Game/Avariika/Meshes'], True)
datas = ar.get_assets_by_path('/Game/Avariika/Meshes', recursive=True)
out.append('asset datas: %d' % len(datas))

BIG = {'Hilux', 'SM_GasPipe', 'SM_Generator', 'SM_Breaker', 'SM_Toilet'}
changed = 0
checked = 0
for d in datas:
    obj = d.get_asset()
    if not isinstance(obj, unreal.Texture2D):
        continue
    checked += 1
    path = obj.get_path_name()
    folder = path.split('/')[-2] if len(path.split('/')) >= 2 else ''
    cap = 2048 if folder in BIG else 1024
    try:
        cur = obj.get_editor_property('max_texture_size')
        if cur != cap:
            obj.set_editor_property('max_texture_size', cap)
            unreal.EditorAssetLibrary.save_loaded_asset(obj)
            changed += 1
    except Exception as e:
        out.append('ERR %s: %s' % (path.split('/')[-1], e))

out.append('textures checked=%d capped=%d' % (checked, changed))
open(r'D:\unrealEngine\avariika\Saved\cap_textures2.txt', 'w', encoding='utf-8').write('\n'.join(out))
