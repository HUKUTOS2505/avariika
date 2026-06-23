import unreal, json, traceback
R={"steps":[],"boxes":0,"err":None}
try:
    les=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    ues=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    eas=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    les.new_level("/Game/Avariika/Maps/L_Dom"); R["steps"].append("new L_Dom")
    world=ues.get_editor_world()
    cube=unreal.load_asset("/Engine/BasicShapes/Cube.Cube")
    SM=unreal.StaticMeshActor
    def box(label,cx,cy,cz,sx,sy,sz,tag="DOM"):
        a=eas.spawn_actor_from_class(SM, unreal.Vector(cx,cy,cz), unreal.Rotator(0,0,0))
        a.static_mesh_component.set_static_mesh(cube)
        a.set_actor_scale3d(unreal.Vector(sx/100.0,sy/100.0,sz/100.0)); a.set_actor_label(label); a.tags=[tag]
        R["boxes"]+=1; return a
    T=20.0; H=300.0  # wall thickness, ceiling height
    def wall_x(lbl,y,x0,x1,gaps,h=H,z0=0.0):
        cur=x0; segs=[]
        for gc,gw in sorted(gaps):
            if gc-gw/2>cur: segs.append((cur,gc-gw/2));
            cur=gc+gw/2
        if x1>cur: segs.append((cur,x1))
        for i,(a,b) in enumerate(segs): box("%s_%d"%(lbl,i),(a+b)/2,y,z0+h/2,(b-a),T,h)
    def wall_y(lbl,x,y0,y1,gaps,h=H,z0=0.0):
        cur=y0; segs=[]
        for gc,gw in sorted(gaps):
            if gc-gw/2>cur: segs.append((cur,gc-gw/2))
            cur=gc+gw/2
        if y1>cur: segs.append((cur,y1))
        for i,(a,b) in enumerate(segs): box("%s_%d"%(lbl,i),x,(a+b)/2,z0+h/2,T,(b-a),h)
    # GROUND 80x80m
    box("Ground",800,600,-15,8000,8000,30,"ENV")
    # HOUSE footprint 16x12m (1600x1200), 3x3 grid cols x:0/550/1050/1600 rows y:0/400/800/1200
    box("House_Floor",800,600,-2,1600,1200,8)
    box("House_Ceiling",800,600,H+4,1600,1200,8)
    # outer walls (front y=0 door gap @ x800 w120)
    wall_x("OutFront",0,0,1600,[(800,120)]); wall_x("OutBack",1200,0,1600,[])
    wall_y("OutLeft",0,0,1200,[]); wall_y("OutRight",1600,0,1200,[(600,120)])  # right gap -> garage
    # interior grid verticals x=550,1050 (door gaps at row mids 200,600,1000)
    wall_y("VA",550,0,1200,[(200,100),(600,100),(1000,100)])
    wall_y("VB",1050,0,1200,[(200,100),(600,100),(1000,100)])
    # interior horizontals y=400,800 (door gaps at col mids 275,800,1325)
    wall_x("HA",400,0,1600,[(275,100),(800,100),(1325,100)])
    wall_x("HB",800,0,1600,[(275,100),(800,100),(1325,100)])
    # GARAGE attached on +X (x 1600..2300, y 200..1000 = 7x8m), big garage door on far +X
    box("Garage_Floor",1950,600,-2,700,800,8)
    box("Garage_Ceiling",1950,600,H+4,700,800,8)
    wall_x("GarFront",200,1600,2300,[]); wall_x("GarBack",1000,1600,2300,[])
    wall_y("GarRight",2300,200,1000,[(600,300)])  # big garage door opening
    # PLAYER START (entrance room center: x800,y200)
    ps=eas.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(800,200,120), unreal.Rotator(0,90,0)); ps.set_actor_label("PlayerStart_Dom")
    # LIGHTING - MOVABLE (the fix: stationary needs build -> black)
    dl=eas.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(800,600,1500), unreal.Rotator(-50,30,0)); dl.set_actor_label("Sun")
    dc=dl.get_component_by_class(unreal.DirectionalLightComponent)
    dc.set_mobility(unreal.ComponentMobility.MOVABLE); dc.set_intensity(8.0)
    try: dc.set_editor_property("atmosphere_sun_light", True)
    except Exception: pass
    sky=eas.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(800,600,800)); sky.set_actor_label("Sky")
    sc=sky.get_component_by_class(unreal.SkyLightComponent); sc.set_mobility(unreal.ComponentMobility.MOVABLE)
    try: sc.set_editor_property("real_time_capture", True)
    except Exception: pass
    sc.set_intensity(1.0)
    eas.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(800,600,0))
    try: sc.recapture_sky()
    except Exception: pass
    R["steps"].append("movable lights")
    # gamemode Avaryo
    gm=unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoGameMode")
    world.get_world_settings().set_editor_property("default_game_mode", gm.generated_class())
    les.save_current_level(); R["steps"].append("saved")
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_blockout.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("BUILD_BLOCKOUT done")
