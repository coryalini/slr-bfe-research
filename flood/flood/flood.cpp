//
//  flood.cpp
//  slr_bfe
//
//  Created by Cory Alini on 6/23/17.
//  Copyright © 2017 slr_bfe.coryalini. All rights reserved.
//

#include "flood.hpp"
#include <iostream>
#include <queue>
#include <limits>
#include <assert.h>


//SLR
/*
 This function is used to start the slr function. It also sets the floodgrid to havent visited
 */

void start_slr(Grid* elevgrid, Grid* floodgrid, float rise) {
    for (int i = 0; i < elevgrid->nrows; i++) {
        for (int j = 0; j < elevgrid->ncols; j++) {
            floodgrid->data[i][j] = HAVENT_VISITED;
        }
    }
    
    std::queue<point> queue;
    findBoundarySeaPoint(elevgrid, &queue);
    compute_slr(elevgrid, floodgrid, rise, queue);
    setNotVisited(elevgrid, floodgrid, rise);
    
}


void compute_slr(Grid* elevgrid, Grid* floodgrid,float rise, std::queue<point>& queue) {
    
    while(queue.empty() != true) {
        point curr = queue.front();
        queue.pop();
        
        //for each neighbor of (i,j)
        for (int k = curr.x-1; k <= curr.x+1; k++) {
            for (int l = curr.y-1; l <= curr.y+1;l++) {
                if (k == curr.x && l == curr.y) {
                    continue;
                }
                if (!insideGrid(elevgrid, k,l)) {
                    continue;
                }
                if (floodgrid->data[k][l] == HAVENT_VISITED) {// not visited
                    point newPoint;
                    newPoint.x = k;
                    newPoint.y = l;
                    if (elevgrid->data[k][l] <=  SEA_LEVEL || elevgrid->data[k][l] == elevgrid->NODATA_value|| elevgrid->data[k][l] ==NEW_WATER) { // if the data is water
                        floodgrid->data[k][l] = elevgrid->NODATA_value;
                        queue.push(newPoint);
                    } else {
                        if (elevgrid->data[k][l] < rise+SEA_LEVEL) {
                            floodgrid->data[k][l] = NEW_WATER;
                            queue.push(newPoint);
                        }
                    }
                }
            }
        }
    }
}

/*
 This function is called by start slr. It goes through each neighbor of whatever point is popped 
 by the queue. Then is the point hadnt been visited, then the function checks if the point is 
 no data value or new water then it just adds it to the queue and sets the floodgrid. Otherwise
 if the point is less than the rise, it sets it to new_water and pushes it to the queue.
 */


//void compute_slr(Grid* elevgrid, Grid* floodgrid,float rise, std::queue<point>& queue) {
//    
//    while(queue.empty() != true) {
//        point curr = queue.front();
//        queue.pop();
//        
//        //for each neighbor of (i,j)
//        for (int k = curr.x-1; k <= curr.x+1; k++) {
//            for (int l = curr.y-1; l <= curr.y+1;l++) {
//                if (k == curr.x && l == curr.y) {
//                    continue;
//                }
//                if (!insideGrid(elevgrid, k,l)) {
//                    continue;
//                }
//                if (floodgrid->data[k][l] == HAVENT_VISITED) {// not visited
//                    point newPoint;
//                    newPoint.x = k;
//                    newPoint.y = l;
//                    if (elevgrid->data[k][l] ==  elevgrid->NODATA_value || elevgrid->data[k][l] ==NEW_WATER) { // if the data is water
//                        floodgrid->data[k][l] = elevgrid->NODATA_value;
//                        queue.push(newPoint);
//                    } else {
//                        if (elevgrid->data[k][l] < rise) {
//                            floodgrid->data[k][l] = NEW_WATER;
//                            queue.push(newPoint);
//                        }
//                    }
//                }
//            }
//        }
//    }
//}



/* MARK: SLR+interp_bfe */
/*
 This function is used to start the slr and bfe function. It also sets the floodgrid to havent visited.
 */
