#include "Cheats.h"
#include "Constant.hpp"
#include "Render.h"
#include "globals.h"

#include <iostream>
#include <windows.h>
#include <math.h>
#include <TlHelp32.h>
#include <codecvt>
/// <summary>
///	通过进程名称获得进程ID
/// </summary>
/// <param name="ProcessName">进程名称</param>
/// <returns>
/// FALSE: 未获取到进程pid
/// TRUE : 获取到进程pid
/// </returns>
DWORD bGetPIDByName(LPCWSTR ProcessName)
{
	DWORD dwPID = 0;
	DWORD result = 0;
	HANDLE hTool = NULL;
	PROCESSENTRY32W* pe = NULL;
	BOOL bRet = FALSE;

	hTool = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe = (PROCESSENTRY32W*)malloc(sizeof(PROCESSENTRY32W));
	if (pe == NULL) {
		bRet = FALSE;
	}
	else {
		pe->dwSize = sizeof(PROCESSENTRY32W);
		result = Process32FirstW(hTool, pe);
		if (result) {
			do
			{
				result = FALSE;
				//std::wcout << pe->szExeFile << "\n";
				if (!lstrcmpW((LPCWSTR)pe->szExeFile, ProcessName)) {
					CloseHandle(hTool);
					dwPID = pe->th32ProcessID;
					bRet = TRUE;
					break;
				}
				bRet = FALSE;
			} while (Process32NextW(hTool, pe));
		}
	}
	return dwPID;
}

/// <summary>
/// 得到模块基地址
/// </summary>
/// <param name="ModuleName">模块名称，如Client.dll</param>
/// <param name="PID">进程id</param>
/// <param name="dwBaseAddr">返回的模块基地址</param>
/// <returns>
/// FALSE: 未获取到DLL基地址
/// TRUE: 获取到DLL基地址，DLL基地址为 dwBaseAddr
/// </returns>
BOOL bGetModBaseAddr(LPCWSTR ModuleName, DWORD PID, DWORD64& dwBaseAddr) {
	MODULEENTRY32 md;
	HANDLE hSnapShot = NULL;
	BOOL result = FALSE;
	BOOL bRet = FALSE;

	md.dwSize = sizeof(MODULEENTRY32);
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
	result = Module32First(hSnapShot, &md);
	if (!hSnapShot)
		bRet = FALSE;
	else if (result) {
		do {
			//wcout << md.szModule << "\n";
			if (!lstrcmpW(md.szModule, ModuleName)) {
				dwBaseAddr = (DWORD64)md.modBaseAddr;
				bRet = TRUE;
				break;
			}
			bRet = FALSE;
		} while (Module32Next(hSnapShot, &md));
		CloseHandle(hSnapShot);
	}
	else {
		CloseHandle(hSnapShot);
		bRet = FALSE;
	}
	return bRet;
}

/// <summary>
/// 通过指针偏移获得内存，返回长度为8bytes，用法参考CE
/// </summary>
/// <param name="hProcess">进程句柄</param>
/// <param name="dwBase">基地址</param>
/// <param name="depth">指针偏移总量</param>
/// <param name="">偏移</param>
/// <returns>内存中的值</returns>
DWORD64 GetProcessMem(HANDLE hProcess, DWORD64 dwBase, int depth, ...) {
	DWORD64 Address;
	va_list dwPointers;
	va_start(dwPointers, depth);
	int nArg;
	DWORD Dr = 0;

	Address = dwBase;
	for (nArg = 0; nArg < depth; nArg++)
	{
		Dr = va_arg(dwPointers, DWORD);
		ReadProcessMemory(hProcess, (PBYTE*)(DWORD64(Address + Dr)), &Address, sizeof(DWORD64), 0);
	}
	va_end(dwPointers);
	return Address;
}

std::wstring charTowstring(char str[]) {
	//std::wcout.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
	//std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	//std::wstring wideStr = converter.from_bytes(str);
	//return wideStr;
	int length = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	WCHAR* buf = new WCHAR[length + 1];
	ZeroMemory(buf, (length + 1) * sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP, 0, str, -1, buf, length);
	std::wstring strRet(buf);
	delete[] buf;
	return strRet;
}

