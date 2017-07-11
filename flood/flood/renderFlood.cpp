//
//  renderFlood.cpp
//  flood
//
//  Created by Cory Alini on 6/28/17.
//  Copyright © 2017 slr_bfe.coryalini. All rights reserved.
//
//
//  main.cpp
//  interpolate_grid
//
//  Created by Cory Alini on 6/26/17.
//  Copyright © 2017 slr_bfe.coryalini. All rights reserved.
//


/* COMMANDS
 q: quit
 w: write to files
 
 e: Draw Elevgrid
 s: Draw the sea
 i: Draw interpolated grid with original on top
 
 */

#include "grid.hpp"

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

#define PRINT_HELP(arg) printf("    " arg " \n");

/* global variables */
const int WINDOWSIZE = 500;
const int POINT_SIZE  = 5.0f;

double ELEV_CONVERTER = 3.28084;
double BFE_CONVERTER = 1;
int interp_bfe_EXISTS = 1, DRAW = 0;
const char *elevname,*bfename,*floodedname, *writeGridname;
Grid elevgrid, bfegrid, floodedgrid, currgrid;

//HACKKK
double rise = 0;

double bfemax,bfemin, elevmax,elevmin,floodmax,floodmin;


enum {ELEV_BFE_FLOOD = 0};
enum {THREE_COLORS =0};

static float numCategories = 6.0;

GLfloat red_pink[3] = {0.969, 0.396, 0.396};
GLfloat black[3] = {0.0, 0.0, 0.0};
GLfloat purple[3] = {0.733,0.557,1};



GLfloat blue[3] = {0.157, 0.325, 0.419};
GLfloat lightblue[3] = {0.604, 0.820, 0.831};
GLfloat green1[3] = {0.945, 1.0, 0.905};
GLfloat greenmid[3] = {0.749, 0.918, 0.737};
GLfloat green2[3] = {0.561, 0.839, 0.580};
GLfloat darkyellow[3] = {0.424, 0.682, 0.459};
GLfloat darkorange[3] = {0.545,0.580,0.455};

GLfloat red1[3] = {0.298,0.298,0.278};
GLfloat red2[3] = {0.188,0.212,0.200};

GLfloat gray1[3] = {0.949, 0.949, 0.949};
GLfloat gray2[3] = {0.729, 0.729, 0.729};
GLfloat gray3[3] = {0.518, 0.518, 0.518};
GLfloat gray4[3] = {0.349, 0.349, 0.349};
GLfloat gray5[3] = {0.196, 0.196, 0.196};
GLfloat gray6[3] = {0.118, 0.118, 0.118};

//GLfloat blue1[3] = {0.568,0.776,0.792};
//GLfloat blue2[3] = {0.572,0.784,0.8};
//GLfloat blue3[3] = {0.529,0.737,0.756};
//GLfloat blue4[3] = {0.455,0.655,0.690};
//GLfloat blue5[3] = {0.380,0.568,0.619};
//GLfloat blue6[3] = {0.305,0.486,0.553};
GLfloat blue1[3] = {0, 0,1.0};
GLfloat blue2[3] = {0, 0,0.9};
GLfloat blue3[3] = {0, 0,0.8};
GLfloat blue4[3] = {0, 0,0.7};
GLfloat blue5[3] = {0, 0,0.6};
GLfloat blue6[3] = {0, 0, 0.5};

/* forward declarations of functions */
void keypress(unsigned char key, int x, int y);
void reset();
void display(void);

void draw_grid(Grid* grid,int grid_type,float rise);
void general_draw_point(point mypoint, Grid* grid,int grid_type, float rise, double minLand, double max);

void getOptExecution(int argc, char* const* argv);
void testMandatoryFlags(int flag, char opt, char* argv);
void tooManyFlagError(char flag, char opt);
void helpFlag();
void printRenderCommands();


void setCurrGrid(Grid* grid);
void combineGrids();
void draw_point_color(double value, double minLand, double max);
void draw_point_binary(double value);
void draw_point_black(double value,double minLand, double max);
void draw_point_combineBFE(double value,double minLand, double max);
GLfloat* interpolate_colors(GLfloat* lowerColor, GLfloat* upperColor,double value,double lowerBound,double upperBound);

