# rescan_drcg.py — заставить asset registry увидеть свежескопированный DrCG без рестарта редактора.
import unreal
ar = unreal.AssetRegistryHelpers.get_asset_registry()
ar.scan_paths_synchronous(['/Game/DrCGLevelDesignTools'], force_rescan=True)
assets = ar.get_assets_by_path('/Game/DrCGLevelDesignTools', recursive=True)
wbp = [str(a.package_name) for a in assets if 'WBP_DrCG' in str(a.asset_name)]
with open(r'C:\unrealEngine\avariika\Saved\drcg_rescan.json', 'w') as f:
    f.write('{"scanned": %d, "widget": %s}' % (len(assets), wbp))
