//1761414855B69983BD8035097EFBD312EB0527F0
#ifndef SILLY_H
#define SILLY_H

#include <limits>
#include <string>


enum Mode { MST, FASTTSP, OPTTSP };

enum Location { Med, MedBorder, Main }; // Speed up distance calculations

struct Coordinate { 
    Coordinate() 
     : x(0), y(0), loc(Location::Main) {}
    int x; int y; Location loc; 
};

struct Vertex {
    bool added = false;
    size_t link = std::string::npos;
    double squared_distance = std::numeric_limits<double>::infinity(); // For OPTMST, use OPTVertex
};

struct OptVertex {
    bool added = false;
    double squared_distance = std::numeric_limits<double>::infinity(); // For OPTMST
};

/*struct OPTVertex {
    bool added = false;
    double distance = std::numeric_limits<double>::infinity(); // For OPTMST, this value represents the true distance (true value stored in distmat)
};*/


#endif 
