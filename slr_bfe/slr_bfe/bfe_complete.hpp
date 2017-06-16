//
//  bfe_complete.hpp
//  slr_bfe
//
//  Created by Cory Alini on 6/13/17.
//  Copyright © 2017 slr_bfe.coryalini. All rights reserved.
//

#ifndef bfe_complete_hpp
#define bfe_complete_hpp
#include "bridge.hpp"


#include <iostream>
#include <queue>
#include <assert.h>

void start_interp_bfe_withFlooded(Grid* elevgrid,Grid* interp_bfegrid, float rise);

void compute_interp_bfe_withFlooded(Grid* elevgrid, Grid* interp_bfegrid,int rise, char** alreadySeen,std::queue<point>& queue);

#endif /* bfe_complete_hpp */

