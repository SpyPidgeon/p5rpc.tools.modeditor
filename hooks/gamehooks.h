#pragma once
#include <cstdint>
#include "guistructs.h"
#include "displaystructwindow.h"
#include "skillstructs.h"
#include "battlestructs.h"
#include "filestructs.h"
#include "encounterstructs.h"
#include "panels.h"

typedef DatUnit* (__stdcall* GetDatUnitByID)(uint16_t ID);
extern GetDatUnitByID GetDatUnit;

typedef void(__stdcall* MouseState)();
extern MouseState oMouseState;
void MouseStateHook();

typedef void(__stdcall* FldPCMoveUpdate)(uintptr_t param1, float param2);
extern FldPCMoveUpdate oMoveUpdate;
void FldPCMoveUpdateHook(uintptr_t param1, float param2);