# unnamed engine
A WIP 2D game framework written in C++ with OpenGL and SDL, backed by a simple Entity Component System

# To Do 

## Current Goal: 
- put velocity in texels/second instead of tiles/second

## Camera / Follow
- different movement types (easein/out stuff)
- be affected by momentum (maybe momentum should be added to velocity.total?)

## Systems
- lighting\*
- text/ui -- try building pause menu which\*
    1. is triggered with a pause event 
    2. makes music quieter
    3. pauses all sfx (minus one channel used in pause menu)
    4. creates simple text gui : {Resume, Restart, Quit} -- each of these send their own events
    - maybe just do bitmap font with instanced letters?

## Components
- Light 
- Attached (relationship)
- EventFlow
    - execute a series of callbacks sequentially
    - max 1 callback per frame 
    - some callbacks may have a wait time
- DebugDraw (exactly like draw but only called in debug mode)

## Entity Prefabs
- particle
    - draw(?), position, velocity, lifetime
    - instead of draw, could do custom component & *instance* it
- death plane

## Physics
- idea: overlapping actors/semisolids nudge each other away or exert a force or something
- low priority: actors always check for a collision with a solid before a semisolid, so semisolid callbacks don't always run. Could be fixed by storing solids and semisolid pointers together for these collision checks, but it's not a huge deal
- fixed updates at 60fps so rounding doesn't fuck me

## Map 
- support rotations / flips? (leaning towards no)
- object layers
    - component factory functions (mostly done)
    - special metadata
        - spawn points
        - camera strat (might want to rework)
    - Templates
- parallax

## Graphics
- outline shader
- repeating texture

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
- Logger queue that runs on another thread
- rework controller system to be manually called by Event
- hot-reloading code (youtube video is bookmarked)
- convenience methods to get bottom position of a collider component && get center from transform + yhalf
    - do this like how i did the constructor for TriggerZone
- a lot of physics stuff (like velocity) is stored as floats even though it could be fixed precision (like nearest tenth of a texel) --> I should use ints for this? #CLEANUP
- velocity is in TILES per second which is a huge footgun #CLEANUP

## Bugs
- isNearZero not working
- cppcheck issues
- depth test broken for Draw components (low priority)
- actors sometimes fall through one way solids if the solid is moving fast enough, probably because solids don't move one pixel at a time, so if the actor isn't already riding the solid, it misses the solid's boundary

## Research
- things i might want to reconsider in the future -- ctrl+f for "RESEARCH" 

# Tools Used
- crunch: CLI texture packer, https://github.com/ChevyRay/crunch
- (tentative) BMFont: Bitmap Font Generator from truetype, https://www.angelcode.com/products/bmfont/


\* = unsure how this will fit into ECS / may have non-ECS component
\*\* = see tools
