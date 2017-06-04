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

#include "slr.h"
#include "parsing.h"

 
#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#ifdef __APPLE__
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif

const int NEW_WATER = -8000;

int DRAW = 0;
int BFE_EXISTS = 1;
GLfloat red_pink[3] = {0.969, 0.396, 0.396};
GLfloat black[3] = {0.0, 0.0, 0.0};

//GLfloat blue[3] = {0.204, 0.533, 1};
//GLfloat lightblue[3] = {0.25, 0.8, 1.0};
//GLfloat green1[3] = {0.475, 0.925, 0.06};
//GLfloat green2[3] = {0.08, 0.611, 0.219};
//GLfloat darkyellow[3] = {0.611, 0.611, 0.08};
//GLfloat darkorange[3] = {0.611,0.349,0.08};
//GLfloat red1[3] = {0.611,0.08,0.08};
//GLfloat red2[3] = {0.262,0.10,0.039};


GLfloat blue[3] = {0.157, 0.325, 0.419};
GLfloat lightblue[3] = {0.604, 0.820, 0.831};
GLfloat green1[3] = {0.945, 1.0, 0.905};
GLfloat green2[3] = {0.561, 0.839, 0.580};

GLfloat darkyellow[3] = {0.424, 0.682, 0.459};
GLfloat darkorange[3] = {0.545,0.580,0.455};
//GLfloat red1[3] = {0.271,0.314,0.231};

GLfloat red1[3] = {0.298,0.298,0.278};
GLfloat red2[3] = {0.188,0.212,0.200};





GLfloat gray1[3] = {0.949, 0.949, 0.949};
GLfloat gray2[3] = {0.729, 0.729, 0.729};
GLfloat gray3[3] = {0.518, 0.518, 0.518};
GLfloat gray4[3] = {0.349, 0.349, 0.349};
GLfloat gray5[3] = {0.196, 0.196, 0.196};
GLfloat gray6[3] = {0.118, 0.118, 0.118};
/* forward declarations of functions */
void display(void);
void keypress(unsigned char key, int x, int y);
void reset();
void calculateGrids();
//std::queue<point> coast_queue;


/* global variables */
const int WINDOWSIZE = 500;
const int POINT_SIZE  = 5.0f;

/* declarations */
const char *elevname, *slrname, *bfename;
Grid elevgrid, bfegrid, slrgrid,bfeslrgrid,origbfegrid;
float rise;
int seaX, seaY;
float minX,minY,maxX,maxY,max,min, minXLand,minYLand,minLand;


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
    float array[9];
    *array = *findMaxMin(&elevgrid, array);
    
    minX = array[0];
    minY = array[1];
    maxX = array[2];
    maxY = array[3];
    min = array[4];
    max = array[5];
    minXLand = array[6];
    minYLand = array[7];
    minLand = array[8];
    
    Grid depthgrid;
    mallocGrid(elevgrid, &depthgrid);
    setHeaders(elevgrid, &depthgrid);
    outputGridWithDepth(&depthgrid, &slrgrid, &elevgrid,rise);
    gridtoFile(&depthgrid, slrname);
    
    if (BFE_EXISTS) {
        outputGridWithDepthWITHBFE(&depthgrid, &slrgrid, &elevgrid,&bfegrid, rise);
        gridtoFile(&depthgrid, slrname);
        
    }
    
    
    
    
    
    //findMax(slrgrid);
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


void draw_point_slr(point mypoint, Grid* grid, float* array) {
    float max1 = array[5];
    
    float numCategories = 6.0;
    double value = grid->data[(int)mypoint.x][(int)mypoint.y];
    double base = max1/numCategories;
    
    if (value == elevgrid.NODATA_value) {
        glColor3fv(blue);
    } else if (value == NEW_WATER) {
        glColor3fv(lightblue);
    } else if (value < base) {
        glColor3fv(green1);
    } else if (value < (2 * base)) {
        glColor3fv(green2);
    } else if (value < (3 * base)) {
        glColor3fv(darkyellow);
    } else if (value < (4 * base)) {
        glColor3fv(darkorange);
    } else if (value < (5 * base)) {
        glColor3fv(red1);
    }  else {
        glColor3fv(red2);
    }
    
    
    float x=0, y=0;  //just to initialize with something
    
    float larger,smaller;
    if (grid->nrows > grid->ncols) {
        larger = grid->nrows;
        smaller = grid->ncols;
        
    } else {
        larger = grid->ncols;
        smaller = grid->ncols;
    }
    
    x = (((mypoint.y)/(larger))*2) - smaller/larger;
    y = -(((mypoint.x)/(larger))*2) + 1;
    
    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();
}



