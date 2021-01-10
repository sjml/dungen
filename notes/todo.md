## TODO
1. Rework river encounters to use centralized system
    - rename central functions
    - rivers should only backflow within chambers, but within chambers, there should be no limit on backflow (flood the chamber until an egress is found, basically, but don't go back along the path or to previous chambers)
2. Styler rework
    - can it handle a region that is both water and plague?
    - rendering of sub-regions; does that work?
    - somewhat related: checking if tile is in region should check subregions
3. Change treasure handling -- raw ore isn't treasure
    - figure out a way for river to wash an actual treasure into a next chamber
    - change visualization of gold vein to something more ore-y
4. Finish agent implementation with river as test case
    - add magma as other agent?
5. Relaxable constraint sets

## Near Term
* make webpage explaining project, linking to releases, etc.
* handle newlines in text rendering? (and fix Great Wyrm label)
* outlines flip edge alignment when regions have disjoint sections
* why is tile choice not using SetTileOutline?

## Tech Debt
* world.c could use a refactor
* note at the top of attributes.c about simplifying it
* RenderingRegions is a holdover from when they were bare TileSets; think about just making a permalist like Agents and TileData have (though TileData's is more by rendering necessity...)
* windows x64 doesn't build because I am a lazy programmer and my use of integers (esp. pointers) is not rigorously careful. macos is cool with it; windows less so. eventually this should get fixed, but for now, keeping Windows to 32-bit is fine
* typemaps should reject inputs that are not tables
* add way to modify attributes instead of having to get/set round-trip through SQLite
* overusing the stretchy buffer right now; look for more chances to just use a static array
    - also the type of the *len calls is ptrdiff_t, not necessarily long

## Horizon
* in-game console
* HLVM exceptions (C + Lua invoking)
* pull sim settings and other stuff out into config file that Lua pulls in
* expose resources directory on mac?
* history recording -- optional live captioning
    - little in-place captions "Natural caverns form with pockets of liquid hot magma" etc.
    - write all out to text and pretty formatting (or JSON+HTML renderer)
    - screenshots/video
    - name generator?
