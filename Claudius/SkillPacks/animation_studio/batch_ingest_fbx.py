# @claudius category: animation_studio
# @claudius description: Import a folder of FBX/USD/OBJ files with consistent destination + material conventions. Designed for CAD-to-Unreal ingest on technical explainer projects.
# @claudius workflow: animation, asset, shot

import os
import unreal


def execute(params):
    source_dir = params.get("source_dir", "")
    dest_path = params.get("dest_path", "/Game/Ingest")
    extensions = [e.lower().lstrip(".") for e in params.get("extensions", ["fbx", "usd", "obj"])]
    auto_material = bool(params.get("auto_material", True))

    if not source_dir or not os.path.isdir(source_dir):
        return {"success": False, "message": f"source_dir not found: {source_dir}"}

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    imported = []
    skipped = []

    files = []
    for root, _, names in os.walk(source_dir):
        for n in names:
            ext = n.rsplit(".", 1)[-1].lower() if "." in n else ""
            if ext in extensions:
                files.append(os.path.join(root, n))

    for full_path in files:
        task = unreal.AssetImportTask()
        task.filename = full_path
        task.destination_path = dest_path
        task.replace_existing = True
        task.automated = True
        task.save = True
        try:
            asset_tools.import_asset_tasks([task])
            for created in (task.imported_object_paths or []):
                imported.append(created)
        except Exception as e:
            skipped.append({"file": full_path, "error": str(e)})

    if auto_material and imported:
        # Touch nothing complex; user is expected to assign materials in a follow-up
        # skill or by hand. Hook included so future versions can wire up a default.
        pass

    return {
        "success": True,
        "message": f"Imported {len(imported)} assets, skipped {len(skipped)}",
        "output": {
            "imported": imported,
            "skipped": skipped,
            "dest_path": dest_path,
        },
    }
