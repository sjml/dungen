%module dungen
%{
    #include "../infrastructure/game.h"
%}

void QuitGame(const char* message);

double GetTime(void);

void SeedRandomString(const char* seedString);
void SeedRandom(uint64_t seed, uint64_t seq);
const char* GetRandomSeed();
float RandomRangeFloat(float min, float max);
int RandomRangeInt(int min, int max);
