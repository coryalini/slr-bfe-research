//
//  bridge.hpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#ifndef bridge_hpp
#define bridge_hpp

#define DEBUG 0
#define debug_printf(string, args ...) if (DEBUG){  \
printf("%s:%d: ", __FUNCTION__, __LINE__ );   \
printf(string , ##args); }


#include <iostream>
#include <queue>

typedef struct {float x,y;} point;

typedef struct _grid {
    long  nrows, ncols;  // the size of the grid
    double yllcorner, xllcorner;
    double cellsize;
    double NODATA_value;
    float** data;   //the 2D array of value in the grid
    double h_min;
    double h_max;
    
} Grid;

extern Grid elevgrid, interp_bfegrid, slrgrid, interp_bfeslrgrid, originterp_bfegrid;
extern int DRAW, interp_bfe_EXISTS, offsets [8][2];
extern float rise;

extern const int NEW_WATER;
extern const int HAVENT_VISITED;
extern float minX,minY,maxX,maxY,max,min, minXLand,minYLand,minLand;

std::queue<point> findSeaPoint(Grid* elevgrid);

void setinterp_bfeNotVisited(Grid* elevgrid, Grid* slrgrid, Grid* interp_bfegrid, float rise);
void setNotVisited(Grid* elevgrid, Grid* slrgrid, float rise);

int insideGrid(Grid* grid, int i, int j);
void findMaxMin(Grid* grid);

void outputGridWithDepth(Grid* g, Grid* slrgrid,Grid* elevgrid,float rise);
void outputGridWithDepthWITHinterp_bfe(Grid* g, Grid* slrgrid,Grid* elevgrid,Grid* interp_bfegrid, float rise);




#endif /* bridge_hpp */



