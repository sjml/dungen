#include "../stdafx.h"

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>

void InitializePlatform() {
#ifdef _DEBUG
	SetCurrentDirectoryA("..\\..\\..");
#else
	if (SetCurrentDirectoryA(".\\Resources") == 0) {
		fprintf(stderr, "ERROR: Could not find Resources directory. Exiting.\n");
		exit(EXIT_FAILURE);
	}
#endif
}

void FinalizePlatform() {
}
