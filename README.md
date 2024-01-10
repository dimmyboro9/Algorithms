# Algorithms
There is a repository containing my implementation of different algorithms in C++, which I created during my studies at FIT CTU.

## Projects
- [BFS modification](#BFS-modification)
- [AVL tree](#AVL-tree)
- [Dynamic programming](#Dynamic-programming)
- [Automata](#Automata)
- [Space search algorithms](#Space-search-algorithms)

## BFS modification
My task is to implement the function `std::list<Place> find_path(const Map &map)`, which will find the shortest possible walk in the given graph (represented as a map of rooms), picking up at least one given component of each type (there are various types of items I have to pick up). The first element of the walk must be `map.start`, the last one `map.end`, and there must be a corridor (an edge) between each pair of consecutive places. If such a walk does not exist, return an empty list. 

The `Map` structure contains the following items:
* `places`: the number of rooms (vertices of the graph),
* `start`, `end`: the room numbers where I start and where I have to deliver the collected items (numbered from zero),
* `connections`: a list of pairs of rooms describing the corridors,
* `items`: a list of lists, where `items[i]` is a list of rooms containing the i-th component.

I can assume that the room numbers are always in the range of `0` to `places - 1` (inclusive).

This program correctly and quickly solve instances with approximately 3000 rooms and up to 6 types of items. Each item can occur in many rooms.

To solve this task I will implement and use **BFS modification**.

## AVL tree
There is a toy store that keeps track of the quantity of purchased and sold toys of different types. My task is to implement a template class `Bestsellers` parameterized by the type `Product`, which serves as an identifier for individual products. It is promised that the type `Product` has a copy `constructor` (and `assignment operator`), `destructor`, `comparison operators`, and `std::hash` is implemented for it. 

The `Bestsellers` class must implement the following public methods:

* `size_t products() const` - returns the number of recorded types of goods (not the number of sold items),
* `void sell(const Product& p, size_t amount)` - records the sale of amount `items` of the product `p`,
* `size_t rank(const Product& p) const` - returns the ranking (from 1) of the best-selling product `p` (if multiple products have sold the same amount, their order can be arbitrary),
* `const Product& product(size_t rank) const` - inverse function to rank (if no exception occurs, it always holds that `product(rank(p)) == p` and `rank(product(r)) == r`),
* `size_t sold(size_t r) const` - returns the number of sold items of the `r`-th best-selling product,
* `size_t sold(size_t from, size_t to) const` - similar to the one-parameter version, but returns the sum of sold items over the interval from `from` to `to` (inclusive on both ends; i.e., `sold(r, r) == sold(r)` and `sold(1, products()`) is the total number of sold items across all types of goods; input where `to` < `from` is considered invalid),
* `size_t first_same(size_t r) const` and `size_t last_same(size_t r) const` - return the smallest (largest) rank `r'` for which `sold(r') == sold(r)`. 

If the arguments of the function calls are invalid (i.e., rank outside the range or an unknown product in the `rank` method), throw an `std::out_of_range exception`.

To implement all these functions, I will implement and use the data structure **AVL tree**.

## Dynamic programming
We have a city map where all streets together form a tree structure (in terms of data structure), which is not very practical, but it doesn't bother anyone. Houses (graph vertices) are located at the nodes of the streets, while the streets themselves are the edges of the graph. There is a Christmas tree on some of the vertices of graph under which gifts are placed. To prevent the Grinch from stealing gifts from children, special agents are placed on the nodes (houses) to ensure the Grinch cannot ruin Christmas.

My task is to find the deployment of secret service agents in the streets of the city so that the maximum number of gifts is protected. However, I should be careful not to have large groups of agents close to each other in the streets (because it's a secret service). Agents are considered to be in the same group if there is no ungifted tree between them.

So assignment is to implement the function `uint64_t solve(const TreeProblem&)`, which calculates how many gifts can be saved with the best possible arrangement of guards. The problem instance is described by the structure `TreeProblem`, which contains the following items:

* `max_group_size`: The maximum size of a group of guards. This value is maximally equal to **2**,
* `gifts`: A vector specifying the number of gifts under each tree. The length of this vector also represents the total number of trees,
* `connections`: A vector of pairs indicating which trees are at opposite ends of the same street, so if both are guarded, their guards belong to the same group.

A naive exponential implementation isn't sufficient. The implementation must be efficient as inputs are large, and the stack size is limited to 1 MiB. It is necessary to correctly solve instances with `max_group_size = 2`.

To solve this problem, I will use the **dynamic programming** approach.

## Automata
The task is to implement algorithms that allow to find the `minimal deterministic finite automaton` accepting the intersection or union of languages specified by pairs of finite automata. The goal is to implement a set of two functions with the following signatures:

* `DFA unify(const NFA&, const NFA&)`,
* `DFA intersect(const NFA&, const NFA&)`.

The input for the algorithms is two automata **NFA**, representing non-deterministic finite automata. Both of these functions must return a minimal deterministic finite automaton **DFA** for the given language.

## Space search algorithms
Within this task, I implement `five methods` of systematic state space search and demonstrate the functionality of the implemented algorithms in solving the pathfinding problem.

Methods I want to implement:
* `Random Search`,
* `DFS` without depth limit,
* `BFS`,
* `Greedy Search`,
* `A* algorithm`

`Input`: Mazes for algorithm testing. Mazes are available in the form of text files.
The maze format:

``` plaintext
XXXXXXXXXXXXX
X           X
X  XXXXXXX  X
X     X     X
X X X X XXX X
X X         X
XX  X XXXXX X
X   X       X
X X X XXX XXX
X           X
X XXX X   X X
X           X
XXXXXXXXXXXXX
start 1, 7
end 5, 3
```

* X represents impassable walls, while spaces indicate free passages.
* The position is given in Cartesian coordinates, with the origin of the coordinate system in the upper-left corner, and the positive y-axis oriented downward.
* "start 1, 7" means that the start is located at coordinates \[1, 7\].

`Output`: Simple visualization of the activity of each algorithm + output of the length of the found path + information about which fields the program includes in the path length.

```plaintext
XXXXXXXXXXXXX
X           X
X  XXXXXXX  X
X ###EX     X
X X#XoX XXX X
X Xooo#     X
XX#oX#XXXXX X
XSooX       X
X#X#X XXX XXX
X           X
X XXX X   X X
X           X
XXXXXXXXXXXXX
---------------------------------------
S Start
E End
# Opened node
o Path
X Wall
space Fresh node
---------------------------------------
Nodes expanded: 17
Path length: 8
```

`Copyright (c) Dmytro Borovko 2024`
