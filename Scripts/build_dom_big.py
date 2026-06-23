import unreal, json, traceback
R={"steps":[],"boxes":0,"err":None}
try:
    les=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    ues=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    eas=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    les.new_level("/Game/Avariika/Maps/L_Dom"); R["steps"].append("new L_Dom")
    world=ues.get_editor_world()
    cube=unreal.load_asset("/Engine/BasicShapes/Cube.Cube"); SM=unreal.StaticMeshActor
    def box(label,cx,cy,cz,sx,sy,sz,tag="DOM"):
        a=eas.spawn_actor_from_class(SM, unreal.Vector(cx,cy,cz), unreal.Rotator(0,0,0))
        a.static_mesh_component.set_static_mesh(cube)
        a.set_actor_scale3d(unreal.Vector(sx/100.0,sy/100.0,sz/100.0)); a.set_actor_label(label); a.tags=[tag]
        R["boxes"]+=1; return a
    def rbox(label,cx,cy,cz,sx,sy,sz,pitch):
        a=eas.spawn_actor_from_class(SM, unreal.Vector(cx,cy,cz), unreal.Rotator(pitch,0,0))
        a.static_mesh_component.set_static_mesh(cube)
        a.set_actor_scale3d(unreal.Vector(sx/100.0,sy/100.0,sz/100.0)); a.set_actor_label(label); a.tags=["DOM"]
        R["boxes"]+=1; return a
    T=25.0; H=300.0
    def wx(lbl,y,x0,x1,gaps,h=H,z0=0.0):
        cur=x0;s=[]
        for gc,gw in sorted(gaps):
            if gc-gw/2>cur:s.append((cur,gc-gw/2))
            cur=gc+gw/2
        if x1>cur:s.append((cur,x1))
        for i,(a,b) in enumerate(s):box("%s_%d"%(lbl,i),(a+b)/2,y,z0+h/2,(b-a)+T,T,h)
    def wy(lbl,x,y0,y1,gaps,h=H,z0=0.0):
        cur=y0;s=[]
        for gc,gw in sorted(gaps):
            if gc-gw/2>cur:s.append((cur,gc-gw/2))
            cur=gc+gw/2
        if y1>cur:s.append((cur,y1))
        for i,(a,b) in enumerate(s):box("%s_%d"%(lbl,i),x,(a+b)/2,z0+h/2,T,(b-a)+T,h)
    # ENV ground 120x120m
    box("Ground",1200,800,-50,12000,12000,100,"ENV")
    # ===== GROUND FLOOR: 24x16m (2400x1600), 4 cols x 3 rows =====
    CX=[0,600,1200,1800,2400]; RY=[0,533,1066,1600]
    STAIR=(3,2)  # back-right cell = stairwell (no floor tile, has ramp down)
    for ci in range(4):
        for ri in range(3):
            if (ci,ri)==STAIR: continue
            x0,x1=CX[ci],CX[ci+1]; y0,y1=RY[ri],RY[ri+1]
            box("F1_%d_%d"%(ci,ri),(x0+x1)/2,(y0+y1)/2,-12,(x1-x0)+T,(y1-y0)+T,24)
    box("F1_Ceiling",1200,800,H+12,2400,1600,24)
    # outer walls (front y=0 door @ x=900 w=130)
    wx("OutF",0,0,2400,[(900,130)]); wx("OutB",1600,0,2400,[])
    wy("OutL",0,0,1600,[]); wy("OutR",2400,0,1600,[(800,130)])  # right -> garage
    # interior verticals x=600,1200,1800 (door gaps at row mids)
    for x in (600,1200,1800): wy("V%d"%x,x,0,1600,[(266,100),(800,100),(1333,100)])
    # interior horizontals y=533,1066 (door gaps at col mids)
    for y in (533,1066): wx("H%d"%y,y,0,2400,[(300,100),(900,100),(1500,100),(2100,100)])
    # ===== BASEMENT: same footprint, z -300..0, ceiling 2.5m =====
    BZ=-300; BH=250.0
    box("Base_Floor",1200,800,BZ-12,2400,1600,24)
    box("Base_Ceiling",1200,800,BZ+BH+12,2400,1600,24)  # = ground floor underside
    # basement outer walls
    wx("BoutF",0,0,2400,[],BH,BZ); wx("BoutB",1600,0,2400,[],BH,BZ)
    wy("BoutL",0,0,1600,[],BH,BZ); wy("BoutR",2400,0,1600,[],BH,BZ)
    # one divider: generator room (back third)
    wx("Bdiv",1066,0,2400,[(1200,120)],BH,BZ)
    # ===== RAMP stairwell (cell 3,2: x1800-2400,y1066-1600) down to basement =====
    # ramp from z0 (y~1100) down to z BZ (y~1500), run ~480, drop 300 -> pitch ~32deg
    rbox("Ramp_Base", 2100, 1330, -150, 200, 560, 25, 32.0)
    # ===== GARAGE 10x8m attached +X (x2400-3400, y350-1150), with FLOOR =====
    box("Garage_Floor",2900,750,-12,1000,800,24)
    box("Garage_Ceiling",2900,750,H+12,1000,800,24)
    wx("GF",350,2400,3400,[]); wx("GB",1150,2400,3400,[])
    wy("GR",3400,350,1150,[(750,350)])  # garage door
    # ===== PlayerStart (entrance room col1,row0 center: x900,y266) =====
    ps=eas.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(900,266,300), unreal.Rotator(0,90,0)); ps.set_actor_label("PlayerStart_Dom")
    # ===== LIGHTS (movable) =====
    dl=eas.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(1200,800,1800), unreal.Rotator(-48,35,0)); dl.set_actor_label("Sun")
    dc=dl.get_component_by_class(unreal.DirectionalLightComponent); dc.set_mobility(unreal.ComponentMobility.MOVABLE); dc.set_intensity(10.0)
    try: dc.set_editor_property("atmosphere_sun_light", True)
    except Exception: pass
    sky=eas.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(1200,800,900)); sky.set_actor_label("Sky")
    sc=sky.get_component_by_class(unreal.SkyLightComponent); sc.set_mobility(unreal.ComponentMobility.MOVABLE)
    try: sc.set_editor_property("real_time_capture", True)
    except Exception: pass
    sc.set_intensity(1.0)
    eas.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(1200,800,0))
    try: sc.recapture_sky()
    except Exception: pass
    gm=unreal.load_asset("/Game/Avariika/Blueprints/BP_AvaryoGameMode")
    world.get_world_settings().set_editor_property("default_game_mode", gm.generated_class())
    les.save_current_level(); R["steps"].append("saved big house+basement+garage")
except Exception as e:
    R["err"]=str(e); R["tb"]=traceback.format_exc()
with open(r"C:/unrealEngine/avariika/Scripts/manifests/_big.json","w") as f: json.dump(R,f,indent=1,default=str)
unreal.log("BUILD_DOM_BIG done boxes=%d"%R["boxes"])
