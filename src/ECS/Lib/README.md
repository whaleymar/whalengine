# Whaley's Entity Component System
This code is heavily based on this writeup: https://austinmorlan.com/posts/entity_component_system/

The main changes are:

1. Does not rely on RTTI to make unique IDs for components/systems. Uses compile-time solution instead
2. Single header (mostly) for a simpler way to change an entity's components 

## Constraints

1. Components should have a default constructor and be copyable (you can still use other constructors for initialization)
2. Components should have a unique type (after name mangling -> aliases aren't unique)
3. Systems should have a default constructor

## TODO

### Need:

- thread-safe system methods
- auto-call systems
- system scheduler (every X frames)

# Nice to have

- Entity should have some compile() method which sets mIsAlive=true and sends a patternChanged signal -> patternChanged signal not sent for dead entities, which will prevent a bunch of patternChanged calls from happening while an entity is initialized
- prefabs
