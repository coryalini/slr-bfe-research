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
#include "bfe.hpp"

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
int HAT = 0;
/* global variables */
const int WINDOWSIZE = 500;
//const int POINT_SIZE  = 5.0f;
int POINT_SIZE;

double ELEV_CONVERTER = 3.28084;
double BFE_CONVERTER = 1;
int interp_bfe_EXISTS = 1, DRAW = 0;
const char *elevname, *writeGridname, *bfename;
Grid elevgrid, bfegrid, interp_bfegrid, currgrid;

//HACKKK
double rise = 0;

enum {ELEV = 0, SEA = 1, INTERP_BFE= 2,COMBINE_INTERP=3};
enum {COLOR = 0, BINARY_COLOR = 1,BLACK_COLOR = 2, COMBINE_COLOR = 3};

static float numCategories = 6.0;

GLfloat red_pink[3] = {0.969, 0.396, 0.396};
GLfloat black[3] = {0.0, 0.0, 0.0};

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

GLfloat blue1[3] = {0.568,0.776,0.792};
GLfloat blue2[3] = {0.572,0.784,0.8};
GLfloat blue3[3] = {0.529,0.737,0.756};
GLfloat blue4[3] = {0.455,0.655,0.690};
GLfloat blue5[3] = {0.380,0.568,0.619};
GLfloat blue6[3] = {0.305,0.486,0.553};

/* forward declarations of functions */
void keypress(unsigned char key, int x, int y);
void reset();
void display(void);

void getOptExecution(int argc, char* const* argv);
void testMandatoryFlags(int flag, char opt, char* argv);
void tooManyFlagError(char flag, char opt);
void helpFlag();
void commandFlag();

void draw_grid(Grid* grid,int grid_type,float rise);
void general_draw_point(point mypoint, Grid* grid,int grid_type, float rise, double minLand, double max);

void waterGrid(Grid* grid);
void setCurrGrid(Grid* grid);
void combineBFEGrids(Grid* bfe, Grid* interp);

void draw_point_color(double value, double minLand, double max);
void draw_point_binary(double value);
void draw_point_black(double value,double minLand, double max);
void draw_point_combineBFE(double value,double minLand, double max);
GLfloat* interpolate_colors(GLfloat* lowerColor, GLfloat* upperColor,double value,double lowerBound,double upperBound);

void change_color_land(double value, double base, double thisMin);
void change_color_gray(double value, double base, double thisMin);
void change_color_blue(double value, double base, double thisMin);