void start_slr_interp_bfe(Grid* elevgrid, Grid* interp_bfegrid,Grid* floodgrid, float rise) {

    for (int i = 0; i < elevgrid->nrows; i++) {
        for (int j = 0; j < elevgrid->ncols; j++) {
            floodgrid->data[i][j] = HAVENT_VISITED;
        }
    }
    std::queue<point> queue;
    findBoundarySeaPoint(elevgrid, &queue);
    compute_slr_interp_bfe(elevgrid, interp_bfegrid, floodgrid,rise, queue);
    setinterp_bfeNotVisited(elevgrid,interp_bfegrid, floodgrid, rise);
}

void compute_slr_interp_bfe(Grid* elevgrid, Grid* interp_bfegrid, Grid* floodgrid,float rise, std::queue<point>& queue) {
    while(queue.empty() != true) {
        point curr = queue.front();
        queue.pop();
        
        
        for (int k = curr.x-1; k <= curr.x+1; k++) {
            for (int l = curr.y-1; l <= curr.y+1;l++) {
                if (k == curr.x && l == curr.y) {
                    continue;
                }
                if (!insideGrid(elevgrid, k, l)) {
                    continue;
                }
                if (floodgrid->data[k][l] == HAVENT_VISITED) {// not visited
                    point newPoint;
                    newPoint.x = k;
                    newPoint.y = l;
                    
                    if (elevgrid->data[k][l] <=  SEA_LEVEL || elevgrid->data[k][l] == elevgrid->NODATA_value|| elevgrid->data[k][l] ==NEW_WATER) { // if the data is water
                        floodgrid->data[k][l] = elevgrid->NODATA_value;
                        queue.push(newPoint);
                    } else {// not given as water
                        if (interp_bfegrid->data[k][l] != interp_bfegrid->NODATA_value) {
                            if (elevgrid->data[k][l] < (rise + interp_bfegrid->data[k][l])) {
                                floodgrid->data[k][l] = NEW_WATER;
                                queue.push(newPoint);
                            }
                        } else {
                            if (elevgrid->data[k][l] <= rise) {
                                floodgrid->data[k][l] = NEW_WATER;
                                queue.push(newPoint);
                            }
                        }
                    }
                }
            }
        }
    }
}

/*
 This function is called by start slr_bfe. It goes through each neighbor of whatever point is popped
 by the queue. Then is the point hadnt been visited, then the function checks if the point is
 no data value or new water then it just adds it to the queue and sets the floodgrid. Otherwise
 if the point is less than the rise with the bfe grid, it sets it to new_water and pushes it to the queue.
 */

//void compute_slr_interp_bfe(Grid* elevgrid, Grid* interp_bfegrid, Grid* floodgrid,float rise, std::queue<point>& queue) {
//    while(queue.empty() != true) {
//        point curr = queue.front();
//        queue.pop();
//        
//        
//        for (int k = curr.x-1; k <= curr.x+1; k++) {
//            for (int l = curr.y-1; l <= curr.y+1;l++) {
//                if (k == curr.x && l == curr.y) {
//                    continue;
//                }
//                if (!insideGrid(elevgrid, k, l)) {
//                    continue;
//                }
//                if (floodgrid->data[k][l] == HAVENT_VISITED) {// not visited
//                    point newPoint;
//                    newPoint.x = k;
//                    newPoint.y = l;
//
//                    if (elevgrid->data[k][l] ==  elevgrid->NODATA_value) { // if the data is water
//                        floodgrid->data[k][l] = elevgrid->NODATA_value;
//                        queue.push(newPoint);
//                    } else {// not given as water
//                        if (interp_bfegrid->data[k][l] != interp_bfegrid->NODATA_value) {
//                            if (elevgrid->data[k][l] < (rise + interp_bfegrid->data[k][l])) {
//                                floodgrid->data[k][l] = NEW_WATER;
//                                queue.push(newPoint);
//                            }
//                        } else {
//                            if (elevgrid->data[k][l] <= rise) {
//                                floodgrid->data[k][l] = NEW_WATER;
//                                queue.push(newPoint);
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//}
//
