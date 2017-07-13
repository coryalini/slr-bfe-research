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

//#define VUL_MARGIN 0.1


const int NEW_WATER = -8000;
const int HAVENT_VISITED = -7000;


/*
 This function takes the elevgrid and goes through the perimeter and finds
 which points are considered sea. Then it puts them on a queue and returns the queue
 
 */
void findSeaPoint(Grid* elevgrid,std::queue<point>* queue) {
    for (int i = 0; i < elevgrid->nrows; i++) {
        point newPoint;
        if (elevgrid->data[i][0] == elevgrid->NODATA_value) {
            newPoint.x = i;
            newPoint.y = 0;
            queue->push(newPoint);
        }
        if (elevgrid->data[i][elevgrid->ncols-1] == elevgrid->NODATA_value) {
            newPoint.x = i;
            newPoint.y = elevgrid->ncols-1;
            queue->push(newPoint);
        }
        
    }
    
    for (int j = 0; j < elevgrid->ncols; j++) {
        point newPoint;
        if (elevgrid->data[0][j] == elevgrid->NODATA_value) {
            newPoint.x = 0;
            newPoint.y = j;
            queue->push(newPoint);
            
        }
        if (elevgrid->data[elevgrid->nrows-1][j] == elevgrid->NODATA_value) {
            newPoint.x = elevgrid->nrows-1;
            newPoint.y = j;
            queue->push(newPoint);
        }

    }
}

/*
 The function goes through the grid and if the value was not visited via 
 BFS it sets it to the adjusted value
 */
void setNotVisited(Grid* elevgrid, Grid* floodedgrid, float rise) {
    for (int i = 0; i < elevgrid->nrows; i++) {
        for (int j = 0; j < elevgrid->ncols; j++) {
            if (floodedgrid->data[i][j] == HAVENT_VISITED) {
                floodedgrid->data[i][j] = elevgrid->data[i][j] - rise;
            }
//            if (floodedgrid->data[i][j] <= rise + VUL_MARGIN && floodedgrid->data[i][j] >= rise - VUL_MARGIN) {
//                floodedgrid->data[i][j] = 0;
//            }
             
        }
    }
}
/*
 The function goes through the grid and if the value was not visited via
 BFS it sets it to the adjusted value
 */
void setinterp_bfeNotVisited(Grid* elevgrid, Grid* interp_bfegrid,Grid* floodedgrid, float rise) {
    for (int i = 0; i < floodedgrid->nrows; i++) {
        for (int j = 0; j < floodedgrid->ncols; j++) {
            if (floodedgrid->data[i][j] == HAVENT_VISITED) {
                if (interp_bfegrid->data[i][j] != interp_bfegrid->NODATA_value) {
                    if (floodedgrid->data[i][j] == NEW_WATER) {
                        floodedgrid->data[i][j] = elevgrid->data[i][j] - (rise+ interp_bfegrid->data[i][j]);

                    } else {
                        floodedgrid->data[i][j] = elevgrid->data[i][j] - rise;
                    }
                } else {
                    floodedgrid->data[i][j] = elevgrid->data[i][j] - rise;
                }
            }
            
//            if (interp_bfegrid->data[i][j] != interp_bfegrid->NODATA_value) {
//                if(elevgrid->data[i][j]<=(rise+ interp_bfegrid->data[i][j]) + VUL_MARGIN && elevgrid->data[i][j] >= (rise+ interp_bfegrid->data[i][j]) - VUL_MARGIN) {
//                    floodedgrid->data[i][j] = 0;
//                }
//            } else {
//                if(elevgrid->data[i][j]<=rise + VUL_MARGIN && elevgrid->data[i][j] >= rise - VUL_MARGIN) {
//                    floodedgrid->data[i][j] = 0;
//                }
//            }

            

//            if (floodedgrid->data[i][j] <= rise + VUL_MARGIN && floodedgrid->data[i][j] >= rise - VUL_MARGIN) {
//                floodedgrid->data[i][j] = 0;
//            }
        }
    }
}


/* MARK: Extra stuff */
/*
 Finds if the point is in or outside of the grid. Returns 0 if outside the grid.
 */
int insideGrid(Grid* grid, int i, int j) {
    if (i < 0 || i >= grid->nrows || j < 0 || j >= grid->ncols) {
        return 0;
    }
    return 1;
}

/*
 Finds the max point of a given grid
 */
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

/*
 Finds the minimum point of a given grid. Excludes NEW_WATER and NODATA_values
 */
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
    
    vg->data = (double**)malloc(eg.nrows * sizeof(double *));
    assert(vg->data);
    
    for(int a = 0; a < eg.nrows; a++) {
        vg->data[a] = (double*)malloc(eg.ncols * sizeof(double));
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

/*
 This function reads the file, reads the headers
 then mallocs a new grid and puts the values in it. 
 */
void readGridfromFile(const char* gridfname, Grid* g, double converter) {
    FILE* f;
    
    printf("reading grid %s\n",gridfname);
    f=fopen(gridfname, "r");
    
    //check to see that usr entered a file
    if (f== NULL) {
        printf("cannot open file %s...\n",gridfname);
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

    
    g->data = (double**)malloc(g->nrows * sizeof(double *));
    assert(g->data);
    for(int i = 0; i < g->nrows; i++) {
        g->data[i] = (double*)malloc(g->ncols * sizeof(double));
        assert(g->data[i]);
    }
    
    for(int i = 0; i < g->nrows; i++) {
        for(int j = 0; j < g->ncols; j++) {
            int y = fscanf(f,"%lf ",&g->data[i][j]);
            if (y != 1) {
                printf("ERROR: fscanf did not properly scan in the grid\n");
                exit(1);
            }
            if (g->data[i][j] != g->NODATA_value) {
                g->data[i][j] *= converter;
            }
        }
    }
    fclose(f);
}

/*
 The function writes the grid to a file.
 */

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
