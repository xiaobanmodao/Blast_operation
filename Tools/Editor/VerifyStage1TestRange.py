import unreal


MAP_PATH = "/Game/BlastOperation/Maps/Dev/BO_TestRange"
EXPECTED_GAME_MODE = "/Script/BlastOperation.BOGameModeBase"
GENERATED_TAG = unreal.Name("Stage1Generated")


world = unreal.EditorLoadingAndSavingUtils.load_map(MAP_PATH)
if not world:
    raise RuntimeError(f"Failed to load map: {MAP_PATH}")

world_settings = world.get_world_settings()
game_mode = world_settings.get_editor_property("default_game_mode")
game_mode_path = game_mode.get_path_name() if game_mode else ""
if game_mode_path != EXPECTED_GAME_MODE:
    raise RuntimeError(f"Unexpected GameMode override: {game_mode_path}")

actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
actors = actor_subsystem.get_all_level_actors()
generated_actors = [actor for actor in actors if GENERATED_TAG in actor.tags]
player_starts = [actor for actor in actors if actor.get_class().get_name() == "PlayerStart"]
targets = [actor for actor in actors if actor.get_class().get_name() == "BOTrainingTarget"]
static_meshes = [actor for actor in generated_actors if actor.get_class().get_name() == "StaticMeshActor"]

if len(player_starts) < 1:
    raise RuntimeError("BO_TestRange has no PlayerStart.")

if len(targets) < 3:
    raise RuntimeError(f"Expected at least 3 BOTrainingTarget actors, found {len(targets)}.")

if len(static_meshes) < 4:
    raise RuntimeError(f"Expected at least 4 generated StaticMeshActor range pieces, found {len(static_meshes)}.")

unreal.log(
    "Stage 1 test range verified: "
    f"GameMode={game_mode_path}, "
    f"PlayerStarts={len(player_starts)}, "
    f"Targets={len(targets)}, "
    f"RangeMeshes={len(static_meshes)}."
)
