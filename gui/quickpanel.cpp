#include "quickpanel.h"
#include "encounterpanel.h"
#include "skillpanel.h"
#include "itempanel.h"
#include <functional>
#include <random>
#include <chrono>
#include <cmath>

void QuickPanel::RenderLogic()
{
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

	if (ImGui::Button("Noclip"))
	{
		selectedIndex = QuickSelection::NOCLIP;
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

		if (ImGui::Button("Toggle"))
		{
			noclip.Toggle();
		}
		ImGui::SameLine();

		ImGui::PushItemFlag(ImGuiItemFlags_ReadOnly, true);
		ImGui::Checkbox("##enabled", &noclip.enabled);
		ImGui::PopItemFlag();

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
void NoclipSettings::NoclipKeyStateOn(const WPARAM wParam)
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

void NoclipSettings::NoclipKeyStateOff(const WPARAM wParam)
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

void NoclipSettings::Toggle()
{
	static auto fldPCFloating = GET_FLOWSCRIPT(FLD_PC_MODEL_SET_FLOATING);

	enabled = !enabled;
	RUN_FLOWSCRIPT(fldPCFloating, (float)enabled);
}

using std::to_underlying;
using std::chrono::steady_clock;

void NoclipSettings::NoclipLogic()
{
	static auto last = steady_clock::now();
	auto now = steady_clock::now();

	auto dt = now - last;
	float deltaTime = std::chrono::duration_cast<std::chrono::seconds>(dt).count();
	deltaTime = std::min(deltaTime, 0.05f);

	auto keyState = quickPanel->keyState;

	static const auto fldPCGetReshnd = GET_FLOWSCRIPT(FLD_PC_GET_RESHND);
	static const auto fldModelAddTranslate = GET_FLOWSCRIPT(FLD_MODEL_ADD_TRANSLATE);
	static const auto fldCameraYaw = GET_FLOWSCRIPT(FLD_CAMERA_GET_YAW);
	static const auto fldModelSync = GET_FLOWSCRIPT(FLD_MODEL_SYNC_TRANSLATE);

	RUN_FLOWSCRIPT(fldCameraYaw);
	float cameraYaw = GET_RETURN_FLOAT();

	RUN_FLOWSCRIPT(fldPCGetReshnd, 0);
	int player = GET_RETURN_INT();

	float radianYaw = cameraYaw * (IM_PI / 180);

	Vector3 left = Vector3(std::cosf(radianYaw),0,-std::sinf(radianYaw));
	Vector3 forward = Vector3(std::sinf(radianYaw), 0, std::cosf(radianYaw));

	forward.x *= speed * deltaTime;
	forward.z *= speed * deltaTime;

	left.x *= speed * deltaTime;
	left.z *= speed * deltaTime;

	float verticalSpeed = speed * deltaTime;

	if (keyState & to_underlying(KeyPressed::FORWARD))
	{
		RUN_FLOWSCRIPT(fldModelAddTranslate, (float)player, forward.x, 0, forward.z, 0);
	}
	if (keyState & to_underlying(KeyPressed::BACKWARD))
	{
		RUN_FLOWSCRIPT(fldModelAddTranslate, (float)player, -forward.x, 0, -forward.z, 0);
	}
	if (keyState & to_underlying(KeyPressed::LEFT))
	{
		RUN_FLOWSCRIPT(fldModelAddTranslate, (float)player, left.x, 0, left.z, 0);
	}
	if (keyState & to_underlying(KeyPressed::RIGHT))
	{
		RUN_FLOWSCRIPT(fldModelAddTranslate, (float)player, -left.x, 0, -left.z, 0);
	}
	if (keyState & to_underlying(KeyPressed::UP))
	{
		RUN_FLOWSCRIPT(fldModelAddTranslate, (float)player, 0, verticalSpeed, 0, 0);
	}
	if (keyState & to_underlying(KeyPressed::DOWN))
	{
		RUN_FLOWSCRIPT(fldModelAddTranslate, (float)player, 0, -verticalSpeed, 0, 0);
	}

	RUN_FLOWSCRIPT(fldModelSync, (float)player);
}