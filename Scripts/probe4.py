# -*- coding: utf-8 -*-
"""Проверить материал слота 0 у 4 субфолдер-мешей: подключена ли base-текстура."""
import unreal
mel = unreal.MaterialEditingLibrary
out = []
NAMES = ['SM_FireExtinguisher', 'SM_FirstAidKit', 'SM_Battery', 'SM_WeldingMachine']
for n in NAMES:
    mesh = unreal.load_asset('/Game/Avariika/Meshes/%s/%s.%s' % (n, n, n))
    if not mesh:
        out.append('%s: НЕТ МЕША' % n); continue
    m0 = mesh.get_material(0)
    base = 'NONE'
    if m0:
        try:
            node = mel.get_material_property_input_node(m0, unreal.MaterialProperty.MP_BASE_COLOR)
            if node and isinstance(node, unreal.MaterialExpressionTextureSample):
                t = node.get_editor_property('texture')
                base = 'TEX ' + (t.get_name() if t else '?')
            elif node:
                base = node.get_class().get_name()
            else:
                base = 'NOT CONNECTED'
        except Exception as e:
            base = 'ERR %s' % e
    out.append('%s: mat0=%s base=%s' % (n, m0.get_name() if m0 else 'None', base))
open(r'C:\unrealEngine\avariika\Saved\probe4.txt', 'w', encoding='utf-8').write('\n'.join(out))
