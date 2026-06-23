# -*- coding: utf-8 -*-
"""Интроспекция Interchange-пайплайна: как включить combine static meshes + как передать
кастомный пайплайн в import_asset (ImportAssetParameters)."""
import unreal
out = []
try:
    p = unreal.InterchangeGenericAssetsPipeline()
    out.append('AssetsPipeline props (combine/mesh/static):')
    out.append('  ' + ', '.join(sorted(x for x in dir(p) if any(k in x.lower() for k in ('combine', 'mesh', 'static')) and not x.startswith('__'))))
    for sub in ('common_meshes_properties', 'mesh_pipeline', 'common_skeletal_meshes_and_animations_properties'):
        try:
            o = p.get_editor_property(sub)
            out.append('%s -> %s' % (sub, type(o).__name__))
            out.append('  combine: ' + ', '.join(sorted(x for x in dir(o) if 'combine' in x.lower())))
        except Exception as e:
            out.append('%s? %s' % (sub, e))
except Exception as e:
    out.append('pipeline construct fail: ' + str(e))
try:
    ip = unreal.ImportAssetParameters()
    out.append('ImportAssetParameters (pipeline/automat): ' + ', '.join(sorted(x for x in dir(ip) if any(k in x.lower() for k in ('pipeline', 'automat')) and not x.startswith('__'))))
except Exception as e:
    out.append('params fail: ' + str(e))
open(r'C:\unrealEngine\avariika\Saved\probe_inter.txt', 'w', encoding='utf-8').write('\n'.join(out))
