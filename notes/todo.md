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
* tools
    - lua console
* somewhere, in one of the simulation elements, there's an infinite loop getting hit
    - found it as hundreds of magmas were being stored in one chamber?
    - maybe hlvm should be able to report what it's current top stack element is
* reposition text, banners, buttons when resolution changes
* handle newlines in text rendering? (and fix Great Wyrm label)
* outlines flip edge alignment when regions have disjoint sections
* why is tile choice not using SetTileOutline?
* successful quit request should halt the stack popping
* make webpage (on itch? prolly) explaining project, linking to releases, etc.

## Tech Debt
* could use a minor but broad code sweep -- lots of cruft leftover from old build systems, etc.
* more graceful failure (at least on Windows) if can't find resources directory
* world.c could use a refactor
    - RenderingRegions is a holdover from when they were bare TileSets; think about just making a permalist like Agents and TileData have (though TileData's is more by rendering necessity...)
        - in general, regions should be managed by the world instead of rendering system
* note at the top of attributes.c about simplifying it
    - also add way to modify attributes instead of having to get/set round-trip through SQLite
* overusing the stretchy buffer right now; look for more chances to just use a static array

## Horizon
* HLVM exceptions (C + Lua invoking)
* pull sim settings and other stuff out into config file that Lua pulls in
* try to read resources from user directory (and add button to tools to copy there)
    - will involve navigating sandbox stuff with Mac and AppImage
* history recording -- optional live captioning
    - little in-place captions "Natural caverns form with pockets of liquid hot magma" etc.
    - write all out to text and pretty formatting (or JSON+HTML renderer)
    - screenshots/video
    - name generator?
