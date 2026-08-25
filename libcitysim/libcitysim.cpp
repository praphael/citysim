// libcitysim.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "libcitysim.h"

std::string Pt2DToString(const Pt2D& pt) {
    return "(" + std::to_string(pt.x/100000.0f) + " km, " + std::to_string(pt.y / 100000.0f) + " km)";
}

int calcLotIdxFromRowCol(const int row, const int col, const int NUM_LOT_DIVS) {
	return row * NUM_LOT_DIVS + col;
}

Pt2D calcRowColFromLotIdx(const int lot_idx, const int NUM_LOT_DIVS) {
	Pt2D rc;
	rc.x = lot_idx % NUM_LOT_DIVS; // col
	rc.y = lot_idx / NUM_LOT_DIVS; // row
	return rc;
}

Pt2D calcCenter(const Lot& l, const std::vector<Pt2D>& pts) {
    Pt2D ctr;
    ctr.x = 0;
    ctr.y = 0;

    auto sz = 0;
    for (auto pt_idx : l.pt_idxs) {
        //std::cout << "pt_idx=" << pt_idx << std::endl;
        auto pt = pts[pt_idx];
        //std::cout << "pt.x=" << pt.x << " pt.y=" << pt.y << std::endl;
        sz += 1;
        ctr.x += pt.x;
        ctr.y += pt.y;
    }
    ctr.x /= sz;
    ctr.y /= sz;
    return ctr;
}

// square of distance
long long calcSqDist(const Pt2D& pt1, const Pt2D& pt2) {
    auto dx = static_cast<long long>(pt1.x - pt2.x);
    auto dy = static_cast<long long>(pt1.y - pt2.y);
    // std::cout << "dx= " << dx << " dy= " << dy << "dx^2= " << dx*dx << " dy^2= " << dy*dy << std::endl;
    return dx * dx + dy * dy;
}

LIBCITYSIM_API void makePoints(const long long BOUNDS_CENTIMETERS,
    const int NUM_LOT_DIVS,
    std::vector<Pt2D>& pts) {
    // need extra row and column to store boundary points
    auto sz = (NUM_LOT_DIVS + 1) * (NUM_LOT_DIVS + 1);

    std::cout << "Reserving points array sz=" << sz << std::endl;
    pts.reserve(sz);
    std::cout << "Generating points" << std::endl;
    // generate points to be used for the lot divisions
    // start in lower-left (southwest), moving west-east then north
    for (auto y_i = 0; y_i <= NUM_LOT_DIVS; y_i++) {
        auto y = y_i * BOUNDS_CENTIMETERS / NUM_LOT_DIVS - (BOUNDS_CENTIMETERS / 2);
        for (auto x_i = 0; x_i <= NUM_LOT_DIVS; x_i++) {
            auto x = x_i * BOUNDS_CENTIMETERS / NUM_LOT_DIVS - (BOUNDS_CENTIMETERS / 2);
            //if(y_i < 2) 
            //    std::cout << x_i << " " << y_i << " -> " << x << "," << y << std::endl;
            pts.push_back({ x, y });
            
        }
    }
}



