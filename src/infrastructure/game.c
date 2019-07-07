#include "../stdafx.h"
#include "game.h"

#include <time.h>

#include "util.h"

double MAX_TIMESTEP = 1.0;

double previousTime, currentTime;

void InitializeGame(void) {
    srand((unsigned int)time(NULL));
    
    previousTime = glfwGetTime();
}

void FinalizeGame(void) {
    
}

void GameTick(void) {
    currentTime = glfwGetTime();
//    double dt = clampd(currentTime - previousTime, 0.0, MAX_TIMESTEP);
}
