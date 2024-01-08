# Algorithms
There is a repository containing my implementation of different algorithms in C++, which I created during my studies at FIT CTU.

## Projects
- [BFS modification](#BFS-modification)
- [AVL tree](#AVL-tree)

## BFS modification
My task is to implement the function `std::list<Place> find_path(const Map &map)`, which will find the shortest possible walk in the given graph (represented as a map of rooms), picking up at least one given component of each type (there are various types of items I have to pick up). The first element of the walk must be `map.start`, the last one `map.end`, and there must be a corridor between each pair of consecutive places. If such a walk does not exist, return an empty list. 

The `Map` structure contains the following items:
* `places`: the number of rooms (vertices of the graph),
* `start`, `end`: the room numbers where I start and where I have to deliver the collected items (numbered from zero),
* `connections`: a list of pairs of rooms describing the corridors,
* `items`: a list of lists, where `items[i]` is a list of rooms containing the i-th component.

I can assume that the room numbers are always in the range of `0` to `places - 1` (inclusive).

This program correctly and quickly solve instances with approximately 3000 rooms and up to 6 types of items. Each item can occur in many rooms.

To solve this task I will implement and use **BFS modification**.

## AVL tree
My task is to implement a template class `Bestsellers` parameterized by the type `Product`, which serves as an identifier for individual products. It is promised that the type `Product` has a copy `constructor` (and `assignment operator`), `destructor`, `comparison operators`, and `std::hash` is implemented for it. 

The `Bestsellers` class must implement the following public methods:

* `size_t products() const` - returns the number of recorded types of goods (not the number of sold items).
* `void sell(const Product& p, size_t amount)` - records the sale of amount `items` of the product `p`.
* `size_t rank(const Product& p) const` - returns the ranking (from 1) of the best-selling product `p` (if multiple products have sold the same amount, their order can be arbitrary).
* `const Product& product(size_t rank) const` - inverse function to rank (if no exception occurs, it always holds that `product(rank(p)) == p` and `rank(product(r)) == r`).
* `size_t sold(size_t r) const` - returns the number of sold items of the `r`-th best-selling product.
* `size_t sold(size_t from, size_t to) const` - similar to the one-parameter version, but returns the sum of sold items over the interval from `from` to `to` (inclusive on both ends; i.e., `sold(r, r) == sold(r)` and `sold(1, products()`) is the total number of sold items across all types of goods; input where `to` < `from` is considered invalid).
* `size_t first_same(size_t r) const` and `size_t last_same(size_t r) const` - return the smallest (largest) rank `r'` for which `sold(r') == sold(r)`. 

If the arguments of the function calls are invalid (i.e., rank outside the range or an unknown product in the `rank` method), throw an `std::out_of_range exception`.

To implement all these functions, I will implement and use the data structure **AVL tree**.
