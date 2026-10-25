#include "gamehooks.h"
#include "flowscriptpanel.h"

GetDatUnitByID GetDatUnit;
FldPCMoveUpdate oMoveUpdate;

std::string GetNameFromBinary(const uint32_t currentIndex, const uintptr_t nameAddress)
{
	unsigned int j = 0;
	unsigned int nameOffset = 0;

	while (j < currentIndex)
	{
		if (*reinterpret_cast<BYTE*>(nameAddress + nameOffset) == 0x00)
		{
			j++;
		}
		nameOffset++;
	}

	std::string binaryName;

	while (*reinterpret_cast<BYTE*>(nameAddress + nameOffset) != 0x00)
	{
		binaryName.push_back((char)*reinterpret_cast<char*>(nameAddress + nameOffset));
		nameOffset++;
	}

	return binaryName;
}

MouseState oMouseState;
extern bool fetchContext;
void MouseStateHook()
{
	if (fetchContext)
		return;

	oMouseState();
}

extern bool lockControls;
void FldPCMoveUpdateHook(uintptr_t param1, float param2)
{
	if (lockControls)
		return;

	oMoveUpdate(param1,param2);
}