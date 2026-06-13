# -*- coding: utf-8 -*-
"""Строгая чистка корня /Game/Avariika/Meshes: оставить только SM_*, M_SM_*, Image_0.
Всё прочее в корне (мусор импорта сигарет: tobacco-*, 6408*, Case_*, Cig_* и т.п.) удалить.
Подпапки (Cigarettes/) не трогаем (recursive=False)."""
import unreal

ROOT = '/Game/Avariika/Meshes'
out = []
killed = []
for a in unreal.EditorAssetLibrary.list_assets(ROOT, recursive=False):
    base = a.split('.')[0]
    nm = base.split('/')[-1]
    if nm.startswith('SM_') or nm.startswith('M_SM_') or nm == 'Image_0':
        continue
    try:
        unreal.EditorAssetLibrary.delete_asset(base)
        killed.append(nm)
    except Exception as e:
        out.append('FAIL %s: %s' % (nm, e))

out.append('killed=%d' % len(killed))
# что осталось в корне
left = [x.split('/')[-1].split('.')[0] for x in unreal.EditorAssetLibrary.list_assets(ROOT, recursive=False)]
out.append('root now (%d): %s' % (len(left), ', '.join(sorted(set(left)))))
out.append('killed list: ' + ', '.join(killed[:40]))
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
open(r'D:\unrealEngine\avariika\Saved\root_clean.txt', 'w', encoding='utf-8').write('\n'.join(out))
