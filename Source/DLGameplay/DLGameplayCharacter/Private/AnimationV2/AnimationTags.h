// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLAnimationMacros.h"

namespace EAnimationTag
{
	/*角色动画状态*/
	namespace ECharacterAnimState
	{
		/*空闲状态*/
		DECLARE_ANIM_TAG(Gameplay.Unit.State, Idle);
		/*在陆地*/
		DECLARE_ANIM_TAG(Gameplay.Character.State, InTheLand);
		/*正在空中*/
		DECLARE_ANIM_TAG(Gameplay.Character.State, InTheSky);
		/*锁定目标的状态*/
		DECLARE_ANIM_TAG(Gameplay.Character.State, LockTarget);
		/*跑*/
		DECLARE_ANIM_TAG(Gameplay.Character.State.Movement, Run);
		/*冲刺( 加速跑 )*/
		DECLARE_ANIM_TAG(Gameplay.Character.State.Movement, Sprint);
		/*走*/
		DECLARE_ANIM_TAG(Gameplay.Character.State.Movement, Walk);
		/*转向*/
		DECLARE_ANIM_TAG(Gameplay.Character.State.Movement, TurnTo);
		/*翻滚*/
		DECLARE_ANIM_TAG(Gameplay.Character.State.Movement, Roll);
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