void draw_point_bfe(point mypoint, Grid* grid) {
    
    float numCategories = 6.0;
    double value = grid->data[(int)mypoint.x][(int)mypoint.y];
    double base = 30/numCategories;
    if (value == elevgrid.NODATA_value) {
        glColor3fv(lightblue);
    } else if (value < base) {
        glColor3fv(gray1);
    } else if (value < (2 * base)) {
        glColor3fv(gray2);
    } else if (value < (3 * base)) {
        glColor3fv(gray3);
    } else if (value < (4 * base)) {
        glColor3fv(gray4);
    } else if (value < (5 * base)) {
        glColor3fv(gray5);
    }  else {
        glColor3fv(gray6);
    }
    
    
    float x=0, y=0;  //just to initialize with something
    
    float larger,smaller;
    if (grid->nrows > grid->ncols) {
        larger = grid->nrows;
        smaller = grid->ncols;
        
    } else {
        larger = grid->ncols;
        smaller = grid->nrows;
    }
    x = (((mypoint.y)/(larger))*2) - smaller/larger;
    y = -(((mypoint.x)/(larger))*2) + 1;
    
    
    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();
}

void draw_point_combo(point mypoint, Grid* main, Grid* subtract, float* array, float currRise) {
    double value1 = main->data[(int)mypoint.x][(int)mypoint.y];
    double value2 = subtract->data[(int)mypoint.x][(int)mypoint.y] - currRise;
    double value;
    if (value1 != elevgrid.NODATA_value && value1 != NEW_WATER) {
        value = value1 - value2;
    } else {
        value = value1;
    }
    
    if (value == NEW_WATER) {
        glColor3fv(lightblue);
    } else {
        glColor3fv(black);
    }
    
    float x=0, y=0;  //just to initialize with something
    float larger,smaller;
    if (main->nrows > main->ncols) {
        larger = main->nrows;
        smaller = main->ncols;
        
    } else {
        larger = main->ncols;
        smaller = main->ncols;
    }
    
    x = (((mypoint.y)/(larger))*2) - smaller/larger;
    y = -(((mypoint.x)/(larger))*2) + 1;
    
    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();
}
void draw_point_diff(point mypoint, Grid* main, Grid* subtract, float* array, float currRise) {
    double value, value1,value2;
    value1 = main->data[(int)mypoint.x][(int)mypoint.y];//slrBFE
    value2 = subtract->data[(int)mypoint.x][(int)mypoint.y]; //SLR
    
    if (value1 == NEW_WATER && value2 != NEW_WATER) {
        if (mypoint.x == 500 && mypoint.y == 500) {
            printf("Vals are %f and %f\n", value1, value2);
        }
        if (mypoint.x == 1000 && mypoint.y == 1000) {
            printf("Vals1000 are %f and %f\n", value1, value2);
        }
        value = 0;
    } else {
        value = 1;
    }

    if (value == 0) {
        glColor3fv(lightblue);
    } else {
        glColor3fv(black);
    }
    
    float x=0, y=0;  //just to initialize with something
    float larger,smaller;
    if (main->nrows > main->ncols) {
        larger = main->nrows;
        smaller = main->ncols;
        
    } else {
        larger = main->ncols;
        smaller = main->ncols;
    }
    
    x = (((mypoint.y)/(larger))*2) - smaller/larger;
    y = -(((mypoint.x)/(larger))*2) + 1;
    
    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();
}


