//
//  slr_bfe.hpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#ifndef slr_bfe_hpp
#define slr_bfe_hpp
#include "bridge.hpp"


#include <iostream>
#include <queue>
#include <assert.h>


/*MARK SLR+interp_bfe*/
void start_slr_interp_bfe(Grid* elevgrid, Grid* slr_interp_bfegrid, Grid* interp_bfegrid, float rise);

void compute_slr_interp_bfe(Grid* elevgrid, Grid* slr_interp_bfegrid, Grid* interp_bfegrid,float rise,std::queue<point>& queue);


#endif /* slr_bfe_hpp */
