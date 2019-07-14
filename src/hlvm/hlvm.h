#pragma once

typedef struct {
    int LuaRefKey;
    char* Name;
} SimulationElement;

void InitializeHLVM(void);

SimulationElement* CreateSimulationElement(char* name);
void HLVMPush(SimulationElement* sim);
void HLVMProcess(void);
