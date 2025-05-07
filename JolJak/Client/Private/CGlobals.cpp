#include "pch.h"
#include "CGlobals.h"

std::atomic<int> g_PlayerID = -1;
std::atomic<bool> g_ServerConnected = false;

std::atomic<float> px = 0.f;
std::atomic<float> py = 0.f;
std::atomic<float> pz = 0.f;
