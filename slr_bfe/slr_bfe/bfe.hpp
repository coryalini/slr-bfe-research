//
//  bfe.hpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#ifndef bfe_hpp
#define bfe_hpp

#include "bridge.hpp"


#include <iostream>
#include <queue>
#include <assert.h>

/*MARK BFE*/
void start_bfe(Grid* elevgrid,Grid* bfegrid, float rise,int seaX, int seaY);

void compute_bfe(Grid* elevgrid, Grid* bfegrid,int rise, char** alreadySeen,std::queue<point>& queue);


#endif /* bfe_hpp */
