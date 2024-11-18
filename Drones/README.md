This is a school project in which I implemented multiple solutions to the Traveling Salesperson Problem. 
# The Traveling Salesperson Problem
This is a classic computer science problem that has countless applications. The problem is to find the shortest path among some list of locations such that a "salesperson" can visit each location once before finally arriving at the original location. 
## Additional Constraints
In this project, this problem was framed in the context of a hypothetical drone delivery service that needs to find the optimal route between a given set of delivery locations. However, there are additional constraints to the basic Traveling Salesperson problem, such as transit being restricted between different campuses of the map.
## Algorithms/Heuristics Used
I was tasked with implementing two solutions to the task. One involved using a heuristic, or a quicker version that finds a near-optimal solution. I chose to implement the Nearest-Neighbor Heuristic. The second solution was a Brute-Force Algorithm with a Branch and Bound optimization. This version finds the exact optimal solution at the expense of runtime and memory. The Branch and Bound component saves a huge amount of time by "pruning" solutions early that are either guaranteed to be invalid or perform worse than the existing best solution.  
