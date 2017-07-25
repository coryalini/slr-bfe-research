//
//  bfe.cpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#include "interpolate.hpp"

/* MARK: bfe */

void interpolation_nn(Grid* origgrid, Grid* interpgrid) {
    
    //initialize
    point newPoint;
    std::queue<point> interpqueue;
    for (int i = 0; i < origgrid->nrows; i++) {
        for (int j = 0; j < origgrid->ncols; j++) {
            if (origgrid->data[i][j] != origgrid->NODATA_value && has_NOData_Neighbor(i,j,origgrid)) {
                newPoint.x = i;
                newPoint.y = j;
                interpqueue.push(newPoint);
            }
        }
    }
    
    printf("size: %lu\n", interpqueue.size());
    
    //GO THROUGH THE BFE POINTS AND INTERPOLATE
    while(interpqueue.empty() != true) {
        point curr = interpqueue.front();
        interpqueue.pop();
        
        //if you are in the queue you must have a bfe value
        assert(interpgrid->data[(int)curr.x][(int)curr.y]!= origgrid->NODATA_value);
        
        //for each neighbor, if inside grid && is no data
        for (int k = curr.x-1; k <= curr.x+1; k++) {
            for (int l = curr.y-1; l <= curr.y+1;l++) {
                if ((k == curr.x && l == curr.y)) {
                    continue;
                }
                if (insideGrid(origgrid, k,l) && interpgrid->data[k][l] == origgrid->NODATA_value) {
                    point newPoint;
                    newPoint.x = k;
                    newPoint.y = l;
                    interpgrid->data[k][l] = interpgrid->data[(int)curr.x][(int)curr.y];
                    interpqueue.push(newPoint);
                    
                }
            }
        }
    }
    
}


void interpolation_idw(Grid* origgrid, Grid* interpgrid) {
    
    //initialize
    point newPoint;
    std::vector<point> interp_boundary_Points;
    for (int i = 0; i < origgrid->nrows; i++) {
        for (int j = 0; j < origgrid->ncols; j++) {
            newPoint.x = i;
            newPoint.y = j;
            if (origgrid->data[i][j] != origgrid->NODATA_value && has_NOData_Neighbor(i,j,origgrid)) {
                    interp_boundary_Points.push_back(newPoint);
            }
        }
    }
    for (int i = 0; i < origgrid->nrows; i++) {
        for (int j = 0; j < origgrid->ncols; j++) {
            if(origgrid->data[i][j] == origgrid->NODATA_value) {
                double val = 0, sum = 0;
                for (int k = 0; k < interp_boundary_Points.size(); k++) {
                    double weight = 1.0/distance_with_p(i, j, interp_boundary_Points[k]);
                    val+=weight * origgrid->data[(int)interp_boundary_Points[k].x][(int)interp_boundary_Points[k].y];
                    sum +=weight;
                }
                double interp_val = val/sum;
                interpgrid->data[i][j]=interp_val;
                
                
            }
        }
    }
}
    
    

double distance_with_p(int i,int j, point interp_point) {
    double distancex = (i-(int)interp_point.x)* (i-(int)interp_point.x);
    double distancey = (j - (int)interp_point.y)*(j - (int)interp_point.y);
    return distancex+distancey;
}

/*
 If point i,j has at least 1 no data neighbor, return 1.
 Else 0
 */

int has_NOData_Neighbor(int i, int j, Grid* origgrid) {
    for (int k = i-1; k <= i+1; k++) {
        for (int l = j-1; l <= j+1;l++) {
            if ((k == i && l == j)) {
                continue;
            }
            if (!insideGrid(origgrid, k,l)){
                continue;
            }
            if (origgrid->data[k][l] == origgrid->NODATA_value){
                return 1;
            }
        }
    }
    //If we are here, none of the neighbors is NODATA
    return 0;
}

