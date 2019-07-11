%module dungen
%{
    #include "../hlvm/hlvm.h"
%}

typedef struct {
    int LuaRefKey;
    const char* Name;
} SimulationElement;

SimulationElement* CreateSimulationElement(const char* name);
void HLVMPush(SimulationElement* sim);
SimulationElement* HLVMPop(void);
