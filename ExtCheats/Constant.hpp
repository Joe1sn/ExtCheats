#pragma once

#include <Windows.h>
#include "BasicStruct.hpp"

namespace CheatSheet
{
	//constexpr LPCSTR PlayerName = "e8ad666b23ddc7de0c46682d70ae68f6";
	constexpr LPCSTR ttfPath = "C:\\Windows\\Fonts\\Dengb.ttf";

	constexpr int BoneSize = 1;
	constexpr int RectSize = 1;
	constexpr int BoxSize = 1;
	constexpr int HPSize = 4;
	constexpr float NameSize = 8;
	constexpr float fontSize = 16;

	constexpr int TriggerBotKey = 0x45;	//E键


}

namespace CSPlayerPawn
{
	constexpr DWORD m_pGameSceneNode = 0x308;
	constexpr DWORD m_iHeal = 0x324;
	constexpr DWORD m_iTeamNum = 0x3C3;
	constexpr DWORD m_fFlags = 0x3CC;
	constexpr DWORD m_Glow = 0xBA0;
	constexpr DWORD X = 0x1274;
	constexpr DWORD Y = 0x1278;
	constexpr DWORD Z = 0x127C;
	constexpr DWORD m_flFlashBangTime = 0x1348;
	constexpr DWORD m_angEyeAngles = 0x1388;
	constexpr DWORD m_iIDEntIndex = 0x13A8;
	//constexpr DWORD m_flDetectedByEnemySensorTime = 0x13E4;
	constexpr DWORD m_aimPunchAngle = 0x14CC;
	constexpr DWORD m_entitySpottedState = 0x2278;
	constexpr DWORD m_iShotsFired = 0x22A4; // int32
	constexpr DWORD m_ArmorValue = 0x22C0;
};

namespace CGlowProperty {
	constexpr std::ptrdiff_t m_fGlowColor = 0x8; // Vector
	constexpr std::ptrdiff_t m_iGlowType = 0x30; // int32
	constexpr std::ptrdiff_t m_iGlowTeam = 0x34; // int32
	constexpr std::ptrdiff_t m_nGlowRange = 0x38; // int32
	constexpr std::ptrdiff_t m_nGlowRangeMin = 0x3C; // int32
	constexpr std::ptrdiff_t m_glowColorOverride = 0x40; // Color
	constexpr std::ptrdiff_t m_bFlashing = 0x44; // bool
	constexpr std::ptrdiff_t m_flGlowTime = 0x48; // float32
	constexpr std::ptrdiff_t m_flGlowStartTime = 0x4C; // float32
	constexpr std::ptrdiff_t m_bEligibleForScreenHighlight = 0x50; // bool
	constexpr std::ptrdiff_t m_bGlowing = 0x51; // bool
}

namespace EntitySpottedState_t {
	constexpr DWORD m_bSpotted = 0x8; // bool
	constexpr DWORD m_bSpottedByMask = 0xC; // uint32[2]
}

namespace CSkeletonInstance {
	constexpr std::ptrdiff_t m_modelState = 0x170; // CModelState
	constexpr std::ptrdiff_t m_bIsAnimationEnabled = 0x3A0; // bool
	constexpr std::ptrdiff_t m_bUseParentRenderBounds = 0x3A1; // bool
	constexpr std::ptrdiff_t m_bDisableSolidCollisionsForHierarchy = 0x3A2; // bool
	constexpr std::ptrdiff_t m_bDirtyMotionType = 0x0; // bitfield:1
	constexpr std::ptrdiff_t m_bIsGeneratingLatchedParentSpaceState = 0x0; // bitfield:1
	constexpr std::ptrdiff_t m_materialGroup = 0x3A4; // CUtlStringToken
	constexpr std::ptrdiff_t m_nHitboxSet = 0x3A8; // uint8
}

namespace CSPlayerController
{
	constexpr DWORD m_iTeamNum = 0x3C3;
	constexpr DWORD m_iszPlayerName = 0x630;
	constexpr DWORD m_iszPlayerName_Size = 128;
	//constexpr DWORD m_steamID = 0x6C0;
	constexpr DWORD m_hPlayerPawn = 0x7DC;
	constexpr DWORD m_bPawnIsAlive = 0x7E4;
	constexpr DWORD m_iPawnHealth = 0x7E8;

};

namespace Engine2DLL
{
	constexpr std::ptrdiff_t dwWindowHeight = 0x5E045C;
	constexpr std::ptrdiff_t dwWindowWidth = 0x5E0458;
}

namespace ClientDLL
{
	//constexpr DWORD C_BaseFlex = 0x192DA94;
	//constexpr DWORD C_BaseFlex = 0x192DB90;
	constexpr DWORD C_CSPlayerController = 0x19FBBB0;	//0x191C5A8;
	constexpr DWORD C_CSPlayerController_Gap = 0x78;
	constexpr DWORD dwViewAngles = 0x1A156F8;
	constexpr DWORD dwViewMatrix = 0x1A085A0;
	constexpr DWORD dwAttack = 0x18097B0;		//256:stop attack , 65537 keep attack
	constexpr DWORD jump = 0x1809CC0;  //256:stop attack , 65537 keep attack
	constexpr DWORD PlayerController = 0x19F6488;// 0x19F4468;
	constexpr DWORD PlayerPawn = 0x1810FA8;
	constexpr DWORD dwSensitivity = 0x1A04C18;	//灵敏度，用法是 dwS->dwS_s
	constexpr DWORD dwSensitivity_sensitivity = 0x40;
};

namespace BoneId
{
	constexpr WORD Waist = 0;	 //0
	constexpr WORD Neck = 5; //1
	constexpr WORD Head = 6; //2
	constexpr WORD ShoulderLeft = 8; //3
	constexpr WORD ForeLeft = 9; //4
	constexpr WORD HandLeft = 11; //5
	constexpr WORD ShoulderRight = 13; //6
	constexpr WORD ForeRight = 14; //7
	constexpr WORD HandRight = 16; //8
	constexpr WORD KneeLeft = 23; //9
	constexpr WORD FeetLeft = 24; //10
	constexpr WORD KneeRight = 26; //11
	constexpr WORD FeetRight = 27; //12
	constexpr WORD BoneIndex[13] = {
		BoneId::Waist, BoneId::Neck, BoneId::Head, BoneId::ShoulderLeft, BoneId::ForeLeft, BoneId::HandLeft, BoneId::ShoulderRight, BoneId::ForeRight, BoneId::HandRight, BoneId::KneeLeft, BoneId::FeetLeft, BoneId::KneeRight, BoneId::FeetRight
	};
}