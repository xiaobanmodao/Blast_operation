import unreal


MAP_PATH = "/Game/BlastOperation/Maps/Dev/BO_TestRange"
GENERATED_TAG = "Stage1Generated"
ACTOR_SUBSYSTEM = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
CONCRETE_PHYSICAL_MATERIAL = unreal.EditorAssetLibrary.load_asset("/Game/BlastOperation/Materials/Physics/PM_BO_Concrete.PM_BO_Concrete")
METAL_PHYSICAL_MATERIAL = unreal.EditorAssetLibrary.load_asset("/Game/BlastOperation/Materials/Physics/PM_BO_Metal.PM_BO_Metal")


def tag_actor(actor: unreal.Actor) -> unreal.Actor:
    actor.tags = [unreal.Name(GENERATED_TAG)]
    return actor


def spawn_cube(name: str, location: unreal.Vector, scale: unreal.Vector, physical_material=None) -> unreal.Actor:
    mesh = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cube.Cube")
    actor = tag_actor(ACTOR_SUBSYSTEM.spawn_actor_from_class(unreal.StaticMeshActor, location))
    actor.set_actor_label(name)
    actor.set_actor_scale3d(scale)
    actor.static_mesh_component.set_static_mesh(mesh)
    actor.static_mesh_component.set_collision_profile_name("BlockAll")
    if physical_material:
        actor.static_mesh_component.set_phys_material_override(physical_material)
    return actor


def clear_generated_actors() -> None:
    for actor in ACTOR_SUBSYSTEM.get_all_level_actors():
        if unreal.Name(GENERATED_TAG) in actor.tags:
            ACTOR_SUBSYSTEM.destroy_actor(actor)


world = unreal.EditorLoadingAndSavingUtils.load_map(MAP_PATH)
if not world:
    raise RuntimeError(f"Failed to load map: {MAP_PATH}")

clear_generated_actors()

player_start = tag_actor(ACTOR_SUBSYSTEM.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(0, 0, 120)))
player_start.set_actor_label("BO_PlayerStart_TestRange")
player_start.set_actor_rotation(unreal.Rotator(0, 0, 0), False)

spawn_cube("BO_TestRange_Floor", unreal.Vector(900, 0, -60), unreal.Vector(20, 12, 0.2), CONCRETE_PHYSICAL_MATERIAL)
spawn_cube("BO_TestRange_Backstop", unreal.Vector(2200, 0, 220), unreal.Vector(0.25, 8, 3.0), METAL_PHYSICAL_MATERIAL)
spawn_cube("BO_TestRange_LeftWall", unreal.Vector(900, -650, 160), unreal.Vector(20, 0.2, 2.2), CONCRETE_PHYSICAL_MATERIAL)
spawn_cube("BO_TestRange_RightWall", unreal.Vector(900, 650, 160), unreal.Vector(20, 0.2, 2.2), CONCRETE_PHYSICAL_MATERIAL)

for index, y in enumerate([-260, 0, 260]):
    target = tag_actor(ACTOR_SUBSYSTEM.spawn_actor_from_class(unreal.BOTrainingTarget, unreal.Vector(1800, y, 120)))
    target.set_actor_label(f"BO_TrainingTarget_{index + 1}")

light = tag_actor(ACTOR_SUBSYSTEM.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(-600, -400, 600)))
light.set_actor_label("BO_TestRange_KeyLight")
light.set_actor_rotation(unreal.Rotator(-45, 25, 0), False)
light.light_component.set_editor_property("intensity", 4.0)

fill = tag_actor(ACTOR_SUBSYSTEM.spawn_actor_from_class(unreal.PointLight, unreal.Vector(600, 0, 500)))
fill.set_actor_label("BO_TestRange_FillLight")
fill.point_light_component.set_editor_property("intensity", 1800.0)
fill.point_light_component.set_editor_property("attenuation_radius", 2200.0)

unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
unreal.log("Stage 1 test range populated.")
