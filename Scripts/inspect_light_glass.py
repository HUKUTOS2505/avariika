# -*- coding: utf-8 -*-
"""Осмотр: (1) световые акторы уровня + ключевые свойства, (2) материал-слоты SM_Hilux
(найти стёкла). Запись в файл."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []

out.append('=== LIGHTING ACTORS ===')
for a in eas.get_all_level_actors():
    cn = a.get_class().get_name()
    lbl = a.get_actor_label()
    if cn in ('DirectionalLight', 'SkyLight', 'SkyAtmosphere', 'PostProcessVolume',
              'ExponentialHeightFog', 'SkyAtmosphereComponent', 'PointLight', 'SpotLight',
              'RectLight', 'VolumetricCloud'):
        info = '%s [%s]' % (lbl, cn)
        try:
            if cn == 'DirectionalLight':
                lc = a.get_component_by_class(unreal.DirectionalLightComponent)
                r = a.get_actor_rotation()
                info += ' intensity=%.1f pitch=%.1f(sun height) visible=%s' % (
                    lc.intensity, r.pitch, lc.is_visible())
            elif cn == 'SkyLight':
                lc = a.get_component_by_class(unreal.SkyLightComponent)
                info += ' intensity=%.3f' % lc.intensity
            elif cn == 'PointLight':
                lc = a.get_component_by_class(unreal.PointLightComponent)
                info += ' intensity=%.0f' % lc.intensity
        except Exception as e:
            info += ' (props err %s)' % e
        out.append(info)

out.append('')
out.append('=== SM_Hilux material slots (glass?) ===')
mesh = unreal.load_asset('/Game/Avariika/Meshes/Hilux/SM_Hilux.SM_Hilux')
if mesh:
    n = mesh.get_num_sections(0)
    for i in range(n):
        m = mesh.get_material(i)
        nm = m.get_name() if m else 'None'
        flag = ' <-- GLASS?' if m and any(k in nm.lower() for k in ('glass', 'window', 'redglass', 'свет', 'light')) else ''
        out.append('slot %2d: %s%s' % (i, nm, flag))
else:
    out.append('SM_Hilux НЕ найден')

open(r'C:\unrealEngine\avariika\Saved\inspect_light_glass.txt', 'w', encoding='utf-8').write('\n'.join(out))
