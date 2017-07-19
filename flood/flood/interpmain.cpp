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

#include <iostream>
#include "grid.hpp"
#include "interpolate.hpp"

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
//size of OpenGL window
const int WINDOWSIZE = 500;

//change size of point
const int POINT_SIZE  = 5.0f;

int WRITE_TO_FILE = 0;//determines whether or not we are to write to file



int interp_bfe_EXISTS = 1, DRAW = 0;
const char *elevname, *writeGridname, *origname;
Grid elevgrid, origgrid, interpgrid;

enum {ELEV = 0, SEA = 1,ELEV_ORIG =2, COMBINE_INTERP=3};
static float numCategories = 6.0;//used for buckets

//COLORS
GLfloat red_pink[3] = {0.969, 0.396, 0.396};
GLfloat black[3] = {0.0, 0.0, 0.0};
GLfloat blue[3] = {0.157, 0.325, 0.419};
//GLfloat blue[3] = {0, 0, 1.0};
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

void getOptExecution(int argc, char* const* argv);
void testMandatoryFlags(int flag, char opt, char* argv);
void tooManyFlagError(char flag, char opt);
void helpFlag();
void printRenderCommands();

void draw_elevgrid();
void draw_sea();
void draw_elev_orig();
void draw_interp_and_original();
void draw_point(int i, int j, Grid* grid);

void draw_color(double value, double minLand, double max);
void draw_binary(double value);
void draw_black(double value,double minLand, double max);
void draw_blue(double value,double minLand, double max);
void draw_combinedOrig_Interp(double value,int orig_or_interp,  double minLand,double max);

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
    printf("Reading elevgrid took %lu seconds %lu milliseconds\n", msec/1000, msec%1000);

    
    clock_t start2 = clock(), diff2;
    readGridfromFile(origname, &origgrid,BFE_CONVERTER);
    diff2 = clock() - start2;
    unsigned long msec2 = diff2 * 1000 / CLOCKS_PER_SEC;
    printf("Reading original grid took %lu seconds %lu milliseconds\n", msec2/1000, msec2%1000);
    if (origgrid.ncols != elevgrid.ncols || origgrid.nrows !=elevgrid.nrows) {
        printf("ERROR:The %s [%ld,%ld] and elevgrid [%ld,%ld] do not have the same grid dimensions!\n",origname, origgrid.nrows,origgrid.ncols,elevgrid.nrows, elevgrid.ncols);
        exit(0);
    }

    mallocGrid(origgrid, &interpgrid);
    setHeaders(origgrid, &interpgrid);
    copyGrid(&origgrid, &interpgrid);
    
    clock_t start3 = clock(), diff3;
    printf("start interpolation\n");
//    start_interp_bfe_withFlooded(&elevgrid,&interpgrid);
    start_interpolation(&origgrid, &interpgrid);

    diff3 = clock() - start3;
    unsigned long msec3 = diff3 * 1000 / CLOCKS_PER_SEC;
    printf("interpolation took %lu seconds %lu milliseconds\n", msec3/1000, msec3%1000);
    if (WRITE_TO_FILE) {
        printf("Writing to file %s\n", writeGridname);
        gridtoFile(&interpgrid, writeGridname);
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

/*
 * This function uses getOpt to read the terminal prompt and do the
 * desired action depending on what it was given.
 * It uses a switch statement to figure out which flag
 * was used.
 */
void getOptExecution(int argc, char* const* argv) {
    
    int opt;
    int eflag = 0, oflag = 0, iflag = 0;
    
    extern char* optarg;
    extern int optopt;
    while ((opt = getopt(argc, argv, "e:i:o:" )) != -1) {
        switch (opt) {
            case 'e':
                elevname = optarg;
                eflag += 1;
                break;
            case 'i':
                iflag+=1;
                origname = optarg;
                break;
            case 'o':
                writeGridname = optarg;
                WRITE_TO_FILE = 1;
                oflag+=1;
                break;
            default:
                printf("Illegal option given\n");
                exit(2);
        
        }
    }
    testMandatoryFlags(eflag, 'e', argv[0]);
    testMandatoryFlags(iflag, 'i', argv[0]);
    
    tooManyFlagError(eflag, 'e');
    tooManyFlagError(iflag, 'i');
    tooManyFlagError(oflag, 'o');

}
void testMandatoryFlags(int flag, char opt, char* argv) {
    if (flag != 1) {	//flag was mandatory
        fprintf(stderr, "%s: missing -%c option\n", argv, opt);
        fprintf(stderr, "usage: %s [-c] -e elevname -b orignalname -w file_to_write\n", argv);
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
    printf("The interpolation simulator takes the following command-line arguments: \n");
    PRINT_HELP("-e <e>: Elevation grid flag")
    PRINT_HELP("-i <i>: Grid that will be interpolated(input)")
    PRINT_HELP("-o <o>: Filename you wish to write your grid to(output). If not given, the code will not write to output")
}
void printRenderCommands() {
    
    printf("The rendering map takes the following commands: \n");
    PRINT_HELP("q: quit")
    PRINT_HELP("e: Draw Elevgrid")
    PRINT_HELP("s: Draw the sea")
    PRINT_HELP("b: Draw the original grid on top of the elevgrid")
    PRINT_HELP("i: Draw interpolated grid with original on top")
    printf("\n");

}

/* ***************************** */
void keypress(unsigned char key, int x, int y) {
    switch(key)    {
        case 'q':
            freeGridData(&elevgrid);
            freeGridData(&origgrid);
            freeGridData(&interpgrid);
            exit(0);
            break;
        case 'e':
            printf("Draw Elevgrid\n");
            DRAW = ELEV;
            break;
        case 's':
            printf("Draw the sea\n");
            DRAW = SEA;
            break;
        case 'i':
            printf("Draw interpolated grid with original on top\n");
            DRAW = COMBINE_INTERP;
            break;
        case 'b':
            printf("Draw elevation grid with the original grid on top\n");
            DRAW =ELEV_ORIG;
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
    switch (DRAW) {
        case ELEV:
            draw_elevgrid();
            break;
        case SEA:
            draw_sea();
            break;
        case ELEV_ORIG:
            draw_elev_orig();
            break;
        case COMBINE_INTERP:
            draw_interp_and_original();
            break;
        default:
            break;
    }
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
 This function is used to draws the elevgrid. 
 It calls draw_color and draw_point.
 
 */
void draw_elevgrid() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(elevgrid.data);
    double max = findMax(&elevgrid);
    double minLand= findMinLand(&elevgrid);
    
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            double value = elevgrid.data[i][j];
            draw_color(value, minLand,max);
            draw_point(i,j, &elevgrid);
        }
    }
}
/*
 This function is used to draws the sea. If the elevgrid value is no data
 then the function will set the value to 0, otherwise set value to 1.
 It calls draw_binary and draw_point.
 
 */
void draw_sea() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(elevgrid.data);
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            double value = elevgrid.data[i][j];
            if (value == elevgrid.NODATA_value) {
                value = 0;
            } else {
                value = 1;
            }
            draw_binary(value);
            draw_point(i,j, &elevgrid);
        }
    }
}
/*
 This function is used to draws the elevgrid with the origgrid on top.
 If the origgrid has a value (ie not nodata) then it sets value to that value
 and calls draw_black. Otherwise it sets the value to the same as the elevgrid
 and calls draw_color
 It calls draw_color, draw_black and draw_point.
 
 */