LIBCITYSIM_API void makeTestLots(const int NUM_LOT_DIVS, 
                                 const std::vector<Pt2D>& pts,
                                 std::vector<Lot>& lots) {
    auto sz = NUM_LOT_DIVS * NUM_LOT_DIVS;
    std::cout << "Reserving lots array sz=" << sz << std::endl;
    lots.reserve(sz);
    std::cout << "Generating lots" << std::endl;
    auto NCHARS_NUM_LOT_DIVS = static_cast<int>(std::log10(NUM_LOT_DIVS)) + 1;
    // generate square lots from the points
    // starting in lower-left (southwest), moving west-east then north
    for (auto y_i = 0; y_i < NUM_LOT_DIVS; y_i++) {
        if (y_i % 10 == 0) {
            std::cout << y_i << "/" << NUM_LOT_DIVS;
            auto nchars = NCHARS_NUM_LOT_DIVS + 1;
            (y_i == 0) ? nchars += 1 : nchars += static_cast<int>(std::log10(y_i)) + 1;
            // erase previous line
            for (int i = 0; i < nchars; i++)
                std::cout << static_cast<char>(8);
        }

        for (auto x_i = 0; x_i < NUM_LOT_DIVS; x_i++) {
            auto lot = Lot{};
            // add 1 because n+1 points per row
            auto pt_idx = y_i * (NUM_LOT_DIVS + 1) + x_i;
            //if (y_i < 2)
            //    std::cout << x_i << ", " << y_i << " -> " << Pt2DToString(pts.at(pt_idx)) << std::endl;
            // 
            // auto lot_idx = y_i * NUM_LOT_DIVS + x_i;
			auto lot_idx = calcLotIdxFromRowCol(y_i, x_i, NUM_LOT_DIVS);

            if (lot_idx != lots.size()) {
                std::cout << "ERROR: lot_idx " << lot_idx << " != lots.size() " << lots.size() << std::endl;
                std::exit(-1);
            }

            lot.type = LOT_TYPE::EMPTY;
            // points generated clockwise from lower left (southwest)
            lot.pt_idxs[0] = pt_idx;
            lot.pt_idxs[1] = pt_idx + NUM_LOT_DIVS + 1;
            lot.pt_idxs[2] = pt_idx + NUM_LOT_DIVS + 2;
            lot.pt_idxs[3] = pt_idx + 1;

            auto pt1 = pts.at(lot.pt_idxs[0]);
            auto pt2 = pts.at(lot.pt_idxs[1]);
            auto pt3 = pts.at(lot.pt_idxs[2]);
            auto pt4 = pts.at(lot.pt_idxs[3]);

            auto dy = pt2.y - pt1.y;
            auto dx = pt3.x - pt2.x;

            // ensure square lots
            if (pt1.x != pt2.x) {
                std::cout << "ERROR: pt1.x != pt2.x" << std::endl; std::exit(-1);
            }
            if (pt1.y != pt4.y) {
                std::cout << "ERROR: pt1.y != pt4.y" << std::endl; std::exit(-1);
            }
            if (pt2.y != pt3.y) {
                std::cout << "ERROR: pt2.y != pt3.y" << std::endl; std::exit(-1);
            }
            if (pt3.x != pt4.x) {
                std::cout << "ERROR: pt3.x != pt4.x" << std::endl; std::exit(-1);
            }
            if (pt4.x - pt1.x != dx) {
                std::cout << "ERROR: pt4.x - pt1.x != dx" << std::endl; std::exit(-1);
            }
            if (pt3.y - pt4.y != dy) {
                std::cout << "ERROR: pt3.y - pt4.y != dy" << std::endl;
                std::cout << "\t dy= " << dy << " pt3.y - pt4.y= " << pt3.y - pt4.y << std::endl;
                std::exit(-1);
            }


            /*
            std::cout << "pt_idx= " << pt_idx;
            std::cout << " (" << pts.at(lot.pt_idxs[0]).x << ", " << pts.at(lot.pt_idxs[0]).y << ")";
            std::cout << " (" << pts.at(lot.pt_idxs[1]).x << ", " << pts.at(lot.pt_idxs[1]).y << ")";
            std::cout << " (" << pts.at(lot.pt_idxs[2]).x << ", " << pts.at(lot.pt_idxs[2]).y << ")";
            std::cout << " (" << pts.at(lot.pt_idxs[3]).x << ", " << pts.at(lot.pt_idxs[3]).y << ")";
            std::cout << std::endl;
            */
            // left/east neighbor
            if (x_i > 0) {
                auto ngbr_idx = calcLotIdxFromRowCol(y_i, x_i - 1, NUM_LOT_DIVS);
                lot.ngbrs.push_back({ ngbr_idx, -1 });
				if(lots.at(ngbr_idx).pt_idxs[3] != lot.pt_idxs[0] || lots.at(ngbr_idx).pt_idxs[2] != lot.pt_idxs[1]) {
					std::cout << "ERROR: left neighbor point does not match" << std::endl;
					std::exit(-1);
				}
            }
            // right/west neighbor
            if (x_i < (NUM_LOT_DIVS - 1)) {
                auto ngbr_idx = calcLotIdxFromRowCol(y_i, x_i + 1, NUM_LOT_DIVS);
                lot.ngbrs.push_back({ ngbr_idx, -1 });
            }

            // south/low neighbor
            if (y_i > 0) {
				auto ngbr_idx = calcLotIdxFromRowCol(y_i - 1, x_i, NUM_LOT_DIVS);
                lot.ngbrs.push_back({ ngbr_idx, -1 });
                if (lots.at(ngbr_idx).pt_idxs[1] != lot.pt_idxs[0] || lots.at(ngbr_idx).pt_idxs[2] != lot.pt_idxs[3]) {
                    std::cout << "ERROR: south neighbor point does not match" << std::endl;
                    std::exit(-1);
                }
            }
            // north/up neighbor            
            if (y_i < (NUM_LOT_DIVS - 1)) {
                auto ngbr_idx = calcLotIdxFromRowCol(y_i + 1, x_i, NUM_LOT_DIVS);
                lot.ngbrs.push_back({ ngbr_idx, -1 });
            }
            lots.push_back(lot);
        }
    }
}

