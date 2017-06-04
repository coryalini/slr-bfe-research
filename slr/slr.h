//
//  slr.h
//  slr
//
//  Created by Cory Alini on 11/19/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//

#ifndef slr_h
#define slr_h

#include <iostream>
#include <queue>
#include "parsing.h"

#endif /* slr_h */

typedef struct {float x,y;} point;

/*MARK SLR*/

void start_slr(Grid* elevgrid, Grid* slrgrid,float rise, int seaX, int seaY);

void compute_slr(Grid* elevgrid, Grid* slrgrid,float rise, std::queue<point>& queue);

void setNotVisited(Grid* elevgrid, Grid* slrgrid, float rise);
std::queue<point> findSeaPoint(Grid* elevgrid);

/*MARK BFE*/
void start_bfe(Grid* elevgrid,Grid* bfegrid, float rise,int seaX, int seaY);

void compute_bfe(Grid* elevgrid, Grid* bfegrid,int rise, char** alreadySeen,std::queue<point>& queue);

void setBFENotVisited(Grid* elevgrid, Grid* slrgrid, Grid* bfegrid, float rise);


/*MARK SLR+BFE*/
void start_slr_bfe(Grid* elevgrid, Grid* slrgrid, Grid* bfegrid, float rise,int seaX, int seaY);

void compute_slr_bfe(Grid* elevgrid, Grid* slrgrid, Grid* bfegrid,float rise,std::queue<point>& queue);

void setBFENotVisited(Grid* elevgrid, Grid* slrgrid, Grid* bfegrid, float rise);


/*MARK All other stuff*/

//std::queue<point> findInitialSea(Grid* elevgrid);

int insideGrid(Grid* grid, int i, int j);

float* findMaxMin(Grid* grid, float* array);

void outputGridWithDepth(Grid* g, Grid* slrgrid,Grid* elevgrid,float rise);

void outputGridWithDepthWITHBFE(Grid* g, Grid* slrgrid,Grid* elevgrid,Grid* bfegrid, float rise);





