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

#include <math.h>
#include <iostream>
#include <queue>
#include <assert.h>

/*MARK bfe*/

void interpolation_nn(Grid* origgrid, Grid* interpgrid);

void interpolation_idw(Grid* origgrid, Grid* interpgrid);

double distance_with_p(int i,int j, point interp_point);
int has_NOData_Neighbor(int i, int j, Grid* origgrid);

#endif /* bfe_hpp */
