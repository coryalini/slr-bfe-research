//
//  slr.c
//  slr
//
//  Created by Cory Alini on 11/19/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//



/* Assumptions:
 no data is water.
 no data + 1 isnt going to be a height
 */


#include <iostream>
#include <queue>
#include <limits>
#include <assert.h>
#include "slr.h"
#include "parsing.h"

#define DEBUG 0

#define debug_printf(string, args ...) if (DEBUG){  \
printf("%s:%d: ", __FUNCTION__, __LINE__ );   \
printf(string , ##args); }


const int NEW_WATER = -8000;
const int HAVENT_VISITED = -9000;

float maxXSLR = 0, maxYSLR= 0;
float minXSLR, minYSLR, minXLandSLR, minYLandSLR;
float maxSLR = 0, minLandSLR =std::numeric_limits<float>::max(), minSLR = std::numeric_limits<float>::max();




int offsets [8][2] ={{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};

void start_slr(Grid* elevgrid, Grid* slrgrid,float rise, int seaX, int seaY) {
    if (elevgrid->data[seaX][seaY] != elevgrid->NODATA_value) {
        printf("ERROR:The point %f that was given is not the sea\n",elevgrid->data[seaX][seaY]);
        return;
    }
    
    
    for (int i = 0; i < elevgrid->nrows; i++) {
        for (int j = 0; j < elevgrid->ncols; j++) {
            slrgrid->data[i][j] = HAVENT_VISITED;
        }
    }
//    std::queue<point> queue;
//    point start;
//    start.x = seaX;
//    start.y = seaY;
//    queue.push(start);
    
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

std::queue<point> findSeaPoint(Grid* elevgrid) {
    std::queue<point> queue;
    for (int i = 0; i < elevgrid->nrows; i++) {
        if (elevgrid->data[i][0] == elevgrid->NODATA_value) {
            point newPoint;
            newPoint.x = i;
            newPoint.y = 0;
            queue.push(newPoint);
        }
    }
    
    for (int j = 0; j < elevgrid->ncols; j++) {
        if (elevgrid->data[0][j] == elevgrid->NODATA_value) {
            point newPoint;
            newPoint.x = 0;
            newPoint.y = j;
            queue.push(newPoint);
            
        }
    }
    for (int i = 0; i < elevgrid->nrows; i++) {
        if (elevgrid->data[i][elevgrid->ncols-1] == elevgrid->NODATA_value) {
            point newPoint;
            newPoint.x = i;
            newPoint.y = elevgrid->ncols-1;
            queue.push(newPoint);
        }
    }
    for (int j = 0; j < elevgrid->ncols; j++) {
        if (elevgrid->data[elevgrid->nrows-1][j] == elevgrid->NODATA_value) {
            point newPoint;
            newPoint.x = elevgrid->nrows-1;
            newPoint.y = j;
            queue.push(newPoint);
        }
    }
    
    return queue;
}

/* MARK: BFE */
void start_bfe(Grid* elevgrid,Grid* bfegrid, float rise, int seaX, int seaY) {
    
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
    compute_bfe(elevgrid,bfegrid,rise,alreadySeen,queue);

//    std::queue<point> queue;
//    point start;
//    start.x = seaX;
//    start.y = seaY;
//    queue.push(start);
//    compute_bfe(elevgrid,bfegrid,rise,alreadySeen,queue);
}

void compute_bfe(Grid* elevgrid, Grid* bfegrid,int rise, char** alreadySeen,std::queue<point>& queue) {
    
    printf("Did the bfe reading work? %f\n",bfegrid->data[elevgrid->nrows-1][0]);
    fflush(stdout);
    //left and right edges
    for (int i = 0; i < elevgrid->nrows; i++) {
        if (bfegrid->data[i][0] == bfegrid->NODATA_value) {
            if(elevgrid->data[i][0] < rise) {
                point newPoint;
                newPoint.x = i;
                newPoint.y = 0;
                queue.push(newPoint);
                alreadySeen[i][0] = 's';
                
            }
        }
    }
    for (int j = 0; j < elevgrid->ncols; j++) {
        if (bfegrid->data[0][j] == bfegrid->NODATA_value) {
            if(elevgrid->data[0][j] < rise) {
                point newPoint;
                newPoint.x = 0;
                newPoint.y = j;
                queue.push(newPoint);
                alreadySeen[0][j] = 's';
                
            }
        }
    }
    for (int i = 0; i < elevgrid->nrows-1; i++) {
        if (bfegrid->data[i][elevgrid->ncols-1] == bfegrid->NODATA_value) {
            if(elevgrid->data[i][elevgrid->ncols-1] < rise) {
                point newPoint;
                newPoint.x = i;
                newPoint.y = elevgrid->ncols-1;
                queue.push(newPoint);
                alreadySeen[i][elevgrid->ncols-1] = 's';
            }
        }
    }
    for (int j = 0; j < elevgrid->ncols-1; j++) {
        if (bfegrid->data[elevgrid->nrows-1][j] == bfegrid->NODATA_value) {
            if(elevgrid->data[elevgrid->nrows-1][j] < rise) {
                point newPoint;
                newPoint.x = elevgrid->nrows-1;
                newPoint.y = j;
                queue.push(newPoint);
                alreadySeen[elevgrid->nrows-1][j] = 's';
                
            }
        }
    }
    
    std::queue<point> bfequeue;
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
                //if not bfe  add to queue and continue
                if(bfegrid->data[newRow][newCol] == bfegrid->NODATA_value) {
                    bfegrid->data[newRow][newCol] = bfegrid->NODATA_value;
                    queue.push(newPoint);
                    alreadySeen[newRow][newCol] = 's';
                } else {
                    bfequeue.push(newPoint);
                    alreadySeen[newRow][newCol] = 'b';
                    
                }
            }
        }
    }
    while(bfequeue.empty() != true) {
        point curr = bfequeue.front();
        bfequeue.pop();
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
            if (bfegrid->data[newRow][newCol] != bfegrid->NODATA_value) {
                bfequeue.push(newPoint);
                alreadySeen[newRow][newCol] = 'b';
            } else {
                
                bfegrid->data[newRow][newCol] = bfegrid->data[(int)curr.x][(int)curr.y];
                bfequeue.push(newPoint);
                alreadySeen[newRow][newCol] = 'b';
            }
        }
    }
}


