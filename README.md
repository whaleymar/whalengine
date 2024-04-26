# unnamed engine
A WIP 2D game framework written in C++ with OpenGL and SDL, backed by a simple Entity Component System

# To Do 

## Current Goal: 
- text system (on hold)
- tiled parser

## Systems
- lighting\*
- text/ui -- try building pause menu which\*
    1. is triggered with a pause event 
    2. makes music quieter
    3. pauses all sfx (minus one channel used in pause menu)
    4. creates simple text gui : {Resume, Restart, Quit} -- each of these send their own events

## Components
- Follow
- Light 
- Trigger
- collision callback (move object, kill player, play animation, etc.)
- Tile

## Entity Prefabs
- particle
    - draw(?), position, velocity, lifetime
    - instead of draw, could do custom component & instance it
- camera
- death plane

## Physics
- Trigger colliders
- X axis momentum is weird
    - jumping off platform gives very little x momentum 
    - moving in same direction as momentum gives too much speed AND is inconsistent -> use 2 players overlapped and i get completely different speeds
    - moving in opposite direction as momentum completely negates it 
    - trying to walk off platform in same direction it's moving is buggy
- "Static" tag that I can give TILE solids which will never move -- I can combine these into a collision mesh?

## Non-ECS Systems
- chunk loading/unloading (quad tree?)
- loading entity/map from file
    - definitely figure out level editor first
    - Scenes
        - hot reloading
- text rendering\*\*
- Level editor\*\*

## Misc
- ECS lib tasks
- make this an actual framework 
    - move stuff in main.cpp into game class
- Logger queue that runs on another thread
- rework controller system to be manually called by Event

## Bugs
- isNearZero not working
- cppcheck issues
- depth test broken for Draw components (low priority)
- Railscontrol timer not working right. Is always synced to others regardless of speed/distance

## Research
- things i might want to reconsider in the future -- ctrl+f for "RESEARCH" 

# Tools Used
- crunch: CLI texture packer, https://github.com/ChevyRay/crunch
- (tentative) BMFont: Bitmap Font Generator from truetype, https://www.angelcode.com/products/bmfont/
- (tentative) Ogmo Editor 3 / Tiled: Level Editor, https://ogmo-editor-3.github.io/ ; https://doc.mapeditor.org/en/stable/manual/layers/


\* = unsure how this will fit into ECS / may have non-ECS component
\*\* = see tools
