#include "Interface/IDLLocalPlayerGameplayInterface.h"


namespace EDLGameplayMsgType
{
	UE_DEFINE_GAMEPLAY_TAG(LocalPlayer_Gameplay_InBattleState, "DLMsgType.LocalPlayer.Gameplay.InBattleState");
	UE_DEFINE_GAMEPLAY_TAG(LocalPlayer_Gameplay_OutBattleState, "DLMsgType.LocalPlayer.Gameplay.OutBattleState");
	UE_DEFINE_GAMEPLAY_TAG(LocalPlayer_Gameplay_PlayerStart, "DLMsgType.LocalPlayer.Gameplay.PlayerStart");
	UE_DEFINE_GAMEPLAY_TAG(LocalPlayer_Gameplay_PlayerDied, "DLMsgType.LocalPlayer.Gameplay.PlayerDied");
	UE_DEFINE_GAMEPLAY_TAG(LocalPlayer_Gameplay_EnterNewScene, "DLMsgType.LocalPlayer.Gameplay.EnterNewScene");
	UE_DEFINE_GAMEPLAY_TAG(LocalPlayer_Gameplay_LoseTarget, "DLMsgType.LocalPlayer.Gameplay.LoseTarget");
	UE_DEFINE_GAMEPLAY_TAG(LocalPlayer_Gameplay_FocusNewTarget, "DLMsgType.LocalPlayer.Gameplay.FocusNewTarget");
};

