#pragma once

#include <string>
#include <cstdint>

class GfxRenderer;

bool isBangla(uint32_t cp);
int measureBanglaRunWidth(const std::string& banglaText);
void renderBanglaRun(const GfxRenderer& renderer, const std::string& banglaText, int* x, int yPos, bool black);