LIBCITYSIM_API void makeTestRoads(const int NUM_ROADS, 
                                  const int NUM_LOT_DIVS,
                                  const std::vector<Pt2D>& pts, 
                                  std::vector<Lot>& lots, 
                                  std::vector<std::vector<int>>& roads) {
    // seed random number generator
    srand((unsigned)time(0));
    auto n_intersect = 0;

    for (int n_road = 0; n_road < NUM_ROADS; n_road++) {
        // pick two random lots at border (edge of map) to connect with a road
        // generate two random nums to represent the offset on border square
        // calculate approprirate lot number for west-east or south-north
        // given that lots are generated starting in southeast corner for lot 0, filling in rows west-east 
        auto offsetBegin = rand() % NUM_LOT_DIVS;
        auto offsetEnd = rand() % NUM_LOT_DIVS;
        auto lotBegin = offsetBegin; // west border
        auto lotEnd = offsetEnd;
        std::vector<int> road;

        // orientation east-west or north-south
        // auto orient = rand()%2;
        auto orient = n_road % 2;
        // west-east case
        if (orient) {
			lotBegin = calcLotIdxFromRowCol(offsetBegin, 0, NUM_LOT_DIVS); // west border
            lotEnd = calcLotIdxFromRowCol(offsetEnd, NUM_LOT_DIVS - 1, NUM_LOT_DIVS); // west border
        }
		// south-north case
        else {
            lotBegin = calcLotIdxFromRowCol(0, offsetBegin, NUM_LOT_DIVS); // west border
            lotEnd = calcLotIdxFromRowCol(NUM_LOT_DIVS-1, offsetEnd, NUM_LOT_DIVS); // west border
        }
        auto ctr1 = calcCenter(lots.at(lotBegin), pts);
        auto ctr2 = calcCenter(lots.at(lotEnd), pts);
        std::cout << std::endl << "orient= " << orient << " lotBegin= " << lotBegin << " lotEnd=" << lotEnd;
        std::cout << std::endl << "ctr1= " << Pt2DToString(ctr1) << " ctr2=" << Pt2DToString(ctr2) << std::endl;
        std::cout << std::endl << "generating road" << std::endl;

        // generate path from lotBegin to lotEnd
        auto lotIdx = lotBegin;
        auto lotIdx_prev = -1;
        auto minIdx_prev = -1;
        std::unordered_set<int> visited;
        auto cnt = 1000;
        auto path_len = 0;
        while (1) {
            auto l = lots.at(lotIdx);
            auto c1 = calcCenter(l, pts);
            auto dist = calcSqDist(c1, ctr2);
			// std::cout << Pt2DToString(c1) << " dist^2= " << dist << std::endl;
            //std::cout << "r= " << r << " c1= (" << c1.x << ", " << c1.y << ") dist= " << dist << std::endl;            
            road.push_back(lotIdx);
            visited.emplace(lotIdx);

            //std::cout << "min_idx= " << min_idx << " r_nxt= " << r_nxt << std::endl;
            
			// if this lot is already a path, then make it an intersection and update costs for adjacent roads
            if (l.type == LOT_TYPE::PATH) {
                // make this an intersection
                lots.at(lotIdx).type = INTERSECT;

                // update costs for roads adjacent at the intersection
				for (auto ngbr_idx = 0; ngbr_idx < l.ngbrs.size(); ngbr_idx++) {

                    auto ngbr_lot_idx = std::get<0>(l.ngbrs.at(ngbr_idx));
					auto ngbr = lots.at(ngbr_lot_idx);
                    if (ngbr.type == LOT_TYPE::PATH || ngbr.type == LOT_TYPE::INTERSECT) {
                        // update cost 
                        lots.at(lotIdx).ngbrs[ngbr_idx] = { ngbr_lot_idx, 1 };

                        // find index of this lot in the neighbor lot
                        // and upcate cost 
                        for (auto ngbr2_idx = 0; ngbr2_idx < ngbr.ngbrs.size(); ngbr2_idx++) {
                            auto ngbr2 = std::get<0>(ngbr.ngbrs.at(ngbr2_idx));
                            if (ngbr2 == lotIdx) {
                                ngbr.ngbrs[ngbr2_idx] = { lotIdx, 1 };
                                break;
                            }
                        }
                    }
				}
                
                n_intersect++;
            }
            else
                lots.at(lotIdx).type = PATH;

			if (lotIdx == lotEnd) {
				std::cout << "Reached lotEnd" << std::endl;
				break;
			}

            auto min_idx = -1;
            long long min_d = -1;

            //std::cout << "\tc1=  " << "(" << c1.x << ", " << c1.y << ")" << std::endl;
            for (auto idx = 0; idx < l.ngbrs.size(); idx++) {
                auto ngbr = l.ngbrs.at(idx);
                auto ngbr_lot_idx = std::get<0>(ngbr);
                //std::cout << "\tngbr_lot_idx= " << ngbr_lot_idx << " min_d= " << min_d << std::endl;
                // skip if we have already been here
                if (visited.count(ngbr_lot_idx) != 0)
                    continue;
                auto ctr = calcCenter(lots.at(ngbr_lot_idx), pts);
                auto d = calcSqDist(ctr, ctr2);
                //std::cout << "\td^2=  " << d << " ctr= (" << ctr.x << ", " << ctr.y << ")" << std::endl;
                if (min_idx < 0 || d < min_d) {
                    min_d = d;
                    min_idx = idx;
                }
            }
            if (min_idx < 0) {
                std::cout << "Could not complete path to lotEnd, aborting" << std::endl;
                break;
            }

            //std::cout << "min_idx= " << min_idx << std::endl;
            auto closest = l.ngbrs[min_idx];
            auto lotIdx_nxt = std::get<0>(closest);

            lots.at(lotIdx).ngbrs[min_idx] = { lotIdx_nxt, 1 };

            // set traversal cost for next lot back to this lot
            auto lot_nxt = lots.at(lotIdx_nxt);
            for (auto i = 0; i < lot_nxt.ngbrs.size(); i++) {
                auto ngbr = lots.at(lotIdx_nxt).ngbrs[i];
                if (std::get<0>(ngbr) == lotIdx) {
                    lots.at(lotIdx_nxt).ngbrs[i] = { lotIdx, 1 };
                    break;
                }
            }

            lotIdx_prev = lotIdx;
            lotIdx = lotIdx_nxt;
            

            /* cnt--;
            if (cnt <= 0)
                break; */
            path_len += 1;
        }
        std::cout << "path_len=" << path_len << std::endl;
        roads.push_back(road);
    }
    std::cout << "intersections: " << n_intersect << std::endl;
}

