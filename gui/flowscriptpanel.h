#pragma once
#include "panels.h"

struct FlowCategory
{
	void* categoryStart;
	uint16_t size;
};

struct FlowFileFunction
{
	char label[0x2C];
	int maxParams = 8;
	std::array<BYTE, 8> rules;
	std::array<BYTE,34> data1;
	std::array<float,16> parameters;
	std::array<BYTE, 0x13B> data2;
	float returnValue = 0;
	std::array<BYTE, 0xE2> data3;

	static FlowFileFunction* GetInstance()
	{
		static FlowFileFunction* instance = new FlowFileFunction();
		const char* label = "MOD_MENU_FLOWSCRIPT";
		strcpy_s(instance->label, 20, label);

		for (int i = 0; i < instance->parameters.size(); i++)
		{
			instance->parameters.at(i) = 0;
		}

		for (int i = 0; i < instance->rules.size(); i++)
		{
			instance->rules.at(i) = 0x1;
		}

		return instance;
	}

	int GetReturnAsInt()
	{
		int data;
		std::memcpy(&data, &returnValue, sizeof(float));
		return data;
	}
};
static FlowFileFunction* modMenuFlowScript = FlowFileFunction::GetInstance();

struct FlowScript
{
	void (*Function)();
	uint8_t paramCount;
	const char* name;
};

static const std::array<const char*, 6> flowCategoryNames =
{
	"Common",
	"Field",
	"AI",
	"Social",
	"Facility",
	"Net"
};

class FlowScriptPanel : SearchablePanel
{
public:
	FlowScriptPanel() { this->label = "FlowScript"; this->childLabel = "Function"; this->refreshable = false; }
	static FlowScriptPanel* GetInstance() { static FlowScriptPanel* instance = new FlowScriptPanel(); return instance; }

	void RenderLogic() override;
	void InspectorLogic() override;
	void ScanValues() override;
	//void ApplyChanges() override;
	//void Refresh() override;
	void RunFunction();

	std::array<FlowCategory, 6>* flowCategories;
	std::vector<FlowScript> flowVector;
	std::vector<float> currentParams;
	FlowFileFunction** flowFunction;

	bool runByModMenu = false;

	int previouslySelected = -1;

	FlowScript* GetFunctionByName(const char* name);
	void RunFlowScriptFunctionByName(const char* name, std::vector<float> args);
	void RunFlowScript(FlowScript* flowObject,std::vector<float> args);
};
static FlowScriptPanel *flowPanel = FlowScriptPanel::GetInstance();

#define RUN_FLOWSCRIPT_BY_NAME(name, ...) flowPanel->RunFlowScriptFunctionByName(#name, std::vector<float>{__VA_ARGS__})
#define RUN_FLOWSCRIPT(flowObject, ...) flowPanel->RunFlowScript(flowObject,std::vector<float>{__VA_ARGS__})
#define GET_FLOWSCRIPT(name) flowPanel->GetFunctionByName(#name)
#define GET_RETURN_FLOAT() modMenuFlowScript->returnValue
#define GET_RETURN_INT() modMenuFlowScript->GetReturnAsInt()