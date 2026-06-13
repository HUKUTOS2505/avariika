# -*- coding: utf-8 -*-
import unreal
out = []
assets = unreal.EditorAssetLibrary.list_assets('/Game/Avariika/Meshes', recursive=True)
out.append('total assets: %d' % len(assets))
shown = 0
for a in assets:
    if 'normal' in a.lower() or 'Image_2' in a or 'BaseColor' in a or 'Image_0' in a:
        obj = unreal.EditorAssetLibrary.load_asset(a)
        cls = obj.get_class().get_name() if obj else 'None'
        is_t2d = isinstance(obj, unreal.Texture2D) if obj else False
        mts = '?'
        if obj:
            try:
                mts = obj.get_editor_property('max_texture_size')
            except Exception as e:
                mts = 'ERR ' + str(e)
        out.append('%s | class=%s is_Texture2D=%s max_texture_size=%s' % (a.split('/')[-1], cls, is_t2d, mts))
        shown += 1
        if shown >= 8:
            break
open(r'D:\unrealEngine\avariika\Saved\probe_tex_type.txt', 'w', encoding='utf-8').write('\n'.join(out))
