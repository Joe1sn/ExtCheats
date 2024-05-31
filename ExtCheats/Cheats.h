#pragma once
#include <Windows.h>
#include <thread>
#include <mutex>
#include "BasicStruct.hpp"
#include "Constant.hpp"

/// <summary>
///	ͨ���������ƻ�ý���ID
/// </summary>
/// <param name="ProcessName">��������</param>
/// <returns>
/// FALSE: δ��ȡ������pid
/// TRUE : ��ȡ������pid
/// </returns>
DWORD bGetPIDByName(LPCWSTR ProcessName);

/// <summary>
/// �õ�ģ�����ַ
/// </summary>
/// <param name="ModuleName">ģ�����ƣ���Client.dll</param>
/// <param name="PID">����id</param>
/// <param name="dwBaseAddr">���ص�ģ�����ַ</param>
/// <returns>
/// FALSE: δ��ȡ��DLL����ַ
/// TRUE: ��ȡ��DLL����ַ��DLL����ַΪ dwBaseAddr
/// </returns>
BOOL bGetModBaseAddr(LPCWSTR ModuleName, DWORD PID, DWORD64& dwBaseAddr);

/// <summary>
/// ͨ��ָ��ƫ�ƻ���ڴ棬���س���Ϊ8bytes���÷��ο�CE
/// </summary>
/// <param name="hProcess">���̾��</param>
/// <param name="dwBase">����ַ</param>
/// <param name="depth">ָ��ƫ������</param>
/// <param name="">ƫ��</param>
/// <returns>�ڴ��е�ֵ</returns>
DWORD64 GetProcessMem(HANDLE hProcess, DWORD64 dwBase, int depth, ...);


std::wstring charTowstring(char str[]);

/// <summary>
/// �������ǳ�
/// </summary>
/// <param name="hProcess">���̾��</param>
/// <param name="dwBase">����ַ</param>
/// <param name="Lenth">ƫ�Ƴ���</param>
/// <param name="player">���ʵ��</param>
/// <returns></returns>
std::string GetPlayerName(HANDLE hProcess, DWORD64 dwBase);

/// <summary>
/// ����ά����ת������Ļ����
/// </summary>
/// <param name="matrix">��Ļ����</param>
/// <param name="vector">��ά����</param>
/// <param name="pos">��ά��Ļ����</param>
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

	Vector Coord;		//����
	Vector Bones[16];	//����
	Pos posBones[16];	//������ά����

	/// Functions ///
	Player();
	Player(const Cheats& cheats, DWORD index);
	~Player();

	void update();
	void GetPawn();
	void GetPosBone(Matrix matrix);
	void DrawBones(Matrix matrix, ImVec4 rgb, float size);
	void DrawBox(Matrix matrix, BoxColor colors, BoxSize sizes);
	void Glowing(DWORD color = 0x800000FF, bool switchs = 1);	//Ĭ��CT�� ABGR
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


