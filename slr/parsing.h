//
//  Parsing.h
//  viewshed
//
//  Created by Cory Alini on 10/8/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//

#ifndef Parsing_h
#define Parsing_h

#include <stdio.h>

extern int viewpointRow;
extern int viewpointColumn;

typedef struct _grid {
    long  nrows, ncols;  // the size of the grid
    double yllcorner, xllcorner;
    double cellsize;
    double NODATA_value;
    float** data;   //the 2D array of value in the grid
    double h_min;
    double h_max;
    
} Grid;

void readGridfromFile(const char* gridfname, Grid* g, int gridType);

void gridtoFile(Grid *grid, const char* name);

void mallocGrid(Grid eg, Grid* vg);

void freeGridData(Grid* grid);

void setHeaders(Grid elevgrid, Grid* newGrid);

void printGrid(Grid g);

void printHeader(Grid g);

void printValues(Grid g);


#endif /* Parsing_h */



