//
//  slr_bfe.cpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#include "slr_bfe.hpp"



/* MARK: SLR+interp_bfe */
void start_slr_interp_bfe(Grid* elevgrid, Grid* slr_interp_bfegrid, Grid* interp_bfegrid, float rise, int seaX, int seaY) {
    if (elevgrid->data[seaX][seaY] != elevgrid->NODATA_value) {
        printf("ERROR:The point %f that was given is not the sea\n",elevgrid->data[seaX][seaY]);
        return;
    }
    
    for (int i = 0; i < elevgrid->nrows; i++) {
        for (int j = 0; j < elevgrid->ncols; j++) {
            slr_interp_bfegrid->data[i][j] = HAVENT_VISITED;
        }
    }
    std::queue<point> queue;
    point start;
    start.x = seaX;
    start.y = seaY;
    queue.push(start);
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
