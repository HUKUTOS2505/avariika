# -*- coding: utf-8 -*-
import unreal
out = []
ar = unreal.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous(['/Game/Adventure_Pack'], True, True)
datas = ar.get_assets_by_path('/Game/Adventure_Pack', recursive=True)
by = {}
for d in datas:
    cn = str(getattr(d, 'asset_class_path', d).asset_name)
    by[cn] = by.get(cn, 0) + 1
out.append('total: %d' % len(datas))
for k in sorted(by, key=lambda k: -by[k])[:12]:
    out.append('  %-26s %d' % (k, by[k]))
# попробовать загрузить скелетный меш Sarah — проверка миграции 5.6->5.7
sk = [str(d.package_name) for d in datas if str(getattr(d, 'asset_class_path', d).asset_name) == 'SkeletalMesh']
out.append('skeletal meshes: %d' % len(sk))
if sk:
    try:
        m = unreal.load_asset(sk[0])
        out.append('loaded OK: %s' % (sk[0] if m else 'NULL'))
    except Exception as e:
        out.append('LOAD ERR: %s' % e)
open(r'D:\unrealEngine\avariika\Saved\scan_sarah.txt', 'w', encoding='utf-8').write('\n'.join(out))
