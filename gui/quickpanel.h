#pragma once
#include "panels.h"
#include "flowscriptpanel.h"
#include "vectormath.h"

struct NoclipSettings
{
	bool enabled = false;
	float speed = 10;

	void NoclipKeyStateOn(WPARAM wParam);
	void NoclipKeyStateOff(WPARAM wParam);
	void NoclipLogic();
};
IMGUI_REFLECT(NoclipSettings,enabled,speed);

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
	TELEPORT,
	NOCLIP,
	QUICK_FUNCTIONS,
	RANDOMIZER
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

class QuickPanel : Panel
{
public:
	QuickPanel() { this->label = "Quick"; this->childLabel = "Options"; }
	static QuickPanel* GetInstance() { static QuickPanel* instance = new QuickPanel(); return instance; }

	void RenderLogic() override;
	void InspectorLogic() override;
	//void ScanValues() override;
	//void ApplyChanges() override;
	//void Refresh() override;

	NoclipSettings noclip;
	BYTE keyState;
	Vector3 teleport;

	CallFieldParams fieldParams;
	CallEventParams eventParams;

	void SavePosition();
	void Teleport(const Vector3& position);

	void CallEvent();
	void CallField();
};
static QuickPanel* quickPanel = QuickPanel::GetInstance();