# Blast Operation Stage 1 Checklist

Date: 2026-07-06

## Goal

Stage 1 turns the project from a clean Unreal foundation into a first playable shooting prototype. The first milestone is a repeatable test range where a player can spawn, move, look, fire a server-authoritative hitscan weapon, damage replicated health, and reset training targets.

## Started

- Added `UBOHealthComponent` with replicated health, armor mitigation, reset, health change, and death delegates.
- Added `UBOWeaponData` as the first data-driven weapon definition type.
- Added `UBOWeaponComponent` with replicated ammo, fire-rate gating, server RPC firing, line trace hit detection, and point damage application.
- Expanded `ABOCharacter` with first-person camera input, movement, look, jump, health, and weapon components.
- Expanded `ABOPlayerController` to load the project-owned `IMC_BO_Combat` mapping context.
- Updated `ABOGameModeBase` to spawn `ABOCharacter` and draw the starter HUD.
- Added `ABOTrainingTarget` with health, death hiding, and automatic reset.
- Added `ABOHUD` with a simple crosshair and HP/ammo readout.
- Added `UBOCombatHUDWidget` as the first native UMG combat HUD.
- Added `Tools/Editor/PopulateStage1TestRange.py` for repeatable test range setup.
- Populated `BO_TestRange` with a PlayerStart, floor, side walls, backstop, lights, and three starter targets.
- Added Rifle and Pistol weapon data assets under `/Game/BlastOperation/Weapons/Data`.
- Added held automatic fire, timed reload, server hit confirmation, basic spread, and local recoil feedback.
- Replaced template input dependencies with project-owned `IA_BO_*` actions and `IMC_BO_Combat`.
- Added weapon slot switching between Rifle and Pistol, preserving per-slot magazine state.
- Added `UBOCombatFeedbackData` and `DA_BO_CombatFeedback` so HUD crosshair and hitmarker presentation are data driven.
- Added `UBOCreateStage1AssetsCommandlet` to create Stage 1 input, weapon, and combat feedback assets from C++.
- Expanded hit confirmation with actual damage, remaining health, and fatal-hit context.
- Added HUD damage numbers, fatal-hit coloring, and elimination feedback.
- Added replicated training target active state plus short local damage pulse feedback before reset.
- Expanded weapon accuracy tuning with stationary, moving, airborne, consecutive-shot, and recovery spread parameters.
- Added dynamic four-segment crosshair expansion driven by current weapon spread.
- Replaced the debug Canvas HUD drawing path with a native UMG widget mounted by `ABOHUD`.
- Added weapon-specific recoil pattern authoring for Rifle and Pistol, with pattern reset and fallback recoil.
- Added per-surface impact feedback data, physical material assets, and multicast impact markers.
- Added Blueprint extension events to `UBOCombatHUDWidget` so a `WBP` layout can inherit the native data path.
- Updated `BO_TestRange` generation to apply Concrete and Metal physical materials to generated range geometry.
- Added a visible first-person weapon view model, editor-visible training target setup, map-level `BOGameModeBase` override, and `VerifyStage1TestRange.py`.

## Current Verification

- `BlastOperationEditor Mac Development` build succeeds.
- `PopulateStage1TestRange.py` commandlet succeeds with 0 errors and 0 warnings.
- `BOCreateStage1Assets` commandlet succeeds with 0 errors and 0 warnings.
- `VerifyStage1TestRange.py` commandlet succeeds with 0 errors and 0 warnings.

## Next

- Replace prototype debug impact markers with final Niagara and authored sound assets.
- Promote the native UMG HUD to a Blueprint-authored visual layout once final visual language is established.
- Run a focused Stage 1 playtest pass and tune Rifle/Pistol recoil pattern values from feel.
