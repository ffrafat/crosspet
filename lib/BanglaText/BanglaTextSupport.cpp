#include "BanglaTextSupport.h"
#include "BanglaTextUtility.hpp"
#include "BTGlyphDecoder.hpp"
#include "BanglaMNRegular_25pt.h"
#include "GfxRenderer.h"

bool isBangla(uint32_t cp) {
    return (cp >= 0x0980 && cp <= 0x09FF);
}

int measureBanglaRunWidth(const std::string& banglaText) {
    BTfont* font = (BTfont*)&BanglaMNRegular_25pt;
    BanglaTextUtility helper(font);
    BTGlyphDecoder decoder(&helper);
    std::vector<int16_t> indices = decoder.getGlyphIndexArrayFromText(banglaText);

    int width = 0;
    for (uint8_t i = 0; i < (uint8_t)indices.size(); ++i) {
        BTGlyph info = readStructFromProgmem(font->glyphArray, indices[i]);
        width += info.xAdvance;
    }
    return width;
}

void renderBanglaRun(const GfxRenderer& renderer, const std::string& banglaText, int* x, int yPos, bool black) {
    BTfont* font = (BTfont*)&BanglaMNRegular_25pt;
    BanglaTextUtility helper(font);
    BTGlyphDecoder decoder(&helper);
    std::vector<int16_t> indices = decoder.getGlyphIndexArrayFromText(banglaText);

    int16_t currentX = *x;
    int16_t currentY = yPos; // Baseline

    for (uint8_t i = 0; i < (uint8_t)indices.size(); ++i) {
        BTGlyph info = readStructFromProgmem(font->glyphArray, indices[i]);
        BTOffset off = {0, 0};

        if (i > 0) {
            std::string markbaseIndex = std::to_string(indices[i]) + "," + std::to_string(indices[i - 1]);
            off = helper.findMarkBaseOffset(markbaseIndex);
        }

        int16_t xa = info.xAdvance;
        int16_t xo = off.x;
        int16_t yo = off.y;

        int16_t x0 = currentX + xo + info.xOffset;
        int16_t y0 = currentY + yo + info.yOffset;

        uint16_t bitmap_start = info.bitmapOffset;
        BTGlyph info_next = readStructFromProgmem(font->glyphArray, indices[i] + 1);
        uint16_t bitmap_end = info_next.bitmapOffset - 1;

        uint16_t bitcount = 1;
        int16_t row = 0;
        int16_t column = 0;

        for (uint16_t byteIdx = bitmap_start; byteIdx <= bitmap_end; byteIdx++) {
            int byte = readByteFromProgmem(font->bitmap, byteIdx);
            for (int j = 7; j >= 0; j--) {
                int bit = byte & (1 << j);
                if (bit) {
                    renderer.drawPixel(x0 + column, y0 + row, black);
                }
                column++;
                if (bitcount % info.width == 0) {
                    row++;
                    column = 0;
                }
                bitcount++;
            }
        }
        currentX += xa;
    }
    *x = currentX;
}
