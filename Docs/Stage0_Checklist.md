# Blast Operation Stage 0 Checklist

Date: 2026-07-06

## Completed

- Connected the local project plan to the GitHub repository target: `xiaobanmodao/Blast_operation`.
- Renamed the Unreal project descriptor to `BlastOperation.uproject`.
- Added a runtime C++ module named `BlastOperation`.
- Added game and editor targets.
- Added native Gameplay Framework base classes:
  - `ABOGameModeBase`
  - `ABOGameStateBase`
  - `ABOPlayerController`
  - `ABOPlayerState`
  - `ABOCharacter`
- Added replicated starter round/team state types.
- Added project log categories.
- Added native Gameplay Tags and `DefaultGameplayTags.ini`.
- Added `Content/BlastOperation` directory structure for core, maps, weapons, gadgets, UI, audio, VFX, data, dev, and test assets.
- Created `/Game/BlastOperation/Maps/Dev/BO_TestRange`.
- Created `/Game/BlastOperation/Maps/Blockout/BO_Map_Foundry_Blockout`.
- Pointed editor, game, and server default maps at `BO_TestRange`.
- Added Unreal-focused `.gitignore`.
- Added `.gitattributes` for Git LFS tracking of Unreal binary assets.

## Deferred

- Dedicated server target creation is deferred until the project uses a source-built Unreal Engine distribution. The installed UE 5.8 distribution reports that server targets are not currently supported.

## Editor Tasks

These tasks require follow-up work in Unreal Editor:

- Add Blueprint subclasses for the native base classes under `/Game/BlastOperation/Core`.
- Point map World Settings at the appropriate Blast Operation GameMode Blueprint once those assets exist.