LIBCITYSIM_API void findMinPath(const std::vector<Lot>& lots, int start_lot_idx, int end_lot_idx) {
    auto cmp_fcn = [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
        return a.second > b.second; // min-heap based on cost
        };
    std::priority_queue < std::pair<int, float>, std::vector<std::pair<int, float>>, decltype(cmp_fcn) > q(cmp_fcn);
    std::unordered_set<int> visited;
    std::unordered_map<int, int> prev;
    q.push({ start_lot_idx, 0.0f });
    visited.insert(start_lot_idx);
    while (!q.empty()) {
        int current = q.top().first;
        int current_cost = q.top().second;
        q.pop();
        if (current == end_lot_idx) {
            // Reconstruct path
            std::vector<int> path;
            for (int at = end_lot_idx; at != start_lot_idx; at = prev[at]) {
                path.push_back(at);
            }
            path.push_back(start_lot_idx);
            std::reverse(path.begin(), path.end());
            std::cout << "Path found: ";
            //for (int lot : path) {
            //	std::cout << lot << " ";
            //}
            std::cout << "Total cost:" << current_cost << std::endl;
            return;
        }
        for (const auto& ngbr : lots[current].ngbrs) {
            auto cst = std::get<1>(ngbr);
            if (cst < 0) continue; // Skip if not a valid path 
            int ngbr_idx = std::get<0>(ngbr);
            if (visited.find(ngbr_idx) == visited.end()) {
                visited.insert(ngbr_idx);
                prev[ngbr_idx] = current;
                q.push({ ngbr_idx, current_cost + cst });
            }
        }
    }
    std::cout << "No path found from " << start_lot_idx << " to " << end_lot_idx << std::endl;
}
LIBCITYSIM_API void testRoadPaths(const std::vector<Lot>& lots, const std::vector<std::vector<int>>& roads, int n_tests) {

    for (int i = 0; i < n_tests; i++) {
        // random point on two roads
        auto r1 = rand() % roads.size();
        auto r2 = -1;
        // ensure different roads
        while (r2 == -1 || r2 == r1) {
            r2 = rand() % roads.size();
        }
        auto rd1 = roads.at(r1);
        auto rd2 = roads.at(r2);
        auto pt1 = rd1.at(rand() % rd1.size());
        auto pt2 = rd2.at(rand() % rd2.size());

        std::cout << "road " << i << " size= " << roads.at(i).size() << std::endl;
        findMinPath(lots, pt1, pt2);
        std::cout << std::endl;
    }
}


// This is an example of an exported variable
LIBCITYSIM_API int nlibcitysim = 0;

// This is an example of an exported function.
LIBCITYSIM_API int fnlibcitysim(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
Clibcitysim::Clibcitysim()
{

    return; 


}
