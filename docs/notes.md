## TODO
1. UI
    1. tile choice with label
    2. checkboxes? 
2. random tiles and die rolls in Lua with optional player overrides
3. Relaxable constraint sets
4. Primordial Age
    - image output / movie (can be manual for now)
5. Chamber/region/room model design
    - (see brainstorm notes)
    - somewhat more than tilesets
        - entering a tile needs to be equivalent to entering the chamber it belongs to
        - currently some linguistic grey areas around tilesets/lists/etc. 
        - rework + clarify as this develops
    - need to have triggers/encounters somehow
        - "Dwarves tunnel into chamber containing plague."
        - "Adventurers come into territory owned by demons."
6. Design and implement agents/entities
    - individuals
        - attributes
        - ascribe agency to actions
    - civilizations
        - have zones
        - special case? or just an individual with a zone?
    - HtHaD calls both "groups"...
    - "Tunneling came to a region owned by demons; resolve conflict with $CURRENT_AGENT."

## Tech Debt
* Update CMakeLists.txt; get Linux building
* set precompiled headers up in Visual Studio
* handle newlines in text rendering?
* figure out text extents ascender/descender issue
* overusing the stretchy buffer right now; look for more chances to just use a static array
* ResolveStyles() is super slow; see if there are some optimizations to be had
* pull dimensions from central spot instead of `1024.0f`-ing all over the place
* figure out why ScreenToWorld needs that multiplier at the end :-/ 
* https://github.com/nothings/stb/blob/master/stb_leakcheck.h

## Horizon
* in-game console
* HLVM exceptions (C + Lua invoking)
* pull sim settings and other stuff out into config file that Lua pulls in
* let regions label themselves too (position in center of set based on farthest X and Y)
* specify (and store) random seed
* let outlines specify inner/outer/center
* Linux build
    - Azure pipeline -> GitHub release (draft)
* expose resources directory on mac?
* history recording -- optional live captioning
    - banner: "The Primordial Age"
    - little in-place captions "Natural caverns form with pockets of liquid hot magma" etc.
    - write all out to text and pretty formatting (or JSON+HTML renderer)
    - screenshots/video
    - name generator?