/// <summary>
/// 获得玩家昵称
/// </summary>
/// <param name="hProcess">进程句柄</param>
/// <param name="dwBase">基地址</param>
/// <param name="Lenth">偏移长度</param>
/// <param name="player">玩家实体</param>
/// <returns></returns>
std::string GetPlayerName(HANDLE hProcess, DWORD64 dwBase) {
	if (dwBase == NULL)
		return FALSE;
	char tempname[CSPlayerController::m_iszPlayerName_Size] = { 0 };

	ReadProcessMemory(hProcess, (PBYTE*)(DWORD64(dwBase)), tempname, CSPlayerController::m_iszPlayerName_Size, NULL);
	return tempname;
	//return charTowstring(tempname);
}

/// <summary>
/// 将三维坐标转换到屏幕坐标
/// </summary>
/// <param name="matrix">屏幕矩阵</param>
/// <param name="vector">三维坐标</param>
/// <param name="pos">二维屏幕坐标</param>
/// <returns></returns>
float WorldToScreen(Matrix matrix, Vector vector, Pos& pos) {
	int i;
	float rec[4];
	//float width, height;

	for (i = 0; i < 4; i++)
		rec[i] = (matrix[i][0] * (vector.x)) + (matrix[i][1] * (vector.y)) + (matrix[i][2] * (vector.z)) + (matrix[i][3] * 1);

	pos.x = rec[0] / rec[3];
	pos.y = rec[1] / rec[3];

	float width = fWidth;
	float height = fHeight;

	pos.x = width / 2 + 0.5 * pos.x * width;
	pos.y = height / 2 - 0.5 * pos.y * height;

	//if (pos.x < 0) pos.x = 0;
	//if (pos.y < 0) pos.y = 0;

	//return (1 / rec[3]);
	return (rec[3]);
}



///  Cheats ///////////////////////////////
Cheats::Cheats()
{
	DWORD pid = 0;
	pid = bGetPIDByName(L"cs2.exe");
	this->hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, pid);
	bGetModBaseAddr(L"client.dll", pid, (this->ClientDLLBase));
	bGetModBaseAddr(L"engine2.dll", pid, (this->EngineeDLLBase));
	DWORD64 ListOffsetA = GetProcessMem(this->hProcess, this->ClientDLLBase + ClientDLL::C_CSPlayerController, 1, 0);
	DWORD64 v2 = GetProcessMem(this->hProcess, ListOffsetA + 8 * (1 >> 9) + 16, 1, 0);
	this->ControllerBase = v2 + ClientDLL::C_CSPlayerController_Gap;

	DWORD temp = GetProcessMem(this->hProcess, this->ClientDLLBase + ClientDLL::dwSensitivity, 2, ClientDLL::dwSensitivity_sensitivity, 0);
	this->Sensetive = *((float*)&temp);
};

Cheats::~Cheats() {};

void Cheats::getSensetive() {
	DWORD temp = GetProcessMem(this->hProcess, this->ClientDLLBase + ClientDLL::dwSensitivity, 3, 0, ClientDLL::dwSensitivity_sensitivity, 0);
	this->Sensetive = *((float*)&temp);
}

///  Player ///////////////////////////////
Player::Player() {};
Player::Player(const Cheats& cheats, DWORD index) : Cheats(cheats)
{
	++index;
	this->Index = index;
	this->PlayerControllerAddr = this->ControllerBase + (index - 1) * ClientDLL::C_CSPlayerController_Gap;
	this->GetPawn();
	DWORD64 ScenceNode = (DWORD64)GetProcessMem(this->hProcess, this->PlayerPawnAddr + CSPlayerPawn::m_pGameSceneNode, 1, 0);
	this->BoneMatrix = (DWORD64)GetProcessMem(this->hProcess, ScenceNode + CSkeletonInstance::m_modelState + 0x80, 1, 0);

	this->update();
}

Player::~Player() {};

