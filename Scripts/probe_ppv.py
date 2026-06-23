# -*- coding: utf-8 -*-
"""Интроспекция FPostProcessSettings: точные имена свойств экспозиции в UE5.7."""
import unreal
out = []
try:
    s = unreal.PostProcessSettings()
    expo = sorted([p for p in dir(s) if 'exposure' in p.lower() or 'override' in p.lower() and 'exposure' in p.lower()])
    allp = sorted([p for p in dir(s) if 'exposure' in p.lower()])
    out.append('EXPOSURE PROPS:')
    out.append('\n'.join(allp))
except Exception as e:
    out.append('introspect fail: ' + str(e))
try:
    aem = [m for m in dir(unreal.AutoExposureMethod) if m.upper().startswith('AEM')]
    out.append('\nAEM ENUM: ' + ', '.join(aem))
except Exception as e:
    out.append('aem fail: ' + str(e))
open(r'C:\unrealEngine\avariika\Saved\ppv_props.txt', 'w', encoding='utf-8').write('\n'.join(out))
print('\n'.join(out))
