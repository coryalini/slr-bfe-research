//
//  bfe.cpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#include "bfe.hpp"

/* MARK: bfe */
void start_interp_bfe(Grid* elevgrid,Grid* interp_bfegrid, float rise, int seaX, int seaY) {
    
    if (elevgrid->data[seaX][seaY] != elevgrid->NODATA_value) {
        printf("ERROR:The point %f that was given is not the sea\n",elevgrid->data[seaX][seaY]);
        return;
    }
    
    char** alreadySeen;
    alreadySeen = (char**)malloc(elevgrid->nrows * sizeof(char *));
    assert(alreadySeen);
    for(int a = 0; a < elevgrid->nrows; a++) {
        alreadySeen[a] = (char*) malloc(elevgrid->ncols * sizeof(char));
        assert(alreadySeen[a]);
    }
    
    for (int i = 0; i < elevgrid->nrows; i++) {
        for (int j = 0; j < elevgrid->ncols; j++) {
            alreadySeen[i][j] = 'u';
        }
    }
    std::queue<point> queue;
    queue = findSeaPoint(elevgrid);
    compute_interp_bfe(elevgrid,interp_bfegrid,rise,alreadySeen,queue);
    
    //    std::queue<point> queue;
    //    point start;
    //    start.x = seaX;
    //    start.y = seaY;
    //    queue.push(start);
    //    compute_interp_bfe(elevgrid,interp_bfegrid,rise,alreadySeen,queue);
}

void compute_interp_bfe(Grid* elevgrid, Grid* interp_bfegrid,int rise, char** alreadySeen,std::queue<point>& queue) {
    //Iterate through the left edge of the terrain
    for (int i = 0; i < elevgrid->nrows; i++) {
        if (interp_bfegrid->data[i][0] == interp_bfegrid->NODATA_value) {
            if(elevgrid->data[i][0] < rise) {
                point newPoint;
                newPoint.x = i;
                newPoint.y = 0;
                queue.push(newPoint);
                alreadySeen[i][0] = 's';
                
            }
        }
    }
    //Iterate through the top edge of the terrain
    for (int j = 0; j < elevgrid->ncols; j++) {
        if (interp_bfegrid->data[0][j] == interp_bfegrid->NODATA_value) {
            if(elevgrid->data[0][j] < rise) {
                point newPoint;
                newPoint.x = 0;
                newPoint.y = j;
                queue.push(newPoint);
                alreadySeen[0][j] = 's';
                
            }
        }
    }
    //Iterate through the right edge of the terrain
    for (int i = 0; i < elevgrid->nrows-1; i++) {
        if (interp_bfegrid->data[i][elevgrid->ncols-1] == interp_bfegrid->NODATA_value) {
            if(elevgrid->data[i][elevgrid->ncols-1] < rise) {
                point newPoint;
                newPoint.x = i;
                newPoint.y = elevgrid->ncols-1;
                queue.push(newPoint);
                alreadySeen[i][elevgrid->ncols-1] = 's';
            }
        }
    }
    //Iterate through the bottom edge of the terrain
    for (int j = 0; j < elevgrid->ncols-1; j++) {
        if (interp_bfegrid->data[elevgrid->nrows-1][j] == interp_bfegrid->NODATA_value) {
            if(elevgrid->data[elevgrid->nrows-1][j] < rise) {
                point newPoint;
                newPoint.x = elevgrid->nrows-1;
                newPoint.y = j;
                queue.push(newPoint);
                alreadySeen[elevgrid->nrows-1][j] = 's';
                
            }
        }
    }
    
    std::queue<point> interp_bfequeue;
    while(queue.empty() != true) {
        point curr = queue.front();
        queue.pop();
        for (int k = 0; k < 8; k++) {
            int newRow = curr.x + offsets[k][0];
            int newCol = curr.y + offsets[k][1];
            if (!insideGrid(elevgrid, (int)newRow, (int)newCol)) {
                continue;
            }
            point newPoint;
            newPoint.x = newRow;
            newPoint.y = newCol;
            if(alreadySeen[newRow][newCol] == 'u') {
                //if not interp_bfe  add to queue and continue
                if(interp_bfegrid->data[newRow][newCol] == interp_bfegrid->NODATA_value) {
                    interp_bfegrid->data[newRow][newCol] = interp_bfegrid->NODATA_value;
                    queue.push(newPoint);
                    alreadySeen[newRow][newCol] = 's';
                } else {
                    interp_bfequeue.push(newPoint);
                    alreadySeen[newRow][newCol] = 'b';
                    
                }
            }
        }
    }
    while(interp_bfequeue.empty() != true) {
        point curr = interp_bfequeue.front();
        interp_bfequeue.pop();
        for (int k = 0; k < 8; k++) {
            int newRow = curr.x + offsets[k][0];
            int newCol = curr.y + offsets[k][1];
            if (!insideGrid(elevgrid, (int)newRow, (int)newCol)) {
                continue;
            }
            point newPoint;
            newPoint.x = newRow;
            newPoint.y = newCol;
            if(alreadySeen[newRow][newCol] != 'u') {
                continue;
            }
            if (interp_bfegrid->data[newRow][newCol] == interp_bfegrid->NODATA_value) {
                interp_bfegrid->data[newRow][newCol] = interp_bfegrid->data[(int)curr.x][(int)curr.y];
            }
            assert(interp_bfegrid->data[newRow][newCol] != interp_bfegrid->NODATA_value);
            interp_bfequeue.push(newPoint);
            alreadySeen[newRow][newCol] = 'b';
        }
    }
    for(int a = 0; a < elevgrid->nrows; a++) {
        free(alreadySeen[a]);
    }
    free(alreadySeen);
   

}
