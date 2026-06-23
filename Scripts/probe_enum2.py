import unreal
out = []
out.append('Collision* types: ' + ', '.join([x for x in dir(unreal) if 'Collision' in x]))
# попробуем кандидаты на enum значений отклика
for name in ['CollisionResponse', 'ECollisionResponse']:
    t = getattr(unreal, name, None)
    if t is not None:
        vals = [x for x in dir(t) if x.startswith('ECR') or x.upper() in ('IGNORE','OVERLAP','BLOCK')]
        out.append('%s values: %s' % (name, vals))
open(r'C:\unrealEngine\avariika\Saved\probe_enum2.txt','w',encoding='utf-8').write('\n'.join(out))
