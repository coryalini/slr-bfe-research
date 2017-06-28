//
//  flood.hpp
//  slr_bfe
//
//  Created by Cory Alini on 6/23/17.
//  Copyright © 2017 slr_bfe.coryalini. All rights reserved.
//

#ifndef flood_hpp
#define flood_hpp

#include "grid.hpp"
#include <stdio.h>


/*MARK SLR*/
void start_slr(Grid* elevgrid,Grid* floodgrid,float rise);

void compute_slr(Grid* elevgrid, Grid* floodgrid,float rise, std::queue<point>& queue);

void setNotVisited(Grid* elevgrid, Grid* floodgrid, float rise);


/*MARK SLR+interp_bfe*/
void start_slr_interp_bfe(Grid* elevgrid, Grid* interp_bfegrid, Grid* floodgrid, float rise);

void compute_slr_interp_bfe(Grid* elevgrid,Grid* interp_bfegrid, Grid* floodgrid, float rise,std::queue<point>& queue);

#endif /* flood_hpp */
