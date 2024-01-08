# Algorithms
There is a repository containing my implementation of different algorithms in C++, which I created during my studies at FIT CTU.

## Projects
- [BFS modification](#BFS-modification)

## BFS modification
My task is to implement the function `std::list<Place> find_path(const Map &map)`, which will find the shortest possible walk in the given graph (represented as a map of rooms), picking up at least one given component of each type (there are various types of items I have to pick up). The first element of the walk must be `map.start`, the last one `map.end`, and there must be a corridor between each pair of consecutive places. If such a walk does not exist, return an empty list. 

The `Map` structure contains the following items:
* `places`: the number of rooms (vertices of the graph),
* `start`, `end`: the room numbers where I start and where I have to deliver the collected items (numbered from zero),
* `connections`: a list of pairs of rooms describing the corridors,
* `items`: a list of lists, where `items[i]` is a list of rooms containing the i-th component.

I can assume that the room numbers are always in the range of `0` to `places - 1` (inclusive).

This program correctly and quickly solve instances with approximately 3000 rooms and up to 6 types of items. Each item can occur in many rooms.
