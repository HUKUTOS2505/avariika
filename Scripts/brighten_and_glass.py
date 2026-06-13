# -*- coding: utf-8 -*-
"""(1) Сделать стартовый мир светлым: поднять DirectionalLight/SkyLight + дневная авто-экспозиция
в PPV_Night (исходные значения пишем в отчёт для отката в ночь).
(2) Стёкла Hilux (slots 38/46/54: glass/glass_001/glass_002) -> прозрачный ДВУСТОРОННИЙ материал."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
tools = unreal.AssetToolsHelpers.get_asset_tools()
mel = unreal.MaterialEditingLibrary
out = []
by = {a.get_actor_label(): a for a in eas.get_all_level_actors()}

# ---- (1) СВЕТ ----
dl = by.get('DirectionalLight')
if dl:
    lc = dl.get_component_by_class(unreal.DirectionalLightComponent)
    out.append('DirectionalLight ORIG intensity=%.3f' % lc.intensity)
    lc.set_intensity(7.0)
    dl.modify()
sk = by.get('SkyLight')
if sk:
    lc = sk.get_component_by_class(unreal.SkyLightComponent)
    out.append('SkyLight ORIG intensity=%.3f' % lc.intensity)
    lc.set_intensity(3.0)
    lc.recapture_sky()
    sk.modify()

ppv = by.get('PPV_Night')
if ppv:
    s = ppv.get_editor_property('settings')
    out.append('PPV ORIG: method_override=%s bias_override=%s bias=%.2f' % (
        s.get_editor_property('override_auto_exposure_method'),
        s.get_editor_property('override_auto_exposure_bias'),
        s.get_editor_property('auto_exposure_bias')))
    # дневная авто-экспозиция (гистограмма), чтобы яркий мир свёлся к норме
    s.set_editor_property('override_auto_exposure_method', True)
    s.set_editor_property('auto_exposure_method', unreal.AutoExposureMethod.AEM_HISTOGRAM)
    s.set_editor_property('override_auto_exposure_min_brightness', True)
    s.set_editor_property('auto_exposure_min_brightness', 0.5)
    s.set_editor_property('override_auto_exposure_max_brightness', True)
    s.set_editor_property('auto_exposure_max_brightness', 2.0)
    s.set_editor_property('override_auto_exposure_bias', True)
    s.set_editor_property('auto_exposure_bias', 1.0)
    ppv.set_editor_property('settings', s)
    out.append('PPV -> daytime histogram auto-exposure')

# ---- (2) СТЕКЛО HILUX ----
GLASS = '/Game/Avariika/Meshes/Hilux/M_HiluxGlass'
if unreal.EditorAssetLibrary.does_asset_exist(GLASS + '.M_HiluxGlass'):
    unreal.EditorAssetLibrary.delete_asset(GLASS + '.M_HiluxGlass')
gm = tools.create_asset('M_HiluxGlass', '/Game/Avariika/Meshes/Hilux', unreal.Material, unreal.MaterialFactoryNew())
gm.set_editor_property('blend_mode', unreal.BlendMode.BLEND_TRANSLUCENT)
gm.set_editor_property('two_sided', True)
# слегка тонированное стекло, мало непрозрачности, гладкое
bc = mel.create_material_expression(gm, unreal.MaterialExpressionConstant3Vector, -400, -150)
bc.set_editor_property('constant', unreal.LinearColor(0.02, 0.03, 0.04, 1.0))
mel.connect_material_property(bc, '', unreal.MaterialProperty.MP_BASE_COLOR)
op = mel.create_material_expression(gm, unreal.MaterialExpressionConstant, -400, 0)
op.set_editor_property('r', 0.18)
mel.connect_material_property(op, '', unreal.MaterialProperty.MP_OPACITY)
rg = mel.create_material_expression(gm, unreal.MaterialExpressionConstant, -400, 120)
rg.set_editor_property('r', 0.06)
mel.connect_material_property(rg, '', unreal.MaterialProperty.MP_ROUGHNESS)
sp = mel.create_material_expression(gm, unreal.MaterialExpressionConstant, -400, 240)
sp.set_editor_property('r', 1.0)
mel.connect_material_property(sp, '', unreal.MaterialProperty.MP_SPECULAR)
mel.recompile_material(gm)
unreal.EditorAssetLibrary.save_loaded_asset(gm)

mesh = unreal.load_asset('/Game/Avariika/Meshes/Hilux/SM_Hilux.SM_Hilux')
applied = []
if mesh:
    for i in range(mesh.get_num_sections(0)):
        m = mesh.get_material(i)
        if m and m.get_name().lower().startswith('glass'):
            mesh.set_material(i, gm)
            applied.append(i)
    unreal.EditorAssetLibrary.save_loaded_asset(mesh)
out.append('glass applied to slots %s' % applied)

les.save_current_level()
out.append('save_dirty=%s' % unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True))
open(r'D:\unrealEngine\avariika\Saved\brighten_and_glass.txt', 'w', encoding='utf-8').write('\n'.join(out))
