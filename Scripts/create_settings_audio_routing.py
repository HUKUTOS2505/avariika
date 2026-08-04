import unreal


CLASS_DIR = "/Game/Avariika/Audio/Classes"
MIX_DIR = "/Game/Avariika/Audio/Mixes"
TARGETS = {
    "master": (
        "/Engine/EngineSounds/Master",
        f"{CLASS_DIR}/SC_Avariika_Master",
    ),
    "sfx": (
        "/Engine/EngineSounds/Master",
        f"{CLASS_DIR}/SC_Avariika_SFX",
    ),
    "music": (
        "/Engine/EngineSounds/Master",
        f"{CLASS_DIR}/SC_Avariika_Music",
    ),
    "voice": (
        "/Engine/EngineSounds/Master",
        f"{CLASS_DIR}/SC_Avariika_Voice",
    ),
    "mix": (
        "/Game/MenuSystemPro/ExampleContent/Sounds/SoundMixes/SCM_MenuMix",
        f"{MIX_DIR}/SM_Avariika_Settings",
    ),
}


def load_or_duplicate(source_path: str, target_path: str):
    existing = unreal.EditorAssetLibrary.load_asset(target_path)
    if existing:
        return existing
    source = unreal.EditorAssetLibrary.load_asset(source_path)
    if not source:
        raise RuntimeError(f"Missing source asset: {source_path}")
    package_path, asset_name = target_path.rsplit("/", 1)
    created = unreal.AssetToolsHelpers.get_asset_tools().duplicate_asset(
        asset_name, package_path, source
    )
    if not created:
        raise RuntimeError(f"Could not create {target_path} from {source_path}")
    return created


def routing_key(asset_path: str) -> str:
    normalized = asset_path.lower()
    if "/music/" in normalized or "music" in normalized:
        return "music"
    if (
        "/voice/" in normalized
        or "dialogue" in normalized
        or "voice" in normalized
        or "radiocomm" in normalized
    ):
        return "voice"
    return "sfx"


unreal.EditorAssetLibrary.make_directory(CLASS_DIR)
unreal.EditorAssetLibrary.make_directory(MIX_DIR)
assets = {
    key: load_or_duplicate(source_path, target_path)
    for key, (source_path, target_path) in TARGETS.items()
}
for key in ("master", "sfx", "music", "voice"):
    assets[key].set_editor_property("child_classes", [])

changed = []
counts = {"sfx": 0, "music": 0, "voice": 0}
for asset_path in unreal.EditorAssetLibrary.list_assets(
    "/Game/Audio", recursive=True, include_folder=False
):
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not isinstance(asset, unreal.SoundBase):
        continue
    key = routing_key(asset_path)
    target_class = assets[key]
    current_class = asset.get_editor_property("sound_class_object")
    if current_class != target_class:
        asset.set_editor_property("sound_class_object", target_class)
        changed.append(asset)
    counts[key] += 1

for asset in assets.values():
    unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False)
for asset in changed:
    unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False)

violations = []
for asset_path in unreal.EditorAssetLibrary.list_assets(
    "/Game/Audio", recursive=True, include_folder=False
):
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not isinstance(asset, unreal.SoundBase):
        continue
    key = routing_key(asset_path)
    if asset.get_editor_property("sound_class_object") != assets[key]:
        violations.append(asset_path)

if violations:
    raise RuntimeError(
        "Audio routing verification failed: " + ", ".join(violations[:20])
    )

unreal.log(
    "[AvAudioRouting] projectClassesReady=true mixReady=true "
    f"sfx={counts['sfx']} music={counts['music']} voice={counts['voice']} "
    f"assetsChanged={len(changed)} violations=0"
)
