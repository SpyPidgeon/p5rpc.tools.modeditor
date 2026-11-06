#include "guistructs.h"
#include "panels.h"
//--------------------------------------------------
// Global Functions
//--------------------------------------------------
std::string GetDLLPath(const std::string& path)
{
	char dllPath[MAX_PATH] = { 0 };
	GetModuleFileNameA(dll_handle, dllPath, MAX_PATH);

	std::string fullPath(dllPath);

	size_t lastSlash = fullPath.find_last_of("\\/");
	std::string baseDirectory = (lastSlash != std::string::npos) ? fullPath.substr(0, lastSlash + 1) : "";

	std::ostringstream ss;
	ss << baseDirectory << path;

	return ss.str();
}

void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

bool CopyToClipboard(const std::string& text)
{
	if (!OpenClipboard(NULL))
	{
		printf("[P5R Mod Editor] Failed to open clipboard.\n");
		return false;
	}

	EmptyClipboard();

	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
	if (!hMem)
	{
		printf("[P5R Mod Editor] Failed to allocate global memory for clipboard.\n");
		CloseClipboard();
		return false;
	}

	char* pMem = static_cast<char*>(GlobalLock(hMem));
	if (!pMem)
	{
		int err = GetLastError();
		GlobalFree(pMem);
		printf("[P5R Mod Editor] Failed to save to clipboard. Error Code: 0x%x\n", err);
		return false;
	}

	memcpy(pMem, text.c_str(), text.size() + 1);
	GlobalUnlock(hMem);

	if (!SetClipboardData(CF_TEXT, hMem))
	{
		int err = GetLastError();
		GlobalFree(hMem);
		CloseClipboard();
		printf("[P5R Mod Editor] Failed to save to clipboard. Error Code: 0x%x\n", err);
		return false;
	}

	CloseClipboard();

	return true;
}