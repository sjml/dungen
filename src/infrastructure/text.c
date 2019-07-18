#include "text.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#include <stb_easy_font.h>
#pragma clang diagnostic pop


void PrintTextString(char *text, float x, float y, gbVec3* color, float scale, float spacing) {
    static char buffer[99999]; // ~500 chars
    int num_quads;
    
    stb_easy_font_spacing(spacing);
    num_quads = stb_easy_font_print(x, y, text, NULL, buffer, sizeof(buffer));

    glColor3fv(color->e);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        glLoadIdentity();
        glScalef(scale, scale, 1.0f);
        glVertexPointer(2, GL_FLOAT, 16, buffer);
        glDrawArrays(GL_QUADS, 0, num_quads*4);
    glPopMatrix();
}
