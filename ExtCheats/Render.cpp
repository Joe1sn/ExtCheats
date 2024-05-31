#include "Render.h"

ImU32 Color(RGB color) {
	return IM_COL32(color.R, color.G, color.B, 255);
}

ImU32 Color(ImVec4 color) {
	return IM_COL32(color.x, color.y, color.z, color.w);

}

void Render::DrawRect(Pos pos_start, Pos pos_end, RGB color, int thickness) {
	ImGui::GetBackgroundDrawList()->AddRect(
		ImVec2(int(pos_end.x), int(pos_end.y)),
		ImVec2(int(pos_start.x), int(pos_start.y)),
		Color(color), 0, 0, thickness
	);
}
void Render::DrawLine(Pos pos_start, Pos pos_end, RGB color, int thickness) {
	ImGui::GetBackgroundDrawList()->AddLine(
		ImVec2(int(pos_end.x), int(pos_end.y)),
		ImVec2(int(pos_start.x), int(pos_start.y)),
		Color(color), thickness
	);
}

void Render::DrawCircle(Pos pos_start, float radius, RGB color, int thickness) {
	ImGui::GetBackgroundDrawList()->AddCircle(
		ImVec2(int(pos_start.x), int(pos_start.y)),
		radius,
		Color(color), 0, thickness
	);
}

//void AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = nullptr);

void Render::ImDrawText(const char text[], Pos pos_start, RGB color, int thickness) {
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//ImFont* font = io.Fonts->AddFontFromFileTTF(CheatSheet::ttfPath, CheatSheet::fontSize, NULL, io.Fonts->GetGlyphRangesDefault());
	ImFont* font = io.Fonts->AddFontFromFileTTF(NULL, CheatSheet::fontSize, NULL, io.Fonts->GetGlyphRangesDefault());
	ImGui::GetBackgroundDrawList()->AddText(
		//ImVec2(int(pos_start.x), int(pos_start.y)), Color(color), text, 0
		font, CheatSheet::fontSize, ImVec2(int(pos_start.x), int(pos_start.y)), Color(color), text
	);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Render::DrawRect(Pos pos_start, Pos pos_end, ImVec4 color, float thickness) {
	ImGui::GetBackgroundDrawList()->AddRect(
		ImVec2(int(pos_end.x), int(pos_end.y)),
		ImVec2(int(pos_start.x), int(pos_start.y)),
		Color(color), 0, 0, thickness
	);
}

void Render::DrawLine(Pos pos_start, Pos pos_end, ImVec4 color, float thickness) {
	if (!pos_start.PosCorrect() || !pos_end.PosCorrect())
		return;
	ImGui::GetBackgroundDrawList()->AddLine(
		ImVec2(int(pos_end.x), int(pos_end.y)),
		ImVec2(int(pos_start.x), int(pos_start.y)),
		Color(color), thickness
	);
}

void Render::DrawCircle(Pos pos_start, float radius, ImVec4 color, float thickness) {
	ImGui::GetBackgroundDrawList()->AddCircle(
		ImVec2(int(pos_start.x), int(pos_start.y)),
		radius,
		Color(color), 0, thickness
	);
}

void Render::ImDrawText(const char text[], Pos pos_start, ImVec4 color, float thickness) {
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//ImFont* font = io.Fonts->AddFontFromFileTTF(CheatSheet::ttfPath, CheatSheet::fontSize, NULL, io.Fonts->GetGlyphRangesDefault());
	//ImFont* font = io.Fonts->AddFontFromFileTTF(CheatSheet::ttfPath, thickness, NULL, io.Fonts->GetGlyphRangesChineseFull());
	ImFont* font = io.Fonts->AddFontFromFileTTF(NULL, CheatSheet::NameSize, NULL, io.Fonts->GetGlyphRangesChineseFull());
	ImGui::GetBackgroundDrawList()->AddText(
		//ImVec2(int(pos_start.x), int(pos_start.y)), Color(color), text, 0
		font, CheatSheet::fontSize, ImVec2(int(pos_start.x), int(pos_start.y)), Color(color), text
	);
}