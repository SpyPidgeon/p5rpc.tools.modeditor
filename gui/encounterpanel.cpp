#include "encounterpanel.h"
#include "signaturescan.h"
#include <fstream>
#include "itemstructs.h"

void EncounterPanel::ScanValues()
{
	std::string pattern = "48 8B 0D ?? ?? ?? ?? 41 FF C0 48 03 CA 48 83 C2 2C 8B 01 0F C8 89 01 0F B7 41 ??";

	DWORD_PTR enemyEncounterAddress = PatternScan(GetModuleHandle(NULL), pattern);
	enemyEncounterAddress = *(DWORD_PTR*)GetAddressFromGlobalRef(enemyEncounterAddress);

	enemyEncountersPTR = (std::array<EnemyEncounter, EnemyEncounter::ENCOUNTER_COUNT>*)enemyEncounterAddress;
	enemyEncounters = *enemyEncountersPTR;

	pattern = "48 8B 0D ?? ?? ?? ?? 41 FF C0 48 03 CA 48 83 C2 08";
	DWORD_PTR forcedPartyAddress = PatternScan(GetModuleHandle(NULL), pattern);
	forcedPartyAddress = *(DWORD_PTR*)GetAddressFromGlobalRef(forcedPartyAddress);

	forcedPartyDataPTR = (std::array<ForcedPartyData, EnemyEncounter::ENCOUNTER_COUNT>*)forcedPartyAddress;
	forcedPartyData = *forcedPartyDataPTR;

	pattern = "48 8B 3D ?? ?? ?? ?? BD 05 00 00 00";
	DWORD_PTR challengesAddress = PatternScan(GetModuleHandle(NULL), pattern);
	challengesAddress = *(DWORD_PTR*)GetAddressFromGlobalRef(challengesAddress);

	challengesPTR = (std::array<ChallengeBattle, ChallengeBattle::CHALLENGE_COUNT>*)challengesAddress;
	challenges = *challengesPTR;

	pattern = "4C 03 05 ?? ?? ?? ?? EB ??";
	DWORD_PTR enemyNamesAddress = PatternScan(GetModuleHandle(NULL), pattern);
	enemyNamesAddress = *(DWORD_PTR*)GetAddressFromGlobalRef(enemyNamesAddress);

	for (int i = 0; i < enemyNames.size(); i++)
	{
		enemyNames.at(i) = GetNameFromBinary(i, enemyNamesAddress);
	}
}

