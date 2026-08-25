// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the LIBCITYSIM_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// LIBCITYSIM_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LIBCITYSIM_EXPORTS
#define LIBCITYSIM_API __declspec(dllexport)
#else
#define LIBCITYSIM_API __declspec(dllimport)
#endif

#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <cstdlib>
#include <ctime> 
#include <unordered_set>
#include <unordered_map>
#include <queue> // for priority_queue


struct Pt2D {
    long long x, y;
};

enum LOT_TYPE { EMPTY, PATH, INTERSECT, RES, COM, IND };

struct Lot {
    LOT_TYPE type; // type of lot (residential, commercial, path clearance, etc.)
    // index into points array
    int pt_idxs[4];
    // ngbrs index and cost
    std::vector<std::tuple<int, float>> ngbrs; // neighbors
};

LIBCITYSIM_API void makePoints(const long long BOUNDS_CENTIMETERS,  const int NUM_LOT_DIVS, std::vector<Pt2D>& pts);
LIBCITYSIM_API void makeTestLots(const int NUM_LOT_DIVS, const std::vector<Pt2D>& pts, std::vector<Lot>& lots);
LIBCITYSIM_API void makeTestRoads(const int NUM_ROADS, const int NUM_LOT_DIVS, const std::vector<Pt2D>& pts, std::vector<Lot>& lots, std::vector<std::vector<int>>& roads);
LIBCITYSIM_API void findMinPath(const std::vector<Lot>& lots, int start_lot_idx, int end_lot_idx);
LIBCITYSIM_API void testRoadPaths(const std::vector<Lot>& lots, const std::vector<std::vector<int>>& roads, int n_tests);

// This class is exported from the dll
class LIBCITYSIM_API Clibcitysim {
public:
	Clibcitysim(void);
	// TODO: add your methods here.
};

extern LIBCITYSIM_API int nlibcitysim;

LIBCITYSIM_API int fnlibcitysim(void);
