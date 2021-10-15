## TODO
0. to resume actual work
    - refresh on chamber progress (merging, moving contents, etc.)
        - complete test setup and execution
        - what is happening with the storage? 
        - maybe distinguish between things that take up space (like flowing water/magma) and things that only have a visual representation of a tag (like population)
    - refresh on river flowing and exploration
        - might junk and remake -- have leveled up as a programmer since last go at it
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
* reposition text, banners, buttons when resolution changes 
* fix text shader to use `texture` instead of `texelFetch` so we can do WebGL 1 instead of 2?
* make webpage explaining project, linking to releases, etc.
* handle newlines in text rendering? (and fix Great Wyrm label)
* outlines flip edge alignment when regions have disjoint sections
* why is tile choice not using SetTileOutline?
* Linux snap package or flatpak or whatever the kids are doing now?
* set up Meson build for iOS 
    - kinda works, but something goes weird if the display scale is 3x
        - sokol bug or my bug? need to investigate (probably my bug; let's be honest)
        - works fine at 1x or 2x though
    - a way to copy to device? probably no avoiding Xcode for that one...
        - Meson can generate an Xcode project... :-/ 
        - or maybe just make a stub Xcode target that invokes Meson and then sees the app as a product? 
        - TestFlight something something? (could get it to the device, but not useful for attaching a debugger...)


## Tech Debt
* more graceful failure (at least on Windows) if can't find resources directory
* figure out why Lua struggles compiling with C11 but only on Ubuntu
* world.c could use a refactor
* note at the top of attributes.c about simplifying it
* RenderingRegions is a holdover from when they were bare TileSets; think about just making a permalist like Agents and TileData have (though TileData's is more by rendering necessity...)
* typemaps should reject inputs that are not tables
* add way to modify attributes instead of having to get/set round-trip through SQLite
* overusing the stretchy buffer right now; look for more chances to just use a static array

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
