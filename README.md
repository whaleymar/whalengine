# unnamed engine
A WIP 2D game framework written in C++ with OpenGL and GLFW, backed by a simple Entity Component System

# To Do 

## Current Goal: Textures and Animations
- next goal: clean up bugs && some todos

## Graphics
- glfw -> sdl (for text and audio)

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
- camera
- death plane

## Physics
- jump corner correction
- end jump if head bonk
- jump sometimes still happens after landing 
- isTrigger flag for IUseCollider (+ maybe rename mIsCollidable -> mIsActive + add new isCollidable flag which determines if only a trigger happens)
- RigidBody is doing too much, needs to be split up:
    - ActorCollider: determines how it interacts with solids 
        - maybe this could be a generic component (that solids also have) that could be used as a trigger, or can combine with RigidBody/SolidBody to get current behavior
    - RigidBody: determines how friction and gravity affect it (so I can have these systems affect entities without collision, like sweat particles)
    - PlatformerController: coyote time, jump time, jump height, isLanding/Grounded/Jumping flags
        - NPCs can have their own component to adjust these

# Animation 
- needs cleanup + some light optimization (remove frequent string comparisons)
- something to prevent syncing, maybe add a random amount of time to secondsPerFrame

## Non-ECS Systems
- chunk loading/unloading
- loading entity/map from file
    - definitely figure out level editor first
    - Scenes
        - hot reloading
- text rendering\*\*
- Level editor\*\*
- RNG

## Misc
- ECS lib tasks
- make this an actual framework 
    - move stuff in main.cpp into game class
- Sys class (WIP name) that holds deltatime, frameNum, etc. so calling them is less annoying
- API change: easier way to convert between collider center and Position
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
