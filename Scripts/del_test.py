import unreal
if unreal.EditorAssetLibrary.does_directory_exist('/Game/Avariika/Meshes/_test'):
    unreal.EditorAssetLibrary.delete_directory('/Game/Avariika/Meshes/_test')
open(r'C:\unrealEngine\avariika\Saved\del_test.txt','w').write('done')
