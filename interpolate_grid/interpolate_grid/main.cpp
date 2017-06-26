//
//  main.cpp
//  interpolate_grid
//
//  Created by Cory Alini on 6/26/17.
//  Copyright © 2017 slr_bfe.coryalini. All rights reserved.
//

#include <iostream>
#include "grid.hpp"
#include "drawing.hpp"
#include "bfe.hpp"


/* forward declarations of functions */
void keypress(unsigned char key, int x, int y);
void reset();
void display(void);



/* global variables */
const int WINDOWSIZE = 500;
const int POINT_SIZE  = 5.0f;
int interp_bfe_EXISTS = 1, DRAW = 0;
const char *elevname, *writeGridname, *bfename;
float rise;



int main(int argc, char * argv[]) {
    
    if (argc != 4) {
        printf("ERROR: Arguments were not included. %d \n", argc);
        return 1;
    }
    
    elevname = argv[1];
    bfename = argv[2];
    writeGridname = argv[3];
    
    clock_t start = clock(), diff;
    readGridfromFile(elevname, &elevgrid,ELEV_TYPE);
    diff = clock() - start;
    unsigned long msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Reading elevgrid took %lu seconds %lu milliseconds\n", msec/1000, msec%1000);

    
    clock_t start2 = clock(), diff2;
    readGridfromFile(bfename, &bfegrid,BFE_TYPE);
    diff2 = clock() - start2;
    unsigned long msec2 = diff2 * 1000 / CLOCKS_PER_SEC;
    printf("Reading interp_bfegrid took %lu seconds %lu milliseconds\n", msec2/1000, msec2%1000);
    mallocGrid(bfegrid, &interp_bfegrid);
    setHeaders(bfegrid, &interp_bfegrid);
    copyGrid(&bfegrid, &interp_bfegrid);
    
    clock_t start3 = clock(), diff3;
    printf("start interp_bfe @ %g\n",rise);
    start_interp_bfe(&elevgrid,&interp_bfegrid, rise);
    diff3 = clock() - start3;
    unsigned long msec3 = diff3 * 1000 / CLOCKS_PER_SEC;
    printf("interp_bfe took %lu seconds %lu milliseconds\n", msec3/1000, msec3%1000);
    
    
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

/* ***************************** */
void keypress(unsigned char key, int x, int y) {
    switch(key)    {
        case 'q':
            freeGridData(&elevgrid);
            freeGridData(&bfegrid);
            freeGridData(&currgrid);
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
        case 'j':
            printf("Draw Water\n");
            DRAW = WATER;
            break;
        case 'b':
            printf("Draw Original BFE\n");
            DRAW = ORIG_BFE;
            break;
        case 'i':
            printf("Draw Interpolated BFE\n");
            DRAW = INTERP_BFE;
            break;
            
    }
    glutPostRedisplay();
}
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); //clear the matrix
    float coloring = 0;
    switch (DRAW) {
        case ELEV:
            setCurrGrid(&elevgrid);
            coloring = COLOR;
            break;
        case WATER:
            waterGrid(&elevgrid);
            coloring = BINARY_COLOR;
            break;
        case ORIG_BFE:
            setCurrGrid(&bfegrid);
            coloring = BLACK_COLOR;
            break;
        case INTERP_BFE:
            setCurrGrid(&interp_bfegrid);
            coloring = BLACK_COLOR;
            break;
            break;
        default:
            break;
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

