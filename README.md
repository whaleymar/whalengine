# unnamed engine
A WIP 2D game framework written in C++ with OpenGL and GLFW, backed by a simple Entity Component System

# To Do 

## Current Goal: Textures and Animations
- next goal: clean up bugs && some todos

## Systems
- sound\*
- lighting\*

## Components
- Follow
- Light 
- lifetime
- collision callback (move object, kill player, play animation, etc.)

## Entity Prefabs
- particle
    - draw, position, velocity, lifetime
- player
- camera
- death plane

## Physics
- jump corner correction
- end jump if head bonk
- jump sometimes still happens after landing 
- ramp up to full run speed

# Animation 
- needs cleanup + some light optimization (remove frequent string comparisons)
- something to prevent syncing

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

## Bugs
- deleting player after cloning makes clone's debug collider draw location wrong (but collision works fine) -> because the componentarray data gets moved to maintain density
    - can't hold on to component pointers outside of the ECS, need to completely rethink CollisionManager
- isNearZero not working
- cppcheck issues

# Tools Used
- crunch: CLI texture packer, https://github.com/ChevyRay/crunch
- (tentative) BMFont: Bitmap Font Generator from truetype, https://www.angelcode.com/products/bmfont/
- (tentative) Ogmo Editor 3 / Tiled: Level Editor, https://ogmo-editor-3.github.io/ ; https://doc.mapeditor.org/en/stable/manual/layers/


\* = unsure how this will fit into ECS / may have non-ECS component
\*\* = see tools
