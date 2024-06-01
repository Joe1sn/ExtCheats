#include "ext/imgui/imgui.h"
#include "BasicStruct.hpp"

ImU32 Color(RGB color);
ImU32 Color(ImVec4 color);


namespace Render {
	void DrawRect(Pos pos_start, Pos pos_end, RGB color, int thickness);
	void DrawLine(Pos pos_start, Pos pos_end, RGB color, int thickness);
	void DrawCircle(Pos pos_start, float radius, RGB color, int thickness);
	void ImDrawText(const char text[], Pos pos_start, RGB color, int thickness);

	void DrawRect(Pos pos_start, Pos pos_end, ImVec4 color, float thickness);
	void DrawLine(Pos pos_start, Pos pos_end, ImVec4 color, float thickness);
	void DrawCircle(Pos pos_start, float radius, ImVec4 color, float thickness);
	void ImDrawText(const char text[], Pos pos_start, ImVec4 color, float thickness);
	void ImDrawWeapon(const char text[], Pos pos_start, ImVec4 color);
}