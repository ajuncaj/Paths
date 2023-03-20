// Project identifier: 5949F553E20B650AB0FB2266D3C0822B13D248B0
#include <algorithm>
#include <cassert>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <math.h>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <getopt.h>
#include <iomanip>
#include <cstring>

using namespace std;


struct Pokemon;
vector<Pokemon> get_input();
void MST(vector<Pokemon>& mons);
void FASTTSP(vector<Pokemon>& mons);


void get_options(int argc, char** argv);

int main(int argc, char** argv) {
    cout << std::setprecision(2); // Always show 2 decimal places
    cout << std::fixed;           // Disable scientific notation for large numbers
    
    ios_base::sync_with_stdio(false); // comment out if you use valgrind
    get_options(argc, argv);
    
    return 0;
}


void get_options(int argc, char** argv) {
    int option_index = 0, option = 0;
    // Don't display getopt error messages about options
    opterr = false;

    // use getopt to find command line options
    struct option longOpts[] = { { "help",  no_argument, nullptr, 'h' },
                                 { "mode",  required_argument, nullptr, 'm' },
                                 { nullptr, 0, nullptr, '\0' } };


    while ((option = getopt_long(argc, argv, "hm:", longOpts, &option_index)) != -1) {
        switch (option) {

        case 'h':
            cout << "Hello Pokemon master! This program will use various algorithms to find the optimal path to all the Pokemon "
                 << "in the region so that you can complete your PokeDex!\n"
                 << "ARGUMENTS: \n"
                 << "-m, --mode: \n"
                 << "Set the program mode to MODE. MODE must be one of MST or FASTTSP. "
                 << "The MODE corresponds to the algorithm poke runs (and therefore what it outputs).\n";
            exit(0);
        case 'm':
            if (optarg[0] == 'M') {
                vector<Pokemon> mons = get_input();
                MST(mons);
            }
            else if (optarg[0] == 'F') {
                vector<Pokemon> mons = get_input();
                FASTTSP(mons);
            }
            else {
                cerr << "<MODE> specified in --mode (-m) not recognized. Please input one of : MST, FASTTSP\n";
                exit(1);
            }
            break;
        }
    }
}

enum class Terrain { Land, Sea, Coast, Visited };

struct Pokemon {
    int x;
    int y;
    Terrain terrain; // For MST mode only.
};

struct Prims {
    bool visited = false;
    double distance = numeric_limits<double>::infinity();
    int parent = -1;
};

Terrain check_terrain(int x, int y) {
    if (x < 0 || y < 0) {
        if (x < 0 && y < 0) {
            return Terrain::Sea;
        }
        else if (x == 0 || y == 0) {
            return Terrain::Coast;
        }
        else {
            return Terrain::Land;
        }
    }
    else {
        return Terrain::Land;
    }
}

vector<Pokemon> get_input() {
    vector<Pokemon> mons;
    int M;
    int x;
    int y;

    cin >> M;
    mons.reserve(M);
    for (int i = 0; i < M; i++) {
        cin >> x >> y;
        Pokemon newMon = { x, y, check_terrain(x, y) };
        mons.push_back(newMon);
    }

    return mons;
}

// Returns the index of the smallest, non-visited pokemon.
int getMin(vector<Prims>& primsIn) {
    int currentIndex = 0;
    double currentDistance = numeric_limits<double>::infinity();
    
    for (size_t i = 0; i < primsIn.size(); i++) {
        if (!primsIn[i].visited && primsIn[i].distance < currentDistance) {
            currentDistance = primsIn[i].distance;
            currentIndex = static_cast<int>(i);
        }
    }
    
    return currentIndex;
}



double eulc_distance(Pokemon& start, Pokemon& end) {
    if ((start.terrain == Terrain::Sea && end.terrain == Terrain::Land) || 
        (start.terrain == Terrain::Land && end.terrain == Terrain::Sea)) {
        return numeric_limits<double>::infinity();
    }
    else {
        double x1 = static_cast<double>(start.x);
        double x2 = static_cast<double>(end.x);
        double y1 = static_cast<double>(start.y);
        double y2 = static_cast<double>(end.y);

        return sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));
    }
}

double eulc_distance_TSP(Pokemon& start, Pokemon& end) {
    double x1 = static_cast<double>(start.x);
    double x2 = static_cast<double>(end.x);
    double y1 = static_cast<double>(start.y);
    double y2 = static_cast<double>(end.y);

    return sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));
}

double eulc_distance_delayed(Pokemon& start, Pokemon& end) {
    double x1 = static_cast<double>(start.x);
    double x2 = static_cast<double>(end.x);
    double y1 = static_cast<double>(start.y);
    double y2 = static_cast<double>(end.y);

    return ((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1));
}

bool check_MST(const vector<Pokemon>& mons) {
    bool MSTable = true;
    bool hasSea = false;
    bool hasShore = false;
    bool hasLand = false;

    for (size_t i = 0; i < mons.size(); i++) {
        if (mons[i].terrain == Terrain::Sea) {
            hasSea = true;
        }
        else if (mons[i].terrain == Terrain::Land) {
            hasLand = true;
        }
        else {
            hasShore = true;
        }
    }

    if (hasLand && hasSea && !hasShore) {
        MSTable = false;
    }

    return MSTable;
}