int main(int argc, char * argv[]) {
    
//    if (argc != 4) {
//        printf("ERROR: Arguments were not included. %d \n", argc);
//        return 1;
//    }
//    
//    elevname = argv[1];
//    bfename = argv[2];
//    writeGridname = argv[3];
    getOptExecution(argc, argv);
    clock_t start = clock(), diff;
    readGridfromFile(elevname, &elevgrid,ELEV_CONVERTER);
    diff = clock() - start;
    unsigned long msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Reading elevgrid took %lu seconds %lu milliseconds\n", msec/1000, msec%1000);

    
    clock_t start2 = clock(), diff2;
    readGridfromFile(bfename, &bfegrid,BFE_CONVERTER);
    diff2 = clock() - start2;
    unsigned long msec2 = diff2 * 1000 / CLOCKS_PER_SEC;
    printf("Reading interp_bfegrid took %lu seconds %lu milliseconds\n", msec2/1000, msec2%1000);
    if (bfegrid.ncols != elevgrid.ncols || bfegrid.nrows !=elevgrid.nrows) {
        printf("ERROR:The %s [%ld,%ld] and elevgrid [%ld,%ld] do not have the same grid dimensions!\n",bfename, bfegrid.nrows,bfegrid.ncols,elevgrid.nrows, elevgrid.ncols);
        exit(0);
    }

    mallocGrid(bfegrid, &interp_bfegrid);
    setHeaders(bfegrid, &interp_bfegrid);
    copyGrid(&bfegrid, &interp_bfegrid);
    
    clock_t start3 = clock(), diff3;
    printf("start interp_bfe\n");
    start_interp_bfe_withFlooded(&elevgrid,&interp_bfegrid, rise);
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

/*
 * This function uses getOpt to read the terminal prompt and do the
 * desired action depending on what it was given.
 * It uses a switch statement to figure out which flag
 * was used.
 */
void getOptExecution(int argc, char* const* argv) {
    
    int opt;
    int hflag = 0, cflag =0, eflag = 0, wflag = 0, iflag = 0;
    
    extern char* optarg;
    extern int optopt;
    while ((opt = getopt(argc, argv, "hce:i:w:" )) != -1) {
        switch (opt) {
            case 'h':
                hflag += 1;
                helpFlag();
                break;
            case 'c':
                commandFlag();
                cflag+=1;
                break;
            case 'e':
                elevname = optarg;
                eflag += 1;
                break;
            case 'w':
                writeGridname = optarg;
                wflag+=1;
                break;
            case 'i':
                HAT = 1;
                iflag+=1;
                bfename = optarg;
                break;
            case 'b':
                bfename = optarg;
                iflag+=1;
                break;
//            case 'r':
//                rise = atof(optarg);
//                rflag+=1;
//                break;
        }
    }
    testMandatoryFlags(eflag, 'e', argv[0]);
    testMandatoryFlags(wflag, 'w', argv[0]);
    testMandatoryFlags(iflag, 'i', argv[0]);
    
    tooManyFlagError(hflag, 'h');
    tooManyFlagError(cflag, 'c');
    tooManyFlagError(eflag, 'e');
    tooManyFlagError(wflag, 'w');
    tooManyFlagError(iflag, 'i');
//    tooManyFlagError(rflag, 'r');
}
void testMandatoryFlags(int flag, char opt, char* argv) {
    if (flag != 1) {	//flag was mandatory
        fprintf(stderr, "%s: missing -%c option\n", argv, opt);
        fprintf(stderr, "usage: %s [-h] [-c] -e elevname [-b bfename] -w file_to_write -r rise \n", argv);
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
    printf("\nThe reference simulator takes the following command-line arguments: \n");
    PRINT_HELP("-h: Optional help flag that prints usage info")
    PRINT_HELP("-c: Optional command flag that prints usage info for rendering")
    PRINT_HELP("-e <e>: Elevation grid flag")
    PRINT_HELP("-i <i>: Interpolated elevation grid")
    PRINT_HELP("-w <w>: Filename you wish to write your grid to")
}
void commandFlag() {
    
    printf("\nThe rendering map takes the following commands: \n");
    PRINT_HELP("q: quit")
    PRINT_HELP("w: write to files")
    
    PRINT_HELP("e: Draw Elevgrid")
    PRINT_HELP("s: Draw the sea")
    PRINT_HELP("i: Draw interpolated grid with original on top")

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
        case 's':
            printf("Draw the sea\n");
            DRAW = SEA;
            break;
        case 'b':
            printf("Draw interpolated grid with original on top\n");
            DRAW = COMBINE_INTERP;
            break;
        case 'i':
            printf("Draw interpolated grid\n");
            DRAW = INTERP_BFE;
            break;
        default:
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
        case SEA:
            waterGrid(&elevgrid);
            coloring = BINARY_COLOR;
            break;
        case INTERP_BFE:
            setCurrGrid(&interp_bfegrid);
            coloring = BLACK_COLOR;
            break;
        case COMBINE_INTERP:
            combineBFEGrids(&bfegrid, &interp_bfegrid);
            coloring = COMBINE_COLOR;
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


void draw_grid(Grid* grid, int grid_type,float rise) {
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(grid->data);
    double max;
    double minLand= findMinLand(grid);
    if (grid_type == COMBINE_COLOR) {
        max = findMax(&bfegrid);
    } else {
        max = findMax(grid);
    }
    
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            point newPoint;
            newPoint.x = i;
            newPoint.y = j;
            general_draw_point(newPoint, grid, grid_type,rise,minLand, max);
        }
    }
}


void general_draw_point(point mypoint, Grid* grid,int grid_type, float rise, double minLand, double max) {
    double value = grid->data[(int)mypoint.x][(int)mypoint.y];
    if (grid_type == COLOR) {
        draw_point_color(value, minLand,max);
    } else if (grid_type == BINARY_COLOR) {
        draw_point_binary(value);
    }  else {
        draw_point_combineBFE(value,minLand, max);
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

void waterGrid(Grid* grid) {
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            if(grid->data[i][j] == grid->NODATA_value) {
                currgrid.data[i][j] = 0;
            } else {
                currgrid.data[i][j] = 1;
            }
        }
    }
}
/*
 When called,ex: grid1 is slr or slr+bfe and grid2 is elev.
 
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
void combineBFEGrids(Grid* bfe, Grid* interp) {
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            if (bfe->data[i][j] != elevgrid.NODATA_value) {
                currgrid.data[i][j] = bfe->data[i][j]+ -elevgrid.NODATA_value;//just a very unlikely value
            } else {
                currgrid.data[i][j] = interp->data[i][j];
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
    double base = (max-thisMin)/numCategories;
//    printf("min: %lf, max: %lf, base:%lf\n",minLand,max,base);
    if (value < -elevgrid.NODATA_value|| value == elevgrid.NODATA_value) {
#ifdef HAT
        glPointSize(5.0f);
#endif

        change_color_gray(value, base, thisMin);
    } else {
#ifdef HAT
        glPointSize(7.0f);
#endif
        value +=elevgrid.NODATA_value;
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


