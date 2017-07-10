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
void start_interp_bfe_withFlooded(Grid* elevgrid, Grid* local_interp_bfegrid);

void compute_interp_bfe_withFlooded(Grid* elevgrid, Grid* local_interp_bfegrid, char** alreadySeen);

void start_interpolation(Grid* origgrid, Grid* interpgrid);
void compute_interpolation(Grid* origgrid, Grid* interpgrid, char** alreadySeen);

#endif /* bfe_hpp */
