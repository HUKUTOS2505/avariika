import unreal, json
B="/Game/PostApocalypticHouse/Mesh/Structure/"
names=["SM_FacadeWall_4m_01a","SM_FacadeWall_3m_01a","SM_FacadeWall_2m_01a","SM_FacadeWall_1m_01a",
       "SM_FacadeWall_DoorCut_01a","SM_FacadeWall_Window_01a","SM_CornerWall_01a","SM_Door_Frame_01a",
       "SM_Floor_4x4m_01a","SM_Floor_3x3m_01a","SM_Floor_3x4m_01a","SM_Floor_2x2m_01a",
       "SM_Stairs_01a","SM_Steps_01a"]
R={}
for n in names:
    m=unreal.load_asset(B+n+"."+n)
    if not m: R[n]="missing"; continue
    try:
        b=m.get_bounds()  # FBoxSphereBounds
        ext=b.box_extent; org=b.origin
        R[n]={"size_cm":[round(ext.x*2,1),round(ext.y*2,1),round(ext.z*2,1)],"origin":[round(org.x,1),round(org.y,1),round(org.z,1)]}
    except Exception as e: R[n]="err "+str(e)
with open(r"D:/unrealEngine/avariika/Scripts/manifests/_mesh_bounds.json","w") as f: json.dump(R,f,indent=1)
unreal.log("MESH_BOUNDS done")
