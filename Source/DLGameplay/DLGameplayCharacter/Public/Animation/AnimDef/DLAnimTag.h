#pragma once

#include "CoreMinimal.h"
#include "Animation/DLAnimMacros.h"
#include "AnimationV2/DLAnimationMacros.h"

namespace EAnimTagDef
{
	inline FName TagBuild(FString Scope, const FString& Tag)
	{
		Scope.AppendChar('.').Append(Tag);
		return FName(Scope);
	}

	/*角色动画状态*/
	namespace ECharacterAnimState
	{
		/*待机*/
		DECLARE_ANIM_TAG(Animation.CharacterAnimState, Idle);
		/*行走(方向)*/
		DECLARE_ANIM_TAG(Animation.CharacterAnimState, WalkDirection);
		/*行走（转身）*/
		DECLARE_ANIM_TAG(Animation.CharacterAnimState, Walk_Turn);
		/*行走(停止)*/
		DECLARE_ANIM_TAG(Animation.CharacterAnimState, Walk_Stop);
		/*锁定行走(方向)*/
		DECLARE_ANIM_TAG(Animation.CharacterAnimState, Locked_Walk_Direction);
		/*小跑（方向）*/
		DECLARE_ANIM_TAG(Animation.CharacterAnimState, Jog_Direction);
		/*小跑（转身）*/
		DECLARE_ANIM_TAG(Animation.CharacterAnimState, Jog_Turn);
		/*小跑（停止）*/
		DECLARE_ANIM_TAG(Animation.CharacterAnimState, Jog_Stop);
		/*锁定小跑(方向)*/
		DECLARE_ANIM_TAG(Animation.CharacterAnimState, Locked_Jog_Direction);
		/*加速（方向）*/
		DECLARE_ANIM_TAG(Animation.CharacterAnimState, Accelerate);
		/*疾跑（方向）*/
		DECLARE_ANIM_TAG(Animation.CharacterAnimState, Sprint_Direction);
		/*疾跑（转身）*/
		DECLARE_ANIM_TAG(Animation.CharacterAnimState, Sprint_Turn);
		/*疾跑（停止）*/
		DECLARE_ANIM_TAG(Animation.CharacterAnimState, Sprint_Stop);
		/*翻滚（方向）*/
		DECLARE_ANIM_TAG(Animation.CharacterAnimState, Roll_Direction);
		/*翻滚后（转身）*/
		DECLARE_ANIM_TAG(Animation.CharacterAnimState, After_Roll_Then_Turn);
		/*后撤*/
		DECLARE_ANIM_TAG(Animation.CharacterAnimState, Retreat);
	}

	//动画名字
	namespace EAnimName
	{
		// DECLARE_ANIM_TAG(AnimName, Idle);
		// DECLARE_ANIM_TAG(AnimName, Death);
		// DECLARE_ANIM_TAG(AnimName, BattleIdle);
		// DECLARE_ANIM_TAG(AnimName, WalkDirectionForward);
		// DECLARE_ANIM_TAG(AnimName, WalkDirectionRight);
		// DECLARE_ANIM_TAG(AnimName, WalkDirectionLeft);
		// DECLARE_ANIM_TAG(AnimName, WalkDirectionBackward);
		// DECLARE_ANIM_TAG(AnimName, WalkTurnRight);
		// DECLARE_ANIM_TAG(AnimName, WalkTurnLeft);
	}

