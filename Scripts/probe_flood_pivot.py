import unreal
out = []
for n in ['SM_Floodlight', 'SM_LightKit']:
    m = unreal.load_asset('/Game/Avariika/Meshes/%s/%s.%s' % (n, n, n))
    if not m:
        out.append(n + ': NONE'); continue
    b = m.get_bounds()
    o = b.origin; e = b.box_extent
    # min/max Z относительно пивота (0)
    out.append('%s: origin=(%.1f,%.1f,%.1f) ext=(%.1f,%.1f,%.1f) minZ=%.1f maxZ=%.1f height=%.1f' % (
        n, o.x, o.y, o.z, e.x, e.y, e.z, o.z - e.z, o.z + e.z, e.z * 2))
open(r'C:\unrealEngine\avariika\Saved\probe_flood_pivot.txt','w',encoding='utf-8').write('\n'.join(out))
