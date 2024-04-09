# unnamed engine
A WIP 2D game framework written in C++ with OpenGL and GLFW, backed by a simple Entity Component System

# To Do 

## Current Goal: Textures and Animations
- set sprite based on atlas metadata
- animation system

## Systems
- sound\*
- lighting\*
- textures (part of Gfx System)\*\*
- animation (part of Gfx System)

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

## Errors
- image lib has exceptions

## Non-ECS Systems
- Scenes
    - hot reloading
- chunk loading/unloading
- loading entity/map from file
    - definitely figure out level editor first
- text rendering\*\*
- PNG parser 
- Level editor\*\*

## Misc
- give vertices a color component (+ isVerticesUpdateNeeded in Draw component)
- ECS lib tasks
- make this an actual framework 
    - move stuff in main.cpp into game class

# Tools 
- crunch: CLI texture packer, https://github.com/ChevyRay/crunch
- BMFont: Bitmap Font Generator from truetype, https://www.angelcode.com/products/bmfont/
- Ogmo Editor 3 / Tiled: Level Editor, https://ogmo-editor-3.github.io/ ; https://doc.mapeditor.org/en/stable/manual/layers/


\* = unsure how this will fit into ECS / may have non-ECS component
\*\* = see tools
