# -*- coding: utf-8 -*-
"""Fix the imported meshy normal map so it does not crash rendering.
The auto-imported normal came in as a colour texture (sRGB, default compression),
which triggers the 'Texture not valid! NormalMap' ensure. Set it to a proper
normal map (TC_NORMALMAP, sRGB off). Headless-safe: only loads/saves assets, no UI.
Run: UnrealEditor-Cmd.exe avariika.uproject -run=pythonscript -script="D:\\unrealEngine\\avariika\\Scripts\\fix_toilet_normal.py"
"""
import unreal

out = []
candidates = [
    '/Game/Avariika/Meshes/normal',
]
for path in candidates:
    if not unreal.EditorAssetLibrary.does_asset_exist(path):
        continue
    tex = unreal.load_asset(path)
    if not isinstance(tex, unreal.Texture2D):
        continue
    # Нормаль: линейная (sRGB off) + сжатие нормал-карты
    tex.set_editor_property('srgb', False)
    tex.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_NORMALMAP)
    tex.set_editor_property('lod_group', unreal.TextureGroup.TEXTUREGROUP_WORLD_NORMAL_MAP)
    unreal.EditorAssetLibrary.save_loaded_asset(tex)
    out.append('FIX %s: srgb=False, TC_NORMALMAP, lod=WorldNormalMap' % path)

if not out:
    out.append('нормал-текстура не найдена по ожидаемым путям')

with open(r'D:\unrealEngine\avariika\Saved\fix_toilet_result.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
