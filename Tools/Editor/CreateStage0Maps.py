import unreal


MAPS = [
    "/Game/BlastOperation/Maps/Dev/BO_TestRange",
    "/Game/BlastOperation/Maps/Blockout/BO_Map_Foundry_Blockout",
]


def create_blank_map(asset_path: str) -> None:
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        unreal.log(f"Stage0 map already exists: {asset_path}")
        return

    world = unreal.EditorLoadingAndSavingUtils.new_map_from_template("", False)
    if not world:
        raise RuntimeError(f"Failed to create blank map for {asset_path}")

    if not unreal.EditorLoadingAndSavingUtils.save_map(world, asset_path):
        raise RuntimeError(f"Failed to save map: {asset_path}")

    unreal.log(f"Created Stage0 map: {asset_path}")


for map_path in MAPS:
    create_blank_map(map_path)

unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)

