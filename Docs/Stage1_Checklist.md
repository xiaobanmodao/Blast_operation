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

## Current Verification

- `BlastOperationEditor Mac Development` build succeeds.
- `PopulateStage1TestRange.py` commandlet succeeds with 0 errors and 0 warnings.

## Next

- Create Rifle and Pistol `UBOWeaponData` assets.
- Add reload input and client-facing reload feedback.
- Add automatic fire hold behavior instead of one-shot trigger only.
- Add hit feedback for target and player damage.
- Start recoil, spread, and movement accuracy tuning.
- Replace the debug Canvas HUD with a first UMG combat HUD when the gameplay values stabilize.
