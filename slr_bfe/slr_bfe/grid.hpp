//
//  bridge.hpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#ifndef grid_hpp
#define grid_hpp

#define DEBUG 0
#define debug_printf(string, args ...) if (DEBUG){  \
printf("%s:%d: ", __FUNCTION__, __LINE__ );   \
printf(string , ##args); }


#include <iostream>
#include <queue>
#include <assert.h>
typedef struct {float x,y;} point;

typedef struct _grid {
    const char* gridname;
    long  nrows, ncols;  // the size of the grid
    double yllcorner, xllcorner;
    double cellsize;
    double NODATA_value;
    float** data;   //the 2D array of value in the grid
    double h_min;
    double h_max;
    
} Grid;
extern Grid elevgrid, bfegrid, slrgrid, slr_interp_bfegrid, interp_bfegrid,currgrid;

//extern Grid elevgrid, interp_bfegrid, slrgrid, slr_interp_bfegrid, originterp_bfegrid, currGrid;
extern int offsets [8][2];

extern const int NEW_WATER;
extern const int HAVENT_VISITED;
//extern double max,min,minLand;
//extern double maxElev,minElev,minLandElev;
extern const char *elevname, *writeGridname, *bfename;

//parsing
extern int ELEV_TYPE;
extern int BFE_TYPE;

std::queue<point> findSeaPoint(Grid* elevgrid);

void setinterp_bfeNotVisited(Grid* elevgrid, Grid* slrgrid, Grid* interp_bfegrid, float rise);
void setNotVisited(Grid* elevgrid, Grid* slrgrid, float rise);

int insideGrid(Grid* grid, int i, int j);
double findMax(Grid* grid);
double findMinLand(Grid* grid);

void copyGrid(Grid* originalGrid, Grid* copyGrid);
void freeGridData(Grid* grid);


void outputGridWithDepth(Grid* g, Grid* slrgrid,Grid* elevgrid,float rise);
void outputGridWithDepthWITHinterp_bfe(Grid* g, Grid* slrgrid,Grid* elevgrid,Grid* interp_bfegrid, float rise);

//parsing
void readGridfromFile(const char* gridfname, Grid* g, int gridType);

void gridtoFile(Grid *grid, const char* name);

void mallocGrid(Grid eg, Grid* vg);

void setHeaders(Grid elevgrid, Grid* newGrid);

void printGrid(Grid g);

void printHeader(Grid g);

void printValues(Grid g);





#endif /* grid_hpp */



