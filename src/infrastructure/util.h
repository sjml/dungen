#pragma once

double clampd(double d, double min, double max);
float clampf(float f, float min, float max);

float lerpf(float f1, float f2, float amount);
double lerpd(double f1, double f2, double amount);

int handleGLErrors(const char* f, const int line);

