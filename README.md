<p align="center"><img src="https://github.com/sjml/dungen/raw/master/art/DunGenLogo.png"></p>

# DunGen
![Build Status](https://img.shields.io/github/workflow/status/sjml/dungen/Build%20DunGen%20-%20all%20platforms/agent-work?label=build%20and%20tests&style=for-the-badge)

It seems like every summer I fall down a rabbit hole making some procedural generator system. This one is from summer of 2019, when I was otherwise busy doing refugee aid work and studying macroeconomics in South Africa. It was a welcome respite from the stress of the days.

DunGen is a narrative-driven procedural dungeon generator/simulator, inspired by the pen-and-paper game ["How to Host a Dungeon"](https://store.tonydowler.com/listing/591021523/how-to-host-a-dungeon-pdf) by Tony Dowler. I first started work on a version of this, with Tony's permission, about 7 years ago, but got caught up with life and some technical challenges and it withered. Recently, after a caving trip, I was inspired to start it up again. Before I used a somewhat heavy C++ prototyping framework; this time I rebuilt it from scratch using just plain C. 

Currently it simulates all of the Primordial Age and the Great Disaster. Implementing agents and civilizations is a bit trickier, and honestly, I might wander away as the semester gets going. But this was fun to make. 

It's more of a programmer's toy than a friendly simulation presentation, so the handles for playing with it can be a little archaic.

I've structured it something like a computer operating on the hex grid like a set of working memory cells. A sort of high-level virtual machine (HLVM), if you will. Look into the [simulation folder](https://github.com/sjml/dungen/tree/main/scripts/simulation) to see how the little programs (written in [Lua](https://www.lua.org/)) that manipulate that memory work. They get called and executed in a stack, exchanging data via registers that the base engine also uses. (Calling this a "VM" is definitely generous.) 

The simulation scripts are located in the `Resources/scripts/simulation` directory on Windows, and on the Mac they are in the .app directory's `Contents/Resources/scripts/simulation` (right-click --> "Show Package Contents").

## Building
The project builds for various platforms using [Meson](https://mesonbuild.com/). In the `buildscripts/setups` folder there are shell scripts that will configure Meson and build an initial debug version of the app. There is also a [Visual Studio Code](https://code.visualstudio.com) setup for building, debugging, and running. 

On Ubuntu (the only Linux distribution I regularly test on), in addition to `meson` you'll need to install: `build-essential clang libx11-dev libxi-dev libxcursor-dev libgl-dev`. On other platforms, everything you need should already be there or in this repo. 

To make a distributable version, look at the scripts the `buildscripts/releases` folder. After running one of those scripts, a runnable set of files ready for zipping/deploying will end up in the `build/DunGen_dist` directory on all platforms.

Note there is code in the project related to building for iOS, but not currently a functional iOS build. My Mac is more or less out of commission at the moment and so that will have to wait. Alack. 
