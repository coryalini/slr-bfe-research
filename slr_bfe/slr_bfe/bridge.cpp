//
//  bridge.cpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#include "bridge.hpp"
Grid elevgrid, interp_bfegrid, slrgrid, slr_interp_bfegrid, originterp_bfegrid,currGrid;
int DRAW = 0;
int interp_bfe_EXISTS = 1;

int offsets [8][2] ={{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};

float rise;

const char *elevname, *writeGridname, *interp_bfename;

const int NEW_WATER = -8000;
const int HAVENT_VISITED = -7000;
double max = 0, minLand =std::numeric_limits<double>::max(), min = std::numeric_limits<double>::max();
double maxElev = 0, minLandElev =std::numeric_limits<double>::max();

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


void setinterp_bfeNotVisited(Grid* elevgrid, Grid* slrgrid, Grid* interp_bfegrid, float rise) {
    for (int i = 0; i < slrgrid->nrows; i++) {
        for (int j = 0; j < slrgrid->ncols; j++) {
            if (slrgrid->data[i][j] == HAVENT_VISITED) {
                if (interp_bfegrid->data[i][j] != interp_bfegrid->NODATA_value) {
                    if (slrgrid->data[i][j] == NEW_WATER) {
                        slrgrid->data[i][j] = elevgrid->data[i][j] - (rise+ interp_bfegrid->data[i][j]);

                    } else {
                        slrgrid->data[i][j] = elevgrid->data[i][j] - rise;

                    }
                    
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


void findMaxMin(Grid* grid) {
    max = 0;
    minLand=std::numeric_limits<float>::max(), min = std::numeric_limits<float>::max();
    for (int i = 0; i < grid->nrows; i++) {
        for (int j = 0; j < grid->ncols; j++) {
            if (grid->data[i][j] > max) {
                max = grid->data[i][j];
            }
            if (grid->data[i][j] < min) {
                min = grid->data[i][j];
            }
//            printf("%f vs %f \n",grid->data[i][j], minLand);
            if (grid->data[i][j] < minLand && grid->data[i][j] != grid->NODATA_value && grid->data[i][j] != NEW_WATER && grid->data[i][j] != HAVENT_VISITED) {
                minLand = grid->data[i][j];
            }
        }
    }
}
void findMaxMinElev() {
    maxElev = 0;
    minLandElev=std::numeric_limits<float>::max();
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            if (elevgrid.data[i][j] > maxElev) {
                maxElev = elevgrid.data[i][j];
            }
            if (elevgrid.data[i][j] < minLandElev && elevgrid.data[i][j] != elevgrid.NODATA_value) {
                minLandElev = elevgrid.data[i][j];
            }
        }
    }
}



void freeGridData(Grid* g) {
    for(int i = 0; i < g->nrows; i++) {
        free(g->data[i]);
    }
    free(g->data);
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
void outputGridWithDepthWITHinterp_bfe(Grid* g, Grid* slrgrid,Grid* elevgrid,Grid* interp_bfegrid, float rise) {
    for (int i = 0; i < g->nrows; i++) {
        for (int j = 0; j < g->ncols; j++) {
            if (slrgrid->data[i][j] == NEW_WATER) {
                g->data[i][j] = (rise+interp_bfegrid->data[i][j]) - elevgrid->data[i][j];
            } else {
                g->data[i][j] = g->NODATA_value;
            }
        }
    }
    
}
