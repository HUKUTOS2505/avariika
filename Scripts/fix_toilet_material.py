# -*- coding: utf-8 -*-
"""Stabilize the toilet: the meshy auto-material samples an invalid normal map
(crashes rendering with 'Texture not valid! NormalMap'). Swap SM_Toilet's
material slot to a plain valid engine material so the level renders safely.
Headless (no render) -> safe. Proper textures wired separately later.
"""
import unreal

out = []
mesh = unreal.load_asset('/Game/Avariika/Meshes/SM_Toilet')
mat = unreal.load_asset('/Engine/BasicShapes/BasicShapeMaterial')
if mesh and mat:
    n = mesh.get_num_sections(0)
    for i in range(max(n, 1)):
        mesh.set_material(i, mat)
    unreal.EditorAssetLibrary.save_loaded_asset(mesh)
    out.append('SM_Toilet: материал слот(ы) -> BasicShapeMaterial, сохранено')
else:
    out.append('меш или материал не найдены: mesh=%s mat=%s' % (bool(mesh), bool(mat)))

with open(r'C:\unrealEngine\avariika\Saved\fix_toilet_mat_result.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
