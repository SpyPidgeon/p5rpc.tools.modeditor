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
	if (ImGui::Button("Field & Event"))
	{
		selectedIndex = QuickSelection::QUICK_FUNCTIONS;
	}

	if (ImGui::Button("Randomizer"))
	{
		selectedIndex = QuickSelection::RANDOMIZER;
	}

	if (ImGui::Button("Teleport"))
	{
		selectedIndex = QuickSelection::TELEPORT;
	}

	if (ImGui::Button("Noclip"))
	{
		selectedIndex = QuickSelection::NOCLIP;
	}

	if (ImGui::Button("Freecam"))
	{
		selectedIndex = QuickSelection::FREECAM;
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

		ImGui::BeginDisabled(true);
		ImGui::PushItemFlag(ImGuiItemFlags_ReadOnly, true);
		ImGui::Checkbox("##enabled", &noclip.enabled);
		ImGui::PopItemFlag();
		ImGui::EndDisabled();

		ImReflect::Input("Noclip", noclip);
		ImGui::TextWrapped("WASD for horizontal movement, Space/LCTRL for vertical movement. F2 shortcut to toggle.");

		if (noclip.enabled == false && keyState != 0)
			keyState = 0;
		break;
	case QuickSelection::FREECAM:
	{
		if (ImGui::Button("Toggle"))
		{
			freecam.Toggle();
		}
		ImGui::BeginDisabled(true);
		ImGui::SameLine();
		ImGui::Checkbox("##freecamtoggle", &freecam.enabled);
		ImGui::EndDisabled();

		ImReflect::Input("Freecam Settings", freecam);
		ImGui::TextWrapped("WASD controls. Does not work in events or battles. F3 Shortcut to activate.");

		ImGui::Separator();

		static std::string saveLabel = "CameraCoords";
		if (ImGui::Button("Save Coordinates"))
		{
			freecam.SaveCurrentCoordinates(saveLabel);
		}
		ImGui::InputText("Coordinate Label", &saveLabel);

		if (!freecam.freeCamCoords.empty() && ImGui::Button("Copy All Coords to Clipboard"))
		{
			CopyToClipboard(freecam.GetAllCoordsText());
		}

		if (!freecam.freeCamCoords.empty() && ImGui::CollapsingHeader("Saved Coordinates"))
		{
			if (ImGui::Button("Remove All"))
			{
				freecam.freeCamCoords.clear();
			}

			for (int i = 0; i < freecam.freeCamCoords.size(); i++)
			{
				auto& coords = freecam.freeCamCoords[i];
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader(coords.title.c_str()))
				{
					ImReflect::Input(coords.title.c_str(), coords);

					std::string buttonLabel = std::format("Teleport To {}", coords.title);

					ImGui::BeginDisabled(!freecam.enabled);
					if (ImGui::Button(buttonLabel.c_str()))
					{
						static const auto fldCameraSetPos = GET_FLOWSCRIPT(FLD_CAMERA_SET_POS);
						static const auto fldCameraSetRot = GET_FLOWSCRIPT(FLD_CAMERA_SET_ROT);

						RUN_FLOWSCRIPT(fldCameraSetPos, coords.position.x, coords.position.y, coords.position.z);
						RUN_FLOWSCRIPT(fldCameraSetRot, coords.qAngle.x, coords.qAngle.y, coords.qAngle.z, coords.qAngle.w);
					}
					if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip | ImGuiHoveredFlags_AllowWhenDisabled))
						ImGui::SetTooltip("Only works when freecam is on.");
					ImGui::EndDisabled();

					if (ImGui::Button("Copy This To Clipboard"))
					{
						if (!CopyToClipboard(coords.GetTextInfo()))
						{
							continue;
						}
					}

					if (ImGui::Button("Remove This"))
					{
						std::swap(coords, freecam.freeCamCoords.back());
						freecam.freeCamCoords.pop_back();
					}
				}
				ImGui::PopID();
			}
		}

		break;
	}
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

		static bool encountersShuffled = false;
		static std::vector<EnemyEncounter> encounterBytes(encounterPanel->enemyEncounters.begin(), encounterPanel->enemyEncounters.end());

		if (ImGui::Button("Randomize Encounters"))
		{
			encountersShuffled = true;
			encounterPanel->RandomizeEncounters(encounterSettings, currentHash);
		}

		if (encountersShuffled)
		{
			if (ImGui::Button("Unrandomize Encounters"))
			{
				encountersShuffled = false;
				std::memcpy(encounterPanel->enemyEncounters.data(), encounterBytes.data(), sizeof(encounterPanel->enemyEncounters));
				encounterPanel->ApplyChanges();
			}
		}

		ImGui::Separator();
		ImGui::Spacing();

		ImGui::SeparatorText("Skill");
		ImGui::TextWrapped("This only shuffles active skills, it skips passives and null skills and does not change the particle effects.");
		ImGui::Spacing();

		static bool skillsShuffled = false;
		static std::vector<ActiveSkill> skillBytes(skillPanel->activeSkillArray.begin(),skillPanel->activeSkillArray.end());
		static std::vector<SkillElement> elementBytes(skillPanel->skillElementArray.begin(), skillPanel->skillElementArray.end());

		if (ImGui::Button("Randomize Skills"))
		{
			skillsShuffled = true;
			skillPanel->RandomizeSkills(currentHash);
		}

		if (skillsShuffled)
		{
			if (ImGui::Button("Unrandomize Skills"))
			{
				skillsShuffled = false;
				std::memcpy(skillPanel->activeSkillArray.data(), skillBytes.data(), sizeof(skillPanel->activeSkillArray));
				std::memcpy(skillPanel->skillElementArray.data(), elementBytes.data(), sizeof(skillPanel->skillElementArray));
				skillPanel->ApplyChanges();
			}
		}

		ImGui::Separator();
		ImGui::Spacing();

		ImGui::SeparatorText("Item");

		static bool randomizeItems = false;
		ImGui::Checkbox("Randomize Consumable Skill", &randomizeItems);

		static bool itemsRandomized = false;
		static std::vector<Consumable> itemBytes(itemPanel->items.consumables.begin(), itemPanel->items.consumables.end());

		if (ImGui::Button("Randomize Consumables"))
		{
			itemsRandomized = true;
			itemPanel->RandomizeConsumables(currentHash, randomizeItems);
		}

		if (itemsRandomized)
		{
			if (ImGui::Button("Unrandomize Items"))
			{
				itemsRandomized = false;
				std::memcpy(itemPanel->items.consumables.data(), itemBytes.data(), sizeof(itemPanel->items.consumables));
			}
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
		(float)fieldParams.entranceMinorID);
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
	enabled = !enabled;
}

