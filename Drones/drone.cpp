//1761414855B69983BD8035097EFBD312EB0527F0
#include "drone.h"
#include <getopt.h>
#include <iostream>
#include <string>
#include <string.h>
#include <limits>
#include <math.h>
#include <iomanip>
#include <vector>
#include <algorithm>

using namespace std;

class Map {
    
    public:



    //////////////////////////////////////////////////
    // GET_OPTIONS                                  //
    //////////////////////////////////////////////////

    void get_options(int argc, char *argv[]) {
        opterr = false;
        int choice;
        int option_index = 0;

        struct option long_options[] = {
            { "mode",     required_argument,       nullptr, 'm' },
            { "help",           no_argument,       nullptr, 'h' },
            { nullptr,                    0,       nullptr, '\0'}
        };

        while ((choice = getopt_long(argc, argv, "m:h", long_options, &option_index)) != -1) {
            switch (choice) {
                case 'm': {
                    if (strcmp(optarg, "MST") == 0) {
                        mode = Mode::MST;
                    }
                    else if (strcmp(optarg, "FASTTSP") == 0) {
                        mode = Mode::FASTTSP;
                    }
                    else if (strcmp(optarg, "OPTTSP") == 0) {
                        mode = Mode::OPTTSP;
                    }
                    else if (strlen(optarg) == 0) {
                        cerr << "Error: No mode specified\n";
                        exit(1);
                    }
                    else {
                        cerr << "Error: Invalid mode\n";
                        exit(1);
                    }
                    break;
                }
                case 'h': {
                    print_help();
                    exit(0);
                }
                default: {
                    cerr << "Error: Invalid command line option\n";
                    exit(0);
                }
            }
        }
    }



    //////////////////////////////////////////////////
    // RUN_PROGRAM                                  //
    //////////////////////////////////////////////////

    // Calls read_map()

    void run_program() {
        read_map(); // Reads map
        if (mode == Mode::MST) {
            create_mst(); // Creates an MST
            print_mst_output(); // Print output
        }
        else if (mode == Mode::FASTTSP) {
            do_farthest_insertion();
            print_tsp_output();
        }
        else {
            create_opttsp();
            print_opttsp_output();
        }
    }



    private:



    //////////////////////////////////////////////////////////////////////
    //   ooooooooo.                          .              .o.         //
    //   `888   `Y88.                      .o8             .888.        //
    //    888   .d88'  .oooo.   oooo d8b .o888oo          .8"888.       //
    //    888ooo88P'  `P  )88b  `888""8P   888           .8' `888.      //
    //    888          .oP"888   888       888          .88ooo8888.     //
    //    888         d8(  888   888       888 .       .8'     `888.    //
    //    o888o        `Y888""8o d888b      "888"     o88o     o8888o   //
    //////////////////////////////////////////////////////////////////////



    //////////////////////////////////////////////////
    // READ_MAP                                     //
    //////////////////////////////////////////////////

    // Reads in coordinates from cin (guaranteed to be well-formed)
    // For Part A, there will always be at least 2 points. For Part B, there will always be at least 3.

    void read_map() {
        size_t num_coords; // The number of coordinates that will be inputted
        cin >> num_coords;

        int next;
        Coordinate next_coord; // Temporary object to hold the next coordinate values
        for (size_t i = 0; i < num_coords; ++i) { // Pushes in one pair of coordinates at a time to the coordinates vector
            cin >> next;
            next_coord.x = next;
            cin >> next;
            next_coord.y = next;
            if (next_coord.x > 0 || next_coord.y > 0) {
                next_coord.loc = Location::Main;
            }
            else if (next_coord.x < 0 && next_coord.y < 0) {
                next_coord.loc = Location::Med;
            }
            else {
                next_coord.loc = Location::MedBorder;
            }
            coordinates.push_back(next_coord);
        }
    }



    //////////////////////////////////////////////////
    // CREATE_MST                                   //
    //////////////////////////////////////////////////

    // Creates an MST using Prim's nested loop algorithm, since the graph will likely be dense

    void create_mst() {
        
        vertices.resize(coordinates.size()); // Resize the vector of Prim vertices to the number of coordinates, filling it with default constructed Vertices.

        if (!vertices.empty()) {
            while (add_closest_vertex()) {
                update_vertex_distance(added_vertices.back());
            } // Add vertices until the function returns false
            if (added_vertices.size() != coordinates.size()) {
                cerr << "Cannot construct MST";
                exit(1);
            }
        }
    }



    //////////////////////////////////////////////////
    // ADD_CLOSEST_VERTEX                           //
    //////////////////////////////////////////////////

