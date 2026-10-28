#include "quickpanel.h"
#include "encounterpanel.h"
#include "skillpanel.h"
#include "itempanel.h"
#include <functional>
#include <random>

void QuickPanel::RenderLogic()
{
	/*
	if (ImGui::Button("Noclip"))
	{
		selectedIndex = QuickSelection::NOCLIP;
	}
	*/

	if (ImGui::Button("Teleport"))
	{
		selectedIndex = QuickSelection::TELEPORT;
	}

	if (ImGui::Button("Field & Event"))
	{
		selectedIndex = QuickSelection::QUICK_FUNCTIONS;
	}

	if (ImGui::Button("Randomizer"))
	{
		selectedIndex = QuickSelection::RANDOMIZER;
	}
}

static EncounterRandomSettings encounterSettings;
char seed[256] = "\0";
size_t currentHash = static_cast<size_t>(std::random_device{}());
void QuickPanel::InspectorLogic()
{
	switch (selectedIndex)
	{
	case QuickSelection::NOCLIP:
		ImReflect::Input("Noclip", noclip);

		if (noclip.enabled == false && keyState != 0)
			keyState = 0;
		break;
	case QuickSelection::TELEPORT:

		ImReflect::Input("Teleport", teleport);

		if (ImGui::Button("Teleport"))
		{
			Teleport(teleport);
		}

		if (ImGui::Button("Save Current Position"))
		{
			SavePosition();
		}

		break;
	case QuickSelection::QUICK_FUNCTIONS:
		ImReflect::Input("Event Parameters", eventParams);
		if (ImGui::Button("Run Event"))
		{
			CallEvent();
		}
		ImGui::Separator();
		ImReflect::Input("Field Parameters", fieldParams);
		if (ImGui::Button("Run Field"))
		{
			CallField();
		}
		break;

	case QuickSelection::RANDOMIZER:
	{
		ImGui::TextWrapped("Warning: This is experimental and can cause softlocks, hardlocks, and crashes. Use at your own risk.");
		ImGui::Spacing();

		if (ImGui::InputText("Seed", seed, 256))
		{
			if (seed[0] != '\0')
				currentHash = std::hash<std::string_view>{}(std::string_view(seed, 256));
			else
			{
				currentHash = static_cast<size_t>(std::random_device{}());
			}
		}
		ImGui::SeparatorText("Encounter");

		ImReflect::Input("Encounter Settings", encounterSettings, config);
		ImGui::TextWrapped("Warning: Randomizing enemies may cause crashes if you do not have Custom Bonus Tweaks!");
		ImGui::Spacing();

		if (ImGui::Button("Randomize Encounters"))
		{
			encounterPanel->RandomizeEncounters(encounterSettings, currentHash);
		}

		ImGui::Separator();
		ImGui::Spacing();

		ImGui::SeparatorText("Skill");
		ImGui::TextWrapped("This only shuffles active skills, it skips passives and null skills and does not change the particle effects.");
		ImGui::Spacing();

		if (ImGui::Button("Randomize Skills"))
		{
			skillPanel->RandomizeSkills(currentHash);
		}

		ImGui::Separator();
		ImGui::Spacing();

		ImGui::SeparatorText("Item");

		static bool randomizeItems = false;
		ImGui::Checkbox("Randomize Consumable Skill", &randomizeItems);

		if (ImGui::Button("Randomize Consumables"))
		{
			itemPanel->RandomizeConsumables(currentHash, randomizeItems);
		}

		ImGui::Separator();
		ImGui::Spacing();

		break;
	}
	}
}

void QuickPanel::SavePosition()
{
	RUN_FLOWSCRIPT_BY_NAME(FLD_PC_GET_RESHND, 0);
	float reshnd = GET_RETURN_INT();

	RUN_FLOWSCRIPT_BY_NAME(FLD_MODEL_GET_X_TRANSLATE, reshnd);
	float x = GET_RETURN_FLOAT();
	RUN_FLOWSCRIPT_BY_NAME(FLD_MODEL_GET_Y_TRANSLATE, reshnd);
	float y = GET_RETURN_FLOAT();
	RUN_FLOWSCRIPT_BY_NAME(FLD_MODEL_GET_Z_TRANSLATE, reshnd);
	float z = GET_RETURN_FLOAT();

	printf("[P5R Mod Editor] Saved Position: %f, %f, %f\n", x, y, z);
	teleport = Vector3(x, y, z);
}

void QuickPanel::Teleport(const Vector3& position)
{
	RUN_FLOWSCRIPT_BY_NAME(FLD_PC_GET_RESHND, 0);
	float reshnd = GET_RETURN_INT();

	RUN_FLOWSCRIPT_BY_NAME(FLD_MODEL_SET_TRANSLATE,reshnd, position.x, position.y, position.z, 0);
	RUN_FLOWSCRIPT_BY_NAME(FLD_MODEL_SYNC_TRANSLATE,reshnd);
}

void QuickPanel::CallEvent()
{
	RUN_FLOWSCRIPT_BY_NAME(CALL_EVENT, (float)eventParams.majorID, (float)eventParams.minorID);
}

void QuickPanel::CallField()
{
	RUN_FLOWSCRIPT_BY_NAME(CALL_FIELD, (float)fieldParams.majorID, (float)fieldParams.minorID, (float)fieldParams.entranceMajorID,
		(float)fieldParams.entranceMajorID);
}

// Noclip
void NoclipSettings::NoclipKeyStateOn(WPARAM wParam)
{
	auto* keyState = &quickPanel->keyState;

	if (wParam == 'W')
		*keyState |= KeyPressed::FORWARD;
	else if (wParam == 'S')
		*keyState |= KeyPressed::BACKWARD;
	else if (wParam == 'A')
		*keyState |= KeyPressed::LEFT;
	else if (wParam == 'D')
		*keyState |= KeyPressed::RIGHT;
	else if (wParam == VK_SPACE)
		*keyState |= KeyPressed::UP;
	else if (wParam == VK_CONTROL)
		*keyState |= KeyPressed::DOWN;
}

void NoclipSettings::NoclipKeyStateOff(WPARAM wParam)
{
	auto* keyState = &quickPanel->keyState;

	if (wParam == 'W')
		*keyState &= ~KeyPressed::FORWARD;
	else if (wParam == 'S')
		*keyState &= ~KeyPressed::BACKWARD;
	else if (wParam == 'A')
		*keyState &= ~KeyPressed::LEFT;
	else if (wParam == 'D')
		*keyState &= ~KeyPressed::RIGHT;
	else if (wParam == VK_SPACE)
		*keyState &= ~KeyPressed::UP;
	else if (wParam == VK_CONTROL)
		*keyState &= ~KeyPressed::DOWN;
}

void NoclipSettings::NoclipLogic()
{

}