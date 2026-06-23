# -*- coding: utf-8 -*-
import unreal
out = []
p = unreal.InterchangeGenericAssetsPipeline()
for sub in ('common_meshes_properties', 'mesh_pipeline'):
    o = p.get_editor_property(sub)
    props = sorted(x for x in dir(o) if any(k in x.lower() for k in ('bake', 'transform', 'global', 'pivot', 'scene', 'combine')) and not x.startswith('__'))
    out.append('%s: %s' % (sub, ', '.join(props)))
open(r'C:\unrealEngine\avariika\Saved\probe_bake.txt', 'w', encoding='utf-8').write('\n'.join(out))