// Main driver for MST mode.
void MST (vector<Pokemon>& mons) {
    if (!check_MST(mons)) {
        cerr << "Cannot construct MST";
        exit(1);
    }
    
    
    // Initialize 'table'
    vector<Prims> primVec;
    primVec.resize(mons.size());
    Prims def;
    fill(primVec.begin(), primVec.end(), def);

    primVec[0].distance = 0;

    int currentMon = 0;
    double distance = 0;
    for (size_t j = 0; j < primVec.size(); j++) {
        currentMon = getMin(primVec); // step one, find smallest distance
        primVec[currentMon].visited = true; // step two
        for (size_t i = 0; i < primVec.size(); i++) { // step 3
            distance = eulc_distance(mons[currentMon], mons[i]);
            if (distance < primVec[i].distance && !primVec[i].visited) {
                primVec[i].distance = distance;
                primVec[i].parent = currentMon;
            }
        }
        
    }

    // Add distances
    double finalDist = 0;
    for (size_t i = 0; i < primVec.size(); i++) {
        finalDist += primVec[i].distance;
    }
    cout << finalDist << '\n';
    // Add verts
    for (size_t i = 0; i < primVec.size(); i++) {
        if (primVec[i].parent == -1) {
            continue;
        }
        else if (primVec[i].parent < static_cast<int>(i)) {
            cout << primVec[i].parent << " " << i << '\n';
        }
        else {
            cout << i << " " << primVec[i].parent << '\n';
        }
    }
    return;
}

// returns the index of the nearest, unvisited neighbor
int nearest_neighbor(vector<Pokemon>& mons, int start) {
    double minDistance = numeric_limits<double>::infinity();
    int nearestNeighbor = 0;

    for (size_t i = 0; i < mons.size(); i++) {
        if (mons[i].terrain != Terrain::Visited) {
            double dist = eulc_distance_delayed(mons[start], mons[i]);
            if (dist < minDistance) {
                nearestNeighbor = static_cast<int>(i);
                minDistance = dist;
            }
        }
    }

    return nearestNeighbor;
}


void two_opt(vector<int>& currentRoute, vector<Pokemon>& mons) {
    
    int size = static_cast<int>(currentRoute.size());
    
    for (int i = 0; i < size - 1; i++) { // Fix conditions later
        for (int j = i + 2; j < size; j++) {
            double currentDist = 0;
            double newDist = 0;

            // Get current distance, get new distance
            if (j == size - 1) { // Case where you need to wrap around
                currentDist = eulc_distance_delayed(mons[currentRoute[i]], mons[currentRoute[i + 1]]) +
                    eulc_distance_delayed(mons[currentRoute[j]], mons[currentRoute[0]]);

                newDist = eulc_distance_delayed(mons[currentRoute[i]], mons[currentRoute[j]]) +
                    eulc_distance_delayed(mons[currentRoute[i + 1]], mons[currentRoute[0]]);

            }
            else {
                currentDist = eulc_distance_delayed(mons[currentRoute[i]], mons[currentRoute[i + 1]]) +
                    eulc_distance_delayed(mons[currentRoute[j]], mons[currentRoute[j + 1]]);

                newDist = eulc_distance_delayed(mons[currentRoute[i]], mons[currentRoute[j]]) +
                    eulc_distance_delayed(mons[currentRoute[i + 1]], mons[currentRoute[j + 1]]);

            }


            if (newDist < currentDist) { // If better, then swap
                reverse(currentRoute.begin() + i + 1, currentRoute.begin() + j + 1);
                return;
            }
        }
    }
}

double route_dist(vector<int>& route, vector<Pokemon>& mons) {
    double dist = 0;
    for (size_t i = 0; i < route.size(); i++) {
        if (i == route.size() - 1) {
            dist += eulc_distance_TSP(mons[route[i]], mons[route[0]]);
        }
        else {
            dist += eulc_distance_TSP(mons[route[i]], mons[route[i + 1]]);
        }
    }

    return dist;
}

// Runs the algorithm for FASTTSP, part B
// V1.0 - Greedy nearest neighbor + 2-opt
// Build your route using greedy nearest neighbor first, then
// go back over with 2-opt to make it better
void FASTTSP(vector<Pokemon>& mons) {
    vector<int> route;
    route.reserve(mons.size());

    route.push_back(0);
    mons[0].terrain = Terrain::Visited;

    for (size_t i = 1; i < mons.size(); i++) { // Check all neighbors, connect, repeat starting with new connection
        int nearest = nearest_neighbor(mons, static_cast<int>(i) - 1); 
        mons[nearest].terrain = Terrain::Visited;
        route.push_back(nearest);
    }
    
    two_opt(route, mons);
    
    cout << route_dist(route, mons) << '\n';
    for (size_t i = 0; i < route.size(); i++) {
        cout << route[i] << " ";
    }
}

