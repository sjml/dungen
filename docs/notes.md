## TODO
1. UI
    1. banner rendering
        - set of strings, sizes, and background colors
        - durations (or click / button press to dismiss)
    2. single button rendering, detecting rollover and push
        - buttons on banner screen?
    3. layout of up to 12 buttons from list of strings
    4. lua creation of button list and pausing to wait for response
        - merge buttons with same description
        - description text at top (or have banner integrated?)
    5. tile choice with label
    6. checkboxes? 
2. random tiles and die rolls in Lua with optional player overrides
3. Relaxable constraint sets
4. Primordial Age
    - image output / movie (can be manual for now)
5. Chamber/region/room model design
    - somewhat more than tilesets
        - entering a tile needs to be equivalent to entering the chamber it belongs to
        - currently some linguistic grey areas around tilesets/lists/etc. 
        - rework + clarify as this develops
    - need to have triggers/encounters somehow
        - "Dwarves tunnel into chamber containing plague."
        - "Adventurers come into territory owned by demons."
6. Design and implement agents/entities
    - individuals
    - civilizations
        - have zones
    - both
        - attributes
        - ascribe agency to actions
            - "Tunneling came to a region owned by demons; resolve conflict with $CURRENT_AGENT."

## Tech Debt
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
* Windows+Linux builds
    - Azure pipeline -> GitHub release (draft)
    - copy acknowledgements alongside exe
    - make readme
    - expose resources directory on mac?
* history recording -- optional live captioning
    - banner: "The Primordial Age"
    - little in-place captions "Natural caverns form with pockets of liquid hot magma" etc.
    - write all out to text and pretty formatting (or JSON+HTML renderer)
    - screenshots/video
    - name generator?