using std::to_underlying;
using std::chrono::steady_clock;

float QuickPanel::GetDeltaTime()
{
	static auto last = steady_clock::now();
	auto now = steady_clock::now();

	auto dt = now - last;
	float deltaTime = std::chrono::duration_cast<std::chrono::seconds>(dt).count();
	deltaTime = std::min(deltaTime, 0.05f);

	return deltaTime;
}

void NoclipSettings::NoclipLogic()
{
	float deltaTime = quickPanel->GetDeltaTime();

	auto keyState = quickPanel->keyState;

	static constexpr int interp = 1;

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

	Vector3 finalVector = Vector3(0,0,0);

	if (keyState & to_underlying(KeyPressed::FORWARD))
	{
		finalVector.x += forward.x;
		finalVector.z += forward.z;
	}
	if (keyState & to_underlying(KeyPressed::BACKWARD))
	{
		finalVector.x -= forward.x;
		finalVector.z -= forward.z;
	}
	if (keyState & to_underlying(KeyPressed::LEFT))
	{
		finalVector.x += left.x;
		finalVector.z += left.z;
	}
	if (keyState & to_underlying(KeyPressed::RIGHT))
	{
		finalVector.x -= left.x;
		finalVector.z -= left.z;
	}
	if (keyState & to_underlying(KeyPressed::UP))
	{
		finalVector.y += verticalSpeed;
	}
	if (keyState & to_underlying(KeyPressed::DOWN))
	{
		finalVector.y -= verticalSpeed;
	}

	RUN_FLOWSCRIPT(fldModelAddTranslate,(float)player, finalVector.x, finalVector.y, finalVector.z, (float)interp);
	RUN_FLOWSCRIPT(fldModelSync, (float)player);
}

