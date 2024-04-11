# unnamed engine
A WIP 2D game framework written in C++ with OpenGL and GLFW, backed by a simple Entity Component System

# To Do 

## Current Goal: Textures and Animations
- set sprite based on atlas metadata
- player animation brain
- reorganize animation code

- next goal: organize main.cpp to use prefabs
- next goal: clean up some todos

## Systems
- sound\*
- lighting\*

## Components
- Follow
- Light 
- lifetime
- collision callback

## Entity Prefabs
- particle
    - draw, position, velocity, lifetime
- static solid tile
    - draw, position, solidbody
- player
- camera

## Physics
- jump corner correction
- end jump if head bonk
- jump sometimes still happens after landing 
- ramp up to full run speed

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

# Tools Used
- crunch: CLI texture packer, https://github.com/ChevyRay/crunch
- (tentative) BMFont: Bitmap Font Generator from truetype, https://www.angelcode.com/products/bmfont/
- (tentative) Ogmo Editor 3 / Tiled: Level Editor, https://ogmo-editor-3.github.io/ ; https://doc.mapeditor.org/en/stable/manual/layers/


\* = unsure how this will fit into ECS / may have non-ECS component
\*\* = see tools
