# -*- coding: utf-8 -*-
"""Удалить осиротевшие root-ассеты 14 моделей (мой хенд-материал/текстуры/меш), которые
заменены нативными версиями в подпапках. Точечно по именам. Унитаз/Image_0 НЕ трогаем."""
import unreal
ROOT = '/Game/Avariika/Meshes'
out = []
NAMES = ['SM_FireExtinguisher', 'SM_FirstAidKit', 'SM_Battery', 'SM_Fuse', 'SM_WeldingMachine',
         'SM_MotionSensor', 'SM_Radio', 'SM_Tester', 'SM_Thermos', 'SM_LightKit', 'SM_TrapKit',
         'SM_GasPipe', 'SM_Generator', 'SM_Breaker']
killed = 0
for n in NAMES:
    cands = [n, 'M_' + n, n + '_BaseColor', n + '_Normal', n + '_Roughness', n + '_Metallic', n + '_Emission']
    for c in cands:
        p = '%s/%s.%s' % (ROOT, c, c)
        if unreal.EditorAssetLibrary.does_asset_exist(p):
            try:
                unreal.EditorAssetLibrary.delete_asset(p)
                killed += 1
            except Exception as e:
                out.append('FAIL %s: %s' % (c, e))
out.append('killed root orphans = %d' % killed)
left = [x.split('/')[-1].split('.')[0] for x in unreal.EditorAssetLibrary.list_assets(ROOT, recursive=False)]
out.append('root now (%d): %s' % (len(left), ', '.join(sorted(set(left)))))
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
open(r'D:\unrealEngine\avariika\Saved\cleanup_old_root.txt', 'w', encoding='utf-8').write('\n'.join(out))
