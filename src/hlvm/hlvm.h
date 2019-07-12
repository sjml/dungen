#pragma once

typedef struct {
    int LuaRefKey;
    const char* Name;
} SimulationElement;

void InitializeHLVM(void);

SimulationElement* CreateSimulationElement(const char* name);
void HLVMPush(SimulationElement* sim);
void HLVMProcess(void);
