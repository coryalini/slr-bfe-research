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
 e: draw elevgrid
 u: draw original bfe
 s: draw slr
 b: draw bfe
 f: draw bfe+slr
 p: draw slr-elev
 o: draw slr+bfe-elev
 d: draw slr+bfe-slr
 i: draw slr+bfe-slr with slr on top
 +: increase the sea level rise
 -: decrease the sea level rise
 */

/* Assumptions:
 no data is water.
 no data + 1 isnt going to be a height
 */


#include "slr.hpp"
#include "bfe.hpp"
#include "slr_bfe.hpp"

#include "parsing.hpp"
#include "drawing.hpp"


#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <time.h>



/* forward declarations of functions */
void display(void);
void keypress(unsigned char key, int x, int y);
void reset();
void calculateGrids();


/* global variables */
const int WINDOWSIZE = 500;
const int POINT_SIZE  = 5.0f;

/* declarations */
const char *elevname, *slrname, *bfename;
int seaX, seaY;


int main(int argc, char * argv[]) {
    
    if (argc < 6 || argc > 8) {
        printf("ERROR: Arguments were not included. %d \n", argc);
        return 1;
    }
    if (argc < 7) {
        BFE_EXISTS = 0;
        
    } else {
        bfename = argv[6];
    }
    elevname = argv[1];
    slrname = argv[2];
    seaX = atoi(argv[3]);
    seaY = atoi(argv[4]);
    rise = atof(argv[5]);
    
    clock_t start = clock(), diff;
    readGridfromFile(elevname, &elevgrid,0);
    diff = clock() - start;
    unsigned long msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Reading elevgrid took %lu seconds %lu milliseconds\n", msec/1000, msec%1000);
    
    mallocGrid(elevgrid, &slrgrid);
    setHeaders(elevgrid, &slrgrid);
    
    if (BFE_EXISTS) {
        clock_t start2 = clock(), diff2;
        readGridfromFile(bfename, &bfegrid,1);
        diff2 = clock() - start2;
        unsigned long msec2 = diff2 * 1000 / CLOCKS_PER_SEC;
        printf("Reading bfegrid took %lu seconds %lu milliseconds\n", msec2/1000, msec2%1000);
        readGridfromFile(bfename, &origbfegrid,1);
        mallocGrid(elevgrid, &bfeslrgrid);
        setHeaders(elevgrid, &bfeslrgrid);
    }
    
    //printHeader(elevgrid);
    printf("The number of cells is %ld\n", elevgrid.nrows * elevgrid.ncols);
    
    calculateGrids();
    
    Grid depthgrid;
    mallocGrid(elevgrid, &depthgrid);
    setHeaders(elevgrid, &depthgrid);
    outputGridWithDepth(&depthgrid, &slrgrid, &elevgrid,rise);
    gridtoFile(&depthgrid, slrname);
    
    if (BFE_EXISTS) {
        outputGridWithDepthWITHBFE(&depthgrid, &slrgrid, &elevgrid,&bfegrid, rise);
        gridtoFile(&depthgrid, slrname);
        
    }

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

void calculateGrids() {
    clock_t start2 = clock(), diff2;
    printf("start SLR @ %g\n",rise);
    start_slr(&elevgrid, &slrgrid, rise,seaX, seaY);
    
    diff2 = clock() - start2;
    unsigned long msec2 = diff2 * 1000 / CLOCKS_PER_SEC;
    printf("SLR took %lu seconds %lu milliseconds\n", msec2/1000, msec2%1000);
    
    if (BFE_EXISTS) {
        clock_t start = clock(), diff;
        printf("start BFE @ %g\n",rise);
        start_bfe(&elevgrid, &bfegrid, rise, seaX, seaY);
        diff = clock() - start;
        unsigned long msec = diff * 1000 / CLOCKS_PER_SEC;
        printf("BFE took %lu seconds %lu milliseconds\n", msec/1000, msec%1000);
        
        clock_t start3 = clock(), diff3;
        printf("start SLR + BFE @ %g\n",rise);
        start_slr_bfe(&elevgrid, &bfeslrgrid, &bfegrid, rise,seaX, seaY);
        diff3 = clock() - start3;
        unsigned long msec3 = diff3 * 1000 / CLOCKS_PER_SEC;
        printf("BFE+SLR took %lu seconds %lu milliseconds\n", msec3/1000, msec3%1000);
    }
}


void display(void) {
    
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); //clear the matrix
    
    if (BFE_EXISTS) {
        switch (DRAW) {
            case 0:
                draw_grid(&slrgrid);
                break;
            case 1:
                draw_bfe(&bfegrid);
                break;
            case 2:
                draw_grid(&bfeslrgrid);
                break;
            case 3:
                draw_grid(&elevgrid);
                break;
            case 4:
                draw_combo(&slrgrid, &elevgrid);
                break;
            case 5:
                draw_combo(&bfeslrgrid, &elevgrid);
                break;
            case 6:
                draw_difference(&bfeslrgrid, &slrgrid);
                break;
            case 7:
                draw_slr_slr_bfe();
                break;
            case 8:
                draw_bfe(&origbfegrid);
                break;
            default:
                break;
        }
    } else {
        if (DRAW == 3) {
            draw_grid(&elevgrid);
        } else if (DRAW == 4) {
            draw_combo(&slrgrid, &elevgrid);
        } else {
            draw_grid(&slrgrid);
        }
    }
    
    /* execute the drawing commands */
    glFlush();
}



/* ****************************** */
void keypress(unsigned char key, int x, int y) {
    switch(key)    {
        case 'q':
            freeGridData(&elevgrid);
            freeGridData(&slrgrid);
            freeGridData(&bfegrid);
            freeGridData(&bfeslrgrid);
            exit(0);
            break;
        case 'w':
            gridtoFile(&bfegrid, bfename);
            gridtoFile(&bfeslrgrid, slrname);
            break;
            
        case 's':
            printf("Draw SLR\n");
            DRAW = 0;
            break;
        case 'b':
            printf("Draw BFE\n");
            DRAW = 1;
            break;
        case 'f':
            printf("Draw BFE + SLR\n");
            DRAW = 2;
            break;
        case 'e':
            printf("Draw Elevgrid\n");
            DRAW = 3;
            break;
        case 'p':
            printf("Draw SLR-Elev\n");
            DRAW = 4;
            break;
        case 'o':
            printf("Draw SLR+BFE-Elev\n");
            DRAW = 5;
            break;
        case 'd':
            printf("Draw SLR+BFE-SLR\n");
            DRAW = 6;
            break;
        case 'i':
            printf("Draw SLR+BFE-SLR with the slr\n");
            DRAW = 7;
            break;
        case 'u':
            printf("Draw Original BFE\n");
            DRAW = 8;
            break;
            
        case '=':
            rise += 1;
            printf("The new rise is %f\n", rise);
            calculateGrids();
            break;
        case '+':
            rise += 1;
            printf("\nThe new rise is %f\n", rise);
            calculateGrids();
            break;
        case '-':
            rise -= 1;
            printf("\nThe new rise is %f\n", rise);
            calculateGrids();
            break;
        case '1':
            rise = 100;
            printf("The new rise is %f\n", rise);
            calculateGrids();
            break;
            
    }
    glutPostRedisplay();
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

