// csgo_esp_v2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "globals.h"
#include "BasicStruct.hpp"
#include "Constant.hpp"
#include "Cheats.h"
#include "Render.h"

#include "ext/imgui/imgui.h"
#include "ext/imgui/imgui_impl_dx11.h"
#include "ext/imgui/imgui_impl_win32.h"


#include <iostream>
#include <iomanip>
#include <thread>
#include <codecvt>
#include <dwmapi.h>
#include <d3d11.h>
#include <windowsx.h>
#include <vector>
#include <algorithm>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

DWORD SelfPlayerIndex = 0;

auto cheats = Cheats();
Player* players = new Player[64];
Player* trigger_players = new Player[64];
SelfPlayer selfplayer;
SelfPlayer trigger_self;

HANDLE TriggerBotThread = NULL;
HANDLE BHOPThread = NULL;
HANDLE SimAimBotThread = NULL;
HANDLE AimBotThread = NULL;
HANDLE NoRcolioThread = NULL;
HANDLE AntiFlashThread = NULL;

// 全局相关变量和快捷键等
bool cheat_menu_show = true;
bool esp = true;
bool show_bone = false;
bool show_rect = true;
bool glowing = false;

bool bhop = false;
bool triggerbot = false;
bool simaimbot = false;
bool aimbot = false;
bool norecolio = false;
bool showfriend = false;
bool shotfriend = false;
bool antiflash = false;

ImVec4 Vec4EnemyBone = ImVec4(255, 0, 0, 255);
ImVec4 Vec4FriendBone = ImVec4(0, 255, 0, 255);
ImVec4 Vec4Box = ImVec4(230, 255, 0, 255);
ImVec4 Vec4HPLine = ImVec4(0, 255, 0, 255);
ImVec4 Vec4AmorLine = ImVec4(0, 255, 255, 255);
ImVec4 Vec4NameColr = ImVec4(255, 255, 0, 255);

float ImBoneSize = 1;
float ImRectSize = 1;
float ImBoxSize = 2;
float ImHPSize = 2;
float ImAmorSize = 2;
float ImfontSize = 18;

float ImAimBotDist2D = 8.0f;
float ImAimBotSmooth = 1.0f;
int ImNoRecolioRate = 50;

int keySwitchMenu = VK_F1;


//void Drawer(HDC hdc);
void Drawer();

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID InitPlayers();
VOID TriggerBot();
VOID SimAimBot();
VOID BHOP();
VOID AimBot();
VOID NoRecoil();
VOID AntiFlashFunc();
// 比较函数，用于按值对下标进行排序
bool compareByValue(const std::pair<double, int>& a, const std::pair<double, int>& b);






