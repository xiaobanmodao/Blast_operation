#include "Core/BOGameplayTags.h"

namespace BOGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Team_Attacker, "Team.Attacker", "Attacking team.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Team_Defender, "Team.Defender", "Defending team.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Round_Phase_WaitingForPlayers, "Round.Phase.WaitingForPlayers", "Match is waiting for enough connected players.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Round_Phase_Warmup, "Round.Phase.Warmup", "Players can warm up before the match starts.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Round_Phase_Buy, "Round.Phase.Buy", "Players can buy weapons and tactical gear.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Round_Phase_Live, "Round.Phase.Live", "Round is live before the bomb is planted.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Round_Phase_BombPlanted, "Round.Phase.BombPlanted", "Bomb has been planted and is counting down.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Round_Phase_RoundEnding, "Round.Phase.RoundEnding", "Round result is being resolved.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Round_Phase_MatchEnding, "Round.Phase.MatchEnding", "Match result is being resolved.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Bullet, "Damage.Bullet", "Damage caused by hitscan or ballistic weapons.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Explosion, "Damage.Explosion", "Damage caused by explosive effects.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Rifle, "Weapon.Rifle", "Primary rifle weapon category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Pistol, "Weapon.Pistol", "Sidearm weapon category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Melee, "Weapon.Melee", "Melee weapon category.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gadget_Smoke, "Gadget.Smoke", "Vision-blocking smoke gadget.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gadget_Flash, "Gadget.Flash", "Blinding flash gadget.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gadget_Explosive, "Gadget.Explosive", "Explosive clearing gadget.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gadget_Decoy, "Gadget.Decoy", "Audio or information deception gadget.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Flashed, "Status.Flashed", "Player is affected by flash blindness.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Suppressed, "Status.Suppressed", "Player or equipment is suppressed by a tactical effect.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Interacting, "Status.Interacting", "Player is performing a locked interaction such as plant or defuse.");
}

