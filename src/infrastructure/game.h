#pragma once

void InitializeGame(const char* startupElement);
void FinalizeGame(void);

int GameTick(void);

double GetTime(void);

void SeedRandomString(const char* seed);
void SeedRandom(uint64_t seed, uint64_t seq);
const char* GetRandomSeed(void);
float RandomRangeFloat(float min, float max);
int RandomRangeInt(int min, int max);

#if !(DUNGEN_MOBILE)
    void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
    void MouseClickCallback(GLFWwindow* window, int button, int action, int mods);
    void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
#else
    void SetTime(double gameTime);
    void TouchCallback(double xpos, double ypos, int isDown);
    void TouchMoveCallback(double xpos, double ypos);
#endif // !(DUNGEN_MOBILE)