void draw_point_slr_slr_bfe(point mypoint, Grid* main, Grid* subtract, float* array, float currRise) {
    float value, value1,value2;
    value1 = main->data[(int)mypoint.x][(int)mypoint.y];//slrBFE
    value2 = subtract->data[(int)mypoint.x][(int)mypoint.y]; //SLR
    
    if (value1 == NEW_WATER && value2 != NEW_WATER) {
        value = 0;
    } else {
        value = 1;
    }
    float bfeval = bfegrid.data[(int)mypoint.x][(int)mypoint.y];
    if (value == 0) {
        float numCategories = 6.0;
        double base = 30/numCategories;
        if (value1 == elevgrid.NODATA_value) {
            glColor3fv(blue);
        } else if (bfeval < base) {
            glColor3fv(gray1);
        } else if (bfeval < (2 * base)) {
            glColor3fv(gray2);
        } else if (bfeval < (3 * base)) {
            glColor3fv(gray3);
        } else if (bfeval < (4 * base)) {
            glColor3fv(gray4);
        } else if (bfeval < (5 * base)) {
            glColor3fv(gray5);
        }  else {
            glColor3fv(gray6);
        }
    } else {
        float max1 = array[5];
        float numCategories = 6.0;
        double base = max1/numCategories;
        
        if (value2 == elevgrid.NODATA_value) {
            glColor3fv(blue);
        } else if (value2 == NEW_WATER) {
            glColor3fv(lightblue);
        } else if (value2 < base) {
            glColor3fv(green1);
        } else if (value2 < (2 * base)) {
            glColor3fv(green2);
        } else if (value2 < (3 * base)) {
            glColor3fv(darkyellow);
        } else if (value2 < (4 * base)) {
            glColor3fv(darkorange);
        } else if (value2 < (5 * base)) {
            glColor3fv(red1);
        }  else {
            glColor3fv(red2);
        }
    }
    
    float x=0, y=0;  //just to initialize with something
    float larger,smaller;
    if (main->nrows > main->ncols) {
        larger = main->nrows;
        smaller = main->ncols;
        
    } else {
        larger = main->ncols;
        smaller = main->ncols;
    }
    
    x = (((mypoint.y)/(larger))*2) - smaller/larger;
    y = -(((mypoint.x)/(larger))*2) + 1;
    
    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();
}

void draw_grid(Grid* grid) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(grid->data);
    float array[9];
    *array = *findMaxMin(grid, array);
    
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            point newPoint;
            newPoint.x = i;
            newPoint.y = j;
            draw_point_slr(newPoint, grid,array);
        }
    }
}
void draw_bfe(Grid* grid) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(grid->data);
    float array[9];
    *array = *findMaxMin(grid, array);
    
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            point newPoint;
            newPoint.x = i;
            newPoint.y = j;
            draw_point_bfe(newPoint, grid);
        }
    }
}

void draw_combo(Grid* main, Grid* subtract) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(slrgrid.data);
    float array[9];
    *array = *findMaxMin(&slrgrid, array);
    
    float currRise = 0;
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            point newPoint;
            newPoint.x = i;
            newPoint.y = j;
            if (DRAW == 5) {
                currRise = bfegrid.data[i][j];
            }
            draw_point_combo(newPoint, main,subtract,array, currRise);
        }
    }
}
void draw_difference(Grid* main, Grid* subtract) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(main->data);
    assert(subtract->data);
    float array[9];
    *array = *findMaxMin(&slrgrid, array);
    
    float currRiseB = 0;
    
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            point newPoint;
            newPoint.x = i;
            newPoint.y = j;
            currRiseB = bfegrid.data[i][j] - rise;
            
            draw_point_diff(newPoint, main,subtract,array, currRiseB);
        }
    }
}


void draw_slr_slr_bfe() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(bfeslrgrid.data);
    assert(slrgrid.data);
    float array[9];
    *array = *findMaxMin(&slrgrid, array);
    
    float currRiseB = 0;
    
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            point newPoint;
            newPoint.x = i;
            newPoint.y = j;
            currRiseB = bfegrid.data[i][j] - rise;
            draw_point_slr_slr_bfe(newPoint, &bfeslrgrid,&slrgrid,array, currRiseB);
        }
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

