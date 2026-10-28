#include "itempanel.h"
#include "signaturescan.h"
#include <fstream>
#include "skillpanel.h"
#include <random>
#include <algorithm>
#include <utility>

void ItemPanel::ScanValues()
{
	GFDItemInfo* info;
	std::string pattern = "48 89 05 ?? ?? ?? ?? 48 C1 E9 06";
	DWORD_PTR address = PatternScan(GetModuleHandle(NULL), pattern);

	info = (GFDItemInfo*)GetAddressFromGlobalRef(address);

	itemPtrs.accessories = (std::array<AccessoryItem,AccessoryItem::ACCESSORY_SIZE>*)info->accessories;
	itemPtrs.armors = (std::array<Armor, Armor::ARMOR_SIZE>*)info->armors;
	itemPtrs.consumables = (std::array<Consumable, Consumable::CONSUMABLE_SIZE>*)info->consumables;
	itemPtrs.keyItems = (std::array<KeyItem, KeyItem::KEY_ITEM_SIZE>*)info->keyItems;
	itemPtrs.melees = (std::array<Melee, Melee::MELEE_SIZE>*)info->melee;
	itemPtrs.outfits = (std::array<Outfit, Outfit::OUTFIT_SIZE>*)info->outfits;
	itemPtrs.rangedWeapons = (std::array<RangedWeapon, RangedWeapon::RANGED_SIZE>*)info->rangedWeapons;
	itemPtrs.skillCards = (std::array<SkillCard, SkillCard::SKILL_CARD_SIZE>*)info->skillCards;
	itemPtrs.treasures = (std::array<Treasure, Treasure::TREASURE_SIZE>*)info->treasure;

	Refresh();

	pattern = "C6 05 ?? ?? ?? ?? 00 33 C0 48 83 C4 60";
	DWORD_PTR nameAddress = PatternScan(GetModuleHandle(NULL), pattern);
	GFDItemNameInfo* nameInfo = (GFDItemNameInfo*)(GetAddressFromMOV(nameAddress) + 9);

	for (int i = 0; i < itemNames.accessory.size(); i++)
	{
		auto& item = itemNames.accessory[i];
		item = GetNameFromBinary(i, (uintptr_t)nameInfo->accessoryNames);
	}
	for (int i = 0; i < itemNames.armor.size(); i++)
	{
		auto& item = itemNames.armor[i];
		item = GetNameFromBinary(i, (uintptr_t)nameInfo->armorNames);
	}
	for (int i = 0; i < itemNames.consumable.size(); i++)
	{
		auto& item = itemNames.consumable[i];
		item = GetNameFromBinary(i, (uintptr_t)nameInfo->consumableNames);
	}
	for (int i = 0; i < itemNames.keyItem.size(); i++)
	{
		auto& item = itemNames.keyItem[i];
		item = GetNameFromBinary(i, (uintptr_t)nameInfo->keyItemNames);
	}
	for (int i = 0; i < itemNames.treasure.size(); i++)
	{
		auto& item = itemNames.treasure[i];
		item = GetNameFromBinary(i, (uintptr_t)nameInfo->treasureNames);
	}
	for (int i = 0; i < itemNames.melee.size(); i++)
	{
		auto& item = itemNames.melee[i];
		item = GetNameFromBinary(i, (uintptr_t)nameInfo->meleeNames);
	}
	for (int i = 0; i < itemNames.outfit.size(); i++)
	{
		auto& item = itemNames.outfit[i];
		item = GetNameFromBinary(i, (uintptr_t)nameInfo->outfitNames);
	}
	for (int i = 0; i < itemNames.skillCard.size(); i++)
	{
		auto& item = itemNames.skillCard[i];
		item = GetNameFromBinary(i, (uintptr_t)nameInfo->skillCardNames);
	}
	for (int i = 0; i < itemNames.rangedWeapon.size(); i++)
	{
		auto& item = itemNames.rangedWeapon[i];
		item = GetNameFromBinary(i, (uintptr_t)nameInfo->rangedNames);
	}
}

