# Blast Operation Stage 1 Checklist

Date: 2026-07-06

## Goal

Stage 1 turns the project from a clean Unreal foundation into a first playable shooting prototype. The first milestone is a repeatable test range where a player can spawn, move, look, fire a server-authoritative hitscan weapon, damage replicated health, and reset training targets.

## Started

- Added `UBOHealthComponent` with replicated health, armor mitigation, reset, health change, and death delegates.
- Added `UBOWeaponData` as the first data-driven weapon definition type.
- Added `UBOWeaponComponent` with replicated ammo, fire-rate gating, server RPC firing, line trace hit detection, and point damage application.
- Expanded `ABOCharacter` with first-person camera input, movement, look, jump, health, and weapon components.
- Expanded `ABOPlayerController` to load starter Enhanced Input mapping contexts from the template assets.
- Updated `ABOGameModeBase` to spawn `ABOCharacter` and draw the starter HUD.
- Added `ABOTrainingTarget` with health, death hiding, and automatic reset.
- Added `ABOHUD` with a simple crosshair and HP/ammo readout.
- Added `Tools/Editor/PopulateStage1TestRange.py` for repeatable test range setup.
- Populated `BO_TestRange` with a PlayerStart, floor, side walls, backstop, lights, and three starter targets.
- Added Rifle and Pistol weapon data assets under `/Game/BlastOperation/Weapons/Data`.
- Added held automatic fire, timed reload, server hit confirmation, basic spread, and local recoil feedback.
- Added project-owned `IA_Reload` by duplicating the template shoot Input Action, while reload still uses a native `R` key fallback until a project-owned Mapping Context is added.

## Current Verification

- `BlastOperationEditor Mac Development` build succeeds.
- `PopulateStage1TestRange.py` commandlet succeeds with 0 errors and 0 warnings.
- `CreateStage1WeaponAssets.py` commandlet succeeds with 0 errors and 0 warnings.

## Next

- Add project-owned Enhanced Input Mapping Context entries for reload and weapon swapping.
- Add weapon switching between Rifle and Pistol.
- Add richer hit feedback for target and player damage.
- Continue recoil, spread, and movement accuracy tuning.
- Replace the debug Canvas HUD with a first UMG combat HUD when the gameplay values stabilize.
