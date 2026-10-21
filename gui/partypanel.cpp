#include "partypanel.h"
#include "signaturescan.h"

using std::format;

// Party Panel
void PartyPanel::ScanValues()
{
	std::string pattern;

	pattern = "48 03 1D ?? ?? ?? ?? E9 ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ??";
	DWORD_PTR partyNamesPTR = PatternScan(GetModuleHandle(NULL), pattern);
	partyNamesPTR = GetAddressFromGlobalRef(partyNamesPTR);

	partyNamesPTR = *(DWORD_PTR*)partyNamesPTR;
	while (*(BYTE*)partyNamesPTR == 0x00)
		partyNamesPTR++;

	DWORD_PTR partyMembersAddress = (DWORD_PTR)GetDatUnit(1);

	for (int i = 0; i < 10; i++)
	{
		partyMemberPTRs[i] = GetDatUnit(i + 1);
		partyMembers[i] = { *partyMemberPTRs[i],GetNameFromBinary(i + 1,partyNamesPTR) };
	}

	pattern = "48 03 1D ?? ?? ?? ?? F3 41 0F 59 FA";
	DWORD_PTR personaNamesAddress = PatternScan(GetModuleHandle(NULL), pattern);
	personaNamesAddress = *(DWORD_PTR*)GetAddressFromGlobalRef(personaNamesAddress);

	if (personaNamesAddress != 0)
	{
		for (int i = 0; i < PERSONA_LIST_SIZE; i++)
		{
			personaNames[i] = GetNameFromBinary(i, personaNamesAddress);
		}
	}
}

// Persona ID
void tag_invoke(ImReflect::ImInput_t, const char* label, PersonaID& value, ImSettings& settings, ImResponse& response)
{
	auto& persona_response = response.get<PersonaID>();

	bool changed = false;

	std::string valueName;

	if (value.id == 0)
		valueName = "Empty Slot";
	else if (value.id < partyPanel->personaNames.size())
		valueName = "ID: " + std::to_string(value.id) + " | " + partyPanel->personaNames[value.id];
	else
		valueName = "Error";

	if (ImGui::BeginCombo(label, valueName.c_str()))
	{
		for (int j = 0; j < partyPanel->personaNames.size(); j++)
		{
			bool is_selected = (j == value.id);

			if (j == 0)
				valueName = "Empty Slot";
			else
				valueName = "ID: " + std::to_string(j) + " | " + partyPanel->personaNames[j];

			if (ImGui::Selectable(valueName.c_str(), is_selected))
			{
				value.id = j;
				changed = true;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	if (changed)
		persona_response.changed();

	ImReflect::Detail::check_input_states(persona_response);
}

// Skills
char skillSearch[256] = "";
void tag_invoke(ImReflect::ImInput_t, const char* label, DatUnit_Skills& value, ImSettings& settings, ImResponse& response)
{
	auto& skill_response = response.get<DatUnit_Skills>();

	bool changed = false;
	if (ImGui::CollapsingHeader("Skills"))
	{
		for (int i = 0; i < 8; i++)
		{
			std::string name = "Skill" + std::to_string(i);
			std::string valueName;

			if (value.skill[i] == 0)
				valueName = format("{} Empty", i);
			else if (value.skill[i] < skillPanel->skillNames.size())
				valueName = format("ID: {:04d} | {}", value.skill[i], skillPanel->skillNames[value.skill[i]]);
			else
				valueName = format("{} Default", i);

			if (ImGui::BeginCombo(name.c_str(), valueName.c_str()))
			{
				ImGui::InputText("Search", skillSearch, 256);
				ImGui::BeginChild("##separate",ImVec2(0, ImGui::GetFrameHeightWithSpacing() * 5));

				for (int j = 0; j < skillPanel->skillNames.size(); j++)
				{
					bool skip = !SearchablePanel::TextMatch(skillSearch, skillPanel->skillNames[j]);

					if (skip)
						continue;

					bool is_selected = (j == value.skill[i]);

					valueName = format("ID: {:04d} | {}", j, skillPanel->skillNames[j]);

					if (ImGui::Selectable(valueName.c_str(), is_selected))
					{
						value.skill[i] = j;
						changed = true;
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndChild();
				ImGui::EndCombo();
			}
		}
	}

	if (changed)
		skill_response.changed();

	ImReflect::Detail::check_input_states(skill_response);
}


void PartyPanel::InspectorLogic()
{
	switch (tab)
	{
	case PartyTab::Members:
		ImReflect::Input(partyMembers[selectedIndex].second.c_str(), partyMembers[selectedIndex].first, config);
		break;
	case PartyTab::Personas:
		auto& persona = partyMembers[selectedIndex].first.StockPersonas[selectedPersona];
		const char* label = personaNames[persona.personaID].c_str();
		ImReflect::Input(label, persona, config);
		break;
	}

	if (ImGui::Button("Apply To This"))
	{
		*partyMemberPTRs[selectedIndex] = partyMembers[selectedIndex].first;
	}


}

void PartyPanel::RenderLogic()
{
	if (ImGui::BeginTabBar("Party"))
	{
		if (ImGui::BeginTabItem("Members"))
		{
			tab = PartyTab::Members;
			for (int i = 0; i < PARTY_MAX; i++)
			{
				const char* label = partyMembers[i].second.c_str();
				if (ImGui::Button(label))
				{
					selectedIndex = i;
				}
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Personas"))
		{
			tab = PartyTab::Personas;
			for (int i = 0; i < PARTY_MAX; i++)
			{
				int equippedPersona = partyMembers[i].first.equippedPersona;
				int personaID = partyMembers[i].first.StockPersonas[equippedPersona].personaID;
				const char* label = personaNames[personaID].c_str();
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader(label))
				{
					for (int j = 0; j < 12; j++)
					{
						auto& persona = partyMembers[i].first.StockPersonas[j];
						std::string personaLabel;

						if (persona.personaID == 0)
						{
							personaLabel = format("{} Empty", j);
						}
						else
						{
							personaLabel = format("{} {}", j, personaNames[persona.personaID]);
						}

						if (ImGui::Button(personaLabel.c_str()))
						{
							selectedIndex = i;
							selectedPersona = j;
						}
					}
				}
				ImGui::PopID();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void PartyPanel::ApplyChanges()
{
	for (int i = 0; i < PARTY_MAX; i++)
	{
		*partyMemberPTRs[i] = partyMembers[i].first;
	}
}

void PartyPanel::Refresh()
{
	for (int i = 0; i < PARTY_MAX; i++)
	{
		partyMembers[i].first = *partyMemberPTRs[i];
	}
}