    // Finds the smallest distance to an un-added vertex and discovers that vertex
    // Also updates the total weight of the MST

    // IDEA: Make vector of already added vertices to save runtime

    bool add_closest_vertex() {
        if (added_vertices.size() == 0) { // Automatically discover the first element in vertices if there are no added vertices
            discover_prim_vertex(0);
            return true;
        }
        size_t closest_vertex; // Keeps track of the index of the closest vertex (cheapest)
        double min_distance = numeric_limits<double>::infinity();
        for (size_t i = 0; i < vertices.size(); ++i) { // Loops through every vertex
            if (!vertices[i].added) {
                if (vertices[i].squared_distance < min_distance) { // Find the shortest edge to a new vertex
                    closest_vertex = i;
                    min_distance = vertices[i].squared_distance;
                }
            }
        }
        // Next vertex to be added has been found
        if (min_distance != numeric_limits<double>::infinity()) { // If a valid edge is found, return true
            discover_prim_vertex(closest_vertex);
            total_mst_weight += sqrt(min_distance);
            return true;
        }
        return false; // If no valid edge is found, return false
    }



    //////////////////////////////////////////////////
    // DISCOVER_PRIM_VERTEX                         //
    //////////////////////////////////////////////////

    // Modifies the parent, distance, and added members when a vertex is added to the MST with Prim's algorithm

    void discover_prim_vertex(size_t new_vertex) {
        vertices[new_vertex].added = true;
        //vertices[new_vertex].squared_distance = 0;
        added_vertices.push_back(new_vertex);
    }



    //////////////////////////////////////////////////
    // UPDATE_VERTEX_DISTANCE                       //
    //////////////////////////////////////////////////

    // Decreases the distance of each un-added vertex if the new distance is shorter as a result of that last-added vertex
    // Should be called right after every insertion

    void update_vertex_distance(size_t last_vertex) {
        for (size_t i = 0; i < vertices.size(); ++i) { // Checks all vertices to see if they need to be updated
            if (!vertices[i].added) {
                double new_distance = get_squared_distance(coordinates[i], coordinates[last_vertex]);
                if (new_distance < vertices[i].squared_distance) {
                    vertices[i].squared_distance = new_distance;
                    vertices[i].link = last_vertex;
                }
            }
        } 
    }



    //////////////////////////////////////////////////
    // GET_SQUARED_DISTANCE                         //
    //////////////////////////////////////////////////

    // Calculates the squared distance between two coordinates using the Euclidian distance formula without the sqrt.
    // The function returns the distance before applying the square root to save time. 
    // The distance between two points on opposite campuses is infinity.

    double get_squared_distance(Coordinate& c1, Coordinate& c2) {
        if ((c1.loc == Location::Med && c2.loc == Location::Main) ||
            (c1.loc == Location::Main && c2.loc == Location::Med)) {
            return numeric_limits<double>::infinity();
        }
        else { // Calculates using Euclidian distance
            double x1 = static_cast<double>(c1.x); // Converts to doubles to avoid integer overflow
            double y1 = static_cast<double>(c1.y);
            double x2 = static_cast<double>(c2.x);
            double y2 = static_cast<double>(c2.y);
            return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
        }
    }



    //////////////////////////////////////////////////
    // PRINT_MST_OUTPUT                             //
    //////////////////////////////////////////////////

    // Prints the output for MST mode

    void print_mst_output() {
        cout << total_mst_weight << "\n"; // Prints weight
        for (size_t i = 0; i < vertices.size(); ++i) { // Print the edges created by adding each of the vertices to the MST
            if (vertices[i].link != string::npos) { // There will be V - 1 edges
                if (i < vertices[i].link) { // Print in ascending index order
                    cout << i << " " << vertices[i].link << "\n";
                }
                else {
                    cout << vertices[i].link << " " << i << "\n";
                }
            }
        }
    }



    ///////////////////////////////////////////////////////////////////
    //   ooooooooo.                          .        oooooooooo.    //
    //   `888   `Y88.                      .o8        `888'   `Y8b   //
    //    888   .d88'  .oooo.   oooo d8b .o888oo       888     888   //
    //    888ooo88P'  `P  )88b  `888""8P   888         888oooo888'   //
    //    888          .oP"888   888       888         888    `88b   //
    //    888         d8(  888   888       888 .       888    .88P   //
    //   o888o        `Y888""8o d888b      "888"      o888bood8P'    //
    ///////////////////////////////////////////////////////////////////



    //////////////////////////////////////////////////
    // PRINT_TSP_OUTPUT                             //
    //////////////////////////////////////////////////



