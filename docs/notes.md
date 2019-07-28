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
3. HLVM exceptions (C + Lua invoking)

## Horizon
* in-game console
* pull dimensions from central spot instead of 1024.0f all over the place
* specify (and store) random seed
* let outlines specify inner/outer/center
* Windows build
    - Azure pipeline -> GitHub release (draft)
* let regions label themselves too
* history recording -- optional live captioning
    - banner: "The Primordial Age"
    - little in-place captions "Natural caverns form with pockets of liquid hot magma" etc.
    - write all out to text and pretty formatting (or JSON+HTML renderer)
    - screenshots/video
    - name generator?
* https://github.com/nothings/stb/blob/master/stb_leakcheck.h
* figure out why ScreenToWorld needs that multiplier at the end :-/ 
