## TODO
0. Mobile
    - reimplement drawing boxes in the choice and banner renderers
    - fix viewport / aspect ratio issues
    - also orientation/rotation is screwy; lock it down
    - implement touch input for choices
    - then get back to agents :) 
1. Styler rework
    - can it handle a region that is both water and plague?
2. Chamber/region/room model design
    - (see brainstorm notes)
    - somewhat more than tilesets
        - entering a tile needs to be equivalent to entering the chamber it belongs to
    - need to have triggers/encounters somehow
        - "Dwarves tunnel into chamber containing plague."
        - "Adventurers come into territory owned by demons."
3. Design and implement agents/entities
    - (update primordial wandering monsters and great wyrm to be agent)
    - individuals
        - attributes
        - ascribe agency to actions
    - civilizations
        - have zones
        - special case? or just an individual with a zone?
    - HtHaD calls both "groups"...
    - "Tunneling came to a region owned by demons; resolve conflict with $CURRENT_AGENT."
4. Relaxable constraint sets

## Tech Debt
* attributes.c is a mess of copypasta; think about making a single attribute-haver object that other things can point to
    - counterpoint: probably won't add any more data types, and it currently works, so ¯\\\_(ツ)\_/¯ 
* RenderingRegions is a holdover from when they were bare TileSets; think about just making a permalist like Agents and TileData have (though TileData's is more by rendering necessity...)
* windows x64 doesn't build because I am a lazy programmer and my use of integers (esp. pointers) is not rigorously careful. macos is cool with it; windows less so. eventually this should get fixed, but for now, keeping Windows to 32-bit is fine
* typemaps should reject inputs that are not tables
* handle newlines in text rendering?
* figure out text extents ascender/descender issue
* add way to modify attributes instead of having to get/set round-trip through SQLite
* overusing the stretchy buffer right now; look for more chances to just use a static array
    - also not a huge deal, but I think the *free functions also NULL the pointer, so might have some redundant calls
    - also the type of the *len calls is ptrdiff_t, not necessarily long
* pull window dimensions from central spot instead of `1024.0f`-ing all over the place
* figure out why ScreenToWorld needs that multiplier at the end :-/ 

## Horizon
* would require overhaul, but:
    - putting gold vein into spaces between tiles instead of tiles themselves
    - more for visual appeal than anything else... might be a better way to accomplish that
* in-game console
* HLVM exceptions (C + Lua invoking)
* pull sim settings and other stuff out into config file that Lua pulls in
* specify (and store) random seed
* expose resources directory on mac?
* history recording -- optional live captioning
    - banner: "The Primordial Age"
    - little in-place captions "Natural caverns form with pockets of liquid hot magma" etc.
    - write all out to text and pretty formatting (or JSON+HTML renderer)
    - screenshots/video
    - name generator?
