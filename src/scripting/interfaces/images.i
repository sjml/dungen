%module dungen
%{
    #include "../infrastructure/images.h"
%}

void LoadColorTable(const char* tablename, const char* filename);
gbVec3 GetColorByIndex(const char* tablename, int i);
gbVec3 GetColorByPercent(const char* tablename, float p);
