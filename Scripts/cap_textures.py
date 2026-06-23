# -*- coding: utf-8 -*-
"""Перфоманс-фикс: ограничить макс. разрешение всех текстур предметов. 4К на мелких пропсах
душат видеопамять (лаги/краши/FPS). Мелкие/носимые -> 1024, крупные пропсы/Hilux -> 2048."""
import unreal
out = []

# подпапки крупных пропсов/транспорта — им 2048, остальным 1024
BIG = {'Hilux', 'SM_GasPipe', 'SM_Generator', 'SM_Breaker', 'SM_Toilet'}

assets = unreal.EditorAssetLibrary.list_assets('/Game/Avariika/Meshes', recursive=True)
changed = 0
for a in assets:
    obj = unreal.EditorAssetLibrary.load_asset(a)
    if not isinstance(obj, unreal.Texture2D):
        continue
    # определить папку
    parts = a.split('/')
    folder = parts[-2] if len(parts) >= 2 else ''
    cap = 2048 if folder in BIG else 1024
    try:
        cur = obj.get_editor_property('max_texture_size')
        if cur != cap:
            obj.set_editor_property('max_texture_size', cap)
            unreal.EditorAssetLibrary.save_loaded_asset(obj)
            changed += 1
    except Exception as e:
        out.append('ERR %s: %s' % (a.split('/')[-1], e))

out.append('textures capped: %d' % changed)
open(r'C:\unrealEngine\avariika\Saved\cap_textures.txt', 'w', encoding='utf-8').write('\n'.join(out))