void Player::GetPawn() {
	DWORD m_hPlayerPawn = GetProcessMem(hProcess, this->PlayerControllerAddr, 2, 0, CSPlayerController::m_hPlayerPawn);

	if (this->ClientDLLBase == 0 || this->hProcess == NULL)
		return;
	DWORD64 entity_list = GetProcessMem(hProcess, this->ClientDLLBase + ClientDLL::C_CSPlayerController, 1, 0);
	if (!entity_list)
		return;
	DWORD64 list_entry = GetProcessMem(hProcess, entity_list + (8 * (this->Index & 0x7FFF) >> 9) + 0x10, 1, 0);
	if (!list_entry)
		return;
	DWORD64 playerPawn = m_hPlayerPawn;
	if (!playerPawn)
		return;
	DWORD64 list_entry2 = GetProcessMem(hProcess, entity_list + 8 * ((m_hPlayerPawn & 0x7FFF) >> 9) + 0x10, 1, 0);
	if (!list_entry2)
		return;
	this->PlayerPawnAddr = GetProcessMem(hProcess, list_entry2 + ClientDLL::C_CSPlayerController_Gap * (m_hPlayerPawn & 0x1FF), 1, 0);
}

void Player::GetPosBone(Matrix matrix) {
	for (size_t i = 0; i < 16; i++)
	{
		WorldToScreen(matrix, this->Bones[i], this->posBones[i]);
	}
}

void Player::update()
{

	DWORD temp = 0;
	this->PlayerControllerAddr = this->ControllerBase + (this->Index - 1) * ClientDLL::C_CSPlayerController_Gap;
	this->GetPawn();
	if (this->PlayerPawnAddr == 0)
		return;

	this->BoneMatrix = (DWORD64)GetProcessMem(this->hProcess, this->PlayerPawnAddr + CSPlayerPawn::m_pGameSceneNode, 2, 0, CSkeletonInstance::m_modelState + 0x80);
	this->Spotted = (DWORD)GetProcessMem(this->hProcess, this->PlayerPawnAddr + CSPlayerPawn::m_entitySpottedState, 2, 0, EntitySpottedState_t::m_bSpotted);

	this->HP = (DWORD)GetProcessMem(this->hProcess, this->PlayerPawnAddr + CSPlayerPawn::m_iHeal, 1, 0);
	this->team = (DWORD)GetProcessMem(this->hProcess, this->PlayerControllerAddr, 2, 0, CSPlayerPawn::m_iTeamNum);
	this->Amor = (DWORD)GetProcessMem(this->hProcess, this->PlayerPawnAddr + CSPlayerPawn::m_ArmorValue, 1, 0);

	DWORD64 name_addr = GetProcessMem(this->hProcess, this->PlayerControllerAddr, 1, 0) + CSPlayerController::m_iszPlayerName;
	if (name_addr != 0)
		this->name = GetPlayerName(this->hProcess, name_addr);

	temp = (DWORD)GetProcessMem(this->hProcess, this->PlayerPawnAddr + CSPlayerPawn::X, 1, 0);
	this->Coord.x = *((float*)&temp);
	temp = (DWORD)GetProcessMem(this->hProcess, this->PlayerPawnAddr + CSPlayerPawn::Y, 1, 0);
	this->Coord.y = *((float*)&temp);
	temp = (DWORD)GetProcessMem(this->hProcess, this->PlayerPawnAddr + CSPlayerPawn::Z, 1, 0);
	this->Coord.z = *((float*)&temp);

	for (size_t j = 0; j < 13; j++)
	{
		temp = (DWORD)GetProcessMem(this->hProcess, this->BoneMatrix + 0x20 * BoneId::BoneIndex[j] + 0, 1, 0);
		this->Bones[j].x = *((float*)&temp);
		temp = (DWORD)GetProcessMem(this->hProcess, this->BoneMatrix + 0x20 * BoneId::BoneIndex[j] + 4, 1, 0);
		this->Bones[j].y = *((float*)&temp);
		temp = (DWORD)GetProcessMem(this->hProcess, this->BoneMatrix + 0x20 * BoneId::BoneIndex[j] + 8, 1, 0);
		this->Bones[j].z = *((float*)&temp);
	}
	this->Bones[13] = this->Bones[2] + Vector{ 0.0f,0.0f,8.0f };	//top
	this->Bones[14] = this->Bones[10] - Vector{ 0.0f,0.0f,8.0f };	//top
	this->Bones[15] = this->Bones[12] - Vector{ 0.0f,0.0f,8.0f };	//top

}