void FreeCamSettings::FreeCamLogic()
{
	if (ImGui::GetCurrentContext() == nullptr)
		return;

	auto keyState = quickPanel->keyState;
	float deltaTime = quickPanel->GetDeltaTime();

	// Quaternion
	static const auto fldCameraRotX = GET_FLOWSCRIPT(FLD_CAMERA_GET_X_ROT);
	static const auto fldCameraRotY = GET_FLOWSCRIPT(FLD_CAMERA_GET_Y_ROT);
	static const auto fldCameraRotZ = GET_FLOWSCRIPT(FLD_CAMERA_GET_Z_ROT);
	static const auto fldCameraRotW = GET_FLOWSCRIPT(FLD_CAMERA_GET_W_ROT);

	// Position
	static const auto fldCameraPosX = GET_FLOWSCRIPT(FLD_CAMERA_GET_X_POS);
	static const auto fldCameraPosY = GET_FLOWSCRIPT(FLD_CAMERA_GET_Y_POS);
	static const auto fldCameraPosZ = GET_FLOWSCRIPT(FLD_CAMERA_GET_Z_POS);

	static const auto fldCameraSetPos = GET_FLOWSCRIPT(FLD_CAMERA_SET_POS);
	static const auto fldCameraSetRot = GET_FLOWSCRIPT(FLD_CAMERA_SET_ROT);

	auto viewport = ImGui::GetMainViewport();
	int centerX = viewport->Size.x / 2;
	int centerY = viewport->Size.y / 2;

	POINT lastMouseCoords = { centerX,centerY };
	POINT newMouseCoords;
	GetCursorPos(&newMouseCoords);

	POINT deltaMouseCoords;
	deltaMouseCoords.x = newMouseCoords.x - lastMouseCoords.x;
	deltaMouseCoords.y = newMouseCoords.y - lastMouseCoords.y;

	lastMouseCoords = newMouseCoords;

	SetCursorPos(centerX,centerY);

	Vector3 currentCoords;
	RUN_FLOWSCRIPT(fldCameraPosX);
	currentCoords.x = GET_RETURN_FLOAT();
	RUN_FLOWSCRIPT(fldCameraPosY);
	currentCoords.y = GET_RETURN_FLOAT();
	RUN_FLOWSCRIPT(fldCameraPosZ);
	currentCoords.z = GET_RETURN_FLOAT();

	angles.yaw -= deltaMouseCoords.x * sensitivity;
	angles.pitch -= deltaMouseCoords.y * sensitivity;

	angles.pitch = std::clamp(angles.pitch, -89.0f, 89.0f);

	Euler currentAngle = angles;

	currentAngle.yaw *= IM_PI / 180;
	currentAngle.pitch *= IM_PI / 180;
	currentAngle.roll *= IM_PI / 180;

	Vector3 forward =
	{
		std::sinf(currentAngle.yaw) * std::cosf(currentAngle.pitch),
		-std::sinf(currentAngle.pitch),
		std::cosf(currentAngle.yaw) * std::cosf(currentAngle.pitch)
	};

	forward = -forward;

	Vector3 left =
	{
		-std::cosf(currentAngle.yaw) * std::cosf(currentAngle.pitch),
		0,
		std::sinf(currentAngle.yaw) * std::cosf(currentAngle.pitch)
	};

	forward.x *= speed * deltaTime;
	forward.y *= speed * deltaTime;
	forward.z *= speed * deltaTime;

	left.x *= speed * deltaTime;
	left.y *= speed * deltaTime;
	left.z *= speed * deltaTime;

	if (keyState & to_underlying(KeyPressed::FORWARD))
		currentCoords += forward;
	if (keyState & to_underlying(KeyPressed::BACKWARD))
		currentCoords -= forward;
	if (keyState & to_underlying(KeyPressed::LEFT))
		currentCoords += left;
	if (keyState & to_underlying(KeyPressed::RIGHT))
		currentCoords -= left;

	RUN_FLOWSCRIPT(fldCameraSetPos, currentCoords.x, currentCoords.y, currentCoords.z);

	if (deltaMouseCoords.x == 0 && deltaMouseCoords.y == 0)
		return;

	Quaternion cameraQuat = Quaternion::EulerToQuat(currentAngle.yaw, currentAngle.pitch, 0.0f);


	if (cameraQuat.w < 0.0f)
		cameraQuat = -cameraQuat;

	RUN_FLOWSCRIPT(fldCameraSetRot, cameraQuat.x, cameraQuat.y, cameraQuat.z, cameraQuat.w);
}