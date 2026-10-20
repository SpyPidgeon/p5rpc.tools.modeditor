#pragma once
#include <vector>
#include <functional>
#include <filesystem>
#include <fstream>
#include "filestructs.h"
#include "battlestructs.h"
#include "skillstructs.h"
#include "itemstructs.h"

std::string GetNameFromBinary(const uint32_t currentIndex, const uintptr_t nameAddress);

typedef DatUnit* (__stdcall* GetDatUnitByID)(uint16_t ID);
extern GetDatUnitByID GetDatUnit;

extern ImSettings config;

constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

struct Panel
{
    Panel();
    Panel(std::string label) : label(label) {}

    std::string label;
    const char* childLabel = "Inspector";
    bool open = false;
    int selectedIndex = 0;
    uint16_t sortingPriority = 0;
    void RenderPanel();

    virtual void RenderLogic() {}
    virtual void InspectorLogic() {}
    virtual void ScanValues() {}
    virtual void ApplyChanges() {}
    virtual void Refresh() {}
};

extern std::vector<Panel*> panels;

struct SearchablePanel : Panel
{
    SearchablePanel() {}

    char searchText[256] = "";
    static bool TextMatch(std::string search, std::string match);
    void RenderLogic() override;

    template<typename T, std::size_t S, std::size_t NS>
    void RenderList(std::array<T, S>* array, std::array<std::string, NS>* names)
    {
        std::string search = searchText;
        for (int i = 0; i < S; i++)
        {
            if (search != "" && !TextMatch(search, names->at(i)))
                continue;

            std::string buttonName = format("ID: {:03d} | {}", i, names->at(i));

            if (ImGui::Button(buttonName.c_str()))
            {
                selectedIndex = i;
            }
        }
    }

    template<typename T, std::size_t S>
    void RenderListNoName(std::array<T, S>* array, const std::string& name)
    {
        for (int i = 0; i < S; i++)
        {
            std::string buttonName = format("ID: {:02d} | {}", i, name);

            if (ImGui::Button(buttonName.c_str()))
            {
                selectedIndex = i;
            }
        }
    }
};