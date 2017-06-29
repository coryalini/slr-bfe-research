//
//  bfe.cpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#include "bfe.hpp"

/* MARK: bfe */
void start_interp_bfe(Grid* elevgrid,Grid* local_interp_bfegrid, float rise) {
    
    char** alreadySeen;
    alreadySeen = (char**)malloc(elevgrid->nrows * sizeof(char *));
    assert(alreadySeen);
    for(int a = 0; a < elevgrid->nrows; a++) {
        alreadySeen[a] = (char*) malloc(elevgrid->ncols * sizeof(char));
        assert(alreadySeen[a]);
    }
    
    for (int i = 0; i < elevgrid->nrows; i++) {
        for (int j = 0; j < elevgrid->ncols; j++) {
            alreadySeen[i][j] = 'u';
        }
    }
    std::queue<point> queue;
    queue = findSeaPoint(elevgrid);
    compute_interp_bfe(elevgrid,local_interp_bfegrid,rise,alreadySeen,queue);
}

void compute_interp_bfe(Grid* elevgrid, Grid* local_interp_bfegrid,int rise, char** alreadySeen,std::queue<point>& queue) {
    //Iterate through left and right edges of the terrain
    for (int i = 0; i < elevgrid->nrows; i++) {
        if (local_interp_bfegrid->data[i][0] == local_interp_bfegrid->NODATA_value) {
            point newPoint;
            
            //Iterate through the left edge of the terrain
            
            if (elevgrid->data[i][0] == elevgrid->NODATA_value) {
                newPoint.x = i;
                newPoint.y = 0;
                alreadySeen[i][0] = 's';
            } else if(elevgrid->data[i][0] < rise) {
                newPoint.x = i;
                newPoint.y = 0;
                alreadySeen[i][0] = 'f';
            }
            
            //Iterate through the right edge of the terrain
            if (elevgrid->data[i][elevgrid->ncols-1] == elevgrid->NODATA_value) {
                newPoint.x = i;
                newPoint.y = elevgrid->ncols-1;
                alreadySeen[i][elevgrid->ncols-1] = 's';
            } else if(elevgrid->data[i][elevgrid->ncols-1] < rise) {
                newPoint.x = i;
                newPoint.y = elevgrid->ncols-1;
                alreadySeen[i][elevgrid->ncols-1] = 'f';
            }
            queue.push(newPoint);
            
            
        }
    }
    //Iterate through top and bottom edges of the terrain
    
    for (int j = 0; j < elevgrid->ncols; j++) {
        if (local_interp_bfegrid->data[0][j] == local_interp_bfegrid->NODATA_value) {
            point newPoint;
            
            //Iterate through the top edge of the terrain
            if (elevgrid->data[0][j] == elevgrid->NODATA_value) {
                newPoint.x = 0;
                newPoint.y = j;
                alreadySeen[0][j] = 's';
            } else if(elevgrid->data[0][j] < rise) {
                newPoint.x = 0;
                newPoint.y = j;
                alreadySeen[0][j] = 'f';
            }
            
            //Iterate through the bottom edge of the terrain
            if (elevgrid->data[elevgrid->nrows-1][j]== elevgrid->NODATA_value) {
                newPoint.x = elevgrid->nrows-1;
                newPoint.y = j;
                alreadySeen[elevgrid->nrows-1][j] = 's';
            } else if(elevgrid->data[elevgrid->nrows-1][j] < rise) {
                newPoint.x = elevgrid->nrows-1;
                newPoint.y = j;
                alreadySeen[elevgrid->nrows-1][j] = 'f';
                
            }
            queue.push(newPoint);
        }
    }
    
    std::queue<point> interp_bfequeue;
    while(queue.empty() != true) {
        point curr = queue.front();
        queue.pop();
        for (int k = curr.x-1; k <= curr.x+1; k++) {
            for (int l = curr.y-1; l <= curr.y+1;l++) {
                if (k == curr.x && l == curr.y) {
                    continue;
                }
                if (!insideGrid(elevgrid, k,l)) {
                    continue;
                }
                point newPoint;
                newPoint.x = k;
                newPoint.y = l;
                if(alreadySeen[k][l] == 'u') {
                    //if not interp_bfe  add to queue and continue
                    if(local_interp_bfegrid->data[k][l] == local_interp_bfegrid->NODATA_value) {
                        local_interp_bfegrid->data[k][l] = local_interp_bfegrid->NODATA_value;
                        queue.push(newPoint);
                        alreadySeen[k][l] = 's';
                    } else {
                        interp_bfequeue.push(newPoint);
                        alreadySeen[k][l] = 'b';
                        
                    }
                }
            }
        }
    }
    while(interp_bfequeue.empty() != true) {
        point curr = interp_bfequeue.front();
        interp_bfequeue.pop();
        for (int k = curr.x-1; k <= curr.x+1; k++) {
            for (int l = curr.y-1; l <= curr.y+1;l++) {
                if (k == curr.x && l == curr.y) {
                    continue;
                }
                if (!insideGrid(elevgrid, k,l)) {
                    continue;
                }
                point newPoint;
                newPoint.x = k;
                newPoint.y = l;
                if(alreadySeen[k][l] != 'u') {
                    continue;
                }
                if (local_interp_bfegrid->data[k][l] == local_interp_bfegrid->NODATA_value) {
                    local_interp_bfegrid->data[k][l] = local_interp_bfegrid->data[(int)curr.x][(int)curr.y];
                }
                assert(local_interp_bfegrid->data[k][l] != local_interp_bfegrid->NODATA_value);
                interp_bfequeue.push(newPoint);
                alreadySeen[k][l] = 'b';
            }
        }
    }
    for(int a = 0; a < elevgrid->nrows; a++) {
        free(alreadySeen[a]);
    }
    free(alreadySeen);
    
    
}








