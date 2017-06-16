//
//  slr.c
//  slr
//
//  Created by Cory Alini on 11/19/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//


#include "slr.hpp"

#include <iostream>
#include <queue>
#include <limits>
#include <assert.h>

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
