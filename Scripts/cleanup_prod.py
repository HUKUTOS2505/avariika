# -*- coding: utf-8 -*-
"""Снести все временные PROD_-акторы (продуктовый кадр), не сохранять временное в уровень.
Уровень на диске уже чистый (продуктовые скрипты его не сохраняли)."""
import unreal
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
out = []
killed = 0
for a in list(eas.get_all_level_actors()):
    try:
        if a.get_actor_label().startswith('PROD_'):
            eas.destroy_actor(a)
            killed += 1
    except Exception:
        pass
out.append('killed PROD_ = %d' % killed)
# проверка
left = [a.get_actor_label() for a in eas.get_all_level_actors() if a.get_actor_label().startswith('PROD_')]
out.append('left = %s' % left)
open(r'D:\unrealEngine\avariika\Saved\cleanup_prod.txt', 'w', encoding='utf-8').write('\n'.join(out))
