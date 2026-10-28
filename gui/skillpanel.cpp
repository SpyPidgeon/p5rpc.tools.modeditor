#include "skillpanel.h"
#include "signaturescan.h"
#include <random>
#include <algorithm>
#include <utility>

using std::format;

// Skill Panel
void SkillPanel::RenderLogic()
{
	Exportable::RenderButton();

	SearchablePanel::RenderLogic();
	if (ImGui::BeginTabBar(label.c_str()))
	{
		if (ImGui::BeginTabItem("Active Skill"))
		{
			if (tab != SkillTab::ACTIVE)
			{
				tab = SkillTab::ACTIVE;
				selectedIndex = 0;
			}

			RenderList(&activeSkillArray, &skillNames);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Skill Element"))
		{
			if (tab != SkillTab::ELEMENT)
			{
				tab = SkillTab::ELEMENT;
				selectedIndex = 0;
			}
			RenderList(&skillElementArray, &skillNames);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Technical Combos"))
		{
			if (tab != SkillTab::TECHNICAL)
			{
				tab = SkillTab::TECHNICAL;
				selectedIndex = 0;
			}
			RenderListNoName(&technicalArray, "Technical Combo");
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Traits"))
		{
			if (tab != SkillTab::TRAIT)
			{
				tab = SkillTab::TRAIT;
				selectedIndex = 0;
			}
			RenderList(&traitsArray, &traitNamesArray);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	if (renderExplorer)
		RenderExplorer();
}

void SkillPanel::ScanValues()
{
	std::string pattern;
	pattern = "48 8D 0D ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 0F 1F 00 8B 41 ??";
	DWORD_PTR activeSkills = PatternScan(GetModuleHandle(NULL), pattern);
	activeSkills = GetAddressFromGlobalRef(activeSkills);

	activeSkillsPTR = (std::array<ActiveSkill, ACTIVE_SKILL_SIZE>*)(activeSkills - 0x20);
	activeSkillArray = *activeSkillsPTR;

	pattern = "48 8D 0D ?? ?? ?? ?? 66 89 05 ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 66 66 0F 1F 84 ?? 00 00 00 00";
	DWORD_PTR skillElements = PatternScan(GetModuleHandle(NULL), pattern);
	skillElementPtr = (std::array<SkillElement, SKILL_ELEMENT_SIZE>*)(GetAddressFromGlobalRef(skillElements) - 4);
	skillElementArray = *skillElementPtr;

	pattern = "48 89 05 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 49 8B C8 48 89 05 ?? ?? ?? ?? 48 D1 E9 66 89 0D ?? ?? ?? ?? 48 8B C8 E8 ?? ?? ?? ?? 66 3B 1D ?? ?? ?? ?? 44 8B C3 73 ?? 48 8B 05 ?? ?? ?? ?? 41 8B C8 41 FF C0 48 8D 14 ?? 0F B7 04 ?? 66 C1 C0 08 66 89 02 0F B7 05 ?? ?? ?? ?? 44 3B C0 72 ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 48 8B C8 48 89 05 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 49 8B C8 48 89 05 ?? ?? ?? ?? 48 D1 E9 66 89 0D ?? ?? ?? ?? 48 8B C8 E8 ?? ?? ?? ?? 66 3B 1D ?? ?? ?? ?? 44 8B C3 73 ?? 66 66 0F 1F 84 ?? 00 00 00 00 48 8B 05 ?? ?? ?? ?? 41 8B C8 41 FF C0 48 8D 14 ?? 0F B7 04 ?? 66 C1 C0 08 66 89 02 0F B7 05 ?? ?? ?? ?? 44 3B C0 72 ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 48 8B C8 48 89 05 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 49 8B C8 48 89 05 ?? ?? ?? ?? 48 D1 E9 66 89 0D ?? ?? ?? ?? 48 8B C8 E8 ?? ?? ?? ?? 66 3B 1D ?? ?? ?? ?? 44 8B C3 73 ?? 48 8B 05 ?? ?? ?? ?? 41 8B C8 41 FF C0 48 8D 14 ?? 0F B7 04 ?? 66 C1 C0 08 66 89 02 0F B7 05 ?? ?? ?? ?? 44 3B C0 72 ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 48 8B C8 48 89 05 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 49 8B C8 48 89 05 ?? ?? ?? ?? 48 D1 E9 66 89 0D ?? ?? ?? ?? 48 8B C8 E8 ?? ?? ?? ?? 66 3B 1D ?? ?? ?? ?? 44 8B C3 73 ?? 66 66 0F 1F 84 ?? 00 00 00 00 48 8B 05 ?? ?? ?? ?? 41 8B C8 41 FF C0 48 8D 14 ?? 0F B7 04 ?? 66 C1 C0 08 66 89 02 0F B7 05 ?? ?? ?? ?? 44 3B C0 72 ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 48 8B C8 48 89 05 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3 2B CA 85 C9 0F 4F C1 48 8D 4D ?? 48 03 45 ?? 48 03 F8 48 8B D7 E8 ?? ?? ?? ?? 8B 45 ?? 48 83 C7 04 0F C8 89 45 ?? 48 8B 4D ?? E8 ?? ?? ?? ?? 4C 8B 45 ?? 48 8B D7 49 8B C8 48 89 05 ?? ?? ?? ?? 48 D1 E9 66 89 0D ?? ?? ?? ?? 48 8B C8 E8 ?? ?? ?? ?? 66 3B 1D ?? ?? ?? ?? 44 8B C3 73 ?? 48 8B 05 ?? ?? ?? ?? 41 8B C8 41 FF C0 48 8D 14 ?? 0F B7 04 ?? 66 C1 C0 08 66 89 02 0F B7 05 ?? ?? ?? ?? 44 3B C0 72 ?? 8B 55 ?? 83 C2 04 8B C2 25 0F 00 00 80 7D ?? FF C8 83 C8 F0 FF C0 F7 D8 41 B8 04 00 00 00 8B C2 1B C9 83 E0 F0 83 E1 10 03 C8 8B C3";
	DWORD_PTR activeSkillNames = PatternScan(GetModuleHandle(NULL), pattern);

	activeSkillNames = *(DWORD_PTR*)GetAddressFromGlobalRef(activeSkillNames);

	if (activeSkillNames != 0)
	{
		for (int i = 0; i < SKILL_ELEMENT_SIZE; i++)
		{
			skillNames[i] = GetNameFromBinary(i, activeSkillNames);
		}
	}

	pattern = "48 8D 05 ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 0F 1F 80 00 00 00 00 8B 48 ??";
	DWORD_PTR technicals = PatternScan(GetModuleHandle(NULL), pattern);
	technicalPtr = (std::array<Technical, TECHNICAL_SIZE>*)((GetAddressFromGlobalRef(technicals)) - 8);
	technicalArray = *technicalPtr;

	pattern = "48 8D 0D ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 66 66 66 0F 1F 84 ?? 00 00 00 00 0F B7 41 ??";
	DWORD_PTR traits = PatternScan(GetModuleHandle(NULL), pattern);
	traitsPtr = (std::array<Trait, TRAIT_SIZE>*)(GetAddressFromGlobalRef(traits) - 4);
	traitsArray = *traitsPtr;

	pattern = "4C 03 05 ?? ?? ?? ?? 48 8B 4B ?? E8 ?? ?? ?? ?? 48 8B 43 ?? BA FD FF 00 00 48 8B 48 ?? 66 C7 41 ?? 00 30";
	DWORD_PTR traitNames = PatternScan(GetModuleHandle(NULL), pattern);
	traitNames = *(DWORD_PTR*)GetAddressFromGlobalRef(traitNames);

	if (traitNames != 0)
	{
		for (int i = 0; i < TRAIT_SIZE; i++)
		{
			traitNamesArray[i] = GetNameFromBinary(i, traitNames);
		}
	}

	currentDirectory = GetDLLPath("");
	currentDirectory.pop_back();
}

void SkillPanel::InspectorLogic()
{
	switch (tab)
	{
	case SkillTab::ACTIVE:
		ImReflect::Input(skillNames[selectedIndex].c_str(), activeSkillArray[selectedIndex], config);

		if (ImGui::Button("Apply To This"))
		{
			activeSkillsPTR->at(selectedIndex) = activeSkillArray[selectedIndex];
		}

		break;
	case SkillTab::ELEMENT:
		ImReflect::Input(skillNames[selectedIndex].c_str(), skillElementArray[selectedIndex], config);

		if (ImGui::Button("Apply To This"))
		{
			skillElementPtr->at(selectedIndex) = skillElementArray.at(selectedIndex);
		}

		break;
	case SkillTab::TECHNICAL:
	{
		auto& selected = technicalArray[selectedIndex];
		ImReflect::Input("Technical Combo", selected, config);

		const char* preview = selected.requireKnowingHeart == knowingHeartNumber.back() ? "Yes" : "No";
		if (ImGui::BeginCombo("Require Knowing Heart", preview))
		{
			for (int i = 0; i < knowingHeartOption.size(); i++)
			{
				bool is_selected = strcmp(preview, knowingHeartOption[i]);

				if (ImGui::Selectable(knowingHeartOption[i], is_selected))
				{
					selected.requireKnowingHeart = knowingHeartNumber[i];
					preview = knowingHeartOption[i];
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Apply To This"))
		{
			technicalPtr->at(selectedIndex) = technicalArray.at(selectedIndex);
		}

		break;
	}
	case SkillTab::TRAIT:
	{
		auto& selected = traitsArray[selectedIndex];
		ImReflect::Input(traitNamesArray[selectedIndex].c_str(), selected, config);
		ImGui::LabelText("##warning", "Warning: flags after Almighty are untested.");
		ImGui::InputInt("Sub Trait ID", (int*)&selected.elementFlags);

		if (ImGui::Button("Apply To This"))
		{
			technicalPtr->at(selectedIndex) = technicalArray.at(selectedIndex);
		}

		break;
	}
	}
}

void SkillPanel::ApplyChanges()
{
	std::memcpy(activeSkillsPTR, activeSkillArray.data(), sizeof(activeSkillArray));
	std::memcpy(skillElementPtr, skillElementArray.data(), sizeof(skillElementArray));
	std::memcpy(technicalPtr, technicalArray.data(), sizeof(technicalArray));
	std::memcpy(traitsPtr, traitsArray.data(), sizeof(traitsArray));
}

void SkillPanel::Refresh()
{
	activeSkillArray = *activeSkillsPTR;
	skillElementArray = *skillElementPtr;
	technicalArray = *technicalPtr;
	traitsArray = *traitsPtr;
}

bool SkillPanel::SkillFilter(const int id)
{
	if (id <= 289 && id >= 280)
		return false;

	auto& skill = activeSkillArray[id];
	auto& element = skillElementArray[id];

	if (skill.casterEffect2 == CasterEffect2::UnknownCasterEffect_CRASH_WARNING || skill.casterEffect2 == CasterEffect2::SummonAlly_CRASH_WARNING
		|| skill.casterEffect2 == CasterEffect2::MeleeContact || skill.casterEffect2 == CasterEffect2::SupportRelated)
		return false;

	if (element.skillType == ElementSkillType::NULLSkill || element.skillType == ElementSkillType::Passive)
		return false;

	return true;
}

void SkillPanel::RandomizeSkills(const size_t seed)
{
	std::mt19937 rng(seed);

	std::vector<int> filteredIndexes;

	for (int i = 1; i < 779; i++)
	{
		if (!SkillFilter(i))
			continue;

		filteredIndexes.push_back(i);
	}

	std::vector<int> preShuffle = filteredIndexes;
	std::shuffle(filteredIndexes.begin(), filteredIndexes.end(),rng);

	std::vector<ActiveSkill> copySkill;
	copySkill.insert(copySkill.end(), std::begin(activeSkillArray), std::end(activeSkillArray));

	std::vector<SkillElement> copyElement;
	copyElement.insert(copyElement.end(), std::begin(skillElementArray), std::end(skillElementArray));

	for (int i = 0; i < filteredIndexes.size(); i++)
	{
		auto& n = activeSkillArray[filteredIndexes[i]];
		auto& n2 = skillElementArray[filteredIndexes[i]];
		auto& o = copySkill[preShuffle[i]];
		auto& o2 = copyElement[preShuffle[i]];

		o = n;
		o2 = n2;
	}

	std::memcpy(activeSkillArray.data(), copySkill.data(), sizeof(activeSkillArray));
	std::memcpy(skillElementArray.data(), copyElement.data(), sizeof(skillElementArray));

	*activeSkillsPTR = activeSkillArray;
	*skillElementPtr = skillElementArray;
}

void SkillPanel::ExportFile(const std::string& directory)
{
	std::vector<BYTE> fileBytes;

	// Repeating swaps to get this back to big endian.
	uint32_t elementSize = _byteswap_ulong(sizeof(skillElementArray));
	fileBytes.insert(fileBytes.end(), (BYTE*)&elementSize, (BYTE*)&elementSize + 4);

	std::array<BYTE, sizeof(skillElementArray)>* skillElementBytes = new std::array<BYTE,sizeof(skillElementArray)>;
	std::memcpy(skillElementBytes->data(), skillElementArray.data(), sizeof(skillElementArray));

	for (int i = 8; i < skillElementBytes->size(); i+=8)
	{
		uint32_t* bytesToSwap = (uint32_t*)(skillElementBytes->data() + i - 4);
		*bytesToSwap = _byteswap_ulong(*bytesToSwap);
	}
	fileBytes.insert(fileBytes.end(), (BYTE*)skillElementBytes, (BYTE*)skillElementBytes + sizeof(*skillElementBytes));
	fileBytes.insert(fileBytes.end(), 12, 0);

	delete(skillElementBytes);

	uint32_t activeSkillSize = _byteswap_ulong(sizeof(activeSkillArray));
	fileBytes.insert(fileBytes.end(), (BYTE*)&activeSkillSize, (BYTE*)&activeSkillSize + 4);

	std::array<BYTE, sizeof(activeSkillArray)>* activeSkillBytes = new std::array<BYTE,sizeof(activeSkillArray)>;
	std::memcpy(activeSkillBytes->data(), activeSkillArray.data(), sizeof(activeSkillArray));

	for (int i = 0; i < activeSkillBytes->size(); i += sizeof(ActiveSkill))
	{
		DWORD_PTR byteBase = (DWORD_PTR)(activeSkillBytes->data() + i);

		uint32_t* bit32Swap = (uint32_t*)byteBase;
		*bit32Swap = _byteswap_ulong(*bit32Swap);

		uint16_t* bit16Swap = (uint16_t*)(byteBase + 4);
		*bit16Swap = _byteswap_ushort(*bit16Swap);

		bit32Swap = (uint32_t*)(byteBase + 0x20);
		*bit32Swap = _byteswap_ulong(*bit32Swap);

		bit16Swap = (uint16_t*)(byteBase + 8);
		*bit16Swap = _byteswap_ushort(*bit16Swap);

		bit16Swap = (uint16_t*)(byteBase + 10);
		*bit16Swap = _byteswap_ushort(*bit16Swap);

		bit16Swap = (uint16_t*)(byteBase + 24);
		*bit16Swap = _byteswap_ushort(*bit16Swap);

		bit16Swap = (uint16_t*)(byteBase + 28);
		*bit16Swap = _byteswap_ushort(*bit16Swap);

		bit32Swap = (uint32_t*)(byteBase + 36);
		*bit32Swap = _byteswap_ulong(*bit32Swap);

		bit32Swap = (uint32_t*)(byteBase + 40);
		*bit32Swap = _byteswap_ulong(*bit32Swap);

		bit32Swap = (uint32_t*)(byteBase + 16);
		*bit32Swap = _byteswap_ulong(*bit32Swap);
	}
	fileBytes.insert(fileBytes.end(), (BYTE*)activeSkillBytes->data(), (BYTE*)activeSkillBytes->data() + sizeof(activeSkillArray));
	fileBytes.insert(fileBytes.end(), 12, 0);

	delete(activeSkillBytes);

	uint32_t technicalSize = _byteswap_ulong(sizeof(technicalArray));
	fileBytes.insert(fileBytes.end(), (BYTE*)&technicalSize, (BYTE*)&technicalSize + 4);

	std::array<BYTE, sizeof(technicalArray)> technicalBytes;
	std::memcpy(technicalBytes.data(), technicalArray.data(), sizeof(technicalArray));

	for (int i = 0; i < technicalBytes.size(); i += 4)
	{
		uint32_t* bit32Swap = (uint32_t*)(technicalBytes.data() + i);
		*bit32Swap = _byteswap_ulong(*bit32Swap);
	}
	fileBytes.insert(fileBytes.end(), (BYTE*)technicalBytes.data(), (BYTE*)technicalBytes.data() + sizeof(technicalBytes));
	fileBytes.insert(fileBytes.end(), 4, 0);

	uint32_t traitSize = _byteswap_ulong(sizeof(traitsArray));
	fileBytes.insert(fileBytes.end(), (BYTE*)&traitSize, (BYTE*)&traitSize + 4);

	std::array<BYTE, sizeof(traitsArray)>* traitBytes = new std::array<BYTE, sizeof(traitsArray)>;
	std::memcpy(traitBytes->data(), traitsArray.data(), sizeof(traitsArray));

	for (int i = 0; i < traitBytes->size(); i += sizeof(Trait))
	{
		DWORD_PTR byteBase = (DWORD_PTR)(traitBytes->data() + i);

		uint16_t* bit16Swap = (uint16_t*)(byteBase);
		*bit16Swap = _byteswap_ushort(*bit16Swap);

		bit16Swap = (uint16_t*)(byteBase + 2);
		*bit16Swap = _byteswap_ushort(*bit16Swap);

		for (int j = 4; j < sizeof(Trait); j += 4)
		{
			uint32_t* bit32Swap = (uint32_t*)(byteBase + j);
			*bit32Swap = _byteswap_ulong(*bit32Swap);
		}
	}
	fileBytes.insert(fileBytes.end(), (BYTE*)traitBytes->data(), (BYTE*)traitBytes->data() + sizeof(*traitBytes));
	fileBytes.insert(fileBytes.end(), 8, 0);

	delete(traitBytes);

	std::ofstream file(currentDirectory + '\\' + "SKILL.TBL", std::ios::binary);

	if (!file.is_open())
	{
		DWORD error = GetLastError();
		printf("File failed to write! Error Code: 0x%x\n",error);
		return;
	}

	file.write((const char*)fileBytes.data(), fileBytes.size());
	file.close();
	printf("Successfully wrote to %s\n", directory.c_str());
}