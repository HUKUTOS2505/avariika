# -*- coding: utf-8 -*-
"""Сильно сбавить экспозицию/свет — увидеть истинный альбедо. И проверить, что у
M_SM_FireExtinguisher base color подключён к нужной текстуре."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}
m = by.get('PROD_Light')
if m:
    m.set_actor_location(unreal.Vector(70.0, -134.0, 430.0), False, True)
    lc = m.get_component_by_class(unreal.PointLightComponent)
    lc.set_intensity(9000.0)
    lc.set_attenuation_radius(1500.0)
    out.append('light dim 9000')
p = by.get('PROD_PPV')
if p:
    s = p.get_editor_property('settings')
    s.set_editor_property('auto_exposure_bias', 3.0)
    p.set_editor_property('settings', s)
    out.append('exposure 3.0')

# проверка материала
mat = unreal.load_asset('/Game/Avariika/Meshes/M_SM_FireExtinguisher.M_SM_FireExtinguisher')
if mat:
    mel = unreal.MaterialEditingLibrary
    try:
        exprs = mel.get_inputs_for_material_property(mat, unreal.MaterialProperty.MP_BASE_COLOR)
        out.append('base inputs=%s' % exprs)
    except Exception as e:
        out.append('inputs err %s' % e)
    # перебрать TextureSample экспрешены
    try:
        for ex in mat.get_editor_property('expressions'):
            if isinstance(ex, unreal.MaterialExpressionTextureSample):
                t = ex.get_editor_property('texture')
                out.append('TexSample tex=%s sampler=%s' % (t.get_name() if t else 'None', ex.get_editor_property('sampler_type')))
    except Exception as e:
        out.append('expr err %s' % e)

open(r'C:\unrealEngine\avariika\Saved\product_dim.txt', 'w', encoding='utf-8').write('\n'.join(out))