/* MARK: SLR+BFE */
void start_slr_bfe(Grid* elevgrid, Grid* slrgrid, Grid* bfegrid, float rise, int seaX, int seaY) {
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
    compute_slr_bfe(elevgrid, slrgrid, bfegrid, rise, queue);
    setBFENotVisited(elevgrid,slrgrid,bfegrid, rise);
    
}

void compute_slr_bfe(Grid* elevgrid, Grid* slrgrid, Grid* bfegrid,float rise, std::queue<point>& queue) {
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
                    if (bfegrid->data[newRow][newCol] != bfegrid->NODATA_value) {
                        if (elevgrid->data[newRow][newCol] < (rise + bfegrid->data[newRow][newCol])) {
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
void setBFENotVisited(Grid* elevgrid, Grid* slrgrid, Grid* bfegrid, float rise) {
    for (int i = 0; i < slrgrid->nrows; i++) {
        for (int j = 0; j < slrgrid->ncols; j++) {
            if (slrgrid->data[i][j] == HAVENT_VISITED) {
                if (bfegrid->data[i][j] != bfegrid->NODATA_value) {
                    slrgrid->data[i][j] = elevgrid->data[i][j] - (rise+ bfegrid->data[i][j]);
                } else {
                    slrgrid->data[i][j] = elevgrid->data[i][j] - rise;
                }
            }
        }
    }
}
void setNotVisited(Grid* elevgrid, Grid* slrgrid, float rise) {
    for (int i = 0; i < elevgrid->nrows; i++) {
        for (int j = 0; j < elevgrid->ncols; j++) {
            if (slrgrid->data[i][j] == HAVENT_VISITED) {
                slrgrid->data[i][j] = elevgrid->data[i][j] - rise;
            }
        }
    }
}

/* MARK: Extra stuff */

int insideGrid(Grid* grid, int i, int j) {
    if (i < 0 || i >= grid->nrows || j < 0 || j >= grid->ncols) {
        return 0;
    }
    return 1;
}


float* findMaxMin(Grid* grid, float* array) {
    minXSLR = (int)grid->nrows;
    minYSLR = (int)grid->ncols;
    for (int i = 0; i < grid->nrows; i++) {
        for (int j = 0; j < grid->ncols; j++) {
            if (grid->data[i][j] > maxSLR) {
                maxXSLR = i;
                maxYSLR = j;
                maxSLR = grid->data[i][j];
            }
            if (grid->data[i][j] < minSLR) {
                minXSLR = i;
                minYSLR = j;
                minSLR = grid->data[i][j];
            }
            if (grid->data[i][j] < minLandSLR && grid->data[i][j] != grid->NODATA_value && grid->data[i][j] != NEW_WATER) {
                minXLandSLR = i;
                minYLandSLR= j;
                minLandSLR = grid->data[i][j];
            }
        }
    }
    array[0] = minXSLR;
    array[1] = minYSLR;
    array[2] = maxXSLR;
    array[3] = maxYSLR;
    array[4] = minSLR;
    array[5] = maxSLR;
    array[6] = minXLandSLR;
    array[7] = minYLandSLR;
    array[8] = minLandSLR;
    
    return array;
}



//const int NEW_WATER = -8000;
void outputGridWithDepth(Grid* g, Grid* slrgrid,Grid* elevgrid,float rise) {
    for (int i = 0; i < g->nrows; i++) {
        for (int j = 0; j < g->ncols; j++) {
            if (slrgrid->data[i][j] == NEW_WATER) {
                g->data[i][j] = rise - elevgrid->data[i][j];
            } else {
                g->data[i][j] = g->NODATA_value;
            }
        }
    }
    
}
void outputGridWithDepthWITHBFE(Grid* g, Grid* slrgrid,Grid* elevgrid,Grid* bfegrid, float rise) {
    for (int i = 0; i < g->nrows; i++) {
        for (int j = 0; j < g->ncols; j++) {
            if (slrgrid->data[i][j] == NEW_WATER) {
                g->data[i][j] = (rise+bfegrid->data[i][j]) - elevgrid->data[i][j];
            } else {
                g->data[i][j] = g->NODATA_value;
            }
        }
    }
    
}