void Player::DrawBones(Matrix matrix, ImVec4 rgb, float size) {
	this->GetPosBone(matrix);
	Vector head = this->Bones[2];
	Vector edge = this->Bones[2] + Vector{ 0,0,4 };

	Pos start = {}, end = {};

	WorldToScreen(matrix, head, start);
	WorldToScreen(matrix, edge, end);

	Render::DrawCircle(start, abs(start.y - end.y), rgb, size);

	Render::DrawLine(this->posBones[2], this->posBones[1], rgb, size);	//head -> neck

	Render::DrawLine(this->posBones[1], this->posBones[3], rgb, size);	//neck ->lf_shoulder
	Render::DrawLine(this->posBones[1], this->posBones[6], rgb, size);	//neck ->rg_shoulder

	Render::DrawLine(this->posBones[3], this->posBones[4], rgb, size);	//lf_shoulder ->lf_amr
	Render::DrawLine(this->posBones[6], this->posBones[7], rgb, size);	//rg_shoulder ->rg_amr

	Render::DrawLine(this->posBones[4], this->posBones[5], rgb, size);	//lf_amr ->lf_hand
	Render::DrawLine(this->posBones[7], this->posBones[8], rgb, size);	//rg_amr ->rg_hand

	Render::DrawLine(this->posBones[1], this->posBones[0], rgb, size);	//neck ->waist

	Render::DrawLine(this->posBones[0], this->posBones[9], rgb, size);	//waist -> lf_keen
	Render::DrawLine(this->posBones[0], this->posBones[11], rgb, size);	//waist -> rg_keen

	Render::DrawLine(this->posBones[9], this->posBones[10], rgb, size);	//lf_keen -> lf_feet
	Render::DrawLine(this->posBones[11], this->posBones[12], rgb, size);	//rg_keen -> rg_feet
}

void Player::DrawBox(Matrix matrix, BoxColor colors, BoxSize sizes) {
	float height, width = 0;

	this->GetPosBone(matrix);

	Pos top = this->posBones[0];
	Pos bottom = this->posBones[0];
	Pos left = this->posBones[0];
	Pos right = this->posBones[0];

	//排序找到上下左右
	for (size_t i = 0; i < 16; i++)
	{
		if (this->posBones[i].x > right.x)
			right = this->posBones[i];
		if (this->posBones[i].y > bottom.y)
			bottom = this->posBones[i];
		if (this->posBones[i].x < left.x)
			left = this->posBones[i];
		if (this->posBones[i].y < top.y)
			top = this->posBones[i];
	}

	//width = height * 0.3f;

	Pos start = { right.x + 10, bottom.y };
	Pos end = { left.x - 10, top.y };
	//Render::DrawLine(end, start, HPLine, CheatSheet::HPSize); 

	if (!start.PosCorrect() || !end.PosCorrect())
		return;

	Render::ImDrawText(this->name.c_str(), Pos{ end.x, start.y }, colors.Name, sizes.szName);
	//DrawText()
	Render::DrawRect(start, end, colors.Box, sizes.szBox);
	height = float(abs(start.y - end.y));
	end.x -= (sizes.szBox + sizes.szPLine / 2);
	start.x = end.x;
	end.y = start.y - height * float(this->HP / 100.f);
	//Render::DrawLine(end, start, ImVec4{ 255,255,255,255 }, sizes.szPLine);
	Render::DrawLine(end, start, colors.HPLine, sizes.szPLine);


	end.x -= (sizes.szPLine + sizes.szALine / 2);
	start.x = end.x;
	end.y = start.y - height * float(this->Amor / 100.f);
	Render::DrawLine(end, start, colors.AmorLine, sizes.szALine);
}

//
void Player::Glowing(DWORD color, bool switchs) {
	if (this->PlayerPawnAddr == 0)
		return;
	DWORD64 OverrideColorAddr = this->PlayerPawnAddr + CSPlayerPawn::m_Glow + CGlowProperty::m_glowColorOverride;
	DWORD64 m_bGlowing = this->PlayerPawnAddr + CSPlayerPawn::m_Glow + CGlowProperty::m_bGlowing;
	WriteProcessMemory(this->hProcess, (LPVOID)(OverrideColorAddr), &color, sizeof(DWORD), NULL);
	WriteProcessMemory(this->hProcess, (LPVOID)(m_bGlowing), &switchs, sizeof(bool), NULL);
}