void change_color_land(double value, double base, double thisMin);
void change_color_gray(double value, double base, double thisMin);
void change_color_blue(double value, double base, double thisMin);

int main(int argc, char * argv[]) {
    
    if (argc != 1) {
        printRenderCommands();
        getOptExecution(argc, argv);
    } else {
        helpFlag();
        printRenderCommands();
        exit(0);
    }
    
    
    clock_t start = clock(), diff;
    readGridfromFile(elevname, &elevgrid,ELEV_CONVERTER);
    diff = clock() - start;
    unsigned long msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Reading %s took %lu seconds %lu milliseconds\n",elevname, msec/1000, msec%1000);
    
    
    clock_t start2 = clock(), diff2;
    readGridfromFile(bfename, &bfegrid,BFE_CONVERTER);
    diff2 = clock() - start2;
    unsigned long msec2 = diff2 * 1000 / CLOCKS_PER_SEC;
    printf("Reading %s took %lu seconds %lu milliseconds\n",bfename, msec2/1000, msec2%1000);
    if (bfegrid.ncols != elevgrid.ncols || bfegrid.nrows !=elevgrid.nrows) {
        printf("ERROR:The %s [%ld,%ld] and elevgrid [%ld,%ld] do not have the same grid dimensions!\n",bfename, bfegrid.nrows,bfegrid.ncols,elevgrid.nrows, elevgrid.ncols);
        exit(0);
    }
    clock_t start3 = clock(), diff3;
    readGridfromFile(floodedname, &floodedgrid,BFE_CONVERTER);
    diff3 = clock() - start3;
    unsigned long msec3= diff3 * 1000 / CLOCKS_PER_SEC;
    printf("Reading %s took %lu seconds %lu milliseconds\n",floodedname, msec3/1000, msec3%1000);
    if (floodedgrid.ncols != elevgrid.ncols || floodedgrid.nrows !=elevgrid.nrows) {
        printf("ERROR:The %s [%ld,%ld] and elevgrid [%ld,%ld] do not have the same grid dimensions!\n",bfename, floodedgrid.nrows,floodedgrid.ncols,elevgrid.nrows, elevgrid.ncols);
        exit(0);
    }
    
    mallocGrid(elevgrid, &currgrid);
    setHeaders(elevgrid, &currgrid);

    bfemin = findMinLand(&bfegrid);
    bfemax = findMax(&bfegrid);
    floodmin = findMinLand(&floodedgrid);
    floodmax = findMax(&floodedgrid);
    elevmin = findMinLand(&elevgrid);
    elevmax = findMax(&elevgrid);
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
/*
 * This function uses getOpt to read the terminal prompt and do the
 * desired action depending on what it was given.
 * It uses a switch statement to figure out which flag
 * was used.
 */
void getOptExecution(int argc, char* const* argv) {
    
    int opt;
    int eflag = 0, oflag = 0, iflag = 0, fflag = 0;
    
    extern char* optarg;
    extern int optopt;

    while ((opt = getopt(argc, argv, "e:f:i:o:" )) != -1) {
        switch (opt) {
            case 'e':
                elevname = optarg;
                eflag += 1;
                break;
            case 'f':
                floodedname = optarg;
                fflag+=1;
                break;
            case 'i':
                bfename = optarg;
                iflag+=1;
                break;
            
            case 'o':
                writeGridname = optarg;
                oflag+=1;
                break;
            default:
                printf("Illegal option given\n");
                exit(2);
                
        }
    }
    testMandatoryFlags(eflag, 'e', argv[0]);
    testMandatoryFlags(iflag, 'i', argv[0]);
    testMandatoryFlags(fflag, 'f', argv[0]);
    
    tooManyFlagError(eflag, 'e');
    tooManyFlagError(oflag, 'o');
    tooManyFlagError(iflag, 'i');
    tooManyFlagError(fflag, 'f');
}
void testMandatoryFlags(int flag, char opt, char* argv) {
    if (flag != 1) {	//flag was mandatory
        fprintf(stderr, "%s: missing -%c option\n", argv, opt);
        fprintf(stderr, "usage: %s -e elevname [-i interpname] -r rise -o file_to_write \n", argv);
        exit(1);
    }
}

void tooManyFlagError(char flag, char opt) {
    if (flag > 1) { //flag declared too often
        printf("Error:  -%c is set multiple times\n", opt);
        exit(1);
    }
}

void helpFlag() {
    printf("The flooding simulator takes the following command-line arguments: \n");
    PRINT_HELP("-e <e>: Elevation grid flag")
    PRINT_HELP("-i <i>: Tidal grid")
    PRINT_HELP("-f <f>: The flooded grid")
    PRINT_HELP("-o <o>: Filename you wish to write your grid to")
}
void printRenderCommands() {
    
    printf("The rendering map takes the following commands: \n");
    PRINT_HELP("q: quit")
    PRINT_HELP("w: write to files")
    
    PRINT_HELP("e: Draw Elevgrid with the bfe grid with flooded")
    
}


/* ***************************** */
void keypress(unsigned char key, int x, int y) {
    switch(key)    {
        case 'q':
            freeGridData(&elevgrid);
            freeGridData(&bfegrid);
            freeGridData(&floodedgrid);
            freeGridData(&currgrid);
            exit(0);
            break;
        case 'w':
            printf("Write to file\n");
            gridtoFile(&currgrid, writeGridname);
            break;
        case 'e':
            printf("Draw Elev+Bfe+flooded\n");
            DRAW = ELEV_BFE_FLOOD;
            break;
        default:
            break;
            
    }
    glutPostRedisplay();
}
//This function is used to determine which grid to display

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); //clear the matrix
    float coloring = 0;
    switch (DRAW) {
        case ELEV_BFE_FLOOD:
            combineGrids();
            coloring = THREE_COLORS;
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

/*
 This function is used to draw a general grid. No particular grid needed
 */
void draw_grid(Grid* grid, int grid_type,float rise) {
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(grid->data);
    double max;
    double minLand= findMinLand(grid);
    max = findMax(grid);
    
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            point newPoint;
            newPoint.x = i;
            newPoint.y = j;
            general_draw_point(newPoint, grid, grid_type,rise,minLand, max);
        }
    }
}

