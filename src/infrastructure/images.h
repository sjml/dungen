#pragma once


unsigned char* GetRawImageData(const char* filename, unsigned int *dataLength, int *texWidth, int *texHeight);
void LoadColorTable(const char* tablename, const char* filename);
gbVec3 GetColorByIndex(const char* tablename, int i);
gbVec3 GetColorByPercent(const char* tablename, float p);
