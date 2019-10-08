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
* reimplement screenshot
* move from manual zip to pipelines task
    - https://docs.microsoft.com/en-us/azure/devops/pipelines/tasks/utility/archive-files?view=azure-devops
    - also maybe github actions? https://github.com/features/actions
* set up proper webpage for emscripten environment
    - also add it to azure pipelines build
    - make webpage explaining project, linking to releases, etc.
* handle newlines in text rendering?

## Tech Debt
* world.c could use a refactor
* attributes.c is a mess of copypasta; think about making a single attribute-haver object that other things can point to
    - counterpoint: probably won't add any more data types, and it currently works, so ¯\\\_(ツ)\_/¯ 
* RenderingRegions is a holdover from when they were bare TileSets; think about just making a permalist like Agents and TileData have (though TileData's is more by rendering necessity...)
* windows x64 doesn't build because I am a lazy programmer and my use of integers (esp. pointers) is not rigorously careful. macos is cool with it; windows less so. eventually this should get fixed, but for now, keeping Windows to 32-bit is fine
* typemaps should reject inputs that are not tables
* figure out text extents ascender/descender issue
* add way to modify attributes instead of having to get/set round-trip through SQLite
* overusing the stretchy buffer right now; look for more chances to just use a static array
    - also not a huge deal, but I think the *free functions also NULL the pointer, so might have some redundant calls
    - also the type of the *len calls is ptrdiff_t, not necessarily long
* figure out why ScreenToWorld needs that multiplier at the end :-/ 

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
