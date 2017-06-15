//
//  bfe_complete.cpp
//  slr_bfe
//
//  Created by Cory Alini on 6/13/17.
//  Copyright © 2017 slr_bfe.coryalini. All rights reserved.
//

#include "bfe_complete.hpp"

/* MARK: bfe */
void start_interp_bfe1(Grid* elevgrid,Grid* interp_bfegrid, float rise, int seaX, int seaY) {
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
    compute_interp_bfe1(elevgrid,interp_bfegrid,rise,alreadySeen,queue);
}

void compute_interp_bfe1(Grid* elevgrid, Grid* interp_bfegrid,int rise, char** alreadySeen,std::queue<point>& queue) {
    
    //Iterate through left and right edges of the terrain
    for (int i = 0; i < elevgrid->nrows; i++) {
        if (interp_bfegrid->data[i][0] == interp_bfegrid->NODATA_value) {
            point newPoint;

            //Iterate through the left edge of the terrain

            if (elevgrid->data[i][0] == elevgrid->NODATA_value) {
                newPoint.x = i;
                newPoint.y = 0;
                alreadySeen[i][0] = 's';
            } else if(elevgrid->data[i][0] < rise) {
                newPoint.x = i;
                newPoint.y = 0;
                alreadySeen[i][0] = 'f';
            }
            
            //Iterate through the right edge of the terrain
            if (elevgrid->data[i][elevgrid->ncols-1] == elevgrid->NODATA_value) {
                newPoint.x = i;
                newPoint.y = elevgrid->ncols-1;
                alreadySeen[i][elevgrid->ncols-1] = 's';
            } else if(elevgrid->data[i][elevgrid->ncols-1] < rise) {
                newPoint.x = i;
                newPoint.y = elevgrid->ncols-1;
                alreadySeen[i][elevgrid->ncols-1] = 'f';
            }
            queue.push(newPoint);

            
        }
    }
    //Iterate through top and bottom edges of the terrain

    for (int j = 0; j < elevgrid->ncols; j++) {
        if (interp_bfegrid->data[0][j] == interp_bfegrid->NODATA_value) {
            point newPoint;

            //Iterate through the top edge of the terrain
            if (elevgrid->data[0][j] == elevgrid->NODATA_value) {
                newPoint.x = 0;
                newPoint.y = j;
                alreadySeen[0][j] = 's';
            } else if(elevgrid->data[0][j] < rise) {
                newPoint.x = 0;
                newPoint.y = j;
                alreadySeen[0][j] = 'f';
            }
            
            //Iterate through the bottom edge of the terrain
            if (elevgrid->data[elevgrid->nrows-1][j]== elevgrid->NODATA_value) {
                newPoint.x = elevgrid->nrows-1;
                newPoint.y = j;
                alreadySeen[elevgrid->nrows-1][j] = 's';
            } else if(elevgrid->data[elevgrid->nrows-1][j] < rise) {
                newPoint.x = elevgrid->nrows-1;
                newPoint.y = j;
                alreadySeen[elevgrid->nrows-1][j] = 'f';
                
            }
            queue.push(newPoint);
        }
    }
    
    
    std::queue<point> interp_bfequeue;
    //FIND THE EDGE OF THE BFE GRID
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
                    if (elevgrid->data[newRow][newCol] == elevgrid->NODATA_value) {
                        alreadySeen[newRow][newCol] = 's';
                    } else {
                        queue.push(newPoint);
                        alreadySeen[newRow][newCol] = 'l';
                    }
                } else {
                    interp_bfequeue.push(newPoint);
                    alreadySeen[newRow][newCol] = 'b';
                    
                }
            }
        }
    }
    //GO THROUGH THE BFE POINTS AND INTERPOLATE
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
            //if the point is unseen, floooded, or land then just skip it
            if(alreadySeen[newRow][newCol] != 'u' && alreadySeen[newRow][newCol] != 'f'&& alreadySeen[newRow][newCol] != 'l') {
                continue;
            }
            if (interp_bfegrid->data[newRow][newCol] == interp_bfegrid->NODATA_value) {
                interp_bfegrid->data[newRow][newCol] = interp_bfegrid->data[(int)curr.x][(int)curr.y];
            }
            assert(interp_bfegrid->data[newRow][newCol] != interp_bfegrid->NODATA_value);
            interp_bfequeue.push(newPoint);
            alreadySeen[newRow][newCol] = 'b'; // change the value so it has a bfe
        }
    }
    
    for(int a = 0; a < elevgrid->nrows; a++) {
        free(alreadySeen[a]);
    }
    free(alreadySeen);
}