    void print_tsp_output() {
        double total_weight = 0;
        for (size_t i = 0; i < vertices.size(); ++i) {
            total_weight += sqrt(vertices[i].squared_distance);
        }
        cout << total_weight << "\n";
        size_t next_vertex = 0;
        for (size_t i = 0; i < vertices.size(); ++i) {
            cout << next_vertex << " ";
            next_vertex = vertices[next_vertex].link;
        }
        cout << "\n";
    }

    
    
    //////////////////////////////////////////////////
    // DO_FARTHEST_INSERTION                        //
    //////////////////////////////////////////////////

    // Use the arbitrary insertion heuristic to create 

    void do_farthest_insertion() {
        vertices.resize(coordinates.size());
        // First, initialize by choosing a "random" vertex (the first coordinates)
        discover_fasttsp_vertex(0);
        update_vertex_distances_fasttsp(0); // Updates farthest_vertex
        // Then, add the farthest vertex into the partial tour
        discover_fasttsp_vertex(farthest_vertex);
        set_edge(0, farthest_vertex);
        set_edge(farthest_vertex, 0);
        update_vertex_distances_fasttsp(farthest_vertex); // The last vertex inserted will be farthest_vertex, which has yet to be updated

        // In loop: Find farthest vertex from any added vertex, then insert in the least costly way
        for (size_t i = 0; i < coordinates.size() - 2; ++i) {
            insert_farthest(); // Insert within an existing edge
            discover_fasttsp_vertex(farthest_vertex); // Mark as added and push_back() into added_vertices
            update_vertex_distances_fasttsp(farthest_vertex); // The last vertex inserted will be farthest_vertex, which has yet to be updated
        }
        // The TSP should be created. The path can be traced by beginning at the first vertex and following the chain of link members.
    }



    //////////////////////////////////////////////////
    // INSERT_FARTHEST                              //
    //////////////////////////////////////////////////

    // Inserts the farthest vertex at the least possible cost  

    void insert_farthest() {
        double lowest_cost = numeric_limits<double>::infinity(); // Keeps track of the "cheapest" insertion cost
        size_t cheapest_edge_start = string::npos; // Keeps track of the starting vertex of the "cheapest" edge to insert into
        double cost;
        for (size_t i = 0; i < added_vertices.size(); ++i) { // "cost" = Cik + Ckj - Cij, where k is the new vertex and i->j is an existing edge
            cost = sqrt(get_euclidian_squared_distance(coordinates[added_vertices[i]], coordinates[farthest_vertex])) + // Cik + 
                   sqrt(get_euclidian_squared_distance(coordinates[farthest_vertex], coordinates[vertices[added_vertices[i]].link])) - // Ckj -
                   sqrt(vertices[added_vertices[i]].squared_distance); // Cij
            if (cost < lowest_cost) {
                lowest_cost = cost;
                cheapest_edge_start = added_vertices[i];
            }
        }
        // The cheapest insertion for the farthest vertex has been found by this point.
        size_t edge_end = vertices[cheapest_edge_start].link;
        // The two lines below turn one existing edge into two edges, which include the newly inserted vertex.
        set_edge(cheapest_edge_start, farthest_vertex); 
        set_edge(farthest_vertex, edge_end);
    }



    //////////////////////////////////////////////////
    // DISCOVER_FASTTSP_VERTEX                      //
    //////////////////////////////////////////////////

    // Same as discover_prim_vertex, but doesn't set distance to 0 because it will store the length of its edge

    void discover_fasttsp_vertex(size_t new_vertex) {
        vertices[new_vertex].added = true;
        added_vertices.push_back(new_vertex);
    }



    //////////////////////////////////////////////////
    // SET_EDGE                                     //
    //////////////////////////////////////////////////

    // Same as discover_prim_vertex, but doesn't set distance to 0 because it will store the length of its edge

    void set_edge(size_t v1, size_t v2) {
        vertices[v1].link  = v2;
        vertices[v1].squared_distance = get_euclidian_squared_distance(coordinates[v1], coordinates[v2]);
    }



    //////////////////////////////////////////////////
    // UPDATE_VERTEX_DISTANCE_FASTTSP               //
    //////////////////////////////////////////////////

    // Like the MST update_distance, except that it doesn't update the vertex.link and updates farthest_vertex

