#include "imguistyling.h"

constexpr ImVec4 UI_RED = ImVec4(1, 0, 0, 1);
constexpr ImVec4 UI_WHITE = ImVec4(1, 1, 1, 1);
constexpr ImVec4 UI_BLACK = ImVec4(0, 0, 0, 1);

void SetImGuiStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	auto viewport = ImGui::GetMainViewport();

	style.FrameBorderSize = viewport->Size.y * 0.02f;
	style.TabBorderSize = viewport->Size.y * 0.02f;
	style.TabRounding = 0.0f;
	style.ImageBorderSize = viewport->Size.y * 0.02f;
	style.PopupBorderSize = viewport->Size.y * 0.02f;
	style.DragDropTargetBorderSize = viewport->Size.y * 0.02f;
	style.WindowBorderSize = viewport->Size.y * 0.02f;

	ImVec4* color = style.Colors;

	color[ImGuiCol_TitleBg] = UI_BLACK;
	color[ImGuiCol_Text] = UI_WHITE;
	color[ImGuiCol_Border] = UI_WHITE;
	color[ImGuiCol_Button] = UI_BLACK;
	color[ImGuiCol_ButtonActive] = UI_RED;
	color[ImGuiCol_ButtonHovered] = UI_RED;
	color[ImGuiCol_FrameBg] = UI_BLACK;
	color[ImGuiCol_Tab] = UI_BLACK;
	color[ImGuiCol_TabActive] = UI_RED;
	color[ImGuiCol_TabHovered] = UI_RED;
	color[ImGuiCol_Header] = UI_BLACK;
	color[ImGuiCol_HeaderHovered] = UI_RED;
	color[ImGuiCol_HeaderActive] = UI_RED;
	color[ImGuiCol_SliderGrab] = UI_WHITE;
}

void SetImReflectConfig()
{

	config = ImSettings();
	config.push_member<&ActiveSkill::validTargetFlags>()
		.as_flags()
		.pop();
	config.push_member<&ActiveSkill::effectChance>()
		.as_slider()
		.min(0)
		.max(100)
		.pop();
	config.push_member<&ActiveSkill::critChance>()
		.as_slider()
		.min(0)
		.max(100)
		.pop();
	config.push_member<&ActiveSkill::accuracy>()
		.as_slider()
		.min(0)
		.max(100)
		.pop();
	config.push_member<&ActiveSkill::commonAilments1>()
		.as_flags()
		.pop();
	config.push_member<&ActiveSkill::specialAilments>()
		.as_flags()
		.pop();
	config.push_member<&ActiveSkill::commonAilments2>()
		.as_flags()
		.pop();
	config.push_member<&ActiveSkill::buffsAndDebuffs>()
		.as_flags()
		.pop();
	config.push_member<&ActiveSkill::commonBuffs>()
		.as_flags()
		.pop();
	config.push_member<&ActiveSkill::shields>()
		.as_flags()
		.pop();
	config.push_member<&ActiveSkill::breakSkills>()
		.as_flags()
		.pop();

	config.push_member<&Technical::applicableAilments>()
		.as_flags()
		.pop();

	config.push_member<&Trait::flags>()
		.as_flags()
		.pop();
	config.push_member<&Trait::effectRate>()
		.as_slider()
		.min(0)
		.max(100)
		.pop();
	config.push_member<&Trait::elementFlags>()
		.as_flags()
		.pop();

	config.push_member<&SkillElement::inheritable>()
		.as_slider()
		.min(0)
		.max(8)
		.pop();

	config.push_member < &DatUnit_Stats::strength>()
		.as_slider()
		.min(0)
		.max(99)
		.pop();
	config.push_member < &DatUnit_Stats::magic>()
		.as_slider()
		.min(0)
		.max(99)
		.pop();
	config.push_member < &DatUnit_Stats::endurance>()
		.as_slider()
		.min(0)
		.max(99)
		.pop();
	config.push_member < &DatUnit_Stats::agility>()
		.as_slider()
		.min(0)
		.max(99)
		.pop();
	config.push_member < &DatUnit_Stats::luck>()
		.as_slider()
		.min(0)
		.max(99)
		.pop();

	config.push<EquippableUser>()
		.as_flags()
		.pop();
	config.push<ItemType>()
		.as_flags()
		.pop();

	config.push<EncounterFlags>()
		.as_flags()
		.pop();
}