//
//  bridge.cpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#include "grid.hpp"
#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

const char *elevname, *writeGridname, *bfename;
Grid elevgrid, bfegrid, slrgrid, slr_interp_bfegrid, interp_bfegrid,currgrid;
int offsets [8][2] ={{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};


const int NEW_WATER = -8000;
const int HAVENT_VISITED = -7000;

float ELEV_CONVERTER = 1;
float BFE_CONVERTER = 1;

int ELEV_TYPE = 0;
int BFE_TYPE = 1;

//float convertToMeters = 3.28;


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
double findMax(Grid* grid) {
    double max = 0;
    for (int i = 0; i < grid->nrows; i++) {
        for (int j = 0; j < grid->ncols; j++) {
            if (grid->data[i][j] > max) {
                max = grid->data[i][j];
            }
        }
    }
    return max;
 
}



double findMinLand(Grid* grid) {
    double minLand = std::numeric_limits<double>::max();
    for (int i = 0; i < grid->nrows; i++) {
        for (int j = 0; j < grid->ncols; j++) {
            if (grid->data[i][j] < minLand && grid->data[i][j] != grid->NODATA_value && grid->data[i][j] != NEW_WATER && grid->data[i][j] != HAVENT_VISITED) {
                minLand = grid->data[i][j];
            }
        }
    }
    return minLand;
    
}



void freeGridData(Grid* g) {
    for(int i = 0; i < g->nrows; i++) {
        free(g->data[i]);
    }
    free(g->data);
}

void mallocGrid(Grid eg, Grid* vg) {
    
    vg->data = (float**)malloc(eg.nrows * sizeof(float *));
    assert(vg->data);
    
    for(int a = 0; a < eg.nrows; a++) {
        vg->data[a] = (float*)malloc(eg.ncols * sizeof(float));
        assert(vg->data[a]);
    }
}

void setHeaders(Grid elevgrid, Grid* newGrid) {
    newGrid->nrows = elevgrid.nrows;
    newGrid->ncols = elevgrid.ncols;
    newGrid->xllcorner = elevgrid.xllcorner;
    newGrid->yllcorner = elevgrid.yllcorner;
    newGrid->cellsize = elevgrid.cellsize;
    newGrid->NODATA_value = elevgrid.NODATA_value;
}

void copyGrid(Grid* originalGrid, Grid* copyGrid) {
    for (int i = 0; i < originalGrid->nrows; i++) {
        for (int j = 0; j < originalGrid->ncols; j++) {
            copyGrid->data[i][j] = originalGrid->data[i][j];
        }
    }
}


void readGridfromFile(const char* gridfname, Grid* g, int gridType) {
    FILE* f;
    
    //    printf("reading grid %s\n",gridfname);
    f=fopen(gridfname, "r");
    
    //check to see that usr entered a file
    if (f== NULL) {
        printf("cannot open file..\n");
        exit(1);
    }
    
    int x = fscanf(f, "ncols         %li\n"
                   "nrows         %li\n"
                   "xllcorner     %lf\n"
                   "yllcorner     %lf\n"
                   "cellsize      %lf\n"
                   "NODATA_value  %lf\n", &g->ncols, &g->nrows,
                   &g->xllcorner, &g->yllcorner, &g->cellsize,
                   &g->NODATA_value);
    
    if (x != 6) {
        printf("ERROR: fscanf did not properly scan in the variables\n");
        exit(1);
    }
    
    if (std::strcmp(gridfname, elevname) != 0) {
        if (g->ncols != elevgrid.ncols || g->nrows !=elevgrid.nrows) {
            printf("ERROR:The %s [%ld,%ld] and elevgrid [%ld,%ld] do not have the same grid dimensions!\n",gridfname, g->nrows,g->ncols,elevgrid.nrows, elevgrid.ncols);
            exit(0);
        }
    }
    
    
    
    
    g->data = (float**)malloc(g->nrows * sizeof(float *));
    assert(g->data);
    for(int i = 0; i < g->nrows; i++) {
        g->data[i] = (float*)malloc(g->ncols * sizeof(float));
        assert(g->data[i]);
    }
    
    for(int i = 0; i < g->nrows; i++) {
        for(int j = 0; j < g->ncols; j++) {
            int y = fscanf(f,"%f ",&g->data[i][j]);
            if (y != 1) {
                printf("ERROR: fscanf did not properly scan in the grid\n");
                exit(1);
            }
            if (g->data[i][j] != g->NODATA_value) {
                if (gridType == ELEV_TYPE) {
                    g->data[i][j] *= ELEV_CONVERTER;
                } else {
                    g->data[i][j] = g->data[i][j]*BFE_CONVERTER;
                    
                }
            }
        }
    }
    fclose(f);
}

void gridtoFile(Grid* g, const char* fileName) {
    FILE* file;
    
    if((file = fopen(fileName, "w+")) == NULL) {
        printf("Cannot open file.\n");
        exit(1);
    }
    
    fprintf(file, "ncols         %ld\n"
            "nrows         %ld\n"
            "xllcorner     %g\n"
            "yllcorner     %g\n"
            "cellsize      %g\n"
            "NODATA_value  %g\n",
            g->ncols, g->nrows,
            g->xllcorner, g->yllcorner,
            g->cellsize, g->NODATA_value);
    
    for(int i = 0; i < g->nrows; i++) {
        for(int j = 0; j < g->ncols; j++) {
            fprintf(file,"%g ",g->data[i][j]);
        }
        fprintf(file,"\n");
    }
    fclose(file);
}



void printGrid(Grid g) {
    printHeader(g);
    printValues(g);
}
void printHeader(Grid g) {
    printf("nrows \t %ld\n",g.nrows);
    printf("ncols \t %ld\n",g.ncols);
    printf("xllcorner \t %g\n",g.xllcorner);
    printf("yllcorner \t %g\n",g.yllcorner);
    printf("cellsize \t %g\n",g.cellsize);
    printf("NODATA_value \t %g\n",g.NODATA_value);
}
void printValues(Grid g) {
    printf("\n");
    for(int i = 0; i < g.nrows; i++) {
        for(int j = 0; j < g.ncols; j++) {
            printf("%2g ", g.data[i][j]);
        }
        printf("\n");
        
    }
    
}