//Draws points depending on the color
void general_draw_point(point mypoint, Grid* grid,int grid_type, float rise, double minLand, double max) {
    double value = grid->data[(int)mypoint.x][(int)mypoint.y];
    draw_point_combineBFE(value,minLand, max);
    
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
/*
 
 This function sets the global variable, currGrid.
 If there is a bfe available, the code utilizes the bfe data given to find what values will be flooded.
 Even if the bfe is given, some values in the bfe grid are NODATA_values, thus
 
 */
void setCurrGrid(Grid* grid){
    for (int i = 0; i < grid->nrows; i++) {
        for (int j = 0; j < grid->ncols; j++) {
            currgrid.data[i][j] = grid->data[i][j];
        }
    }
}
/*
 When called,ex: grid1 is slr or slr+bfe and grid2 is elev.
 
 This function sets the global variable, currGrid.
 If there is a bfe available, the code utilizes the given rise to find what values will be flooded.
 
 */


void combineGrids() {
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            if (bfegrid.data[i][j] != elevgrid.NODATA_value) {
                currgrid.data[i][j] = bfegrid.data[i][j]+ 1000;//just a very unlikely value
            } else if (floodedgrid.data[i][j] == NEW_WATER) {
                currgrid.data[i][j] = floodedgrid.data[i][j];//just a very unlikely value
            } else {
                currgrid.data[i][j] = elevgrid.data[i][j];
            }
        }
    }
}


void draw_point_color(double value, double minLand, double max) {
    double base;
    double thisMin = minLand;
    if (minLand < 0) {
        thisMin = 0;
    }
    base = (max-thisMin)/numCategories;
    change_color_land(value, base, thisMin);
}

void draw_point_binary(double value) {
    if (value == 0) {
        glColor3fv(blue);
    } else {
        glColor3fv(black);
    }
}
void draw_point_black(double value,double minLand, double max) {
    assert(minLand > 0);
    double base = (max-minLand)/numCategories;
    change_color_gray(value, base, minLand);
    
}

