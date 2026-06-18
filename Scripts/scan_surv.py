import unreal
unreal.AssetRegistryHelpers.get_asset_registry().scan_paths_synchronous(["/Game/Survival_SFX"], True)
n=len(unreal.AssetRegistryHelpers.get_asset_registry().get_assets_by_path("/Game/Survival_SFX", recursive=True))
open("D:/unrealEngine/avariika/Saved/scan_surv.txt","w").write("Survival_SFX assets: %d"%n)
