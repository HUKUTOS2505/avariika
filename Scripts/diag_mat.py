# -*- coding: utf-8 -*-
"""Диагностика: какой материал на мешах и подключён ли base color. Также средний цвет
base-текстуры (через первый пиксель не получим, но проверим имя/srgb/размер)."""
import unreal
out = []

for name in ['SM_FireExtinguisher', 'SM_Battery', 'SM_Radio', 'SM_FirstAidKit']:
    mesh = unreal.load_asset('/Game/Avariika/Meshes/%s.%s' % (name, name))
    if not mesh:
        out.append('%s: НЕТ МЕША' % name)
        continue
    mats = []
    try:
        n = mesh.get_num_sections(0)
    except Exception:
        n = 1
    for i in range(max(n, 1)):
        m = mesh.get_material(i)
        mats.append(m.get_name() if m else 'None')
    out.append('%s: sections=%d mats=%s' % (name, n, mats))

    # проверить base color подключение материала
    mat = unreal.load_asset('/Game/Avariika/Meshes/M_%s.M_%s' % (name, name))
    if mat:
        try:
            node, idx = unreal.MaterialEditingLibrary.get_material_property_input_node(mat, unreal.MaterialProperty.MP_BASE_COLOR)
            out.append('  M_%s base node=%s' % (name, node.get_name() if node else 'NONE-NOT-CONNECTED'))
        except Exception as e:
            out.append('  M_%s base check err: %s' % (name, e))

# текстура base: srgb/размер
for tn in ['SM_FireExtinguisher_BaseColor', 'SM_Radio_BaseColor']:
    t = unreal.load_asset('/Game/Avariika/Meshes/%s.%s' % (tn, tn))
    if t:
        out.append('%s srgb=%s w=%s h=%s comp=%s' % (
            tn, t.get_editor_property('srgb'),
            t.blueprint_get_size_x() if hasattr(t, 'blueprint_get_size_x') else '?',
            t.blueprint_get_size_y() if hasattr(t, 'blueprint_get_size_y') else '?',
            t.get_editor_property('compression_settings')))

open(r'C:\unrealEngine\avariika\Saved\diag_mat.txt', 'w', encoding='utf-8').write('\n'.join(out))