void ItemPanel::RenderLogic()
{
	Exportable::RenderButton();
	SearchablePanel::RenderLogic();

	if (ImGui::BeginTabBar("Items",ImGuiTabBarFlags_FittingPolicyScroll))
	{
		uint8_t i = 0;
		visit_struct::for_each(itemNames, [&i,this](const char* name, const auto& member)
			{
				if (ImGui::BeginTabItem(name))
				{
					if ((ItemTab)i != selectedTab)
					{
						selectedTab = (ItemTab)i;
						selectedIndex = 0;
					}

					for (int j = 0; j < member.size(); j++)
					{
						if (!TextMatch(searchText, member.at(j)))
							continue;

						std::string label = std::format("ID: {:03d} | {}", j, member.at(j));
						if (ImGui::Button(label.c_str()))
						{
							selectedIndex = j;
						}
					}

					ImGui::EndTabItem();
				}
				i++;
			});

		ImGui::EndTabBar();
	}

	if (renderExplorer)
		RenderExplorer();
}

char skillSearchBuffer[256] = "\0";
void tag_invoke(ImReflect::ImInput_t, const char* label, SkillID& value, ImSettings& settings, ImResponse& response)
{
	auto& skill_response = response.get<SkillID>();

	std::string& currentValue = skillPanel->skillNames.at(value.skillID);
	std::string preview = std::format("ID: {:03d} | {}", value.skillID, currentValue);

	ImGui::SetNextWindowSizeConstraints(ImVec2(0, ImGui::GetFrameHeightWithSpacing() * 10), ImVec2(FLT_MAX, FLT_MAX));
	if (ImGui::BeginCombo(label, preview.c_str()))
	{
		ImGui::InputText("Search", skillSearchBuffer, 256);

		ImGui::BeginChild("##child", ImVec2(0, ImGui::GetFrameHeightWithSpacing() * 10));
		for (int i = 0; i < skillPanel->skillNames.size(); i++)
		{
			auto& currentName = skillPanel->skillNames.at(i);
			bool is_selected = value.skillID == i;

			std::string dispName = std::format("ID: {:03d} | {}", i, currentName);

			if (skillSearchBuffer[0] != '\0' && !SearchablePanel::TextMatch(skillSearchBuffer, dispName))
				continue;

			if (ImGui::Selectable(dispName.c_str(), is_selected, 0))
				value.skillID = i;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndChild();

		ImGui::EndCombo();
	}

	ImReflect::Detail::check_input_states(skill_response);
}

void ItemPanel::InspectorLogic()
{
	switch (selectedTab)
	{
	case ItemTab::Accessory:
		ImReflect::Input(itemNames.accessory.at(selectedIndex).c_str(), items.accessories.at(selectedIndex), config);

		if (ImGui::Button("Apply To This"))
		{
			itemPtrs.accessories->at(selectedIndex) = items.accessories.at(selectedIndex);
		}

		break;
	case ItemTab::Armor:
		ImReflect::Input(itemNames.armor.at(selectedIndex).c_str(), items.armors.at(selectedIndex),config);

		if (ImGui::Button("Apply To This"))
		{
			itemPtrs.armors->at(selectedIndex) = items.armors.at(selectedIndex);
		}

		break;
	case ItemTab::Consumable:
		ImReflect::Input(itemNames.consumable.at(selectedIndex).c_str(), items.consumables.at(selectedIndex), config);

		if (ImGui::Button("Apply To This"))
		{
			itemPtrs.consumables->at(selectedIndex) = items.consumables.at(selectedIndex);
		}

		break;
	case ItemTab::KeyItem:
		ImReflect::Input(itemNames.keyItem.at(selectedIndex).c_str(), items.keyItems.at(selectedIndex), config);

		if (ImGui::Button("Apply To This"))
		{
			itemPtrs.keyItems->at(selectedIndex) = items.keyItems.at(selectedIndex);
		}

		break;
	case ItemTab::Treasure:
		ImReflect::Input(itemNames.treasure.at(selectedIndex).c_str(), items.treasures.at(selectedIndex), config);

		if (ImGui::Button("Apply To This"))
		{
			itemPtrs.treasures->at(selectedIndex) = items.treasures.at(selectedIndex);
		}

		break;
	case ItemTab::Melee:
		ImReflect::Input(itemNames.melee.at(selectedIndex).c_str(), items.melees.at(selectedIndex), config);

		if (ImGui::Button("Apply To This"))
		{
			itemPtrs.melees->at(selectedIndex) = items.melees.at(selectedIndex);
		}

		break;
	case ItemTab::Outfit:
		ImReflect::Input(itemNames.outfit.at(selectedIndex).c_str(), items.outfits.at(selectedIndex), config);

		if (ImGui::Button("Apply To This"))
		{
			itemPtrs.outfits->at(selectedIndex) = items.outfits.at(selectedIndex);
		}

		break;
	case ItemTab::SkillCard:
		ImReflect::Input(itemNames.skillCard.at(selectedIndex).c_str(), items.skillCards.at(selectedIndex), config);

		if (ImGui::Button("Apply To This"))
		{
			itemPtrs.skillCards->at(selectedIndex) = items.skillCards.at(selectedIndex);
		}

		break;
	case ItemTab::RangedWeapon:
		ImReflect::Input(itemNames.rangedWeapon.at(selectedIndex).c_str(), items.rangedWeapons.at(selectedIndex), config);

		if (ImGui::Button("Apply To This"))
		{
			itemPtrs.rangedWeapons->at(selectedIndex) = items.rangedWeapons.at(selectedIndex);
		}

		break;
	}
}

void ItemPanel::Refresh()
{
	items.accessories = *itemPtrs.accessories;
	items.armors = *itemPtrs.armors;
	items.consumables = *itemPtrs.consumables;
	items.keyItems = *itemPtrs.keyItems;
	items.melees = *itemPtrs.melees;
	items.outfits = *itemPtrs.outfits;
	items.rangedWeapons = *itemPtrs.rangedWeapons;
	items.skillCards = *itemPtrs.skillCards;
	items.treasures = *itemPtrs.treasures;
}

void ItemPanel::ApplyChanges()
{
	*itemPtrs.accessories = items.accessories;
	*itemPtrs.armors = items.armors;
	*itemPtrs.consumables = items.consumables;
	*itemPtrs.keyItems = items.keyItems;
	*itemPtrs.melees = items.melees;
	*itemPtrs.outfits = items.outfits;
	*itemPtrs.rangedWeapons = items.rangedWeapons;
	*itemPtrs.skillCards = items.skillCards;
	*itemPtrs.treasures = items.treasures;
}

void ItemPanel::RandomizeConsumables(const size_t seed, const bool randomize)
{
	std::mt19937 rng(seed);

	std::vector<Consumable> consumableCopy;
	consumableCopy.insert(consumableCopy.end(), items.consumables.begin(), items.consumables.end());

	if (randomize)
	{
		for (auto& consumable : consumableCopy)
		{
			std::uniform_int_distribution dist(1, 779);
			int newID = dist(rng);

			if (consumable.icon != ItemType::Consumable)
				continue;

			if (!skillPanel->SkillFilter(newID))
			{
				while (true)
				{
					newID = dist(rng);

					if (skillPanel->SkillFilter(newID))
						break;
				}

				consumable.skill.skillID = newID;
			}
		}
	}
	else
	{
		std::vector<int> filteredIndexes;

		for (int i = 0; i < items.consumables.size(); i++)
		{
			auto& consumable = items.consumables[i];
			if (skillPanel->SkillFilter(consumable.skill.skillID))
			{
				filteredIndexes.push_back(i);
			}
		}

		std::vector<int> preShuffle = filteredIndexes;
		std::shuffle(filteredIndexes.begin(), filteredIndexes.end(), rng);

		for (int i = 0; i < filteredIndexes.size(); i++)
		{
			auto& n = items.consumables[filteredIndexes[i]];
			auto& o = consumableCopy[preShuffle[i]];

			o.skill = n.skill;
		}
	}

	std::memcpy(items.consumables.data(), consumableCopy.data(), sizeof(items.consumables));
	*itemPtrs.consumables = items.consumables;
}

void ItemPanel::ExportFile(const std::string& directory)
{
	std::vector<BYTE> fileBytes;

	uint32_t accessorySize = sizeof(items.accessories);
	accessorySize = _byteswap_ulong(accessorySize);
	fileBytes.insert(fileBytes.end(), (BYTE*)&accessorySize, (BYTE*)&accessorySize + 4);

	std::array<AccessoryItem, AccessoryItem::ACCESSORY_SIZE>* accessoryBytes = new std::array<AccessoryItem, AccessoryItem::ACCESSORY_SIZE>;
	std::memcpy(accessoryBytes->data(), items.accessories.data(), sizeof(items.accessories));

	AccessoryItem::SwapData(accessoryBytes);
	fileBytes.insert(fileBytes.end(), (BYTE*)accessoryBytes->data(), (BYTE*)accessoryBytes->data() + sizeof(*accessoryBytes));
	delete(accessoryBytes);

	fileBytes.insert(fileBytes.end(), 12, 0);

	uint32_t armorSize = sizeof(items.armors);
	armorSize = _byteswap_ulong(armorSize);
	fileBytes.insert(fileBytes.end(), (BYTE*)&armorSize, (BYTE*)&armorSize + 4);

	std::array<Armor, Armor::ARMOR_SIZE>* armorBytes = new std::array<Armor, Armor::ARMOR_SIZE>;
	std::memcpy(armorBytes->data(), items.armors.data(), sizeof(items.armors));

	Armor::SwapBytes(armorBytes);
	fileBytes.insert(fileBytes.end(), (BYTE*)armorBytes->data(), (BYTE*)armorBytes->data() + sizeof(*armorBytes));
	delete(armorBytes);

	fileBytes.insert(fileBytes.end(), 12, 0);

	uint32_t consumableSize = sizeof(items.consumables);
	consumableSize = _byteswap_ulong(consumableSize);
	fileBytes.insert(fileBytes.end(), (BYTE*)&consumableSize, (BYTE*)&consumableSize + 4);

	std::array<Consumable, Consumable::CONSUMABLE_SIZE>* consumableBytes = new std::array<Consumable, Consumable::CONSUMABLE_SIZE>;
	std::memcpy(consumableBytes->data(), items.consumables.data(), sizeof(items.consumables));

	Consumable::SwapBytes(consumableBytes);
	fileBytes.insert(fileBytes.end(), (BYTE*)consumableBytes->data(), (BYTE*)consumableBytes->data() + sizeof(*consumableBytes));
	delete(consumableBytes);

	fileBytes.insert(fileBytes.end(), 12, 0);

	uint32_t keyItemSize = sizeof(items.keyItems);
	keyItemSize = _byteswap_ulong(keyItemSize);
	fileBytes.insert(fileBytes.end(), (BYTE*)&keyItemSize, (BYTE*)&keyItemSize + 4);

	std::array<KeyItem, KeyItem::KEY_ITEM_SIZE>* keyItemBytes = new std::array<KeyItem, KeyItem::KEY_ITEM_SIZE>;
	std::memcpy(keyItemBytes->data(), items.keyItems.data(), sizeof(items.keyItems));

	KeyItem::SwapBytes(keyItemBytes);
	fileBytes.insert(fileBytes.end(), (BYTE*)keyItemBytes->data(), (BYTE*)keyItemBytes->data() + sizeof(*keyItemBytes));
	delete(keyItemBytes);

	fileBytes.insert(fileBytes.end(), 12, 0);

	uint32_t treasureSize = sizeof(items.treasures);
	treasureSize = _byteswap_ulong(treasureSize);
	fileBytes.insert(fileBytes.end(), (BYTE*)&treasureSize, (BYTE*)&treasureSize + 4);

	std::array<Treasure, Treasure::TREASURE_SIZE>* treasureBytes = new std::array<Treasure, Treasure::TREASURE_SIZE>;
	std::memcpy(treasureBytes->data(), items.treasures.data(), sizeof(items.treasures));

	Treasure::SwapBytes(treasureBytes);
	fileBytes.insert(fileBytes.end(), (BYTE*)treasureBytes->data(), (BYTE*)treasureBytes->data() + sizeof(*treasureBytes));
	delete(treasureBytes);

	fileBytes.insert(fileBytes.end(), 12, 0);

	uint32_t meleeSize = sizeof(items.melees);
	meleeSize = _byteswap_ulong(meleeSize);
	fileBytes.insert(fileBytes.end(), (BYTE*)&meleeSize, (BYTE*)&meleeSize + 4);

	std::array<Melee, Melee::MELEE_SIZE>* meleeBytes = new std::array<Melee, Melee::MELEE_SIZE>;
	std::memcpy(meleeBytes->data(), items.melees.data(), sizeof(items.melees));

	Melee::SwapBytes(meleeBytes);
	fileBytes.insert(fileBytes.end(), (BYTE*)meleeBytes->data(), (BYTE*)meleeBytes->data() + sizeof(*meleeBytes));
	delete(meleeBytes);

	fileBytes.insert(fileBytes.end(), 12, 0);

	uint32_t outfitSize = sizeof(items.outfits);
	outfitSize = _byteswap_ulong(outfitSize);
	fileBytes.insert(fileBytes.end(), (BYTE*)&outfitSize, (BYTE*)&outfitSize + 4);

	std::array<Outfit, Outfit::OUTFIT_SIZE>* outfitBytes = new std::array<Outfit, Outfit::OUTFIT_SIZE>;
	std::memcpy(outfitBytes->data(), items.outfits.data(), sizeof(items.outfits));

	Outfit::SwapBytes(outfitBytes);
	fileBytes.insert(fileBytes.end(), (BYTE*)outfitBytes->data(), (BYTE*)outfitBytes->data() + sizeof(*outfitBytes));
	delete(outfitBytes);

	fileBytes.insert(fileBytes.end(), 12, 0);

	uint32_t skillCardSize = sizeof(items.skillCards);
	skillCardSize = _byteswap_ulong(skillCardSize);
	fileBytes.insert(fileBytes.end(), (BYTE*)&skillCardSize, (BYTE*)&skillCardSize + 4);
	
	std::array<SkillCard, SkillCard::SKILL_CARD_SIZE>* skillCardBytes = new std::array<SkillCard, SkillCard::SKILL_CARD_SIZE>;
	std::memcpy(skillCardBytes->data(), items.skillCards.data(), sizeof(items.skillCards));

	SkillCard::SwapBytes(skillCardBytes);
	fileBytes.insert(fileBytes.end(), (BYTE*)skillCardBytes->data(), (BYTE*)skillCardBytes->data() + sizeof(*skillCardBytes));
	delete(skillCardBytes);

	fileBytes.insert(fileBytes.end(), 4, 0);

	uint32_t rangedSize = sizeof(items.rangedWeapons);
	rangedSize = _byteswap_ulong(rangedSize);
	fileBytes.insert(fileBytes.end(), (BYTE*)&rangedSize, (BYTE*)&rangedSize + 4);

	std::array<RangedWeapon, RangedWeapon::RANGED_SIZE>* rangedBytes = new std::array<RangedWeapon, RangedWeapon::RANGED_SIZE>;
	std::memcpy(rangedBytes->data(), items.rangedWeapons.data(),sizeof(items.rangedWeapons));

	RangedWeapon::SwapBytes(rangedBytes);
	fileBytes.insert(fileBytes.end(), (BYTE*)rangedBytes->data(), (BYTE*)rangedBytes->data() + sizeof(*rangedBytes));
	delete(rangedBytes);

	fileBytes.insert(fileBytes.end(), 12, 0);
	fileBytes.insert(fileBytes.end(), FOOTER_BYTES.data(), FOOTER_BYTES.data() + sizeof(FOOTER_BYTES));

	std::ofstream file(currentDirectory + '\\' + "ITEM.TBL", std::ios::binary);

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