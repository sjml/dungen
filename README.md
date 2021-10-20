<p align="center"><img src="https://github.com/sjml/dungen/raw/main/art/DunGenLogo.png"></p>

# DunGen
[![Build Status](https://img.shields.io/github/workflow/status/sjml/dungen/Build%20DunGen%20-%20all%20platforms/agent-work?label=build%20and%20tests&style=for-the-badge)](https://github.com/sjml/dungen/actions/workflows/build-all.yml)

DunGen is a narrative-driven procedural dungeon generator/simulator, inspired by the pen-and-paper game ["How to Host a Dungeon"](https://store.tonydowler.com/listing/591021523/how-to-host-a-dungeon-pdf) by Tony Dowler. I first started work on a version of this, with Tony's permission, around 2012, but got caught up with life and some technical challenges and it withered. The first time, I used a somewhat heavy C++ prototyping framework. In 2019, I was in South Africa and went on a caving trip, which inspired me to pull this out of the mothballs. That time I rebuilt it from scratch using just plain C and OpenGL. In 2021, I was living in Lebanon and went on another caving trip, and again was inspired. Instead of a full rebuild, I started using this project as my testbed for trying new build systems, new rendering layers, etc. So now it uses sokol headers which can make it run on Mac, Windows, Linux, iOS, and the web. Fun! 

All of that's to say that I still haven't really made any progress on the actual content of it, but I also have fun figuring out build systems and stuff. As I write this, though, I think I'm done faffing about with the technical infrastructure and ready to get back to actually making the thing itself. 

Currently it simulates all of the Primordial Age and the Great Disaster. Implementing agents and civilizations is a bit trickier...

It's more of a programmer's toy than a friendly simulation presentation, so the handles for playing with it can be a little archaic.

I've structured it something like a computer operating on the hex grid like a set of working memory cells. A sort of high-level virtual machine (HLVM), if you will. Look into the [simulation folder](https://github.com/sjml/dungen/tree/main/scripts/simulation) to see how the little programs (written in [Lua](https://www.lua.org/)) that manipulate that memory work. They get called and executed in a stack, exchanging data via registers that the base engine also uses. (Calling this a "VM" is definitely generous.) 

The simulation scripts are located in the `Resources/scripts/simulation` directory on Windows, and on the Mac they are in the .app directory's `Contents/Resources/scripts/simulation` (right-click --> "Show Package Contents").

## Building
The project builds for various platforms using [Meson](https://mesonbuild.com/). In the `buildscripts/setups` folder there are shell scripts that will configure Meson and build an initial debug version of the app. There is also a [Visual Studio Code](https://code.visualstudio.com) setup for building, debugging, and running. 

On Ubuntu (the only Linux distribution I regularly test on), in addition to `meson` you'll need to install: `build-essential pkg-config clang libx11-dev libxi-dev libxcursor-dev libgl-dev`. On other platforms, everything you need should already be there or in this repo. 

To make a distributable version, look at the scripts the `buildscripts/releases` folder. After running one of those scripts, a runnable set of files ready for zipping/deploying will end up in the `build/DunGen_dist` directory on all platforms. The scripts in the `buildscripts/setups` folder are similar, but make debug builds for development and don't do the final distribution packaging.