void draw_elev_orig() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    double maxBFE = findMax(&origgrid);
    double minLandBFE= findMinLand(&origgrid);
    double maxELEV = findMax(&elevgrid);
    double minLandELEV= findMinLand(&elevgrid);
    
    
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            double value;
            if (origgrid.data[i][j] != elevgrid.NODATA_value) {
                value = origgrid.data[i][j];
                draw_blue(value, minLandBFE, maxBFE);
            } else {
                value = elevgrid.data[i][j];
                draw_color(value, minLandELEV,maxELEV);
            }
            draw_point(i,j, &elevgrid);
        }
    }
}
/*
 This function is used to draws the elevgrid with the origgrid on top.
 If the origgrid has a value (ie not nodata) then calls draw_combinedOrig_Interp 
 with a type of 1. Otherwise the type is 0
 It calls draw_combinedOrig_Interp and draw_point.
 
 */
void draw_interp_and_original() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    double max = findMax(&origgrid);
    double minLand= findMinLand(&origgrid);
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            double value = interpgrid.data[i][j];
            if (origgrid.data[i][j] != elevgrid.NODATA_value) {
                draw_combinedOrig_Interp(value,1,minLand,max);
            } else {
                draw_combinedOrig_Interp(value,0,minLand,max);
            }
            draw_point(i,j, &elevgrid);
        }
    }
}

/*
 This function is used to draw a particular point of a grid. It takes the grid and
 the point as inputs. It calculates which is larger row or column and calculates 
 where the point should land on the window.
 */

void draw_point(int i, int j, Grid* grid) {
    point myPoint;
    myPoint.x = i;
    myPoint.y = j;
    float x=0, y=0;  //just to initialize with something
    
    float larger,smaller;
    if (grid->nrows > grid->ncols) {
        larger = grid->nrows;
        smaller = grid->ncols;
        
    } else {
        larger = grid->ncols;
        smaller = grid->ncols;
    }
    
    x = (((myPoint.y)/(larger))*2) - smaller/larger;
    y = -(((myPoint.x)/(larger))*2) + 1;
    
    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();
}


void draw_color(double value, double minLand, double max) {
    double base;
    double thisMin = minLand;
    if (minLand < 0) {
        thisMin = 0;
    }
    base = (max-thisMin)/numCategories;
    change_color_land(value, base, thisMin);
}

void draw_binary(double value) {
    if (value == 0) {
        glColor3fv(blue);
    } else {
        glColor3fv(black);
    }
}
void draw_black(double value,double minLand, double max) {
    assert(minLand > 0);
    double base = (max-minLand)/numCategories;
    change_color_gray(value, base, minLand);
    
}
void draw_blue(double value,double minLand, double max) {
    assert(minLand > 0);
    double base = (max-minLand)/numCategories;
    change_color_blue(value, base, minLand);
    
}

/*
 This particular draw_point is used to render the bfe
 with interp grid. The numbers are adjusted so that the value
 gets corrected
 */

void draw_combinedOrig_Interp(double value,int orig_or_interp,  double minLand,double max) {
    double thisMin = minLand;
    if (minLand < 0) {
        thisMin = 0;
    }
    double base = (max-thisMin)/numCategories;
    if (orig_or_interp == 0) {
        change_color_gray(value, base, thisMin);
    } else {
        change_color_blue(value, base, thisMin);
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