void Player::Print() {
	std::cout << this->name << "\t" << std::hex << this->PlayerPawnAddr << "\t";
	if (this->team == 2)
		std::cout << "[Team]: T\t";
	else if (this->team == 3)
		std::cout << "[Team]: CT\t";
	else
		std::cout << "[Team]: " << this->team << "\t";
	std::cout << "[HP]: " << this->HP << "\t" << "[Crood]: ";	this->Coord.Print();
	std::cout << std::endl;
}

void Player::name_setter(std::string mem_name)
{
	this->name = mem_name;
}



///  SelfPlayer ///////////////////////////
SelfPlayer::SelfPlayer() {};
SelfPlayer::SelfPlayer(const Cheats& cheats) {
	this->ClientDLLBase = cheats.ClientDLLBase;
	this->hProcess = cheats.hProcess;
	this->PlayerControllerAddr = this->ClientDLLBase + ClientDLL::PlayerController;//GetProcessMem(this->hProcess, this->ClientDLLBase + ClientDLL::PlayerController, 1, 0);
	this->PlayerPawnAddr = GetProcessMem(this->hProcess, this->ClientDLLBase + ClientDLL::PlayerPawn, 1, 0);
	this->update_fast();
}
SelfPlayer::SelfPlayer(const Player& player) : Player(player)
{
	this->hProcess = player.hProcess;
	this->crosshair_addr = this->PlayerPawnAddr + CSPlayerPawn::m_iIDEntIndex;
	this->matrix_addr = GetProcessMem(this->hProcess, this->ClientDLLBase, 1, 0);
	this->GetSelfAngle();
}

SelfPlayer::~SelfPlayer() {};

void SelfPlayer::update_fast() {
	DWORD temp = 0;
	if (this->PlayerPawnAddr == 0)
		return;
	this->GetMatrix();
	this->HP = (DWORD)GetProcessMem(this->hProcess, this->PlayerPawnAddr + CSPlayerPawn::m_iHeal, 1, 0);
	this->team = (DWORD)GetProcessMem(this->hProcess, this->PlayerControllerAddr, 2, 0, CSPlayerPawn::m_iTeamNum);
	this->Amor = (DWORD)GetProcessMem(this->hProcess, this->PlayerControllerAddr, 2, 0, CSPlayerController::m_iPawnHealth);

	DWORD64 name_addr = GetProcessMem(this->hProcess, this->PlayerControllerAddr, 1, 0) + CSPlayerController::m_iszPlayerName;
	if (name_addr != 0)
		this->name = GetPlayerName(this->hProcess, name_addr);

	temp = (DWORD)GetProcessMem(this->hProcess, this->PlayerPawnAddr + CSPlayerPawn::X, 1, 0);
	this->Coord.x = *((float*)&temp);
	temp = (DWORD)GetProcessMem(this->hProcess, this->PlayerPawnAddr + CSPlayerPawn::Y, 1, 0);
	this->Coord.y = *((float*)&temp);
	temp = (DWORD)GetProcessMem(this->hProcess, this->PlayerPawnAddr + CSPlayerPawn::Z, 1, 0);
	this->Coord.z = *((float*)&temp);

	this->BoneMatrix = (DWORD64)GetProcessMem(this->hProcess, this->PlayerPawnAddr + CSPlayerPawn::m_pGameSceneNode, 2, 0, CSkeletonInstance::m_modelState + 0x80);

	for (size_t j = 0; j < 13; j++)
	{
		temp = (DWORD)GetProcessMem(this->hProcess, this->BoneMatrix + 0x20 * BoneId::BoneIndex[j] + 0, 1, 0);
		this->Bones[j].x = *((float*)&temp);
		temp = (DWORD)GetProcessMem(this->hProcess, this->BoneMatrix + 0x20 * BoneId::BoneIndex[j] + 4, 1, 0);
		this->Bones[j].y = *((float*)&temp);
		temp = (DWORD)GetProcessMem(this->hProcess, this->BoneMatrix + 0x20 * BoneId::BoneIndex[j] + 8, 1, 0);
		this->Bones[j].z = *((float*)&temp);
	}
	this->crosshair_addr = this->PlayerPawnAddr + CSPlayerPawn::m_iIDEntIndex;
	this->GetSelfAngle();
}

