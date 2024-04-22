# unnamed engine
A WIP 2D game framework written in C++ with OpenGL and GLFW, backed by a simple Entity Component System

# To Do 

## Current Goal: 

## Graphics
- glfw -> sdl (for text and audio)

## Systems
- sound\*
- lighting\*
- ui\*

## Components
- Follow
- Light 
- lifetime
- Trigger
- collision callback (move object, kill player, play animation, etc.)

## Entity Prefabs
- particle
    - draw, position, velocity, lifetime
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
- chunk loading/unloading
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

## Bugs
- isNearZero not working
- cppcheck issues
- depth test broken for Draw components (low priority)

## Research
- things i might want to reconsider in the future -- ctrl+f for "RESEARCH" 

# Tools Used
- crunch: CLI texture packer, https://github.com/ChevyRay/crunch
- (tentative) BMFont: Bitmap Font Generator from truetype, https://www.angelcode.com/products/bmfont/
- (tentative) Ogmo Editor 3 / Tiled: Level Editor, https://ogmo-editor-3.github.io/ ; https://doc.mapeditor.org/en/stable/manual/layers/


\* = unsure how this will fit into ECS / may have non-ECS component
\*\* = see tools
