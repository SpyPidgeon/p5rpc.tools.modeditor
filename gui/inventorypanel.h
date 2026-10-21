#pragma once
#include "panels.h"

template<std::size_t N>
struct InventoryType
{
	std::array<uint8_t, N> items;
	std::array<std::string, N> names;
	DWORD_PTR address = NULL;
	DWORD_PTR namesAddress = NULL;
	char search[256] = "";

	void SetData(DWORD_PTR address)
	{
		this->address = address;
		//Refresh();
	}

	void SetNames(DWORD_PTR address)
	{
		namesAddress = address;
	}

	void Refresh()
	{
		std::memcpy(items.data(), (void*)address, N);

		for (int i = 0; i < N; i++)
		{
			names[i] = GetNameFromBinary(i, namesAddress);
		}
	}

	void RenderSliders()
	{
		ImGui::InputText("Search", search, 256);
		for (int i = 0; i < items.size(); i++)
		{
			if (!SearchablePanel::TextMatch(search, names[i]))
				continue;

			int value = items[i];
			ImGui::SliderInt(std::format("{} | {}", i, names[i]).c_str(), &value, 0, 99);
			items[i] = value;
		}

	}

	void ApplyValues()
	{
		std::memcpy((void*)address, items.data(), N);
	}
};

struct Inventory
{
	InventoryType<296> melee;
	InventoryType<299> armor;
	InventoryType<512> accessory;
	InventoryType<696> consumable;
	InventoryType<256> key;
	InventoryType<256> material;
	InventoryType<401> card;
	InventoryType<286> outfit;
	InventoryType<256> ranged;
};
VISITABLE_STRUCT(Inventory, melee, armor, accessory, consumable, key, material, card, outfit, ranged);
IMGUI_REFLECT(Inventory, melee, armor, accessory, consumable, key, material, card, outfit, ranged);

struct RefreshInventory
{
	template<typename T>
	void operator()(const char* name, T& member)
	{
		member.Refresh();
	}
};

void RefreshFullInventory(Inventory& inv);
void SetInventoryAddresses(Inventory& inv, std::array<DWORD_PTR, 9>& addresses);
void SetInventoryNames(Inventory& inv, std::array<DWORD_PTR, 9>& addresses);
void ApplyAllInventoryValues(Inventory& inv);

class InventoryPanel : Panel
{
public:
    InventoryPanel() { this->label = "Inventory"; }
    static InventoryPanel* GetInstance() { static InventoryPanel* instance = new InventoryPanel(); return instance; }

    void RenderLogic() override;
    void InspectorLogic() override;
    void ScanValues() override;
	void ApplyChanges() override;
	void Refresh() override;

	Inventory playerInventory;
private:
	uint16_t selected = 0;
};
static InventoryPanel* inventoryPanel = InventoryPanel::GetInstance();