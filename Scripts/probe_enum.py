import unreal
out = []
out.append('CollisionResponse: ' + ', '.join([x for x in dir(unreal.CollisionResponse) if not x.startswith('_')]))
out.append('CollisionChannel(pawn?): ' + ', '.join([x for x in dir(unreal.CollisionChannel) if 'PAWN' in x.upper() or 'pawn' in x]))
open(r'D:\unrealEngine\avariika\Saved\probe_enum.txt','w',encoding='utf-8').write('\n'.join(out))
