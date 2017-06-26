//
//  main.c
//  viewshed
//
//  Created by Cory Alini on 10/8/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//

/* COMMANDS:
 q: quit
 w: write to files
 
 e: Draw Elevgrid
 s: Draw SLR
 d: Draw SLR-Flooded (SLR-elev)
 a: Draw SLR with gray land
 j: Draw the original sea
 k: Draw Sea+SLR-Flooded
 
 b: Draw original BFE
 i: Draw interpolated BFE
 c: Draw BFE+SLR
 v: Draw BFE+SLR-Flooded
 l: Draw Sea+(BFE+SLR)-Flooded
 f: Draw Stormflooding (BFE+SLR)-SLR
 
 
 +: increase the sea level rise
 =: increase the sea level rise
 -: decrease the sea level rise
 */

/* Assumptions:
 no data is water.
 no data + 1 isnt going to be a height
 */


#include "flood.hpp"
#include "bfe.hpp"
#include "grid.hpp"
#include "drawing.hpp"


#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <time.h>



/* forward declarations of functions */
void keypress(unsigned char key, int x, int y);
void reset();
void calculateGrids(Grid* elevgrid);
void display(void);

/* global variables */
const int WINDOWSIZE = 500;
const int POINT_SIZE  = 5.0f;
int interp_bfe_EXISTS = 1, DRAW = 0;
const char *elevname, *writeGridname, *bfename;
float rise;



int main(int argc, char * argv[]) {
    
    if (argc < 4 || argc > 5) {
        printf("ERROR: Arguments were not included. %d \n", argc);
        return 1;
    }
    if (argc < 5) {
        interp_bfe_EXISTS = 0;
        
    } else {
       bfename = argv[4];
    }
    elevname = argv[1];
    writeGridname = argv[2];
    rise = atof(argv[3]);
    
    clock_t start = clock(), diff;
    readGridfromFile(elevname, &elevgrid,ELEV_TYPE);
    diff = clock() - start;
    unsigned long msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Reading elevgrid took %lu seconds %lu milliseconds\n", msec/1000, msec%1000);
    
    mallocGrid(elevgrid, &slrgrid);
    setHeaders(elevgrid, &slrgrid);
    
    if (interp_bfe_EXISTS) {
        clock_t start2 = clock(), diff2;
        readGridfromFile(bfename, &bfegrid,BFE_TYPE);
        diff2 = clock() - start2;
        unsigned long msec2 = diff2 * 1000 / CLOCKS_PER_SEC;
        printf("Reading interp_bfegrid took %lu seconds %lu milliseconds\n", msec2/1000, msec2%1000);
        mallocGrid(bfegrid, &interp_bfegrid);
        setHeaders(bfegrid, &interp_bfegrid);
        copyGrid(&bfegrid, &interp_bfegrid);

        clock_t start = clock(), diff;
        printf("start interp_bfe @ %g\n",rise);
        start_interp_bfe(&elevgrid,&interp_bfegrid, rise);
        diff = clock() - start;
        unsigned long msec = diff * 1000 / CLOCKS_PER_SEC;
        printf("interp_bfe took %lu seconds %lu milliseconds\n", msec/1000, msec%1000);

        
        mallocGrid(elevgrid, &slr_interp_bfegrid);
        setHeaders(elevgrid, &slr_interp_bfegrid);
    }
    
    calculateGrids(&elevgrid);
    mallocGrid(elevgrid, &currgrid);
    setHeaders(elevgrid, &currgrid);
    
    
    //GLUT stuff
    //------------------------------------------
    /* initialize GLUT  */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOWSIZE, WINDOWSIZE);
    glutInitWindowPosition(100,100);
    glutCreateWindow(argv[0]);
    
    /* register callback functions */
    glutDisplayFunc(display);
    glutKeyboardFunc(keypress);
    
    
    /* init GL */
    /* set background color black*/
    //glClearColor(0, 0, 0, 0);
    glClearColor(255, 255, 255, 0);
    /* circular points */
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glPointSize(POINT_SIZE);
    
    /* give control to event handler */
    glutMainLoop();
    
    return 0;
}

void calculateGrids(Grid* elevgrid) {
    clock_t start2 = clock(), diff2;
    printf("start SLR @ %g\n",rise);
    slrgrid = start_slr(elevgrid, rise);
    
    diff2 = clock() - start2;
    unsigned long msec2 = diff2 * 1000 / CLOCKS_PER_SEC;
    printf("SLR took %lu seconds %lu milliseconds\n", msec2/1000, msec2%1000);
    
    if (interp_bfe_EXISTS) {
        clock_t start3 = clock(), diff3;
        printf("start SLR + interp_bfe @ %g\n",rise);
        slr_interp_bfegrid = start_slr_interp_bfe(elevgrid, &interp_bfegrid, rise);
        diff3 = clock() - start3;
        unsigned long msec3 = diff3 * 1000 / CLOCKS_PER_SEC;
        printf("interp_bfe+SLR took %lu seconds %lu milliseconds\n", msec3/1000, msec3%1000);
    }
}




