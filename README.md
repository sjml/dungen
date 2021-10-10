<p align="center"><img src="https://github.com/sjml/dungen/raw/master/art/DunGenLogo.png"></p>

# DunGen

[![Build Status](https://optimistpanda.visualstudio.com/DunGen/_apis/build/status/sjml.dungen2?branchName=master)](https://optimistpanda.visualstudio.com/DunGen/_build/latest?definitionId=1&branchName=master)

It seems like every summer I fall down a rabbit hole making some procedural generator system. This one is from summer of 2019, when I was otherwise busy doing refugee aid work and studying macroeconomics in South Africa. It was a welcome respite from the stress of the days.

DunGen is a narrative-driven procedural dungeon generator/simulator, inspired by the pen-and-paper game ["How to Host a Dungeon"](https://store.tonydowler.com/listing/591021523/how-to-host-a-dungeon-pdf) by Tony Dowler. I first started work on a version of this, with Tony's permission, about 7 years ago, but got caught up with life and some technical challenges and it withered. Recently, after a caving trip, I was inspired to start it up again. Before I used a somewhat heavy C++ prototyping framework; this time I rebuilt it from scratch using just plain C. 

Currently it simulates all of the Primordial Age and the Great Disaster. Implementing agents and civilizations is a bit trickier, and honestly, I might wander away as the semester gets going. But this was fun to make. 

It's more of a programmer's toy than a friendly simulation presentation, so the handles for playing with it can be a little archaic.

I've structured it something like a computer operating on the hex grid like a set of working memory cells. A sort of high-level virtual machine (HLVM), if you will. Look into the [simulation folder](https://github.com/sjml/dungen2/tree/master/scripts/simulation) to see how the little programs (written in [Lua](https://www.lua.org/)) that manipulate that memory work. They get called and executed in a stack, exchanging data via registers that the base engine also uses. (Calling this a "VM" is definitely generous.) 

The simulation scripts are located in the `Resources/scripts/simulation` directory on Windows, and on the Mac they are in the .app directory's `Contents/Resources/scripts/simulation` (right-click --> "Show Package Contents").

## Building
Meson

(TODO: expand and explain)
