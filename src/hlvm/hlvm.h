#pragma once

typedef struct {
    int LuaRefKey;
    const char* Name;
} SimulationElement;

void InitializeHLVM(void);

SimulationElement* CreateSimulationElement(const char* name);
void HLVMPush(SimulationElement* sim);
SimulationElement* HLVMPop(void);
void HLVMProcess(void);
