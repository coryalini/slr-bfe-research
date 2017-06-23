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
Grid start_slr(Grid* elevgrid,float rise) {
    Grid local_slrgrid;
    mallocGrid(*elevgrid, &local_slrgrid);
    setHeaders(*elevgrid, &local_slrgrid);
    for (int i = 0; i < elevgrid->nrows; i++) {
        for (int j = 0; j < elevgrid->ncols; j++) {
            local_slrgrid.data[i][j] = HAVENT_VISITED;
        }
    }
    
    std::queue<point> queue;
    queue = findSeaPoint(elevgrid);
    compute_slr(elevgrid, &local_slrgrid, rise, queue);
    setNotVisited(elevgrid, &local_slrgrid, rise);
    
    return local_slrgrid;
}


void compute_slr(Grid* elevgrid, Grid* local_slrgrid,float rise, std::queue<point>& queue) {
    
    while(queue.empty() != true) {
        point curr = queue.front();
        queue.pop();
        for (int k = 0; k < 8; k++) {
            int newRow = curr.x + offsets[k][0];
            int newCol = curr.y + offsets[k][1];
            if (!insideGrid(elevgrid, (int)newRow, (int)newCol)) {
                continue;
            }
            if (local_slrgrid->data[newRow][newCol] == HAVENT_VISITED) {// not visited
                point newPoint;
                newPoint.x = newRow;
                newPoint.y = newCol;
                if (elevgrid->data[newRow][newCol] ==  elevgrid->NODATA_value) { // if the data is water
                    local_slrgrid->data[newRow][newCol] = elevgrid->NODATA_value;
                    queue.push(newPoint);
                } else {
                    if (elevgrid->data[newRow][newCol] < rise) {
                        local_slrgrid->data[newRow][newCol] = NEW_WATER;
                        queue.push(newPoint);
                    }
                }
            }
        }
    }
}



/* MARK: SLR+interp_bfe */
Grid start_slr_interp_bfe(Grid* elevgrid, Grid* interp_bfegrid, float rise) {
    Grid local_slr_interp_bfegrid;
    mallocGrid(*elevgrid, &local_slr_interp_bfegrid);
    setHeaders(*elevgrid, &local_slr_interp_bfegrid);
    
    for (int i = 0; i < elevgrid->nrows; i++) {
        for (int j = 0; j < elevgrid->ncols; j++) {
            local_slr_interp_bfegrid.data[i][j] = HAVENT_VISITED;
        }
    }
    std::queue<point> queue;
    queue = findSeaPoint(elevgrid);
    compute_slr_interp_bfe(elevgrid, &local_slr_interp_bfegrid, interp_bfegrid, rise, queue);
    setinterp_bfeNotVisited(elevgrid,&local_slr_interp_bfegrid,interp_bfegrid, rise);

    return local_slr_interp_bfegrid;
}

void compute_slr_interp_bfe(Grid* elevgrid, Grid* local_slr_interp_bfegrid, Grid* interp_bfegrid,float rise, std::queue<point>& queue) {
    while(queue.empty() != true) {
        point curr = queue.front();
        queue.pop();
        for (int k = 0; k < 8; k++) {
            int newRow = curr.x + offsets[k][0];
            int newCol = curr.y + offsets[k][1];
            if (!insideGrid(elevgrid, (int)newRow, (int)newCol)) {
                continue;
            }
            if (local_slr_interp_bfegrid->data[newRow][newCol] == HAVENT_VISITED) {// not visited
                if (elevgrid->data[newRow][newCol] ==  elevgrid->NODATA_value) { // if the data is water
                    local_slr_interp_bfegrid->data[newRow][newCol] = elevgrid->NODATA_value;
                    point newPoint;
                    newPoint.x = newRow;
                    newPoint.y = newCol;
                    queue.push(newPoint);
                } else {// not given as water
                    if (interp_bfegrid->data[newRow][newCol] != interp_bfegrid->NODATA_value) {
                        if (elevgrid->data[newRow][newCol] < (rise + interp_bfegrid->data[newRow][newCol])) {
                            local_slr_interp_bfegrid->data[newRow][newCol] = NEW_WATER;
                            point newPoint;
                            newPoint.x = newRow;
                            newPoint.y = newCol;
                            queue.push(newPoint);
                        }
                    } else {
                        if (elevgrid->data[newRow][newCol] < rise) {
                            local_slr_interp_bfegrid->data[newRow][newCol] = NEW_WATER;
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

