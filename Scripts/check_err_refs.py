import unreal
_OUT=[]
def line(s=""):
    unreal.log("ERRREF| "+str(s)); _OUT.append(str(s))

ar = unreal.AssetRegistryHelpers.get_asset_registry()

targets = [
 "/Game/_Packs/FootstepSystem/FootstepLogic/Blueprints/AC_Footstep_Logic",
 "/Game/_Packs/FootstepSystem/Character/Blueprints/BP_FootstepCharacter",
 "/Game/_Packs/FootstepSystem/Character/Mannequin/Animations/ThirdPerson_AnimBP_Footstep",
 "/Game/_Packs/Hyper/Core/MainMenu/BPI_Ingame_Menu",
 "/Game/_Packs/Hyper/UI/Widgets/Specific/Interaction/Can_Interact/BP_CanInteract_Button",
 "/Game/_Packs/Hyper/Core/WeatherManager/BlueprintInterface/BPI_Biome",
 "/Game/_Packs/Hyper/Core/WeatherManager/BlueprintInterface/BPI_ControlArea_NotifyPlayer",
 "/Game/_Packs/Hyper/Core/WeatherManager/BlueprintInterface/BPI_WeatherGameState",
 "/Game/_Packs/Hyper/UI/Blueprints/HUD/HUD_Master",
 "/Game/IndustrialFactory/Demo/EpicContent/ThirdPersonBP/Blueprints/bp_ThirdPersonCharacter_SF",
 "/Game/AnimX/_Common/Demo/ThirdPersonBP/Blueprints/ThirdPersonCharacter",
 "/Game/AnimX/Dogs/AnimBP_Dog_GH",
 "/Game/AnimX/_Common/Demo/ThirdPersonBP/Mannequin/Animations/ThirdPerson_AnimBP",
 "/Game/FirstPerson/Anims/ABP_FP_Copy",
]

opts = unreal.AssetRegistryDependencyOptions(include_hard_package_references=True, include_soft_package_references=True)
GAME_PREFIXES = ("/Game/Avariika/", "/Game/blueprinsTest/")  # our own content
any_game_ref = False
for t in targets:
    refs = ar.get_referencers(t, opts) or []
    # referencers OUTSIDE the same top-level pack folder
    own_pack = "/" + t.split("/")[2] if len(t.split("/")) > 2 else ""
    external = [str(r) for r in refs if not str(r).startswith("/Game/" + t.split("/")[2] + "/")]
    game_refs = [r for r in external if any(str(r).startswith(p) for p in GAME_PREFIXES)]
    flag = "  <<< OUR GAME REFERENCES THIS" if game_refs else ""
    if game_refs:
        any_game_ref = True
    line("%s : refs=%d external=%d%s" % (t.split("/")[-1], len(refs), len(external), flag))
    for g in game_refs:
        line("      OUR-REF: %s" % g)

# Also: is any of them placed in the active maps?
line("--- any referenced by our maps? ---")
for mp in ("/Game/Avariika/Maps/Lvl_FirstPerson", "/Game/Avariika/Maps/L_MainMenu"):
    deps = ar.get_dependencies(mp, opts) or []
    hits = [str(d) for d in deps if any(d2 in str(d) for d2 in ("FootstepSystem","Hyper","IndustrialFactory","AnimX","ABP_FP_Copy","ThirdPerson"))]
    line("%s -> suspicious deps: %s" % (mp.split("/")[-1], hits if hits else "NONE"))

line("=== ANY GAME REF: %s ===" % any_game_ref)
with open("C:/unrealEngine/avariika/Scripts/check_err_refs_out.txt","w",encoding="utf-8") as f:
    f.write("\n".join(_OUT))
