//
//  Parsing.c
//  viewshed
//
//  Created by Cory Alini on 10/8/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "Parsing.h"

int viewpointRow;
int viewpointColumn;
float convertToMeters = 3.28;

void readGridfromFile(const char* gridfname, Grid* g, int gridType) {
    FILE* f;
    
    printf("reading grid %s\n",gridfname);
    f=fopen(gridfname, "r");
    
    //check to see that usr entered a file
    if (f== NULL) {
        printf("cannot open file..\n");
        exit(1);
    }
    
    int x = fscanf(f, "ncols         %li\n"
                   "nrows         %li\n"
                   "xllcorner     %lf\n"
                   "yllcorner     %lf\n"
                   "cellsize      %lf\n"
                   "NODATA_value  %lf\n", &g->ncols, &g->nrows,
                   &g->xllcorner, &g->yllcorner, &g->cellsize,
                   &g->NODATA_value);
    
    if (x != 6) {
        printf("ERROR: fscanf did not properly scan in the variables\n");
        exit(1);
    }
    g->data = (float**)malloc(g->nrows * sizeof(float *));
    assert(g->data);
    for(int i = 0; i < g->nrows; i++) {
        g->data[i] = (float*)malloc(g->ncols * sizeof(float));
        assert(g->data[i]);
    }
    
    for(int i = 0; i < g->nrows; i++) {
        for(int j = 0; j < g->ncols; j++) {
            int y = fscanf(f,"%f ",&g->data[i][j]);
            if (gridType && g->data[i][j] != g->NODATA_value ) {
                g->data[i][j] = g->data[i][j]/convertToMeters;
            }
            if (y != 1) {
                printf("ERROR: fscanf did not properly scan in the grid\n");
                exit(1);
            }
        }
    }
    fclose(f);
}
void mallocGrid(Grid eg, Grid* vg) {
    vg->data = (float**)malloc(eg.nrows * sizeof(float *));
    assert(vg->data);
    
    for(int a = 0; a < eg.nrows; a++) {
        vg->data[a] = (float*)malloc(eg.ncols * sizeof(float));
        assert(vg->data[a]);
    }
}
void gridtoFile(Grid* g, const char* fileName) {
    FILE* file;
    
    if((file = fopen(fileName, "w+")) == NULL) {
        printf("Cannot open file.\n");
        exit(1);
    }
    
    fprintf(file, "ncols         %ld\n"
            "nrows         %ld\n"
            "xllcorner     %g\n"
            "yllcorner     %g\n"
            "cellsize      %g\n"
            "NODATA_value  %g\n",
            g->ncols, g->nrows,
            g->xllcorner, g->yllcorner,
            g->cellsize, g->NODATA_value);
    
    for(int i = 0; i < g->nrows; i++) {
        for(int j = 0; j < g->ncols; j++) {
            fprintf(file,"%g ",g->data[i][j]);
        }
        fprintf(file,"\n");
    }
    fclose(file);
}


void freeGridData(Grid* g) {
    for(int i = 0; i < g->nrows; i++) {
        free(g->data[i]);
    }
    free(g->data);
}


void setHeaders(Grid elevgrid, Grid* newGrid) {
    newGrid->nrows = elevgrid.nrows;
    newGrid->ncols = elevgrid.ncols;
    newGrid->xllcorner = elevgrid.xllcorner;
    newGrid->yllcorner = elevgrid.yllcorner;
    newGrid->cellsize = elevgrid.cellsize;
    newGrid->NODATA_value = elevgrid.NODATA_value;
}

void printGrid(Grid g) {
    printHeader(g);
    printValues(g);
}
void printHeader(Grid g) {
    printf("nrows \t %ld\n",g.nrows);
    printf("ncols \t %ld\n",g.ncols);
    printf("xllcorner \t %g\n",g.xllcorner);
    printf("yllcorner \t %g\n",g.yllcorner);
    printf("cellsize \t %g\n",g.cellsize);
    printf("NODATA_value \t %g\n",g.NODATA_value);
}
void printValues(Grid g) {
    printf("\n");
    for(int i = 0; i < g.nrows; i++) {
        for(int j = 0; j < g.ncols; j++) {
                printf("%2g ", g.data[i][j]);
        }
        printf("\n");
        
    }
    
}


