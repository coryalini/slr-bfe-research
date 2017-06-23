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
void start_slr(Grid* elevgrid, Grid* slrgrid,float rise) {
    for (int i = 0; i < elevgrid->nrows; i++) {
        for (int j = 0; j < elevgrid->ncols; j++) {
            slrgrid->data[i][j] = HAVENT_VISITED;
        }
    }
    
    std::queue<point> queue;
    queue = findSeaPoint(elevgrid);
    compute_slr(elevgrid, slrgrid, rise, queue);
    setNotVisited(elevgrid, slrgrid, rise);
}


void compute_slr(Grid* elevgrid, Grid* slrgrid,float rise, std::queue<point>& queue) {
    
    while(queue.empty() != true) {
        point curr = queue.front();
        queue.pop();
        for (int k = 0; k < 8; k++) {
            int newRow = curr.x + offsets[k][0];
            int newCol = curr.y + offsets[k][1];
            if (!insideGrid(elevgrid, (int)newRow, (int)newCol)) {
                continue;
            }
            if (slrgrid->data[newRow][newCol] == HAVENT_VISITED) {// not visited
                point newPoint;
                newPoint.x = newRow;
                newPoint.y = newCol;
                if (elevgrid->data[newRow][newCol] ==  elevgrid->NODATA_value) { // if the data is water
                    slrgrid->data[newRow][newCol] = elevgrid->NODATA_value;
                    queue.push(newPoint);
                } else {
                    if (elevgrid->data[newRow][newCol] < rise) {
                        slrgrid->data[newRow][newCol] = NEW_WATER;
                        queue.push(newPoint);
                    }
                }
            }
        }
    }
}



/* MARK: SLR+interp_bfe */
void start_slr_interp_bfe(Grid* elevgrid, Grid* slr_interp_bfegrid, Grid* interp_bfegrid, float rise) {
    for (int i = 0; i < elevgrid->nrows; i++) {
        for (int j = 0; j < elevgrid->ncols; j++) {
            slr_interp_bfegrid->data[i][j] = HAVENT_VISITED;
        }
    }
    std::queue<point> queue;
    queue = findSeaPoint(elevgrid);
    compute_slr_interp_bfe(elevgrid, slr_interp_bfegrid, interp_bfegrid, rise, queue);
    setinterp_bfeNotVisited(elevgrid,slr_interp_bfegrid,interp_bfegrid, rise);
}

void compute_slr_interp_bfe(Grid* elevgrid, Grid* slr_interp_bfegrid, Grid* interp_bfegrid,float rise, std::queue<point>& queue) {
    while(queue.empty() != true) {
        point curr = queue.front();
        queue.pop();
        for (int k = 0; k < 8; k++) {
            int newRow = curr.x + offsets[k][0];
            int newCol = curr.y + offsets[k][1];
            if (!insideGrid(elevgrid, (int)newRow, (int)newCol)) {
                continue;
            }
            if (slr_interp_bfegrid->data[newRow][newCol] == HAVENT_VISITED) {// not visited
                if (elevgrid->data[newRow][newCol] ==  elevgrid->NODATA_value) { // if the data is water
                    slr_interp_bfegrid->data[newRow][newCol] = elevgrid->NODATA_value;
                    point newPoint;
                    newPoint.x = newRow;
                    newPoint.y = newCol;
                    queue.push(newPoint);
                } else {// not given as water
                    if (interp_bfegrid->data[newRow][newCol] != interp_bfegrid->NODATA_value) {
                        if (elevgrid->data[newRow][newCol] < (rise + interp_bfegrid->data[newRow][newCol])) {
                            slr_interp_bfegrid->data[newRow][newCol] = NEW_WATER;
                            point newPoint;
                            newPoint.x = newRow;
                            newPoint.y = newCol;
                            queue.push(newPoint);
                        }
                    } else {
                        if (elevgrid->data[newRow][newCol] < rise) {
                            slr_interp_bfegrid->data[newRow][newCol] = NEW_WATER;
                            point newPoint;
                            newPoint.x = newRow;
                            newPoint.y = newCol;
                            queue.push(newPoint);
                        }
                        
                    }
                    
                }
            }
        }
    }
}

