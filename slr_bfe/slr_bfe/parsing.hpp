//
//  Parsing.h
//  viewshed
//
//  Created by Cory Alini on 10/8/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//

#ifndef Parsing_h
#define Parsing_h

#include "bridge.hpp"

#include <stdio.h>
extern int viewpointRow;
extern int viewpointColumn;

extern int ELEV_TYPE;
extern int BFE_TYPE;

void readGridfromFile(const char* gridfname, Grid* g, int gridType);

void gridtoFile(Grid *grid, const char* name);

void mallocGrid(Grid eg, Grid* vg);

void freeGridData(Grid* grid);

void setHeaders(Grid elevgrid, Grid* newGrid);

void printGrid(Grid g);

void printHeader(Grid g);

void printValues(Grid g);


#endif /* Parsing_h */




