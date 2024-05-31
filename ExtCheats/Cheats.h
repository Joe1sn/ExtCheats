#pragma once
#include <Windows.h>
#include <thread>
#include <mutex>
#include "BasicStruct.hpp"
#include "Constant.hpp"

/// <summary>
///	通过进程名称获得进程ID
/// </summary>
/// <param name="ProcessName">进程名称</param>
/// <returns>
/// FALSE: 未获取到进程pid
/// TRUE : 获取到进程pid
/// </returns>
DWORD bGetPIDByName(LPCWSTR ProcessName);

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
BOOL bGetModBaseAddr(LPCWSTR ModuleName, DWORD PID, DWORD64& dwBaseAddr);

/// <summary>
/// 通过指针偏移获得内存，返回长度为8bytes，用法参考CE
/// </summary>
/// <param name="hProcess">进程句柄</param>
/// <param name="dwBase">基地址</param>
/// <param name="depth">指针偏移总量</param>
/// <param name="">偏移</param>
/// <returns>内存中的值</returns>
DWORD64 GetProcessMem(HANDLE hProcess, DWORD64 dwBase, int depth, ...);


std::wstring charTowstring(char str[]);

/// <summary>
/// 获得玩家昵称
/// </summary>
/// <param name="hProcess">进程句柄</param>
/// <param name="dwBase">基地址</param>
/// <param name="Lenth">偏移长度</param>
/// <param name="player">玩家实体</param>
/// <returns></returns>
std::string GetPlayerName(HANDLE hProcess, DWORD64 dwBase);

/// <summary>
/// 将三维坐标转换到屏幕坐标
/// </summary>
/// <param name="matrix">屏幕矩阵</param>
/// <param name="vector">三维坐标</param>
/// <param name="pos">二维屏幕坐标</param>
/// <returns></returns>
float WorldToScreen(Matrix matrix, Vector vector, Pos& pos);





class Cheats
{
public:
	HANDLE hProcess;
	DWORD64 ControllerBase;
	DWORD64 ClientDLLBase;
	DWORD64 EngineeDLLBase;
	float Sensetive;

	Cheats();
	~Cheats();
	void getSensetive();
};

class Player :public Cheats
{
public:
	/// Variable ///
	DWORD64 PlayerControllerAddr;
	DWORD64 PlayerPawnAddr;
	DWORD64 BoneMatrix;
	DWORD64 Index;

	DWORD HP;
	DWORD team;
	DWORD Amor;
	std::string name;
	DWORD Spotted;

	Vector Coord;		//坐标
	Vector Bones[16];	//骨骼
	Pos posBones[16];	//骨骼二维坐标

	/// Functions ///
	Player();
	Player(const Cheats& cheats, DWORD index);
	~Player();

	void update();
	void GetPawn();
	void GetPosBone(Matrix matrix);
	void DrawBones(Matrix matrix, ImVec4 rgb, float size);
	void DrawBox(Matrix matrix, BoxColor colors, BoxSize sizes);
	void Glowing(DWORD color = 0x800000FF, bool switchs = 1);	//默认CT蓝 ABGR
	void Print();

	void name_setter(std::string mem_name);
};

class SelfPlayer :public Player
{
public:
	/// Variable ///
	DWORD64 crosshair_addr;
	DWORD64 matrix_addr;
	Matrix matrix;
	HANDLE hProcess;
	Vector angle;

	/// Functions ///
	SelfPlayer();
	SelfPlayer(const Cheats& cheats);
	SelfPlayer(const Player& player);
	~SelfPlayer();

	void GetSelfAngle();
	Vector GetAngles(Vector coord);
	Vector CalcAngle(Vector local, Vector enemy);
	void GetMatrix();

	void ChangeAngle(Vector angle);
	void TriggerShot();
	void BHop();
	void update_fast();
	void AntiFlash();
};


