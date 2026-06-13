# -*- coding: utf-8 -*-
"""Scene-import glb (собирает многоузловую модель как актёры с трансформами).
Репортит, что создалось, чтобы понять структуру и спозиционировать. ASCII."""
import os
import unreal

RAW = r'D:\unrealEngine\avariika\RawAssets\SM_Gazelle'
DEST = '/Game/Avariika/Meshes/GazelleScene'
out = []

glb = next((os.path.join(RAW, f) for f in sorted(os.listdir(RAW)) if f.lower().endswith('.glb')), None)
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
before = set(a.get_name() for a in eas.get_all_level_actors())

try:
    mgr = unreal.InterchangeManager.get_interchange_manager_scripted()
    sd = mgr.create_source_data(glb)
    params = unreal.ImportAssetParameters()
    params.is_automated = True
    has_scene = hasattr(mgr, 'import_scene')
    out.append('has import_scene = %s' % has_scene)
    if has_scene:
        ok = mgr.import_scene(DEST, sd, params)
        out.append('import_scene ok=%s' % ok)
    else:
        out.append('no import_scene method')
except Exception as e:
    out.append('SCENE FAIL: ' + str(e))

after = eas.get_all_level_actors()
new = [a for a in after if a.get_name() not in before]
out.append('new actors = %d' % len(new))
# найти «корни» (без родителя) среди новых
roots = [a for a in new if a.get_attach_parent_actor() is None]
out.append('root actors among new = %d' % len(roots))
for a in new[:25]:
    par = a.get_attach_parent_actor()
    out.append('  %s [%s] parent=%s' % (a.get_actor_label(), a.get_class().get_name(), par.get_actor_label() if par else '-'))

open(r'D:\unrealEngine\avariika\Saved\gazelle_scene.txt', 'w', encoding='utf-8').write('\n'.join(out))
