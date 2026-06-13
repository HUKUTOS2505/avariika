# -*- coding: utf-8 -*-
"""M_Toilet был ошибочно удалён чисткой и восстановлен из git на диск. Пере-сканировать,
загрузить и переназначить на SM_Toilet (его слот обнулился). Сохранить."""
import unreal
out = []
ar = unreal.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous(['/Game/Avariika/Meshes'], True)

mat = unreal.EditorAssetLibrary.load_asset('/Game/Avariika/Meshes/M_Toilet.M_Toilet')
mesh = unreal.EditorAssetLibrary.load_asset('/Game/Avariika/Meshes/SM_Toilet.SM_Toilet')
out.append('mat=%s mesh=%s' % (bool(mat), bool(mesh)))
if mat and mesh:
    for i in range(max(mesh.get_num_sections(0), 1)):
        mesh.set_material(i, mat)
    unreal.EditorAssetLibrary.save_loaded_asset(mesh)
    cur = mesh.get_material(0)
    out.append('SM_Toilet mat[0] = %s' % (cur.get_name() if cur else 'None'))
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
open(r'D:\unrealEngine\avariika\Saved\fix_toilet_mat.txt', 'w', encoding='utf-8').write('\n'.join(out))
