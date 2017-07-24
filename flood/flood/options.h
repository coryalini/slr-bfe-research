//
//  options.h
//  flood
//
//  Created by Cory Alini on 7/19/17.
//  Copyright © 2017 slr_bfe.coryalini. All rights reserved.
//

#ifndef options_h
#define options_h

#define SEA_IS_NODATA 0
#define SEA_LEVEL 0

#define VULNERABLE_AREAS 0
#define VUL_MARGIN 0.1


#define NEW_WATER -8000
#define HAVENT_VISITED -7000

//used to convert elevgrid units
#define ELEV_CONVERTER 3.28084//convert elev from m to ft
//#define ELEV_CONVERTER 1//convert elev from m to ft



//used to convert bfegrid units
#define BFE_CONVERTER 1
//#define BFE_CONVERTER 0.3048


#endif /* options_h */
