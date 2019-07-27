#include "../stdafx.h"
#include "text.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#include <stb_easy_font.h>
#pragma clang diagnostic pop

#include <stb_rect_pack.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wcomma"
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include <stb_truetype.h>
#pragma clang diagnostic pop



void InitializeText() {

}

void FinalizeText() {

}

void PrintTextString(char *text, gbVec2* pos, gbVec4* color, float scale, float spacing) {

}