int __stdcall WinMain(HINSTANCE instance, HINSTANCE pInstance, LPSTR lpCmd, int cmdShow) {
	InitPlayers();
	//std::thread(TriggerBot).detach();
	//std::thread(BHOP).detach();

	iWidth = GetProcessMem(cheats.hProcess, cheats.EngineeDLLBase + Engine2DLL::dwWindowWidth, 1, 0);
	iHeight = GetProcessMem(cheats.hProcess, cheats.EngineeDLLBase + Engine2DLL::dwWindowHeight, 1, 0);
	//fWidth = abs(rect.left - rect.right);
	//fHeight = abs(rect.bottom - rect.top);
	fWidth = int(iWidth);
	fHeight = int(iHeight);

	LPCWSTR CLASS_NAME = L"window class";
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = CLASS_NAME;
	wc.hInstance = instance;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wc);
	HWND hwnd = NULL;
	hwnd = CreateWindowExW(0, CLASS_NAME, L"Made By Joe1sn",
		WS_EX_TOPMOST | WS_EX_TRANSPARENT, CW_USEDEFAULT, CW_USEDEFAULT,
		//WS_POPUP| WS_EX_TOPMOST | WS_EX_TRANSPARENT, CW_USEDEFAULT, CW_USEDEFAULT,
		iWidth, iHeight, NULL, NULL, instance, NULL
	);
	if (!hwnd) {
		MessageBox(NULL, L"Error in allocate window", L"Error", NULL);
		return 1;
	}

	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
	//BOOL result = SetWindowDisplayAffinity(hwnd, WDA_MONITOR);
	HWND CS2 = FindWindow(NULL, L"Counter-Strike 2");
	RECT rect = { 0 };
	if (CS2) {
		GetWindowRect(CS2, &rect);
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		MoveWindow(hwnd, rect.left, rect.top, iWidth, iHeight, TRUE);
	}
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	ShowWindow(hwnd, cmdShow);
	UpdateWindow(hwnd);

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.RefreshRate.Numerator = 60U;
	sd.BufferDesc.RefreshRate.Denominator = 1U;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.SampleDesc.Count = 1U;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2U;
	sd.OutputWindow = hwnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	constexpr D3D_FEATURE_LEVEL levels[2]{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
	};

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	IDXGISwapChain* swap_chain = nullptr;
	ID3D11RenderTargetView* render_target_view = nullptr;
	D3D_FEATURE_LEVEL level = {};

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		levels,
		2U,
		D3D11_SDK_VERSION,
		&sd,
		&swap_chain,
		&device,
		&level,
		&context
	);

	ID3D11Texture2D* back_buffer = nullptr;
	swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
	if (back_buffer) {
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
		back_buffer->Release();
	}
	else
		return 0;

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//ImFont* font = io.Fonts->AddFontFromFileTTF(CheatSheet::iconPath, CheatSheet::fontSize, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	//io.Fonts->Build();
	//io.Fonts->AddFontDefault(); // 添加默认字体

	io.Fonts->AddFontFromFileTTF(CheatSheet::ttfPath, CheatSheet::fontSize, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	static const ImWchar custom_rangesA[] = { 0xE001, 0xE204, 0 };
	static const ImWchar custom_rangesB[] = { 0x30, 0x78, 0 };
		
	io.Fonts->AddFontFromFileTTF(CheatSheet::iconPathA, CheatSheet::IconSize, nullptr, custom_rangesA); // 添加自定义字体2
	io.Fonts->AddFontFromFileTTF(CheatSheet::iconPathB, CheatSheet::IconSize, nullptr, custom_rangesA); // 添加自定义字体2
	io.Fonts->AddFontFromFileTTF(CheatSheet::iconPathC, CheatSheet::IconSize,nullptr, custom_rangesB); // 添加自定义字体2

	// 构建字体纹理
	unsigned char* tex_pixels = NULL;
	int tex_width, tex_height;
	io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_width, &tex_height);



	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, context);



	float circle_radius = 50.0f;
	bool running = true;
	bool show_another_window = true;
	while (running) {
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);

			if (msg.message == WM_QUIT)
				running = false;
		}

		if (!running)
			break;

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		//// 菜单栏测试
		//1.是否显示菜单栏
		if (GetAsyncKeyState(keySwitchMenu) & 1) {
			cheat_menu_show = !cheat_menu_show;
		}


		if (cheat_menu_show) {
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::Text("ExtCheats，By Joe1sn");
			ImGui::Text("https://github.com/Joe1sn/ExtCheats");
			ImGui::Checkbox("ESP", &esp);

			ImGui::Checkbox("Show Bone", &show_bone);
			ImGui::SameLine();
			ImGui::Checkbox("Show Box", &show_rect);
			ImGui::SameLine();
			ImGui::Checkbox("Glowing", &glowing);
			ImGui::SameLine();
			ImGui::Checkbox("Show Firend", &showfriend);
			

			ImGui::Checkbox("AimBot", &aimbot);
			ImGui::SameLine();
			ImGui::Checkbox("Aim Firend", &shotfriend);
			
			ImGui::Checkbox("TriggerBot", &triggerbot);
			ImGui::SameLine();
			ImGui::Checkbox("Bhop", &bhop);
			ImGui::SameLine();
			ImGui::Checkbox("Anti Flash", &antiflash);
			ImGui::SameLine();
			ImGui::Checkbox("No recolio", &norecolio);



			ImGui::SliderFloat("AimBot Effect Distance", &ImAimBotDist2D, 5.0f, 50.0f);
			ImGui::SliderFloat("AimBot Move Speed", &ImAimBotSmooth, 1.0f, 8.0f);

			ImGui::SliderInt("Recolio Rate", &ImNoRecolioRate, 15, 128);

			//
			ImGui::ColorEdit3("enemy bone color", (float*)&Vec4EnemyBone);
			ImGui::ColorEdit3("friend bone color", (float*)&Vec4FriendBone);
			ImGui::SliderFloat("bone size", &ImBoneSize, 1.0f, 5.0f);
			//
			ImGui::ColorEdit3("box color", (float*)&Vec4Box);
			ImGui::SliderFloat("box size", &ImBoxSize, 1.0f, 5.0f);
			//
			ImGui::ColorEdit3("hp line color", (float*)&Vec4HPLine);
			ImGui::SliderFloat("hp line size", &ImHPSize, 2.0f, 8.0f);
			//
			ImGui::ColorEdit3("armor line color", (float*)&Vec4AmorLine);
			ImGui::SliderFloat("armor line size", &ImAmorSize, 2.0f, 8.0f);
			//
			ImGui::ColorEdit3("name color", (float*)&Vec4NameColr);
			//ImGui::SliderFloat("name size", &ImfontSize, 4.0f, 32.0f);

			if (ImGui::Button("Reset Color&Size")) {
				Vec4EnemyBone = ImVec4(255, 0, 0, 255);
				Vec4FriendBone = ImVec4(0, 255, 0, 255);
				Vec4Box = ImVec4(230, 255, 0, 255);
				Vec4HPLine = ImVec4(0, 255, 0, 255);
				Vec4AmorLine = ImVec4(0, 255, 255, 255);
				Vec4NameColr = ImVec4(255, 255, 0, 255);


				ImBoneSize = 1;
				ImRectSize = 1;
				ImBoxSize = 2;
				ImHPSize = 2;
				ImAmorSize = 2;
				ImfontSize = 18;
				ImAimBotDist2D = 5.0f;
				ImAimBotSmooth = 1.0f;
				ImNoRecolioRate = 50;
			}
		}

		//// DRAWING
		if (esp)
			Drawer();
		////

		if (bhop && BHOPThread == NULL) {
			BHOPThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BHOP, instance, NULL, NULL);
		}
		else if (!bhop && BHOPThread != NULL) {
			CloseHandle(BHOPThread);
			BHOPThread = NULL;
		}

		if (triggerbot && TriggerBotThread == NULL) {
			TriggerBotThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TriggerBot, instance, NULL, NULL);
		}
		else if (!triggerbot && TriggerBotThread != NULL) {
			CloseHandle(TriggerBotThread);
			TriggerBotThread = NULL;
		}

		if (simaimbot && SimAimBotThread == NULL) {
			SimAimBotThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SimAimBot, instance, NULL, NULL);
		}
		else if (!simaimbot && SimAimBotThread != NULL) {
			CloseHandle(SimAimBotThread);
			SimAimBotThread = NULL;
		}

		if (aimbot && AimBotThread == NULL) {
			norecolio = !aimbot;
			AimBotThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AimBot, instance, NULL, NULL);
		}
		else if (!aimbot && AimBotThread != NULL) {
			CloseHandle(AimBotThread);
			AimBotThread = NULL;
		}

		if (norecolio && NoRcolioThread == NULL) {
			aimbot = !norecolio;
			NoRcolioThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NoRecoil, instance, NULL, NULL);
		}
		else if (!norecolio && NoRcolioThread != NULL) {
			CloseHandle(NoRcolioThread);
			NoRcolioThread = NULL;
		}
				
		if (antiflash && AntiFlashThread == NULL) {
			AntiFlashThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AntiFlashFunc, instance, NULL, NULL);
		}
		else if (!antiflash && AntiFlashThread != NULL) {
			CloseHandle(AntiFlashThread);
			AntiFlashThread = NULL;
		}

		ImGui::Render();
		float color[4] = { 0,0,0,0 };

		context->OMSetRenderTargets(1, &render_target_view, nullptr);

		context->ClearRenderTargetView(render_target_view, color);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		swap_chain->Present(0, 0);
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();
	if (swap_chain) {
		swap_chain->Release();
	}
	if (context) {
		context->Release();
	}
	if (device) {
		device->Release();
	}
	if (render_target_view) {
		render_target_view->Release();
	}

	DestroyWindow(hwnd);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	static HDC hdcBuffer = NULL;
	static HBITMAP hbmBuffer = NULL;

	if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hwnd, message, wParam, lParam);
}

