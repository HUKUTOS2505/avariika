import unreal, os, json
STAGE = r"D:/unrealEngine/avariika/RawAssets/_char_stage/mesh"
DEST = "/Game/Characters/Operator"
OUT = r"D:/unrealEngine/avariika/Scripts/build_operator_material.json"
tools = unreal.AssetToolsHelpers.get_asset_tools()
mel = unreal.MaterialEditingLibrary
eal = unreal.EditorAssetLibrary
R = {"imported": [], "errors": [], "assigned": False, "bounds": None}


def err(s):
    R["errors"].append(s)


def import_tex(fname, asset_name, is_normal, is_linear_gray):
    src = os.path.join(STAGE, fname)
    if not os.path.exists(src):
        err("missing " + fname)
        return None
    t = unreal.AssetImportTask()
    t.set_editor_property("filename", src)
    t.set_editor_property("destination_path", DEST)
    t.set_editor_property("destination_name", asset_name)
    t.set_editor_property("automated", True)
    t.set_editor_property("replace_existing", True)
    t.set_editor_property("save", True)
    tools.import_asset_tasks([t])
    tex = unreal.load_asset(DEST + "/" + asset_name + "." + asset_name)
    if not tex:
        err("import failed " + asset_name)
        return None
    if is_normal:
        tex.set_editor_property("srgb", False)
        tex.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_NORMALMAP)
        tex.set_editor_property("lod_group", unreal.TextureGroup.TEXTUREGROUP_CHARACTER_NORMAL_MAP)
    elif is_linear_gray:
        tex.set_editor_property("srgb", False)
        tex.set_editor_property("lod_group", unreal.TextureGroup.TEXTUREGROUP_CHARACTER)
    tex.set_editor_property("max_texture_size", 2048)
    eal.save_loaded_asset(tex, False)
    R["imported"].append(asset_name)
    return tex


base = unreal.load_asset(DEST + "/texture_0.texture_0")
norm = import_tex("Meshy_AI_full_body_character_c_biped_texture_0_normal.png", "T_Operator_N", True, False)
rough = import_tex("Meshy_AI_full_body_character_c_biped_texture_0_roughness.png", "T_Operator_R", False, True)

# ---- Build M_Operator ----
mat = unreal.load_asset(DEST + "/M_Operator.M_Operator")
if mat is None:
    mat = tools.create_asset("M_Operator", DEST, unreal.Material, unreal.MaterialFactoryNew())
R["material"] = mat is not None

if mat:
    try:
        if base:
            b = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -450, -250)
            b.set_editor_property("texture", base)
            b.set_editor_property("sampler_type", unreal.MaterialSamplerType.SAMPLERTYPE_COLOR)
            mel.connect_material_property(b, "RGB", unreal.MaterialProperty.MP_BASE_COLOR)
        if norm:
            n = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -450, 100)
            n.set_editor_property("texture", norm)
            n.set_editor_property("sampler_type", unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
            mel.connect_material_property(n, "RGB", unreal.MaterialProperty.MP_NORMAL)
        if rough:
            r = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -450, 400)
            r.set_editor_property("texture", rough)
            r.set_editor_property("sampler_type", unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_GRAYSCALE)
            mel.connect_material_property(r, "R", unreal.MaterialProperty.MP_ROUGHNESS)
        # metallic intentionally left at 0 (cloth/leather/rubber; Meshy metallic over-blows)
        mel.recompile_material(mat)
        eal.save_loaded_asset(mat, False)
    except Exception as e:
        err("graph: " + str(e))

# ---- Assign to mesh ----
sk = unreal.load_asset(DEST + "/SK_Operator.SK_Operator")
if sk and mat:
    try:
        slot = unreal.SkeletalMaterial()
        slot.set_editor_property("material_interface", mat)
        slot.set_editor_property("material_slot_name", "Material_1")
        sk.set_editor_property("materials", [slot])
        eal.save_loaded_asset(sk, False)
        R["assigned"] = True
    except Exception as e:
        err("assign: " + str(e))
    try:
        bnds = sk.get_bounds()
        be = bnds.box_extent
        R["bounds"] = [be.x, be.y, be.z]  # half-extents (cm)
    except Exception as e:
        err("bounds: " + str(e))

with open(OUT, "w") as f:
    json.dump(R, f, indent=1)
unreal.log("BUILD_MAT: " + json.dumps(R))
