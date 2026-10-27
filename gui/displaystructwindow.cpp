#include "displaystructwindow.h"
#include <format>

extern ImSettings config;

const char* windowName = "SpyPigeon's Mod Editor Prerelease Version 0.4.2";

void RenderStructWidgets()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x,viewport->Size.y * 0.05f));

	if (ImGui::Begin(windowName, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
	{
		for (auto panel : panels)
		{
			ImGui::SameLine();
			if (ImGui::Button(panel->label.c_str()))
			{
				panel->open = !panel->open;

				for (auto others : panels)
				{
					if (others != panel)
					{
						others->open = false;
					}
				}
			}

			if (panel->open)
				panel->RenderPanel();
		}
	}

	ImGui::End();
}