    void update_vertex_distances_fasttsp(size_t last_vertex) {
        farthest_vertex = string::npos;
        for (size_t i = 0; i < vertices.size(); ++i) { // Checks all vertices to see if they need to be updated
            if (!vertices[i].added) {
                double new_distance = get_euclidian_squared_distance(coordinates[i], coordinates[last_vertex]);
                if (new_distance < vertices[i].squared_distance) {
                    vertices[i].squared_distance = new_distance;
                }
                // Keep farthest_vertex updated (unless the distance is infinity)
                if (farthest_vertex == string::npos || new_distance > vertices[farthest_vertex].squared_distance) {
                    farthest_vertex = i;
                }
            }
        } 
    }



    //////////////////////////////////////////////////
    // GET_EUCLIDIAN_SQUARED_DISTANCE               //
    //////////////////////////////////////////////////

    // Ignores campus location

    double get_euclidian_squared_distance(Coordinate& c1, Coordinate& c2) {
        double x1 = static_cast<double>(c1.x); // Converts to doubles to avoid integer overflow
        double y1 = static_cast<double>(c1.y);
        double x2 = static_cast<double>(c2.x);
        double y2 = static_cast<double>(c2.y);
        return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
    }


    ///////////////////////////////////////////////////////////////////
    //   ooooooooo.                          .          .oooooo.     //
    //   `888   `Y88.                      .o8         d8P'  `Y8b    // 
    //    888   .d88'  .oooo.   oooo d8b .o888oo      888            // 
    //    888ooo88P'  `P  )88b  `888""8P   888        888            //
    //    888          .oP"888   888       888        888            //
    //    888         d8(  888   888       888 .      `88b    ooo    //
    //   o888o        `Y888""8o d888b      "888"       `Y8bood8P'    //
    ///////////////////////////////////////////////////////////////////                                                      
                                                          


    void create_opttsp() {
        curr_weight = 0;
        best_path.resize(coordinates.size());
        path.resize(coordinates.size());
        // Create a ditance matrix for fast access of distances between coordinates
        distmat.resize(coordinates.size());
        for (size_t i = 0; i < coordinates.size(); ++i) {
            distmat[i].resize(coordinates.size());
            for (size_t j = 0; j < coordinates.size(); ++j) {
                distmat[i][j] = sqrt(get_euclidian_squared_distance(coordinates[i], coordinates[j]));
            }
        }
        
        do_farthest_insertion();
        calc_tsp_estimate(); // Sets the intial best path to the one calculated by FASTTSP
        path = added_vertices; // Starts with optimal path, might reach pruning faster
        vertices.clear(); // Reset vertices so it can be used by MST
        added_vertices.clear(); // Reset for MST
        genPerms(1);

    }

    // Uses FASTTSP to create an initial currBest benchmark 

    void calc_tsp_estimate() {
        size_t next_vertex = 0;
        for (size_t i = 0; i < vertices.size(); ++i) {
            best_weight += distmat[i][vertices[i].link];
            best_path[i] = next_vertex;
            next_vertex = vertices[next_vertex].link;
        }
    }


    //////////////////////////////////////////////////
    // GEN_PERMS                                    //
    //////////////////////////////////////////////////


    
    void genPerms(size_t perm_length) {
        if (perm_length == path.size()) {
            calc_tsp_weight();
            return;
        }  // if ..complete path
        if (!promising(perm_length)) {
            return;
        }  // if ..not promising

        for (size_t i = perm_length; i < path.size(); ++i) {

            swap(path[perm_length], path[i]);
            curr_weight += distmat[path[perm_length]][path[perm_length - 1]]; // For comparison aginst lower bound

            genPerms(perm_length + 1);

            curr_weight -= distmat[path[perm_length]][path[perm_length - 1]]; // Undo changes
            swap(path[perm_length], path[i]);
        
        }  // for ..unpermuted elements
    }  // genPerms()



    bool promising(size_t perm_length) {
        if (path.size() - perm_length < 5) {
            return true;
        }
        total_mst_weight = 0;
        double closest_front = numeric_limits<double>::infinity();
        double closest_back = numeric_limits<double>::infinity();
        for (size_t i = perm_length; i < path.size(); ++i) {

            // Find the lowest possible length of arms connecting MST to TSP
            if (distmat[path[i]][path.front()] < closest_front) {
                closest_front = distmat[path[i]][path.front()];
            }
            if (distmat[path[i]][path[perm_length - 1]] < closest_back) {
                closest_back = distmat[path[i]][path[perm_length - 1]];
            }
        }
        total_mst_weight += closest_back + closest_front; // Add the length of the arms to the estimate
        if (total_mst_weight + curr_weight < best_weight) { // If the arms + curr_weight is already too expensive
            create_partial_mst(perm_length);
            double lower_bound = curr_weight + total_mst_weight;
            if (lower_bound > best_weight) {
                OptVertices.clear();
                added_vertices.clear();
                return false;
            }
            OptVertices.clear();
            added_vertices.clear();
            return true;
        }
        return false;
    }


