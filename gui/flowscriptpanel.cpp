#include "flowscriptpanel.h"
#include "signaturescan.h"

void FlowScriptPanel::ScanValues()
{
	std::string pattern;

	pattern = "4C 8D 3D ?? ?? ?? ?? 8B F5";
	DWORD_PTR flowStart = PatternScan(GetModuleHandle(NULL), pattern);
	flowStart = GetAddressFromGlobalRef(flowStart);

	pattern = "4C 8B 05 ?? ?? ?? ?? 41 8B 50 ?? 29 CA";
	DWORD_PTR flowFuncPtr = PatternScan(GetModuleHandle(NULL), pattern);
	flowFuncPtr = GetAddressFromGlobalRef(flowFuncPtr);
	flowFunction = (FlowFileFunction**)flowFuncPtr;

	flowCategories = (std::array<FlowCategory,6>*)flowStart;
}

void FlowScriptPanel::RenderLogic()
{
	SearchablePanel::RenderLogic();

	ImGui::BeginTabBar(this->label.c_str());

	for (int i = 0; i < flowCategories->size(); i++)
	{
		const char* categoryLabel = flowCategoryNames[i];
		if (ImGui::BeginTabItem(categoryLabel))
		{
			if (previouslySelected != i)
			{
				selectedIndex = 0;
				previouslySelected = i;

				flowVector.clear();

				size_t newSize = flowCategories->at(i).size;
				flowVector.resize(newSize);
				FlowScript* memoryStart = (FlowScript*)flowCategories->at(i).categoryStart;
				std::memcpy(flowVector.data(), memoryStart, flowVector.size() * sizeof(FlowScript));
			}

			for (int j = 0; j < flowVector.size(); j++)
			{
				auto& flowObject = flowVector[j];
				std::string label = flowObject.name == nullptr ? "Empty" : flowObject.name;

				if (!TextMatch(searchText, label))
					continue;

				label = std::format("{} | {}", j, label);

				ImGui::PushID(j);
				if (ImGui::Button(label.c_str()))
				{
					selectedIndex = j;
					currentParams.clear();
					currentParams.resize(flowVector[j].paramCount);
				}
				ImGui::PopID();
			}

			ImGui::EndTabItem();
		}
	}

	ImGui::EndTabBar();
}

void FlowScriptPanel::InspectorLogic()
{
	auto& flowObject = flowVector[selectedIndex];
	std::string label = flowObject.name == nullptr ? "Empty" : flowObject.name;
	
	ImGui::LabelText("##label", label.c_str());
	ImGui::Separator();
	for (int i = 0; i < currentParams.size(); i++)
	{
		std::string label = std::format("Param{}", i);
		ImGui::InputFloat(label.c_str(), &currentParams[i], 0, 0, "%g");
	}

	if (ImGui::Button("Run"))
	{
		RunFunction();
	}

	ImGui::LabelText("##retval","Last Returned Value");
	ImGui::LabelText("As Float", std::to_string(GET_RETURN_FLOAT()).c_str());
	ImGui::LabelText("As Integer", std::to_string(GET_RETURN_INT()).c_str());
}

void FlowScriptPanel::RunFunction()
{
	auto* flowObject = &flowVector[selectedIndex];
	RunFlowScript(flowObject, currentParams);
}

FlowScript* FlowScriptPanel::GetFunctionByName(const char* name)
{
	for (int i = 0; i < flowCategories->size(); i++)
	{
		auto* category = &flowCategories->at(i);
		for (int j = 0; j < category->size; j++)
		{
			FlowScript* function = (FlowScript*)((intptr_t)category->categoryStart + j * sizeof(FlowScript));
			if (std::string_view(function->name) == name)
			{
				return function;
			}
		}
	}

	printf("[P5R Mod Editor] GetFunctionByName returned nullptr!\n");
	return nullptr;
}

void FlowScriptPanel::RunFlowScriptFunctionByName(const char* name, std::vector<float> args)
{
	FlowScript* flowObject = GetFunctionByName(name);

	if (flowObject == nullptr)
		return;

	RunFlowScript(flowObject, args);
}

void FlowScriptPanel::RunFlowScript(FlowScript* flowObject, std::vector<float> args)
{
	if (flowObject == nullptr)
		return;

	if (args.size() > 8)
	{
		printf("[P5R Mod Editor] RunFlowScript: Too many arguments!");
		return;
	}

	modMenuFlowScript->parameters.fill(0);

	for (int i = 0; i < args.size(); i++)
	{
		int j = 15 - (i * 2);
		modMenuFlowScript->parameters.at(j) = args[i];
	}

	*flowFunction = modMenuFlowScript;
	runByModMenu = true;
	flowObject->Function();
	runByModMenu = false;
	*flowFunction = nullptr;
}