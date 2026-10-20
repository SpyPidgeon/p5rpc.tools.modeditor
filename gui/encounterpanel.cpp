#include "encounterpanel.h"
#include "signaturescan.h"
#include <fstream>

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

	pattern = "48 89 05 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 49 8B C8 48 89 05 ?? ?? ?? ?? 48 D1 E9 66 89 0D ?? ?? ?? ?? 48 8B C8 E8 ?? ?? ?? ?? 66 3B 1D ?? ?? ?? ?? 44 8B C3 73 ?? 48 8B 05 ?? ?? ?? ?? 41 8B C8 41 FF C0 48 8D 14 ?? 0F B7 04 ?? 66 C1 C0 08 66 89 02 0F B7 05 ?? ?? ?? ?? 44 3B C0 72 ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 48 8B C8 48 89 05 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 49 8B C8 48 89 05 ?? ?? ?? ?? 48 D1 E9 66 89 0D ?? ?? ?? ?? 48 8B C8 E8 ?? ?? ?? ?? 66 3B 1D ?? ?? ?? ?? 44 8B C3 73 ?? 66 66 0F 1F 84 ?? 00 00 00 00 48 8B 05 ?? ?? ?? ?? 41 8B C8 41 FF C0 48 8D 14 ?? 0F B7 04 ?? 66 C1 C0 08 66 89 02 0F B7 05 ?? ?? ?? ?? 44 3B C0 72 ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 48 8B C8 48 89 05 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 49 8B C8 48 89 05 ?? ?? ?? ?? 48 D1 E9 66 89 0D ?? ?? ?? ?? 48 8B C8 E8 ?? ?? ?? ?? 66 3B 1D ?? ?? ?? ?? 44 8B C3 73 ?? 48 8B 05 ?? ?? ?? ?? 41 8B C8 41 FF C0 48 8D 14 ?? 0F B7 04 ?? 66 C1 C0 08 66 89 02 0F B7 05 ?? ?? ?? ?? 44 3B C0 72 ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 48 8B C8 48 89 05 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 49 8B C8 48 89 05 ?? ?? ?? ?? 48 D1 E9 66 89 0D ?? ?? ?? ?? 48 8B C8 E8 ?? ?? ?? ?? 66 3B 1D ?? ?? ?? ?? 44 8B C3 73 ?? 66 66 0F 1F 84 ?? 00 00 00 00 48 8B 05 ?? ?? ?? ?? 41 8B C8 41 FF C0 48 8D 14 ?? 0F B7 04 ?? 66 C1 C0 08 66 89 02 0F B7 05 ?? ?? ?? ?? 44 3B C0 72 ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 48 8B C8 48 89 05 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 49 8B C8 48 89 05 ?? ?? ?? ?? 48 D1 E9 66 89 0D ?? ?? ?? ?? 48 8B C8 E8 ?? ?? ?? ?? 66 3B 1D ?? ?? ?? ?? 44 8B C3 73 ?? 48 8B 05 ?? ?? ?? ?? 41 8B C8 41 FF C0 48 8D 14 ?? 0F B7 04 ?? 66 C1 C0 08 66 89 02 0F B7 05 ?? ?? ?? ?? 44 3B C0 72 ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3";

}

void EncounterPanel::RenderLogic()
{
	Exportable::RenderButton();

	if (ImGui::BeginTabBar("Encounters"))
	{
		if (ImGui::BeginTabItem("Encounters"))
		{
			tab = EncounterTab::ENCOUNTER;
			
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
			tab = EncounterTab::PARTY;

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
			tab = EncounterTab::CHALLENGE;

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

void tag_invoke(ImReflect::ImInput_t, const char* label, BattleUnit& value, ImSettings& settings, ImResponse& response)
{
	auto& unit_response = response.get<BattleUnit>();



	if (ImGui::BeginCombo(label))
}

void EncounterPanel::InspectorLogic()
{
	switch(tab)
	{
	case EncounterTab::ENCOUNTER:
	{
		std::string indexString = std::format("Encounter ID: {:03d}", selectedIndex);
		ImReflect::Input(indexString.c_str(), enemyEncounters.at(selectedIndex),config);
		break;
	}
	case EncounterTab::PARTY:
	{
		std::string indexString = std::format("Encounter ID: {:03d}", selectedIndex);
		ImReflect::Input(indexString.c_str(), forcedPartyData.at(selectedIndex),config);
		break;
	}
	case EncounterTab::CHALLENGE:
	{
		std::string indexString = std::format("Challenge ID: {:03d}", selectedIndex);
		ImReflect::Input(indexString.c_str(), challenges.at(selectedIndex),config);
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