void draw_point_combineBFE(double value,double minLand,double max) {
    double thisMin = minLand;
    if (minLand < 0) {
        thisMin = 0;
    }
    double base;
    if (value > 1000) {
        value -=1000;
        base = (bfemax-bfemin)/numCategories;
        change_color_blue(value, base, bfemin);
    } else if (value == NEW_WATER) {
        glColor3fv(purple);
    } else {
        base = (elevmax-elevmin)/numCategories;
        change_color_land(value, base, elevmin);
    }
}



void change_color_land(double value, double base, double thisMin) {
    if (value == elevgrid.NODATA_value) {
        glColor3fv(blue);
    } else if (value == NEW_WATER) {
        glColor3fv(lightblue);
    } else if (value < (thisMin+base)) {
        glColor3fv(interpolate_colors(green1, greenmid,value,thisMin,(thisMin + base)));
    } else if (value < (thisMin + 2 * base)) {
        glColor3fv(interpolate_colors(greenmid, green2,value,(thisMin + base),(thisMin + 2*base)));
    } else if (value < (thisMin + 3 * base)) {
        glColor3fv(interpolate_colors(green2, darkyellow,value,(thisMin + 2*base),(thisMin + 3*base)));
    } else if (value < (thisMin + 4 * base)) {
        glColor3fv(interpolate_colors(darkyellow, darkorange,value,(thisMin + 3*base),(thisMin + 4*base)));
    } else if (value < (thisMin + 5 * base)) {
        glColor3fv(interpolate_colors(darkorange, red1,value,(thisMin + 4*base),(thisMin + 5*base)));
    }  else {
        glColor3fv(interpolate_colors(red1, red2,value,(thisMin + 5*base),(thisMin+6*base)));
    }
}

void change_color_gray(double value, double base, double thisMin) {
    if (value == elevgrid.NODATA_value) {
        glColor3fv(blue);
    } else if (value < thisMin+base) {
        glColor3fv(interpolate_colors(gray1, gray2,value,thisMin,thisMin+base));
    } else if (value < (thisMin+2 * base)) {
        glColor3fv(interpolate_colors(gray2, gray3,value,thisMin+base,thisMin+2*base));
    } else if (value < (thisMin+3 * base)) {
        glColor3fv(interpolate_colors(gray3, gray4,value,thisMin+2*base,thisMin+3*base));
    } else if (value < (thisMin+4 * base)) {
        glColor3fv(interpolate_colors(gray4, gray5,value,thisMin+3*base,thisMin+4*base));
    } else if (value < (thisMin+5 * base)) {
        glColor3fv(interpolate_colors(gray5, gray6,value,thisMin+4*base,thisMin+5*base));
    }  else {
        glColor3fv(interpolate_colors(gray6, black,value,thisMin+5*base,thisMin+6*base));
    }
}
void change_color_blue(double value, double base, double thisMin) {
    if (value < thisMin+base) {
        glColor3fv(interpolate_colors(blue6, blue5,value,thisMin,thisMin+base));
    } else if (value < (thisMin+2 * base)) {
        glColor3fv(interpolate_colors(blue5, blue4,value,thisMin+base,thisMin+2*base));
    } else if (value < (thisMin+3 * base)) {
        glColor3fv(interpolate_colors(blue4, blue3,value,thisMin+2*base,thisMin+3*base));
    } else if (value < (thisMin+4 * base)) {
        glColor3fv(interpolate_colors(blue3, blue2,value,thisMin+3*base,thisMin+4*base));
    } else if (value < (thisMin+5 * base)) {
        glColor3fv(interpolate_colors(blue2, blue1,value,thisMin+4*base,thisMin+5*base));
    }  else {
        glColor3fv(interpolate_colors(blue1, lightblue,value,thisMin+5*base,thisMin+6*base));
    }
}


GLfloat* interpolate_colors(GLfloat* lowerColor, GLfloat* upperColor,double value,double lowerBound,double upperBound) {
    static GLfloat newColor[3] = {-1,-1,-1};
    for(int i = 0; i < 3; i ++) {
        newColor[i] = (double)lowerColor[i] + ((value-lowerBound)/(upperBound-lowerBound))*(double)(upperColor[i]-lowerColor[i]);
    }
    return newColor;
    
}


