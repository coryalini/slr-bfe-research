//
//  slr_bfe.cpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#include "slr_bfe.hpp"



/* MARK: SLR+interp_bfe */
void start_slr_interp_bfe(Grid* elevgrid, Grid* slrgrid, Grid* interp_bfegrid, float rise, int seaX, int seaY) {
    if (elevgrid->data[seaX][seaY] != elevgrid->NODATA_value) {
        printf("ERROR:The point %f that was given is not the sea\n",elevgrid->data[seaX][seaY]);
        return;
    }
    
    for (int i = 0; i < elevgrid->nrows; i++) {
        for (int j = 0; j < elevgrid->ncols; j++) {
            slrgrid->data[i][j] = HAVENT_VISITED;
        }
    }
    std::queue<point> queue;
    point start;
    start.x = seaX;
    start.y = seaY;
    queue.push(start);
    compute_slr_interp_bfe(elevgrid, slrgrid, interp_bfegrid, rise, queue);
    setinterp_bfeNotVisited(elevgrid,slrgrid,interp_bfegrid, rise);
    
}

void compute_slr_interp_bfe(Grid* elevgrid, Grid* slrgrid, Grid* interp_bfegrid,float rise, std::queue<point>& queue) {
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
                if (elevgrid->data[newRow][newCol] ==  elevgrid->NODATA_value) { // if the data is water
                    slrgrid->data[newRow][newCol] = elevgrid->NODATA_value;
                    point newPoint;
                    newPoint.x = newRow;
                    newPoint.y = newCol;
                    queue.push(newPoint);
                } else {
                    if (interp_bfegrid->data[newRow][newCol] != interp_bfegrid->NODATA_value) {
                        if (elevgrid->data[newRow][newCol] < (rise + interp_bfegrid->data[newRow][newCol])) {
                            slrgrid->data[newRow][newCol] = NEW_WATER;
                            point newPoint;
                            newPoint.x = newRow;
                            newPoint.y = newCol;
                            queue.push(newPoint);
                        }
                    } else {
                        if (elevgrid->data[newRow][newCol] < rise) {
                            slrgrid->data[newRow][newCol] = NEW_WATER;
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
