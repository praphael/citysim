#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <cstdlib>
#include <ctime> 
#include <unordered_set>
#include <unordered_map>
#include <queue> // for priority_queue

// constexpr long long BOUNDS_CENTIMETERS = 80 * 1000 * 100; // 80 km
constexpr long long BOUNDS_CENTIMETERS = 80 * 1000 * 100; // 80 km
constexpr int LOT_SIZE_M = 10;
constexpr long long NUM_LOT_DIVS = (BOUNDS_CENTIMETERS / 100) / LOT_SIZE_M;
constexpr int NUM_ROADS = 10;

#include "..\libcitysim\libcitysim.h"

int main() {
    std::vector<Pt2D> pts;
    std::vector<Lot> lots;
	std::vector<std::vector<int>> roads;
	makePoints(BOUNDS_CENTIMETERS, NUM_LOT_DIVS, pts);
    makeTestLots(NUM_LOT_DIVS, pts, lots);
    makeTestRoads(NUM_ROADS, NUM_LOT_DIVS, pts, lots, roads);
    testRoadPaths(lots, roads, 5);
}