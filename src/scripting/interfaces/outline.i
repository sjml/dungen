%module dungen
%{
    #include "../infrastructure/outline.h"
%}

Outline* CreateOutline(TileSet* ts);
void UpdateOutline(Outline* o, TileSet* ts);
void DestroyOutline(Outline* o);
