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


#define VUL_MARGIN 0.1
#define NEW_WATER -8000
#define HAVENT_VISITED -7000

typedef struct {float x,y;} point;

typedef struct _grid {
  const char* gridname;
  long  nrows, ncols;  // the size of the grid
  double yllcorner, xllcorner;
  double** data;   //the 2D array of value in the grid
  double cellsize;
  double NODATA_value;
  double min;
  double max;
} Grid;


void findSeaPoint(Grid* elevgrid,std::queue<point>*);

void setinterp_bfeNotVisited(Grid* elevgrid, Grid* interp_bfegrid,
			     Grid* floodedgrid, float rise);
void setNotVisited(Grid* elevgrid, Grid* floodedgrid, float rise);

int insideGrid(Grid* grid, int i, int j);
double findMax(Grid* grid);
double findMinLand(Grid* grid);

void copyGrid(Grid* originalGrid, Grid* copyGrid);
void freeGridData(Grid* grid);

//parsing
void readGridfromFile(const char* gridfname, Grid* g, double converter);

void gridtoFile(Grid *grid, const char* name);

void mallocGrid(Grid eg, Grid* vg);

void setHeaders(Grid elevgrid, Grid* newGrid);

void printGrid(Grid g);

void printHeader(Grid g);

void printValues(Grid g);





#endif /* grid_hpp */