void EncounterPanel::RenderLogic()
{
	Exportable::RenderButton();

	if (ImGui::BeginTabBar("Encounters"))
	{
		if (ImGui::BeginTabItem("Encounters"))
		{
			if (tab != EncounterTab::ENCOUNTER)
			{
				tab = EncounterTab::ENCOUNTER;
				selectedIndex = 0;
			}
			
			for (int i = 0; i < EnemyEncounter::ENCOUNTER_COUNT; i++)
			{
				std::string indexString = std::format("Encounter ID: {:03d}",i);
				if (ImGui::Button(indexString.c_str()))
				{
					selectedIndex = i;
				}
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Forced Party Data"))
		{
			if (tab != EncounterTab::PARTY)
			{
				tab = EncounterTab::PARTY;
				selectedIndex = 0;
			}

			for (int i = 0; i < EnemyEncounter::ENCOUNTER_COUNT; i++)
			{
				std::string indexString = std::format("Encounter ID: {:03d}", i);
				if (ImGui::Button(indexString.c_str()))
				{
					selectedIndex = i;
				}
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Challenges"))
		{
			if (tab != EncounterTab::CHALLENGE)
			{
				tab = EncounterTab::CHALLENGE;
				selectedIndex = 0;
			}

			for (int i = 0; i < ChallengeBattle::CHALLENGE_COUNT; i++)
			{
				std::string indexString = std::format("Challenge ID: {:03d}", i);

				if (ImGui::Button(indexString.c_str()))
				{
					selectedIndex = i;
				}
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	if (renderExplorer)
		Exportable::RenderExplorer();
}

char unitSearchBuffer[256] = "\0";
void tag_invoke(ImReflect::ImInput_t, const char* label, BattleUnit& value, ImSettings& settings, ImResponse& response)
{
	auto& unit_response = response.get<BattleUnit>();

	std::string& currentValue = encounterPanel->enemyNames.at(value.unitID);
	std::string preview = std::format("ID: {:03d} | {}",value.unitID,currentValue);

	ImGui::SetNextWindowSizeConstraints(ImVec2(0, ImGui::GetFrameHeightWithSpacing() * 10), ImVec2(FLT_MAX, FLT_MAX));
	if (ImGui::BeginCombo(label, preview.c_str()))
	{
		ImGui::InputText("Search", unitSearchBuffer, 256);

		ImGui::BeginChild("##child",ImVec2(0,ImGui::GetFrameHeightWithSpacing() * 10));
		for (int i = 0; i < encounterPanel->enemyNames.size(); i++)
		{
			auto& currentName = encounterPanel->enemyNames.at(i);
			bool is_selected = value.unitID == i;

			std::string dispName = std::format("ID: {:03d} | {}",i,currentName);

			if (unitSearchBuffer[0] != '\0' && !SearchablePanel::TextMatch(unitSearchBuffer, dispName))
				continue;

			if (ImGui::Selectable(dispName.c_str(), is_selected,0))
				value.unitID = i;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndChild();

		ImGui::EndCombo();
	}

	ImReflect::Detail::check_input_states(unit_response);
}

// TODO: Finish dropdown system for Item IDs
struct ItemOffsets
{
	const uint16_t item_Melee = 0;
	const uint16_t item_Armor = 0x1000;
	const uint16_t item_Accessory = 0x2000;
	const uint16_t item_Consumable = 0x3000;
	const uint16_t item_KeyItem = 0x4000;
	const uint16_t item_Material = 0x5000;
	const uint16_t item_SkillCard = 0x6000;
	const uint16_t item_Outfit = 0x7000;
	const uint16_t item_Ranged = 0x8000;
};
VISITABLE_STRUCT(ItemOffsets, item_Melee, item_Armor, item_Accessory, item_Consumable, item_KeyItem, item_Material, item_SkillCard, item_Outfit, item_Ranged);

void tag_invoke(ImReflect::ImInput_t, const char* label, ItemID& value, ImSettings& settings, ImResponse& response)
{
	auto& item_response = response.get<ItemID>();

	int newValue = (int)value.itemID;
	if (ImGui::InputInt(label,&newValue))
	{
		value.itemID = newValue;
	}

	ImReflect::Detail::check_input_states(item_response);
}

void EncounterPanel::InspectorLogic()
{
	switch(tab)
	{
	case EncounterTab::ENCOUNTER:
	{
		std::string indexString = std::format("Encounter ID: {:03d}", selectedIndex);
		ImReflect::Input(indexString.c_str(), enemyEncounters.at(selectedIndex),config);

		if (ImGui::Button("Apply To This"))
		{
			enemyEncountersPTR->at(selectedIndex) = enemyEncounters.at(selectedIndex);
		}

		break;
	}
	case EncounterTab::PARTY:
	{
		std::string indexString = std::format("Encounter ID: {:03d}", selectedIndex);
		ImReflect::Input(indexString.c_str(), forcedPartyData.at(selectedIndex),config);

		if (ImGui::Button("Apply To This"))
		{
			forcedPartyDataPTR->at(selectedIndex) = forcedPartyData.at(selectedIndex);
		}

		break;
	}
	case EncounterTab::CHALLENGE:
	{
		std::string indexString = std::format("Challenge ID: {:03d}", selectedIndex);
		ImReflect::Input(indexString.c_str(), challenges.at(selectedIndex),config);

		if (ImGui::Button("Apply To This"))
		{
			challengesPTR->at(selectedIndex) = challenges.at(selectedIndex);
		}

		break;
	}
	}
}

void EncounterPanel::Refresh()
{
	enemyEncounters = *enemyEncountersPTR;
	forcedPartyData = *forcedPartyDataPTR;
	challenges = *challengesPTR;
}

void EncounterPanel::ApplyChanges()
{
	*enemyEncountersPTR = enemyEncounters;
	*forcedPartyDataPTR = forcedPartyData;
	*challengesPTR = challenges;
}

void EncounterPanel::ExportFile(const std::string& directory)
{
	std::vector<BYTE> fileBytes;

	uint32_t encountersSize = _byteswap_ulong(sizeof(enemyEncounters));
	fileBytes.insert(fileBytes.end(), (BYTE*)&encountersSize, (BYTE*)&encountersSize + 4);

	std::array<EnemyEncounter, EnemyEncounter::ENCOUNTER_COUNT>* encounterArray = new std::array<EnemyEncounter, EnemyEncounter::ENCOUNTER_COUNT>;
	std::memcpy(encounterArray->data(),enemyEncounters.data(),sizeof(enemyEncounters));

	EnemyEncounter::SwapBytes(encounterArray);
	fileBytes.insert(fileBytes.end(), (BYTE*)encounterArray->data(), (BYTE*)encounterArray->data() + sizeof(*encounterArray));
	delete(encounterArray);

	fileBytes.insert(fileBytes.end(), 12, 0);

	uint32_t forcedPartySize = _byteswap_ulong(sizeof(forcedPartyData));
	fileBytes.insert(fileBytes.end(), (BYTE*)&forcedPartySize, (BYTE*)&forcedPartySize + 4);

	std::array<ForcedPartyData, EnemyEncounter::ENCOUNTER_COUNT>* forcedPartyArray = new std::array<ForcedPartyData, EnemyEncounter::ENCOUNTER_COUNT>;
	std::memcpy(forcedPartyArray->data(), forcedPartyData.data(), sizeof(*forcedPartyArray));

	ForcedPartyData::SwapBytes(forcedPartyArray);
	fileBytes.insert(fileBytes.end(), (BYTE*)forcedPartyArray->data(), (BYTE*)forcedPartyArray->data() + sizeof(*forcedPartyArray));
	delete(forcedPartyArray);

	fileBytes.insert(fileBytes.end(), 12, 0);

	uint32_t challengeSize = _byteswap_ulong(sizeof(challenges));
	fileBytes.insert(fileBytes.end(), (BYTE*)&challengeSize, (BYTE*)&challengeSize + 4);

	std::array<ChallengeBattle, ChallengeBattle::CHALLENGE_COUNT>* challengeArray = new std::array<ChallengeBattle, ChallengeBattle::CHALLENGE_COUNT>;
	std::memcpy(challengeArray->data(), challenges.data(), sizeof(*challengeArray));

	ChallengeBattle::SwapBytes(challengeArray);
	fileBytes.insert(fileBytes.end(), (BYTE*)challengeArray->data(), (BYTE*)challengeArray->data() + sizeof(*challengeArray));
	delete(challengeArray);

	fileBytes.insert(fileBytes.end(), 12, 0);

	std::ofstream file(currentDirectory + '\\' + "ENCOUNT.TBL", std::ios::binary);

	if (!file.is_open())
	{
		DWORD error = GetLastError();
		printf("File failed to write! Error Code: 0x%x\n", error);
		return;
	}

	file.write((const char*)fileBytes.data(), fileBytes.size());
	file.close();
	printf("Successfully wrote to %s\n", directory.c_str());
}