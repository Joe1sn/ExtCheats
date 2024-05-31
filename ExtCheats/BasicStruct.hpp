#pragma once
#include <Windows.h>
#include <iostream>
#include "Constant.hpp"
#include "globals.h"
#include "ext/imgui/imgui.h"

typedef struct {
	ImU32 R;
	ImU32 G;
	ImU32 B;
}RGB;

constexpr RGB EnemyBone = { 255,0,0 };
constexpr RGB AlieBone = { 0,255,0 };
constexpr RGB Box = { 255,255,250 };
constexpr RGB HPLine = { 0,255,0 };
constexpr RGB NameColr = { 255,255,0 };

typedef struct {
	ImVec4 Box;
	ImVec4 HPLine;
	ImVec4 Name;
	ImVec4 AmorLine;
} BoxColor;

typedef struct {
	float szBox;
	float szPLine;
	float szName;
	float szALine;
} BoxSize;

//3D×ø±ê
struct Vector {
	float x, y, z;

	Vector operator-(Vector second) {
		return Vector{ x - second.x,y - second.y, z - second.z };
	};

	Vector operator+(Vector second) {
		return Vector{ x + second.x,y + second.y, z + second.z };
	};

	Vector operator*(Vector second) {
		return Vector{ x * second.x,y * second.y, z * second.z };
	};

	Vector operator/(Vector second) {
		return Vector{ x / second.x,y / second.y, z / second.z };
	};

	bool isZero() {
		if (x != 0 && y != 0 && z != 0)
			return true;
		else
			return false;
	}

	VOID Print() {
		std::cout << "<" << x << ", " << y << ", " << z << ">";
	}
};

//[DEBUG]-3D×ø±ê
typedef struct _dvector {
	DWORD64 x;
	DWORD64 y;
	DWORD64 z;
}DVector, * pDVector;

//ÆÁÄ»×ø±ê
//typedef float Pos[2];	//{ X , Y }
typedef struct _pos {
	float x;
	float y;

	BOOL PosCorrect() {
		if (x <= 0 || x >= fWidth)
			return FALSE;
		if (y <= 0 || y >= fHeight)
			return FALSE;
		return TRUE;
	}
}Pos, * pPos;

//ÆÁÄ»¾ØÕó
typedef float Matrix[4][4];