int main() {
	HDC hdc = NULL;
	InitPlayers();
	auto localplayer = SelfPlayer(cheats);
	while (true)
	{
		for (size_t i = 0; i < 64; i++)
		{
			players[i].Print();
			players[i].update();
		}
		//Drawer();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

VOID InitPlayers()
{
	selfplayer = SelfPlayer(cheats);
	for (size_t i = 0; i < 64; i++)
	{
		players[i] = Player(cheats, i);
	}
}

//void Drawer(HDC hdc) {
void Drawer() {
	Vector headshot = { 0 };
	Pos Bone[13] = { 0 };
	Pos posName = { 0 };
	float bone_cord = 0.0f;
	float coord_cord = 0.0f;

	for (size_t i = 0; i < 64; i++)
	{
		selfplayer.update_fast();
		selfplayer.GetMatrix();
		players[i].update();
		if ((players[i].HP > 100) ||
			(players[i].HP == 0) ||
			(players[i].team == 0) ||
			players[i].name == "" ||
			players[i].PlayerPawnAddr == 0 ||
			players[i].name == selfplayer.name
			)
			continue;
		if (!showfriend && players[i].team == selfplayer.team)
			continue;
		//if (
		//(
		//	pow((selfplayer.Coord.x - players[i].Coord.x), 2) + 
		//	pow((selfplayer.Coord.y - players[i].Coord.y), 2) + 
		//	pow((selfplayer.Coord.z - players[i].Coord.z), 2)
		//) > 8000000
		//	)
		//	continue;
		coord_cord = WorldToScreen(selfplayer.matrix, players[i].Coord, posName);

		if (coord_cord < 0.01f)
			continue;

		if (players[i].name == selfplayer.name)
			continue;
		if (showfriend) {
			if (players[i].team != selfplayer.team) {
				if(show_rect)
					players[i].DrawBox(selfplayer.matrix, BoxColor{ Vec4Box,Vec4HPLine,Vec4NameColr,Vec4AmorLine }, BoxSize{ ImBoxSize,ImHPSize,ImfontSize,ImAmorSize });
				if(show_bone)
					players[i].DrawBones(selfplayer.matrix, Vec4EnemyBone, ImBoneSize);
				if(glowing)
					players[i].Glowing();
			}
			else if (players[i].team == selfplayer.team) {
				if (show_rect)
					players[i].DrawBox(selfplayer.matrix, BoxColor{ Vec4Box,Vec4HPLine,Vec4NameColr,Vec4AmorLine }, BoxSize{ ImBoxSize,ImHPSize,ImfontSize,ImAmorSize });
				if (show_bone)
					players[i].DrawBones(selfplayer.matrix, Vec4FriendBone, ImBoneSize);
				if (glowing)
					players[i].Glowing(0x8000FFFF);
			}
		}
		else {
			if (show_rect)
				players[i].DrawBox(selfplayer.matrix, BoxColor{ Vec4Box,Vec4HPLine,Vec4NameColr,Vec4AmorLine }, BoxSize{ ImBoxSize,ImHPSize,ImfontSize,ImAmorSize });
			if (show_bone)
				players[i].DrawBones(selfplayer.matrix, Vec4EnemyBone, ImBoneSize);
			if (glowing)
				players[i].Glowing();
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

VOID TriggerBot() {
	auto localplayer = SelfPlayer(cheats);

	while (true)
	{
		if (!triggerbot)
			ExitThread(0);
		localplayer.TriggerShot();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

VOID SimAimBot()
{
	auto localplayer = SelfPlayer(cheats);
	DWORD lapping = 0;
	for (size_t i = 0; i < 64; i++)
	{
		trigger_players[i] = Player(cheats, i);
	}
	Player* shotat_players = new Player[64];
	double distance[64] = { 0 };
	while (true)
	{
		if (!simaimbot)
			ExitThread(0);
		int shotat_index = 0;

		//获得设计目标
		for (size_t i = 0; i < 64; i++)
		{
			localplayer.update_fast();
			localplayer.GetMatrix();
			trigger_players[i].update();
			if ((trigger_players[i].HP > 100) ||
				(trigger_players[i].HP == 0) ||
				(trigger_players[i].team == 0) ||
				trigger_players[i].name == "" ||
				trigger_players[i].PlayerPawnAddr == 0 ||
				trigger_players[i].name == localplayer.name ||
				trigger_players[i].team == localplayer.team
				)
				continue;
			shotat_players[shotat_index] = trigger_players[i];
			shotat_index++;
		}

		//射击距离折算
		// 1.计算每个距离
		for (size_t i = 0; i < shotat_index; i++)
		{
			distance[i] = sqrt(
				pow(shotat_players[i].Bones[2].x, 2) +
				pow(shotat_players[i].Bones[2].y, 2) +
				pow(shotat_players[i].Bones[2].z, 2)
			);
		}
		// 2.进行排序

		// 创建一个存储值和下标的向量
		std::vector<std::pair<double, int>> elements_with_indices(shotat_index);
		for (int i = 0; i < shotat_index; ++i) {
			elements_with_indices[i] = std::make_pair(distance[i], i);
		}
		std::sort(elements_with_indices.begin(), elements_with_indices.end(), compareByValue);
		std::vector<int> sorted_indices(shotat_index);
		for (int i = 0; i < shotat_index; ++i) {
			sorted_indices[i] = elements_with_indices[i].second;
		}

		//进行射击
		if (!(GetAsyncKeyState(CheatSheet::TriggerBotKey)))
			continue;
		for (const auto& i : sorted_indices) {
			lapping = GetProcessMem(localplayer.hProcess, cheats.ClientDLLBase + 0x173B568, 2, 0, 0x14F8);
			while (lapping < 129) {
				lapping = GetProcessMem(localplayer.hProcess, cheats.ClientDLLBase + 0x173B568, 2, 0, 0x14F8);
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			//std::cout << i << "  ";
			//shotat_players[i].Print();
			{
				Vector headshot = localplayer.CalcAngle(localplayer.Bones[2], shotat_players[i].Bones[2]);
				localplayer.ChangeAngle(headshot);
				std::this_thread::sleep_for(std::chrono::milliseconds(3));
				localplayer.TriggerShot();
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

}


VOID BHOP() {
	auto localplayer = SelfPlayer(cheats);

	while (true)
	{
		localplayer.BHop();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (!bhop)
			ExitThread(0);
	}
}

VOID AimBot() {
	auto localplayer = SelfPlayer(cheats);
	Player* aimPlayers = new Player[64];
	DWORD temp = 0;
	Vector aimPunch = { 0,0,0 };
	int aimat_index = 0;
	size_t i = 0;
	for (size_t i = 0; i < 64; i++)
	{
		aimPlayers[i] = Player(cheats, i);
	}
	while (true)
	{
		if (!aimbot)
			ExitThread(0);
		if (!GetAsyncKeyState(VK_LBUTTON))
			continue;
		auto bestFov = ImAimBotDist2D;
		Vector bestAngle = { 0,0,0 };

		for (i = 0; i < 64; i++)
		{
			aimPlayers[i].update();
			localplayer.update_fast();
			if ((aimPlayers[i].HP > 100) ||
				(aimPlayers[i].HP == 0) ||
				(aimPlayers[i].team == 0) ||
				aimPlayers[i].name == "" ||
				aimPlayers[i].PlayerPawnAddr == 0 ||
				aimPlayers[i].name == localplayer.name ||
				//aimPlayers[i].team == localplayer.team ||
				!aimPlayers[i].Spotted
				)continue;
			temp = GetProcessMem(localplayer.hProcess, localplayer.PlayerPawnAddr + CSPlayerPawn::m_aimPunchAngle, 1, 0);
			aimPunch.x = *((float*)&temp);
			temp = GetProcessMem(localplayer.hProcess, localplayer.PlayerPawnAddr + CSPlayerPawn::m_aimPunchAngle, 1, 0);
			aimPunch.y = *((float*)&temp);
			aimPunch = aimPunch * Vector{ 2,2,2 };
			localplayer.GetSelfAngle();
			auto angle = localplayer.CalcAngle(localplayer.Bones[2], aimPlayers[i].Bones[2]) - localplayer.angle;
			const auto fov = std::hypot(angle.x, angle.y);
			if (fov < bestFov) {
				bestFov = fov;
				bestAngle = angle;
				aimat_index = i;
			}
		}
		if (!shotfriend && aimPlayers[aimat_index].team == localplayer.team)
			continue;
		if (bestAngle.x != 0 && bestAngle.y != 0) {
			localplayer.ChangeAngle(bestAngle / Vector{ ImAimBotSmooth ,ImAimBotSmooth ,ImAimBotSmooth } + localplayer.angle);
			localplayer.GetSelfAngle();
			DWORD shotFire = GetProcessMem(localplayer.hProcess, localplayer.PlayerPawnAddr + CSPlayerPawn::m_iShotsFired, 1, 0);
			if (shotFire) {
				temp = GetProcessMem(localplayer.hProcess, localplayer.PlayerPawnAddr + CSPlayerPawn::m_aimPunchAngle, 1, 0);
				aimPunch.x = *((float*)&temp);
				temp = GetProcessMem(localplayer.hProcess, localplayer.PlayerPawnAddr + CSPlayerPawn::m_aimPunchAngle + 4, 1, 0);
				aimPunch.y = *((float*)&temp);
				aimPunch = aimPunch * Vector{ 2,2,2 };
				bestAngle = localplayer.angle - aimPunch;
				if (bestAngle.x > 89.f)
					bestAngle.x = 89.f;
				if (bestAngle.x < -89.f)
					bestAngle.x = -89.f;
				while (bestAngle.y > 180.f)
					bestAngle.y -= 360.f;
				while (bestAngle.y < -180.f)
					bestAngle.y += 360.f;
				localplayer.ChangeAngle(bestAngle);
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

}

VOID NoRecoil() {
	auto localplayer = SelfPlayer(cheats);

	localplayer.GetPawn();
	DWORD shotsFired;
	DWORD temp = 0;
	Vector aimPunch = { 0,0,0 };
	Vector bestAngle = { 0,0,0 };
	Vector oldPunch = { 0,0,0 };
	DWORD lapping = 0;
	while (true)
	{
		if (!norecolio)
			ExitThread(0);
		DWORD shotFire = GetProcessMem(localplayer.hProcess, localplayer.PlayerPawnAddr + CSPlayerPawn::m_iShotsFired, 1, 0);
		if (shotFire) {
			temp = GetProcessMem(localplayer.hProcess, localplayer.PlayerPawnAddr + CSPlayerPawn::m_aimPunchAngle, 1, 0);
			aimPunch.x = *((float*)&temp);
			temp = GetProcessMem(localplayer.hProcess, localplayer.PlayerPawnAddr + CSPlayerPawn::m_aimPunchAngle + 4, 1, 0);
			aimPunch.y = *((float*)&temp);
			aimPunch = aimPunch * Vector{ 2,2,2 };
			bestAngle = localplayer.angle - aimPunch;
			if (bestAngle.x > 89.f)
				bestAngle.x = 89.f;
			if (bestAngle.x < -89.f)
				bestAngle.x = -89.f;
			while (bestAngle.y > 180.f)
				bestAngle.y -= 360.f;
			while (bestAngle.y < -180.f)
				bestAngle.y += 360.f;
			localplayer.ChangeAngle(bestAngle);
		}
		else {
			lapping = GetProcessMem(localplayer.hProcess, localplayer.ClientDLLBase + 0x173B568, 2, 0, 0x14F8);
			if (lapping > ImNoRecolioRate)
				localplayer.GetSelfAngle();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

VOID AntiFlashFunc() {
	auto localplayer = SelfPlayer(cheats);

	while (true)
	{
		if (!antiflash)
			ExitThread(0);
		localplayer.AntiFlash();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

// 比较函数，用于按值对下标进行排序
bool compareByValue(const std::pair<double, int>& a, const std::pair<double, int>& b) {
	return a.first < b.first;
}