/* MARK: bfe */
void start_interp_bfe_withFlooded(Grid* elevgrid, Grid* local_interp_bfegrid,float rise) {

    char** alreadySeen;
    alreadySeen = (char**)malloc(elevgrid->nrows * sizeof(char *));
    assert(alreadySeen);
    for(int a = 0; a < elevgrid->nrows; a++) {
        alreadySeen[a] = (char*) malloc(elevgrid->ncols * sizeof(char));
        assert(alreadySeen[a]);
    }
    //initialize
    for (int i = 0; i < elevgrid->nrows; i++) {
        for (int j = 0; j < elevgrid->ncols; j++) {
            alreadySeen[i][j] = 'u';
        }
    }
    std::queue<point> queue;
    queue = findSeaPoint(elevgrid);
    compute_interp_bfe_withFlooded(elevgrid,local_interp_bfegrid,rise,alreadySeen,queue);
}

void compute_interp_bfe_withFlooded(Grid* elevgrid, Grid* local_interp_bfegrid,int rise, char** alreadySeen,std::queue<point>& queue) {
    
    //Iterate through left and right edges of the terrain
    for (int i = 0; i < elevgrid->nrows; i++) {
        if (local_interp_bfegrid->data[i][0] == local_interp_bfegrid->NODATA_value) {
            point newPoint;
            
            //Iterate through the left edge of the terrain
            
            if (elevgrid->data[i][0] == elevgrid->NODATA_value) {
                newPoint.x = i;
                newPoint.y = 0;
                alreadySeen[i][0] = 's';
            } else if(elevgrid->data[i][0] < rise) {
                printf("yup\n");
                newPoint.x = i;
                newPoint.y = 0;
                alreadySeen[i][0] = 'f';
            }
            
            //Iterate through the right edge of the terrain
            if (elevgrid->data[i][elevgrid->ncols-1] == elevgrid->NODATA_value) {
                newPoint.x = i;
                newPoint.y = elevgrid->ncols-1;
                alreadySeen[i][elevgrid->ncols-1] = 's';
            } else if(elevgrid->data[i][elevgrid->ncols-1] < rise) {
                newPoint.x = i;
                newPoint.y = elevgrid->ncols-1;
                alreadySeen[i][elevgrid->ncols-1] = 'f';
            }
            queue.push(newPoint);
            
            
        }
    }
    //Iterate through top and bottom edges of the terrain
    
    for (int j = 0; j < elevgrid->ncols; j++) {
        if (local_interp_bfegrid->data[0][j] == local_interp_bfegrid->NODATA_value) {
            point newPoint;
            
            //Iterate through the top edge of the terrain
            if (elevgrid->data[0][j] == elevgrid->NODATA_value) {
                newPoint.x = 0;
                newPoint.y = j;
                alreadySeen[0][j] = 's';
            } else if(elevgrid->data[0][j] < rise) {
                newPoint.x = 0;
                newPoint.y = j;
                alreadySeen[0][j] = 'f';
            }
            
            //Iterate through the bottom edge of the terrain
            if (elevgrid->data[elevgrid->nrows-1][j]== elevgrid->NODATA_value) {
                newPoint.x = elevgrid->nrows-1;
                newPoint.y = j;
                alreadySeen[elevgrid->nrows-1][j] = 's';
            } else if(elevgrid->data[elevgrid->nrows-1][j] < rise) {
                newPoint.x = elevgrid->nrows-1;
                newPoint.y = j;
                alreadySeen[elevgrid->nrows-1][j] = 'f';
                
            }
            queue.push(newPoint);
        }
    }
    
    
    std::queue<point> interp_bfequeue;
    //FIND THE EDGE OF THE BFE GRID
    while(queue.empty() != true) {
        point curr = queue.front();
        queue.pop();
        for (int k = curr.x-1; k <= curr.x+1; k++) {
            for (int l = curr.y-1; l <= curr.y+1;l++) {
                if (k == curr.x && l == curr.y) {
                    continue;
                }
                if (!insideGrid(elevgrid, k,l)) {
                    continue;
                }
                point newPoint;
                newPoint.x = k;
                newPoint.y = l;
                if(alreadySeen[k][l] == 'u') {
                    //if not interp_bfe  add to queue and continue
                    if(local_interp_bfegrid->data[k][l] == local_interp_bfegrid->NODATA_value) {
                        local_interp_bfegrid->data[k][l] = local_interp_bfegrid->NODATA_value;
                        if (elevgrid->data[k][l] == elevgrid->NODATA_value) {
                            alreadySeen[k][l] = 's';
                        } else {
                            queue.push(newPoint);
                            alreadySeen[k][l] = 'l';
                        }
                    } else {
                        interp_bfequeue.push(newPoint);
                        alreadySeen[k][l] = 'b';
                        
                    }
                }
            }
        }
    }
    //GO THROUGH THE BFE POINTS AND INTERPOLATE
    while(interp_bfequeue.empty() != true) {
        point curr = interp_bfequeue.front();
        interp_bfequeue.pop();
        for (int k = curr.x-1; k <= curr.x+1; k++) {
            for (int l = curr.y-1; l <= curr.y+1;l++) {
                if (k == curr.x && l == curr.y) {
                    continue;
                }
                if (!insideGrid(elevgrid, k,l)) {
                    continue;
                }
                point newPoint;
                newPoint.x = k;
                newPoint.y = l;
                //if the point is unseen, floooded, or land then just skip it
                if(alreadySeen[k][l] != 'u' && alreadySeen[k][l] != 'f'&& alreadySeen[k][l] != 'l') {
                    continue;
                }
                if (local_interp_bfegrid->data[k][l] == local_interp_bfegrid->NODATA_value) {
                    local_interp_bfegrid->data[k][l] = local_interp_bfegrid->data[(int)curr.x][(int)curr.y];
                }
                assert(local_interp_bfegrid->data[k][l] != local_interp_bfegrid->NODATA_value);
                interp_bfequeue.push(newPoint);
                alreadySeen[k][l] = 'b'; // change the value so it has a bfe
            }
        }
    }
    
    for(int a = 0; a < elevgrid->nrows; a++) {
        free(alreadySeen[a]);
    }
    free(alreadySeen);
}