    void calc_tsp_weight() {
        /*double weight = 0;
        for (size_t i = 0; i < path.size() - 1; ++i) {
            weight += distmat[path[i]][path[i + 1]];
        }
        weight += distmat[path.front()][path.back()];
        
        if (weight < best_weight) {
            best_weight = weight;
            best_path = path;
        }
        */
        if (curr_weight + distmat[path.front()][path.back()] < best_weight) {
            best_weight = curr_weight + distmat[path.front()][path.back()];
            best_path = path;
        }
        //curr_weight -= distmat[path.front()][path.back()];
    }

    
    // Creates a partial MST for lower bound estimate

    void create_partial_mst(size_t perm_length) {
        OptVertices.resize(path.size()); // Resize the vector of Prim vertices to the number of coordinates, filling it with default constructed Vertices.
        for (size_t i = perm_length; i < path.size(); ++i) {
            add_closest_vertex_opttsp(perm_length);
            update_vertex_distance_opttsp(perm_length);
        }
        added_vertices.clear();
    }


    void add_closest_vertex_opttsp(size_t perm_length) {
        if (added_vertices.empty()) { // Automatically discover the first element in vertices if there are no added vertices
            discover_prim_vertex_opttsp(path[perm_length]);
            return;
        }
        size_t closest_vertex = string::npos; // Keeps track of the index of the closest vertex (cheapest)
        double min_distance = numeric_limits<double>::infinity();
        for (size_t i = perm_length; i < path.size(); ++i) { // Loops through every vertex
            if (!OptVertices[path[i]].added) {
                if (OptVertices[path[i]].squared_distance < min_distance) { // Find the shortest edge to a new vertex
                    closest_vertex = path[i];
                    min_distance = OptVertices[path[i]].squared_distance;
                }
            }
        }
        // Next vertex to be added has been found
        if (min_distance != numeric_limits<double>::infinity()) { // If a valid edge is found, return true
            discover_prim_vertex_opttsp(closest_vertex);
            total_mst_weight += min_distance;
        }
    }



    void update_vertex_distance_opttsp(size_t perm_length) {
        for (size_t i = perm_length; i < path.size(); ++i) { // Checks all vertices to see if they need to be updated
            if (!OptVertices[path[i]].added) { // The index stored in path
                double new_distance = distmat[path[i]][added_vertices.back()];
                if (new_distance < OptVertices[path[i]].squared_distance) {
                    OptVertices[path[i]].squared_distance = new_distance;
                    //vertices[i].link = last_added;
                }
            }
        } 
    }



    void discover_prim_vertex_opttsp(size_t new_vertex) {
        OptVertices[new_vertex].added = true;
        //vertices[new_vertex].squared_distance = 0;
        added_vertices.push_back(new_vertex);
    }



    // Problem: Vertices will not be initialized to their corresponding coordinates because we don't know what order they will be inserted
    
    // Vertices vector should be size of path.size()
    // indices of vertices and added_vertices will match up
    // To get the coordinate of vertices[i], do added_vertices[i]


    void print_opttsp_output() {
        cout << best_weight << "\n";
        for (size_t i = 0; i < best_path.size(); ++i) {
            cout << best_path[i] << " ";
        }
        cout << "\n";
    }


    //////////////////////////////////////////////////
    // PRINT_HELP                                   //
    //////////////////////////////////////////////////

    // Prints help message in the event of -h

    void print_help() { 
        cout << "Help message\n";
    }



    //////////////////////////////////////////////////
    // MEMBER VARIABLES/CONTAINERS                  //
    //////////////////////////////////////////////////

    vector<vector<double>> distmat;
    vector<Coordinate> coordinates; // The order will be kept the same as when they are read in
    vector<Vertex> vertices;
    vector<OptVertex> OptVertices;
    vector<size_t> added_vertices;
    vector<size_t> best_path;
    vector<size_t> path;
    double total_mst_weight = 0;
    double best_weight = 0;
    double curr_weight = 0;
    Mode mode;
    size_t farthest_vertex = string::npos;
};



int main(int argc, char *argv[]) {
    cout << setprecision(2); //Always show 2 decimal places
    cout << std::fixed; //Disable scientific notation for large numbers
    std::ios_base::sync_with_stdio(false);
    Map map;
    map.get_options(argc, argv);
    map.run_program();
    
    return 0;
}


