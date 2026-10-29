#include "panels.h"
#include <format>
#include "signaturescan.h"

using std::format;
std::vector<Panel*> panels;
ImSettings config;

// Panel
Panel::Panel()
{
	label = "Default";
	panels.push_back(this);
}

void Panel::RenderPanel()
{
	auto viewport = ImGui::GetMainViewport();
	ImVec2 pos = viewport->Pos;
	int h = viewport->Size.y;
	int headerHeight = h * 0.05f;
	int windowSizeX = viewport->Size.x * 0.25f;
	int windowSizeY = viewport->Size.y - headerHeight;

	ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y + headerHeight));
	ImGui::SetNextWindowSize(ImVec2(windowSizeX,windowSizeY));

	if (ImGui::Begin(label.c_str(),nullptr, windowFlags))
	{
		if (refreshable)
		{
			if (ImGui::Button("Apply"))
			{
				ApplyChanges();
			}
			ImGui::SameLine();
			if (ImGui::Button("Refresh"))
			{
				Refresh();
			}
		}

		RenderLogic();
		ImGui::End();
	}

	int childX = viewport->Size.x - windowSizeX;

	ImGui::SetNextWindowPos(ImVec2(childX, pos.y + headerHeight));
	ImGui::SetNextWindowSize(ImVec2(windowSizeX,windowSizeY));

	if (ImGui::Begin(childLabel,nullptr, windowFlags))
	{
		InspectorLogic();
		ImGui::End();
	}
}

// Searchable
void SearchablePanel::RenderLogic()
{
	ImGui::InputText("Search", searchText, 256);
}

bool SearchablePanel::TextMatch(std::string search, std::string match)
{
	if (search.size() > match.size())
		return false;

	for (int i = 0; i < match.size(); i++)
	{
		if (i < search.size()) search[i] = std::tolower(search[i]);
		match[i] = std::tolower(match[i]);
	}

	return match.find(search) != std::string::npos;
}