	namespace EBoneName
	{
		DECLARE_ANIM_TAG(BoneName, Root);
		DECLARE_ANIM_TAG(BoneName, IK_Hand_Root);
		DECLARE_ANIM_TAG(BoneName, ik_hand_r);
		DECLARE_ANIM_TAG(BoneName, ik_hand_l);
		DECLARE_ANIM_TAG(BoneName, IK_Foot_Root);
		DECLARE_ANIM_TAG(BoneName, ik_foot_r);
		DECLARE_ANIM_TAG(BoneName, ik_foot_l);
		DECLARE_ANIM_TAG(BoneName, Bip001);
		DECLARE_ANIM_TAG(BoneName, Pelvis);
		DECLARE_ANIM_TAG(BoneName, Spine);
		DECLARE_ANIM_TAG(BoneName, Spine1);
		DECLARE_ANIM_TAG(BoneName, Spine2);
		DECLARE_ANIM_TAG(BoneName, Neck);
		DECLARE_ANIM_TAG(BoneName, Head);
		DECLARE_ANIM_TAG(BoneName, L_Clavicle);
		DECLARE_ANIM_TAG(BoneName, L_UpperArm);
		DECLARE_ANIM_TAG(BoneName, L_Forearm);
		DECLARE_ANIM_TAG(BoneName, L_Hand);
		DECLARE_ANIM_TAG(BoneName, L_Finger0);
		DECLARE_ANIM_TAG(BoneName, L_Finger01);
		DECLARE_ANIM_TAG(BoneName, L_Finger02);
		DECLARE_ANIM_TAG(BoneName, L_Finger1);
		DECLARE_ANIM_TAG(BoneName, L_Finger11);
		DECLARE_ANIM_TAG(BoneName, L_Finger12);
		DECLARE_ANIM_TAG(BoneName, L_Finger2);
		DECLARE_ANIM_TAG(BoneName, L_Finger21);
		DECLARE_ANIM_TAG(BoneName, L_Finger22);
		DECLARE_ANIM_TAG(BoneName, L_Finger3);
		DECLARE_ANIM_TAG(BoneName, L_Finger31);
		DECLARE_ANIM_TAG(BoneName, L_Finger32);
		DECLARE_ANIM_TAG(BoneName, L_Finger4);
		DECLARE_ANIM_TAG(BoneName, L_Finger41);
		DECLARE_ANIM_TAG(BoneName, L_Finger42);
		DECLARE_ANIM_TAG(BoneName, L_ForeTwist);
		DECLARE_ANIM_TAG(BoneName, L_ForeTwist1);
		DECLARE_ANIM_TAG(BoneName, LUpArmTwist);
		DECLARE_ANIM_TAG(BoneName, LUpArmTwist1);
		DECLARE_ANIM_TAG(BoneName, R_Clavicle);
		DECLARE_ANIM_TAG(BoneName, R_UpperArm);
		DECLARE_ANIM_TAG(BoneName, R_Forearm);
		DECLARE_ANIM_TAG(BoneName, R_Hand);
		DECLARE_ANIM_TAG(BoneName, R_Finger0);
		DECLARE_ANIM_TAG(BoneName, R_Finger01);
		DECLARE_ANIM_TAG(BoneName, R_Finger02);
		DECLARE_ANIM_TAG(BoneName, R_Finger1);
		DECLARE_ANIM_TAG(BoneName, R_Finger11);
		DECLARE_ANIM_TAG(BoneName, R_Finger12);
		DECLARE_ANIM_TAG(BoneName, R_Finger2);
		DECLARE_ANIM_TAG(BoneName, R_Finger21);
		DECLARE_ANIM_TAG(BoneName, R_Finger22);
		DECLARE_ANIM_TAG(BoneName, R_Finger3);
		DECLARE_ANIM_TAG(BoneName, R_Finger31);
		DECLARE_ANIM_TAG(BoneName, R_Finger32);
		DECLARE_ANIM_TAG(BoneName, R_Finger4);
		DECLARE_ANIM_TAG(BoneName, R_Finger41);
		DECLARE_ANIM_TAG(BoneName, R_Finger42);
		DECLARE_ANIM_TAG(BoneName, R_ForeTwist);
		DECLARE_ANIM_TAG(BoneName, R_ForeTwist1);
		DECLARE_ANIM_TAG(BoneName, RUpArmTwist);
		DECLARE_ANIM_TAG(BoneName, RUpArmTwist1);
		DECLARE_ANIM_TAG(BoneName, Breast_L);
		DECLARE_ANIM_TAG(BoneName, Breast_R);
		DECLARE_ANIM_TAG(BoneName, L_Thigh);
		DECLARE_ANIM_TAG(BoneName, L_Calf);
		DECLARE_ANIM_TAG(BoneName, L_Foot);
		DECLARE_ANIM_TAG(BoneName, L_Toe0);
		DECLARE_ANIM_TAG(BoneName, SkinMend_Calf_R01);
		DECLARE_ANIM_TAG(BoneName, SkinMend_Knee_L);
		DECLARE_ANIM_TAG(BoneName, R_Thigh);
		DECLARE_ANIM_TAG(BoneName, R_Calf);
		DECLARE_ANIM_TAG(BoneName, R_Foot);
		DECLARE_ANIM_TAG(BoneName, R_Toe0);
		DECLARE_ANIM_TAG(BoneName, SkinMend_Calf_L01);
		DECLARE_ANIM_TAG(BoneName, SkinMend_Knee_R);
		DECLARE_ANIM_TAG(BoneName, Stretch_Pelvis_R);
		DECLARE_ANIM_TAG(BoneName, Stretch_Pelvis_L);
		DECLARE_ANIM_TAG(BoneName, VB_FootTarget_L);
		DECLARE_ANIM_TAG(BoneName, VB_FootTarget_R);
	}
}