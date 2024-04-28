# unnamed engine
A WIP 2D game framework written in C++ with OpenGL and SDL, backed by a simple Entity Component System

# To Do 

## Current Goal: 
- text system (on hold)
- tiled parser
    - parse map data (DONE)
    - display correct sprites (DONE)
    - bake collision mesh (DONE)
    - enable hot-reloading
    - store tile material data 
    - object layers
    - parallax
    - move tiled map(s) and tilesets into data/ directory
- game class

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
- hot-reloading code (youtube video is bookmarked)

## Bugs
- isNearZero not working
- cppcheck issues
- depth test broken for Draw components (low priority)

## Research
- things i might want to reconsider in the future -- ctrl+f for "RESEARCH" 

# Tools Used
- crunch: CLI texture packer, https://github.com/ChevyRay/crunch
- (tentative) BMFont: Bitmap Font Generator from truetype, https://www.angelcode.com/products/bmfont/


\* = unsure how this will fit into ECS / may have non-ECS component
\*\* = see tools
