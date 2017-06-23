//
//  bfe.hpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#ifndef bfe_hpp
#define bfe_hpp

#include "grid.hpp"


#include <iostream>
#include <queue>
#include <assert.h>

/*MARK bfe*/
void start_interp_bfe(Grid* elevgrid,Grid* interp_bfegrid, float rise,int seaX, int seaY);

void compute_interp_bfe(Grid* elevgrid, Grid* interp_bfegrid,int rise, char** alreadySeen,std::queue<point>& queue);

void start_interp_bfe_withFlooded(Grid* elevgrid,Grid* interp_bfegrid, float rise);

void compute_interp_bfe_withFlooded(Grid* elevgrid, Grid* interp_bfegrid,int rise, char** alreadySeen,std::queue<point>& queue);


#endif /* bfe_hpp */
