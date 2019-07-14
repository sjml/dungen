%module dungen
%{
    #include "../hlvm/hlvm.h"
%}

typedef struct {
    int LuaRefKey;
    char* Name;
} SimulationElement;

SimulationElement* CreateSimulationElement(char* name);
void HLVMPush(SimulationElement* sim);
SimulationElement* HLVMPop(void);
