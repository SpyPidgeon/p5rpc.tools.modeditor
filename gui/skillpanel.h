#pragma once
#include "panels.h"
#include "exportable.h"

enum class SkillTab
{
    ACTIVE,
    ELEMENT,
    TECHNICAL,
    TRAIT
};

class SkillPanel : SearchablePanel, Exportable
{
public:
    SkillPanel() { this->label = "Skill Table"; sortingPriority = 10; }

    static SkillPanel* GetInstance()
    {
        static SkillPanel* instance = new SkillPanel();
        return instance;
    }

    void RenderLogic() override;
    void InspectorLogic() override;
    void ScanValues() override;
    void ApplyChanges() override;
    void Refresh() override;

    void ExportFile(const std::string& directory) override;

    static constexpr uint16_t ACTIVE_SKILL_SIZE = 800;
    static constexpr uint16_t SKILL_ELEMENT_SIZE = 1056;
    static constexpr uint8_t TECHNICAL_SIZE = 17;
    static constexpr uint16_t TRAIT_SIZE = 299;

    std::array<ActiveSkill, ACTIVE_SKILL_SIZE> activeSkillArray;
    std::array<ActiveSkill, ACTIVE_SKILL_SIZE>* activeSkillsPTR;

    std::array<std::string, SKILL_ELEMENT_SIZE> skillNames;
    std::array<SkillElement, SKILL_ELEMENT_SIZE> skillElementArray;
    std::array<SkillElement, SKILL_ELEMENT_SIZE>* skillElementPtr;

    std::array<Technical, TECHNICAL_SIZE>* technicalPtr;
    std::array<Technical, TECHNICAL_SIZE> technicalArray;

    std::array<int, 2> knowingHeartNumber =
    {
        0,805306685
    };
    std::array<const char*, 2> knowingHeartOption =
    {
        "No",
        "Yes"
    };

    std::array<Trait, TRAIT_SIZE>* traitsPtr;
    std::array<Trait, TRAIT_SIZE> traitsArray;
    std::array<std::string, TRAIT_SIZE> traitNamesArray;

    void RandomizeSkills(const size_t seed);

private:
    SkillTab tab = SkillTab::ACTIVE;
};
static SkillPanel* skillPanel = SkillPanel::GetInstance();