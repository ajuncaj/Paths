# Paths
Given by EECS 281 @ UofM

This is a nice little program dealing with the implemenation of pathing algorithms (with a fun pokemon theme!). Essentially the traveling salesman problem, the goal is to "catch" all pokemon on map by visiting them as effeciently as possible (i.e. create a Minimum Spanning Tree) while also respecting some terrain rules (must go from water -> coast -> land for example).

It implements two pathing algorithms:
1. Greedy nearest neighbor using Kruskal's
2. 2-opt + greedy nearest neighbor
