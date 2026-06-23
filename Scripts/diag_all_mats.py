# -*- coding: utf-8 -*-
"""Диагностика материалов/текстур всех предметных мешей: для слота 0 — какая текстура
привязана к BASE COLOR и из какого исходника; UV-каналы и секции меша. Ловим
'нет текстуры'/'не та текстура'."""
import unreal
mel = unreal.MaterialEditingLibrary
out = []

NAMES = ['SM_WeldingMachine', 'SM_Radio', 'SM_MotionSensor', 'SM_FirstAidKit', 'SM_TrapKit',
         'SM_Thermos', 'SM_Fuse', 'SM_FireExtinguisher', 'SM_Battery', 'SM_LightKit',
         'SM_Tester', 'SM_GasPipe', 'SM_Generator', 'SM_Breaker', 'SM_Toilet']

SUB = {'SM_Cigarettes': '/Game/Avariika/Meshes/Cigarettes/SM_Cigarettes.SM_Cigarettes'}


def tex_src(tex):
    try:
        aid = tex.get_editor_property('asset_import_data')
        f = aid.get_first_filename()
        return f.split('\\')[-1].split('/')[-1]
    except Exception:
        return '?'


def base_tex(mat):
    try:
        node = mel.get_material_property_input_node(mat, unreal.MaterialProperty.MP_BASE_COLOR)
    except Exception as e:
        return 'ERR(%s)' % e
    if node is None:
        return 'NOT CONNECTED'
    if isinstance(node, unreal.MaterialExpressionTextureSample):
        t = node.get_editor_property('texture')
        if t:
            return 'tex=%s src=%s srgb=%s' % (t.get_name(), tex_src(t), t.get_editor_property('srgb'))
        return 'TextureSample but no texture'
    return 'node=%s (not a texture)' % node.get_class().get_name()


def report_mesh(label, path):
    mesh = unreal.load_asset(path)
    if not mesh:
        out.append('%s: НЕТ МЕША (%s)' % (label, path)); return
    try:
        uv = mesh.get_num_uv_channels(0)
    except Exception:
        uv = '?'
    nsec = mesh.get_num_sections(0)
    m0 = mesh.get_material(0)
    line = '%-20s UV=%s sections=%d mat0=%s' % (label, uv, nsec, m0.get_name() if m0 else 'None')
    out.append(line)
    if m0:
        out.append('    base: %s' % base_tex(m0))


for n in NAMES:
    report_mesh(n, '/Game/Avariika/Meshes/%s.%s' % (n, n))
for n, p in SUB.items():
    report_mesh(n, p)

open(r'C:\unrealEngine\avariika\Saved\diag_all_mats.txt', 'w', encoding='utf-8').write('\n'.join(out))