/* ***************************** */
void keypress(unsigned char key, int x, int y) {
    switch(key)    {
        case 'q':
            freeGridData(&elevgrid);
            freeGridData(&bfegrid);
            freeGridData(&currgrid);
            freeGridData(&slrgrid);
            freeGridData(&slr_interp_bfegrid);
            freeGridData(&interp_bfegrid);
            exit(0);
            break;
        case 'w':
            printf("Write to file\n");
            gridtoFile(&currgrid, writeGridname);
            break;
        case 'e':
            printf("Draw Elevgrid\n");
            DRAW = ELEV;
            break;
        case 's':
            printf("Draw SLR\n");
            DRAW = SLR;
            break;
        case 'd':
            printf("Draw SLR-Flooded(SLR-Elev)\n");
            DRAW = SLR_ELEV;
            break;
        case 'a':
            printf("Draw SLR Gray\n");
            DRAW = SLR_GRAY;
            break;
        case 'j':
            printf("Draw Water\n");
            DRAW = WATER;
            break;
        case 'k':
            printf("Draw Water+SLR-Flooded(SLR-Elev)\n");
            DRAW = WATER_SLR_ELEV;
            break;
        case 'b':
            printf("Draw Original BFE\n");
            DRAW = ORIG_BFE;
            break;
        case 'i':
            printf("Draw Interpolated BFE\n");
            DRAW = INTERP_BFE;
            break;
        case 'c':
            printf("Draw SLR_BFE\n");
            DRAW = SLRINTERP_BFE;
            break;
    
        case 'v':
            printf("Draw SLR_BFE-Flooded(SLR+BFE-Elev)\n");
            DRAW = SLRINTERP_BFE_ELEV;
            break;
        case 'l':
            printf("Draw Water+SLR_BFE-Flooded(SLR+BFE-Elev)\n");
            DRAW = WATER_SLRINTERP_BFE_ELEV;
            break;
        case 'f':
            printf("Draw Stormflooding (BFE_SLR - SLR)\n");
            DRAW = SLRINTERP_BFEMINUSSLR;
            break;
        case '=':
            rise += 1;
            printf("The new rise is %f\n", rise);
            calculateGrids(&elevgrid);
            break;
        case '+':
            rise += 1;
            printf("\nThe new rise is %f\n", rise);
            calculateGrids(&elevgrid);
            break;
        case '-':
            rise -= 1;
            printf("\nThe new rise is %f\n", rise);
            calculateGrids(&elevgrid);
            break;
        case '5':
            rise += 5;
            printf("The new rise is %f\n", rise);
            calculateGrids(&elevgrid);
            break;
            
    }
    glutPostRedisplay();
}
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); //clear the matrix
    float coloring = 0;
    if (interp_bfe_EXISTS) {
        switch (DRAW) {
            case ELEV:
                setCurrGrid(&elevgrid);
                coloring = COLOR;
                break;
            case SLR:
                setCurrGrid(&slrgrid);
                coloring = COLOR;
                break;
            case SLR_ELEV:
                combineGrids_nobfe(&slrgrid, &elevgrid,rise);
                coloring = COMBINE_COLOR;
                break;
            case SLR_GRAY:
                setCurrGrid(&slrgrid);
                coloring = GRAY_BLUE;
                break;
            case WATER:
                waterGrid(&elevgrid);
                coloring = BINARY_COLOR;
                break;
            case WATER_SLR_ELEV:
                combineGrids_nobfe(&slrgrid, &elevgrid,rise);
                coloring = COMBINE_WATER;
                break;
            case ORIG_BFE:
                setCurrGrid(&bfegrid);
                coloring = BLACK_COLOR;
                break;
            case INTERP_BFE:
                setCurrGrid(&interp_bfegrid);
                coloring = BLACK_COLOR;
                break;
            case SLRINTERP_BFE:
                setCurrGrid(&slr_interp_bfegrid);
                coloring = COLOR;
                break;
                
            case SLRINTERP_BFE_ELEV:
                combineGrids_bfe(&slr_interp_bfegrid, &elevgrid,rise);
                coloring = COMBINE_COLOR_BFE;
                break;
            case WATER_SLRINTERP_BFE_ELEV:
                combineGrids_bfe(&slr_interp_bfegrid, &elevgrid,rise);
                coloring = COMBINE_WATER_BFE;
                break;
                
            case SLRINTERP_BFEMINUSSLR:
                diffGrids(&slr_interp_bfegrid, &slrgrid, NEW_WATER,rise);
                coloring = COMBINE_COLOR_BFE;
                break;
            default:
                break;
        }
        
    } else {
        switch (DRAW) {
            case ELEV:
                setCurrGrid(&elevgrid);
                coloring = COLOR;
                break;
            case SLR:
                setCurrGrid(&slrgrid);
                coloring = COLOR;
                break;
            case SLR_GRAY:
                setCurrGrid(&slrgrid);
                coloring = GRAY_BLUE;
                break;
            case SLR_ELEV:
                combineGrids_nobfe(&slrgrid, &elevgrid,rise);
                coloring = COMBINE_COLOR;
                break;
            case WATER:
                waterGrid(&elevgrid);
                coloring = BINARY_COLOR;
                break;
                
            case WATER_SLR_ELEV:
                combineGrids_nobfe(&slrgrid, &elevgrid,rise);
                coloring = COMBINE_WATER;
                break;
            default:
//                break;
                return;
        }
    }
    draw_grid(&currgrid, coloring,rise);
    
    /* execute the drawing commands */
    glFlush();
}
/* Handler for window re-size event. Called back when the window first appears and
 whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    
    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset
    gluOrtho2D(0.0, (GLdouble) width, 0.0, (GLdouble) height);
}

