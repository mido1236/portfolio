## Entity Component System

This project is a (not so) simple entity component system in C++ that I made 
to improve and show off my C++ and CMake skills.

Of particular interest is the utilization of C++ techniques such as 
**generics**, **templates**, **lambdas**, as well as 
the **Standard Template Library** (STL) and, of course, 
the **Simple DirectMedia Layer** (SDL).

The overall approach is based on the following component array class:

```
template<typename T>
class ComponentArray : public IComponentArray {
vector<T> components;
vector<Entity> entities;
unordered_map<Entity, size_t> entityToIndex;
// Rest of class
}
```

Each instance of the `ComponentArray` class is associated with a specific 
component type `T`. Types include Player, Health, Position, etc. For example, 
the ComponentArray instance for the Health component would essentially map entities to 
their health components. 

Of note, however, is that the component data
is stored in a vector, whereas the map that actually exists in the instance
maps entities to their index in the vector instead. This is done for caching 
purposes by improving data locality.

### Screenshot
![Main](img_game.png)