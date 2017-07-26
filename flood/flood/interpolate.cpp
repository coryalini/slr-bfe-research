//
//  bfe.cpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#include "interpolate.hpp"

/* MARK: bfe */


/*
 Interpolation using nearest neighbor. Whichever neighbor was nearest gives
 the point its bfe value
 */
void interpolation_nn(Grid* origgrid, Grid* interpgrid) {
    
    //initialize
    point newPoint;
    std::queue<point> interpqueue;
    for (int i = 0; i < origgrid->nrows; i++) {
        for (int j = 0; j < origgrid->ncols; j++) {
            if (origgrid->data[i][j] != origgrid->NODATA_value && isBoundary(i,j,origgrid)) {
                newPoint.x = i;
                newPoint.y = j;
                interpqueue.push(newPoint);
            }
        }
    }
    
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
/*
 Interpolation using inverse distance weighted (idw). Really really slow. 
 It basically takes teh idw of all the bfe points and assigns it to the curr point

 */

void interpolation_idw(Grid* origgrid, Grid* interpgrid) {
    
    //initialize
    point newPoint;
    std::vector<point> boundary;
    for (int i = 0; i < origgrid->nrows; i++) {
        for (int j = 0; j < origgrid->ncols; j++) {
            newPoint.x = i;
            newPoint.y = j;
            if (origgrid->data[i][j] != origgrid->NODATA_value && isBoundary(i,j,origgrid)) {
                    boundary.push_back(newPoint);
            }
        }
    }
    for (int i = 0; i < origgrid->nrows; i++) {
        for (int j = 0; j < origgrid->ncols; j++) {
            if(origgrid->data[i][j] == origgrid->NODATA_value) {
                double val = 0, sum = 0;
                for (int k = 0; k < boundary.size(); k++) {
                    double distancex = i-(int)boundary[k].x;
                    double distancey = j-(int)boundary[k].y;
                    double distance =distancex*distancex + distancey*distancey;
                    
                    double weight = 1.0/distance;
                    val+=weight * origgrid->data[(int)boundary[k].x][(int)boundary[k].y];
                    sum +=weight;
                }//for k
                double interp_val = val/sum;
                interpgrid->data[i][j]=interp_val;
                
            }
        }
    }
}
/*
 Interpolation using approximate idw. This method runs a lot faster than just idw
 It first grabs all the points in a group. ex, all the bfe points with bfe value of 4
 that are next to each other. Then the algorithm prunes it such that there are only a certain
 number of points. 
 
 then it goes through using these vectors and does the idw part just not with all of the points
 */
void interpolation_approx_idw(Grid* origgrid, Grid* interpgrid){
    srand ((unsigned int)time(NULL));

    char** marked = (char**)malloc(origgrid->nrows * sizeof(char *));
    assert(marked);
    for(int a = 0; a < origgrid->nrows; a++) {
        marked[a] = (char*) malloc(origgrid->ncols * sizeof(char));
        assert(marked[a]);
    }
    
    for (int i = 0; i < origgrid->nrows; i++) {
        for (int j = 0; j < origgrid->ncols; j++) {
            marked[i][j] = 'n';
        }
    }

    point newPointBoundary;
    point newPoint;
    std::vector< std::vector<point> > allboundary;
//    vector<point> *allboundary = new vector<point>(366, vector<int>(4));

    for (int i = 0; i < origgrid->nrows; i++) {
        for (int j = 0; j < origgrid->ncols; j++) {
            if(origgrid->data[i][j] != origgrid->NODATA_value && isBoundary(i, j, origgrid) && marked[i][j] == 'n'){
//                std::vector<point> thisboundary;
                std::vector<point>* thisboundary = new std::vector<point>();

                std::vector<point> localvector;

                newPointBoundary.x = i;
                newPointBoundary.y = j;
                //we start to find a new boundary piece
                thisboundary->push_back(newPointBoundary);
                localvector.push_back(newPointBoundary);

                //find and mark the part of the boundary that has the same value as (i,j)
                marked[i][j] = 'y';
                
                while(localvector.empty() != true){
//                    printf("thisboundary: %lu\n",thisboundary.size());
//                    printf("localvec: %lu\n",localvector.size());
                    point curr = localvector.back();
                    int k = curr.x;
                    int l = curr.y;
                    localvector.pop_back();
                    for (int p = k-1; p <= k+1; p++) {
                        for (int q = l-1; q <= l+1;q++) {
                            if ((p == k && q == l) || !insideGrid(origgrid, p,q)){
                                continue;
                            }
                            if (isBoundary(p, q, origgrid) && (origgrid->data[k][l] == origgrid->data[p][q]) && (marked[p][q] == 'n')) {
//                                printf("[%d,%d]: %lf\n", p,q,origgrid->data[p][q]);
                                marked[p][q] = 'y';
                                newPoint.x = p;
                                newPoint.y = q;
                                localvector.push_back(newPoint);
                                thisboundary->push_back(newPoint);
                            }
                        }//for q
                    }//for p
                    

                }//while loop
                
                //claim: the part of the boundary that has the same value as (i,j) and is connected to it is now marked , and is stored in boundary
  
                pruneBoundary(thisboundary);
                allboundary.push_back(*thisboundary);
                //we have the points in this piece. We can prune as we like, for e.g. we can do : keep 10% of the points; or keep 20 points
            }
        }//for j
    }//for i
    for (int i = 0; i < origgrid->nrows; i++) {
        for (int j = 0; j < origgrid->ncols; j++) {
            if(origgrid->data[i][j] == origgrid->NODATA_value) {
                double val = 0, sum = 0;
                for (int k = 0; k < allboundary.size(); k++) {
                    for (int l = 0; l < allboundary[k].size(); l++){
                        int x =(int)allboundary[k].at(l).x;
                        int y =(int)allboundary[k].at(l).y;
                        double distancex = i-x;
                        double distancey = j-y;
                        double distance =distancex*distancex + distancey*distancey;
                        
                        double weight = 1.0/distance;
                        val+=weight * origgrid->data[x][y];
                        sum +=weight;
                    }
                }//for k
                double interp_val = val/sum;
                interpgrid->data[i][j]=interp_val;
                
            }
        }
    }
}

    
void pruneBoundary(std::vector<point>* thisboundary) {
    int desiredVectorSize = 4;
//    printf("size:%lu\n",thisboundary->size());
    while (thisboundary->size() > desiredVectorSize) {
        
        //generates random number between 0 and 1 less than the vector sie
        int random = rand() % thisboundary->size();
        thisboundary->erase(thisboundary->begin() + random);
    }
}




/*
 If point i,j has at least 1 no data neighbor, return 1.
 Else 0
 */

int isBoundary(int i, int j, Grid* origgrid) {
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

