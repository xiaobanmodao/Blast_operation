import unreal


WEAPON_DATA_DIR = "/Game/BlastOperation/Weapons/Data"
INPUT_ACTION_DIR = "/Game/BlastOperation/Input/Actions"


def ensure_directory(path: str) -> None:
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)


def create_or_load_data_asset(name: str):
    asset_path = f"{WEAPON_DATA_DIR}/{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        return unreal.EditorAssetLibrary.load_asset(asset_path)

    factory = unreal.DataAssetFactory()
    factory.set_editor_property("data_asset_class", unreal.BOWeaponData)
    return unreal.AssetToolsHelpers.get_asset_tools().create_asset(name, WEAPON_DATA_DIR, unreal.BOWeaponData, factory)


def set_properties(asset, properties: dict) -> None:
    for key, value in properties.items():
        asset.set_editor_property(key, value)
    unreal.EditorAssetLibrary.save_loaded_asset(asset)


def create_weapon_data_assets() -> None:
    ensure_directory(WEAPON_DATA_DIR)

    rifle = create_or_load_data_asset("DA_BO_Rifle")
    set_properties(
        rifle,
        {
            "display_name": "BR-01 Rifle",
            "magazine_size": 30,
            "is_automatic": True,
            "fire_rate_rpm": 620.0,
            "damage": 31.0,
            "range": 12000.0,
            "headshot_multiplier": 4.0,
            "reload_duration": 1.85,
            "stationary_spread_degrees": 0.07,
            "moving_spread_degrees": 1.35,
            "recoil_pitch_degrees": 0.42,
            "recoil_yaw_degrees": 0.18,
        },
    )

    pistol = create_or_load_data_asset("DA_BO_Pistol")
    set_properties(
        pistol,
        {
            "display_name": "BP-9 Pistol",
            "magazine_size": 12,
            "is_automatic": False,
            "fire_rate_rpm": 360.0,
            "damage": 34.0,
            "range": 9000.0,
            "headshot_multiplier": 3.0,
            "reload_duration": 1.35,
            "stationary_spread_degrees": 0.12,
            "moving_spread_degrees": 1.8,
            "recoil_pitch_degrees": 0.55,
            "recoil_yaw_degrees": 0.22,
        },
    )


create_weapon_data_assets()


def create_reload_input_action() -> None:
    ensure_directory(INPUT_ACTION_DIR)

    target_path = f"{INPUT_ACTION_DIR}/IA_Reload"
    if unreal.EditorAssetLibrary.does_asset_exist(target_path):
        return

    action = unreal.EditorAssetLibrary.duplicate_asset("/Game/Variant_Shooter/Input/Actions/IA_Shoot", target_path)
    if action:
        unreal.EditorAssetLibrary.save_loaded_asset(action)


create_reload_input_action()

unreal.log("Stage 1 weapon assets created or updated.")
