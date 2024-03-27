# Whaley's Entity Component System
This code is heavily based on this writeup: https://austinmorlan.com/posts/entity_component_system/

The main changes are:

1. Does not rely on RTTI to make unique IDs for components/systems. Uses compile-time solution instead
2. Single header (mostly) for a simpler way to change an entity's components 

## Constraints

1. Components should have a default constructor and be copyable
2. Components should have a unique type (after name mangling -> aliases aren't unique)