void SelfPlayer::GetSelfAngle() {
	DWORD temp = 0;
	temp = GetProcessMem(this->hProcess, this->ClientDLLBase + ClientDLL::dwViewAngles + 0, 1, 0);
	this->angle.x = *((float*)&temp);
	temp = GetProcessMem(this->hProcess, this->ClientDLLBase + ClientDLL::dwViewAngles + 4, 1, 0);
	this->angle.y = *((float*)&temp);
	//temp = GetProcessMem(this->hProcess, this->ClientDLLBase + ClientDLL::dwViewAngles + 8, 1, 0);
	this->angle.z = 0.f;
}

Vector SelfPlayer::GetAngles(Vector coord)
{
	Vector result = { 0 };
	result.y = std::atan2(coord.y, coord.x) * (180.0f / 3.14159265359f);
	result.x = std::atan2(-coord.z, std::hypot(coord.x, coord.y)) * (180.0f / 3.14159265359f);
	result.z = 0.0f;

	return result;
}

Vector SelfPlayer::CalcAngle(Vector local, Vector enemy)
{
	Vector result = { 0 };
	result = enemy - local;
	result = this->GetAngles(result);

	return result;
}

void SelfPlayer::GetMatrix()
{
	int i, j;
	DWORD temp;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			DWORD64 addr = this->ClientDLLBase + ClientDLL::dwViewMatrix + (16 * i) + (4 * j);
			temp = (DWORD)GetProcessMem(this->hProcess, addr, 1, 0);
			this->matrix[i][j] = *((float*)&temp);
		}
	}
}


void SelfPlayer::ChangeAngle(Vector angle) {
	SIZE_T bytewrite;
	WriteProcessMemory(this->hProcess, LPVOID(this->ClientDLLBase + ClientDLL::dwViewAngles), &angle.x, sizeof(int), &bytewrite);
	WriteProcessMemory(this->hProcess, LPVOID(this->ClientDLLBase + ClientDLL::dwViewAngles + 4), &angle.y, sizeof(int), &bytewrite);
}

void SelfPlayer::TriggerShot() {
	SIZE_T bytewrite;
	int attack = 65537;
	int stop_attack = 256;

	DWORD amied = GetProcessMem(this->hProcess, crosshair_addr, 1, 0);
	//DWORD lapping = GetProcessMem(this->hProcess, this->ClientDLLBase + 0x173B568, 1, 0x14F8);
	if ((amied != 0xFFFFFFFF) && (GetAsyncKeyState(CheatSheet::TriggerBotKey)))
	{
		WriteProcessMemory(this->hProcess, LPVOID(this->ClientDLLBase + ClientDLL::dwAttack), &attack, sizeof(int), &bytewrite);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		WriteProcessMemory(this->hProcess, LPVOID(this->ClientDLLBase + ClientDLL::dwAttack), &stop_attack, sizeof(int), &bytewrite);
		//std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}
}

void SelfPlayer::BHop() {
	DWORD Active = 65537;
	DWORD DeActive = 256;
	int m_fFlags = GetProcessMem(this->hProcess, this->PlayerPawnAddr + CSPlayerPawn::m_fFlags, 1, 0);
	if (GetAsyncKeyState(VK_SPACE) && m_fFlags & (1 << 0)) {
		WriteProcessMemory(this->hProcess, LPVOID(this->ClientDLLBase + ClientDLL::jump), &Active, sizeof(int), NULL);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		WriteProcessMemory(this->hProcess, LPVOID(this->ClientDLLBase + ClientDLL::jump), &DeActive, sizeof(int), NULL);
	}
}

void SelfPlayer::AntiFlash() {
	DWORD active = 0;
	WriteProcessMemory(this->hProcess, (LPVOID)(this->PlayerPawnAddr + CSPlayerPawn::m_flFlashBangTime), &active, 4, NULL);
}

