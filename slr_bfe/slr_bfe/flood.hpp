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
Grid start_slr(Grid* elevgrid,float rise);

void compute_slr(Grid* elevgrid, Grid* local_slrgrid,float rise, std::queue<point>& queue);

void setNotVisited(Grid* elevgrid, Grid* local_slrgrid, float rise);


/*MARK SLR+interp_bfe*/
Grid start_slr_interp_bfe(Grid* elevgrid, Grid* interp_bfegrid, float rise);

void compute_slr_interp_bfe(Grid* elevgrid, Grid* local_slr_interp_bfegrid, Grid* interp_bfegrid,float rise,std::queue<point>& queue);

#endif /* flood_hpp */
