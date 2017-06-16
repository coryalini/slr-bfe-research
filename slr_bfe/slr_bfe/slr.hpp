//
//  slr.h
//  slr
//
//  Created by Cory Alini on 11/19/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//

#ifndef slr_h
#define slr_h
#include "bridge.hpp"

#include <iostream>
#include <queue>
/*MARK SLR*/
void start_slr(Grid* elevgrid, Grid* slrgrid,float rise);

void compute_slr(Grid* elevgrid, Grid* slrgrid,float rise, std::queue<point>& queue);

void setNotVisited(Grid* elevgrid, Grid* slrgrid, float rise);



#endif /* slr_h */

