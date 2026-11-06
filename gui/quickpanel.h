#pragma once
#include "panels.h"
#include "flowscriptpanel.h"
#include "vectormath.h"

struct NoclipSettings
{
	bool enabled = false;
	float speed = 500;

	void NoclipKeyStateOn(const WPARAM wParam);
	void NoclipKeyStateOff(const WPARAM wParam);
	void NoclipLogic();
	void Toggle();
};
IMGUI_REFLECT(NoclipSettings,speed);

enum KeyPressed : BYTE
{
	FORWARD = 1 << 0,
	BACKWARD = 1 << 1,
	LEFT = 1 << 2,
	RIGHT = 1 << 3,
	UP = 1 << 4,
	DOWN = 1 << 5
};

enum QuickSelection
{
	QUICK_FUNCTIONS,
	RANDOMIZER,
	TELEPORT,
	NOCLIP,
	FREECAM
};

struct CallFieldParams
{
	int majorID = 0;
	int minorID = 0;
	int entranceMajorID = 0;
	int entranceMinorID = 0;
};
IMGUI_REFLECT(CallFieldParams, majorID, minorID, entranceMajorID, entranceMinorID);

struct CallEventParams
{
	int majorID = 0;
	int minorID = 0;
};
IMGUI_REFLECT(CallEventParams, majorID, minorID);

struct FreeCamCoord
{
	std::string title = "CameraCoord";
	Vector3 position;
	Euler angle;
	Quaternion qAngle;

	std::string GetTextInfo()
	{
		std::string textInfo;
		textInfo = std::format("Title: {}\nPosition: {}\nEuler Angle: {}\nQuaternion Angle {}\n--------------------------------------------------------------------------------------\r\n"
			,title, position, angle, qAngle);

		return textInfo;
	}
};
IMGUI_REFLECT(FreeCamCoord, title, position, angle, qAngle);

struct FreeCamSettings
{
	std::vector<FreeCamCoord> freeCamCoords;
	bool enabled = false;
	float speed = 300;
	float sensitivity = 0.25f;

	Euler angles = {0,0,0};

	void FreeCamLogic();

	void Toggle()
	{
		static auto fldCameraLock = GET_FLOWSCRIPT(FLD_CAMERA_LOCK);
		static auto fldCameraUnlock = GET_FLOWSCRIPT(FLD_CAMERA_UNLOCK);

		static const auto fldCameraPitch = GET_FLOWSCRIPT(FLD_CAMERA_GET_PITCH);
		static const auto fldCameraYaw = GET_FLOWSCRIPT(FLD_CAMERA_GET_YAW);
		static const auto fldCameraRoll = GET_FLOWSCRIPT(FLD_CAMERA_GET_ROLL);

		enabled = !enabled;

		if (enabled)
		{
			RUN_FLOWSCRIPT(fldCameraLock);

			RUN_FLOWSCRIPT(fldCameraYaw);
			angles.yaw = GET_RETURN_FLOAT();
			RUN_FLOWSCRIPT(fldCameraPitch);
			angles.pitch = GET_RETURN_FLOAT();
			RUN_FLOWSCRIPT(fldCameraRoll);
			angles.roll = GET_RETURN_FLOAT();
		}
		else
			RUN_FLOWSCRIPT(fldCameraUnlock);
	}

	void SaveCurrentCoordinates(const std::string& label)
	{
		// Position
		static const auto fldCameraPosX = GET_FLOWSCRIPT(FLD_CAMERA_GET_X_POS);
		static const auto fldCameraPosY = GET_FLOWSCRIPT(FLD_CAMERA_GET_Y_POS);
		static const auto fldCameraPosZ = GET_FLOWSCRIPT(FLD_CAMERA_GET_Z_POS);

		// Euler
		static const auto fldCameraPitch = GET_FLOWSCRIPT(FLD_CAMERA_GET_PITCH);
		static const auto fldCameraYaw = GET_FLOWSCRIPT(FLD_CAMERA_GET_YAW);
		static const auto fldCameraRoll = GET_FLOWSCRIPT(FLD_CAMERA_GET_ROLL);

		// Quaternion
		static const auto fldCameraRotX = GET_FLOWSCRIPT(FLD_CAMERA_GET_X_ROT);
		static const auto fldCameraRotY = GET_FLOWSCRIPT(FLD_CAMERA_GET_Y_ROT);
		static const auto fldCameraRotZ = GET_FLOWSCRIPT(FLD_CAMERA_GET_Z_ROT);
		static const auto fldCameraRotW = GET_FLOWSCRIPT(FLD_CAMERA_GET_W_ROT);

		Vector3 position;
		Euler angle;
		Quaternion qAngle;

		RUN_FLOWSCRIPT(fldCameraPosX);
		position.x = GET_RETURN_FLOAT();
		RUN_FLOWSCRIPT(fldCameraPosY);
		position.y = GET_RETURN_FLOAT();
		RUN_FLOWSCRIPT(fldCameraPosZ);
		position.z = GET_RETURN_FLOAT();

		RUN_FLOWSCRIPT(fldCameraYaw);
		angle.yaw = GET_RETURN_FLOAT();
		RUN_FLOWSCRIPT(fldCameraPitch);
		angle.pitch = GET_RETURN_FLOAT();
		RUN_FLOWSCRIPT(fldCameraRoll);
		angle.roll = GET_RETURN_FLOAT();

		RUN_FLOWSCRIPT(fldCameraRotX);
		qAngle.x = GET_RETURN_FLOAT();
		RUN_FLOWSCRIPT(fldCameraRotY);
		qAngle.y = GET_RETURN_FLOAT();
		RUN_FLOWSCRIPT(fldCameraRotZ);
		qAngle.z = GET_RETURN_FLOAT();
		RUN_FLOWSCRIPT(fldCameraRotW);
		qAngle.w = GET_RETURN_FLOAT();

		freeCamCoords.push_back({label, position, angle, qAngle });
	}

	std::string GetAllCoordsText()
	{
		std::string textInfo;

		for (auto& coords : freeCamCoords)
		{
			textInfo += coords.GetTextInfo();
		}

		return textInfo;
	}
};
IMGUI_REFLECT(FreeCamSettings, speed, sensitivity);

class QuickPanel : Panel
{
public:
	QuickPanel() { this->label = "Quick"; this->childLabel = "Options"; this->refreshable = false; }
	static QuickPanel* GetInstance() { static QuickPanel* instance = new QuickPanel(); return instance; }

	void RenderLogic() override;
	void InspectorLogic() override;
	//void ScanValues() override;
	//void ApplyChanges() override;
	//void Refresh() override;

	NoclipSettings noclip;
	FreeCamSettings freecam;
	BYTE keyState;
	Vector3 teleport;

	CallFieldParams fieldParams;
	CallEventParams eventParams;

	void SavePosition();
	void Teleport(const Vector3& position);
	float GetDeltaTime();

	void CallEvent();
	void CallField();
};
static QuickPanel* quickPanel = QuickPanel::GetInstance();