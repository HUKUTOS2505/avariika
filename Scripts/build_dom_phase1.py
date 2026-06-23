import unreal, json, traceback
R={"steps":[],"boxes":0,"err":None}
try:
    les=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    ues=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    eas=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    MAP="/Game/Avariika/Maps/L_Dom_Phase1"
    # create fresh level
    les.new_level(MAP); R["steps"].append("new_level")
    world=ues.get_editor_world()
    cube=unreal.load_asset("/Engine/BasicShapes/Cube.Cube")
    SM=unreal.StaticMeshActor
    def box(label,cx,cy,cz,sx,sy,sz):
        a=eas.spawn_actor_from_class(SM, unreal.Vector(cx,cy,cz), unreal.Rotator(0,0,0))
        c=a.static_mesh_component; c.set_static_mesh(cube)
        a.set_actor_scale3d(unreal.Vector(sx/100.0, sy/100.0, sz/100.0))
        a.set_actor_label(label); a.tags=["DOM_GB"]
        R["boxes"]+=1; return a
    T=20.0; H=280.0  # wall thickness, floor1 height (cm)
    # footprint 1300 x 1100, origin corner (0,0)
    def wall_y(lbl,x,y0,y1,gaps,z0=0.0,h=H):
        segs=[]; cur=y0
        for (gc,gw) in sorted(gaps):
            s0=gc-gw/2.0; s1=gc+gw/2.0
            if s0>cur: segs.append((cur,s0))
            cur=s1
        if y1>cur: segs.append((cur,y1))
        for i,(a,b) in enumerate(segs):
            box("%s_%d"%(lbl,i), x, (a+b)/2.0, z0+h/2.0, T, (b-a), h)
    def wall_x(lbl,y,x0,x1,gaps,z0=0.0,h=H):
        segs=[]; cur=x0
        for (gc,gw) in sorted(gaps):
            s0=gc-gw/2.0; s1=gc+gw/2.0
            if s0>cur: segs.append((cur,s0))
            cur=s1
        if x1>cur: segs.append((cur,x1))
        for i,(a,b) in enumerate(segs):
            box("%s_%d"%(lbl,i), (a+b)/2.0, y, z0+h/2.0, (b-a), T, h)
    # floor
    box("DOM_Floor1", 650,550,-10, 1300,1100,20)
    # outer walls (front y=0 with door gap at x650 w110)
    wall_x("DOM_OuterFront", 0,    0,1300, [(650,110)])
    wall_x("DOM_OuterBack",  1100, 0,1300, [])
    wall_y("DOM_OuterLeft",  0,    0,1100, [])
    wall_y("DOM_OuterRight", 1300, 0,1100, [])
    # interior verticals x=450, x=850 (door gaps at row mids y=175,550,925)
    wall_y("DOM_VWallA", 450, 0,1100, [(175,100),(550,100),(925,100)])
    wall_y("DOM_VWallB", 850, 0,1100, [(175,100),(550,100),(925,100)])
    # interior horizontals y=350, y=750 (door gaps at col mids x=225,650,1075)
    wall_x("DOM_HWallA", 350, 0,1300, [(225,100),(650,100),(1075,100)])
    wall_x("DOM_HWallB", 750, 0,1300, [(225,100),(650,100),(1075,100)])
    # PlayerStart at entrance (col2,row1 center: x650,y175)
    ps=eas.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(650,175,100), unreal.Rotator(0,90,0))
    ps.set_actor_label("PlayerStart_Dom"); R["steps"].append("playerstart")
    # placeholder: ЩИТОК near entrance (on left wall of entrance room)
    sh=box("PH_Shitok", 470,175,130, 30,60,120); sh.tags=["DOM_GB","PH"]
    # light: dim directional + skylight (greybox-visible, не кромешно)
    dl=eas.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(650,550,600), unreal.Rotator(-45,30,0))
    try: dl.directional_light_component.set_intensity(1.5)
    except Exception as e: R["steps"].append("dl "+str(e))
    dl.set_actor_label("Sun_Dim")
    sky=eas.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(650,550,400))
    try: sky.sky_light_component.set_intensity(0.4)
    except Exception: pass
    sky.set_actor_label("SkyLight_Dom")
    # gamemode override -> Avaryo (grey болван spawns)
    gm=unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoGameMode")
    try:
        world.get_world_settings().set_editor_property("default_game_mode", gm.generated_class())
        R["steps"].append("gamemode set")
    except Exception as e: R["steps"].append("gm err "+str(e))
    les.save_current_level(); R["steps"].append("saved")
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_dom1.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("BUILD_DOM_PHASE1 done")
