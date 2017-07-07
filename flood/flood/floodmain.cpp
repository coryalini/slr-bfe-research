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
 f: Draw Flooded grid
 g: Draw Flooded grid with gray land
 j: Draw just flooded grid (no elev)
 s: Draw the sea
 h: Draw just flooded grid displaying water
 i: Draw interpolated bfe
 
 +: increase the sea level rise
 =: increase the sea level rise
 -: decrease the sea level rise
 */

/* Assumptions:
 no data is water.
 no data + 1 isnt going to be a height
 */


#include "flood.hpp"
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
//double ELEV_CONVERTER = 1;
double BFE_CONVERTER = 1;


int interp_bfe_EXISTS = 1, DRAW = 0;
const char *elevname, *writeGridname, *bfename;

enum {ELEV = 0, FLOODED = 1,FLOODED_GRAY= 2,FLOODED_ELEV=3,SEA= 4,WATER_FLOODED_ELEV=5,INTERP_BFE=6 };
enum {COLOR = 0, BLACK_COLOR = 1, BINARY_COLOR = 2, COMBINE_COLOR = 3, COMBINE_WATER = 5, GRAY_BLUE= 7};



static float numCategories = 6.0;
Grid elevgrid, interp_bfegrid, currgrid,floodedgrid;

float rise;
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
void calculateGrids(Grid* elevgrid);
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
void combineGrids_nobfe(Grid* grid1, Grid* grid2, float rise);
void combineGrids_bfe(Grid* grid1, Grid* grid2,float rise);

void draw_point_color(double value, double minLand, double max);
void draw_point_black(double value,double minLand, double max);
void draw_point_binary(double value);
void draw_point_combine(double value, double minLandElev, double theRise);
void draw_point_combine_water(double value,double minLandElev,double theRise);
void draw_point_see_slr_better(double value,double minLand, double max);

void change_color_land(double value, double base, double thisMin);
void change_color_gray(double value, double base, double thisMin);
void change_color_blue(double value, double base, double thisMin);

GLfloat* interpolate_colors(GLfloat* lowerColor, GLfloat* upperColor,double value,double lowerBound,double upperBound);



int main(int argc, char * argv[]) {
    if (argc != 1) {
        getOptExecution(argc, argv);
    } else {
        helpFlag();
        exit(0);
    }
    printf("%s\n",elevname);
    clock_t start = clock(), diff;
    readGridfromFile(elevname, &elevgrid,ELEV_CONVERTER);
    diff = clock() - start;
    unsigned long msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Reading elevgrid took %lu seconds %lu milliseconds\n", msec/1000, msec%1000);
    
    mallocGrid(elevgrid, &floodedgrid);
    setHeaders(elevgrid, &floodedgrid);
    
    if (interp_bfe_EXISTS) {
        clock_t start2 = clock(), diff2;
        readGridfromFile(bfename, &interp_bfegrid,BFE_CONVERTER);
        if (interp_bfegrid.ncols != elevgrid.ncols || interp_bfegrid.nrows !=elevgrid.nrows) {
            printf("ERROR:The %s [%ld,%ld] and elevgrid [%ld,%ld] do not have the same grid dimensions!\n",bfename, interp_bfegrid.nrows,interp_bfegrid.ncols,elevgrid.nrows, elevgrid.ncols);
            exit(0);
        }
        diff2 = clock() - start2;
        unsigned long msec2 = diff2 * 1000 / CLOCKS_PER_SEC;
        printf("Reading interp_bfegrid took %lu seconds %lu milliseconds\n", msec2/1000, msec2%1000);
    }
    
    
//   HACK FOR DATA WITH NEGATIVE VALUES
    start_slr(&elevgrid,&floodedgrid, 0);
    for (int i = 0; i < elevgrid.nrows; i++) {
        for(int j = 0; j < elevgrid.ncols; j++){
            if (floodedgrid.data[i][j] == NEW_WATER) {
                elevgrid.data[i][j] = elevgrid.NODATA_value;
            } else {
                elevgrid.data[i][j] = floodedgrid.data[i][j];
            }
        }
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
    if (!interp_bfe_EXISTS) {
        clock_t start2 = clock(), diff2;
        printf("start SLR @ %g\n",rise);
        start_slr(elevgrid,&floodedgrid, rise);
        
        diff2 = clock() - start2;
        unsigned long msec2 = diff2 * 1000 / CLOCKS_PER_SEC;
        printf("SLR took %lu seconds %lu milliseconds\n", msec2/1000, msec2%1000);
    } else {
        clock_t start3 = clock(), diff3;
        printf("start SLR + interp_bfe @ %g\n",rise);
        start_slr_interp_bfe(elevgrid, &interp_bfegrid,&floodedgrid, rise);
        diff3 = clock() - start3;
        unsigned long msec3 = diff3 * 1000 / CLOCKS_PER_SEC;
        printf("interp_bfe+SLR took %lu seconds %lu milliseconds\n", msec3/1000, msec3%1000);
    }
}
/*
 * This function uses getOpt to read the terminal prompt and do the
 * desired action depending on what it was given.
 * It uses a switch statement to figure out which flag
 * was used.
 */
void getOptExecution(int argc, char* const* argv) {

    int opt;
    int cflag =0, eflag = 0, wflag = 0, iflag = 0, rflag = 0;
    
    extern char* optarg;
    extern int optopt;
    while ((opt = getopt(argc, argv, "ce:i:w:r:" )) != -1) {
        switch (opt) {
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
                bfename = optarg;
                interp_bfe_EXISTS = 1;
                iflag+=1;
                break;
            case 'r':
                rise = atof(optarg);
                rflag+=1;
                break;
        }
    }
    testMandatoryFlags(eflag, 'e', argv[0]);
    testMandatoryFlags(wflag, 'w', argv[0]);
    testMandatoryFlags(rflag, 'r', argv[0]);
    
    tooManyFlagError(cflag, 'c');
    tooManyFlagError(eflag, 'e');
    tooManyFlagError(wflag, 'w');
    tooManyFlagError(iflag, 'i');
    tooManyFlagError(rflag, 'r');
}
void testMandatoryFlags(int flag, char opt, char* argv) {
    if (flag != 1) {	//flag was mandatory
        fprintf(stderr, "%s: missing -%c option\n", argv, opt);
        fprintf(stderr, "usage: %s [-c] -e elevname [-i interpname] -w file_to_write -r rise \n", argv);
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
    PRINT_HELP("-c: Optional command flag that prints usage info for rendering")
    PRINT_HELP("-e <e>: Elevation grid flag")
    PRINT_HELP("-i <i>: Interpolated elevation grid")
    PRINT_HELP("-w <w>: Filename you wish to write your grid to")
    PRINT_HELP("-r <r>: The desired rise amount")
}
void commandFlag() {
    
    printf("The rendering map takes the following commands: \n");
    PRINT_HELP("q: quit")
    PRINT_HELP("w: write to files")
    
    PRINT_HELP("e: Draw Elevgrid")
    PRINT_HELP("f: Draw Flooded grid")
    PRINT_HELP("g: Draw Flooded grid with gray land")
    PRINT_HELP("j: Draw just flooded grid (no elev)")
    PRINT_HELP("s: Draw the sea")
    PRINT_HELP("h: Draw just flooded grid displaying water")
    PRINT_HELP("i: Draw interpolated bfe")
    
    PRINT_HELP("+: increase the sea level rise")
    PRINT_HELP("=: increase the sea level rise")
    PRINT_HELP("-: decrease the sea level rise")
}


/* ***************************** */
void keypress(unsigned char key, int x, int y) {
    switch(key)    {
        case 'q':
            freeGridData(&elevgrid);
            freeGridData(&currgrid);
            freeGridData(&floodedgrid);
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
        case 'f':
            printf("Draw Flooded grid\n");
            DRAW = FLOODED;
            break;
        case 'g':
            printf("Draw Flooded grid with gray land\n");
            DRAW = FLOODED_GRAY;
            break;
        case 'j':
            printf("Draw just flooded grid (no elev)\n");
            DRAW = FLOODED_ELEV;
            break;
        case 's':
            printf("Draw Sea\n");
            DRAW = SEA;
            break;
        case 'h':
            printf("Draw Water+SLR-Flooded(SLR-Elev)\n");
            DRAW = WATER_FLOODED_ELEV;
            break;
            
        case 'i':
            printf("Draw Interpolated BFE\n");
            DRAW = INTERP_BFE;
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
        case FLOODED:
            setCurrGrid(&floodedgrid);
            coloring = COLOR;
            break;
        case FLOODED_GRAY:
            setCurrGrid(&floodedgrid);
            coloring = GRAY_BLUE;
            break;
        case SEA:
            waterGrid(&elevgrid);
            coloring = BINARY_COLOR;
            break;
        case FLOODED_ELEV:
            if (interp_bfe_EXISTS) {
                combineGrids_bfe(&floodedgrid, &elevgrid,rise);
            } else {
                combineGrids_nobfe(&floodedgrid, &elevgrid,rise);
            }
            coloring = COMBINE_COLOR;
            break;
        case WATER_FLOODED_ELEV:
            if (interp_bfe_EXISTS) {
                combineGrids_bfe(&floodedgrid, &elevgrid,rise);

            } else {
                combineGrids_nobfe(&floodedgrid, &elevgrid,rise);
            }
            coloring = COMBINE_WATER;
            break;
        case INTERP_BFE:
            if (interp_bfe_EXISTS) {
                setCurrGrid(&interp_bfegrid);
                coloring = BLACK_COLOR;
                break;
            } else {
                return;
            }
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



// DRAWING STUFF
void draw_grid(Grid* grid, int grid_type,float rise) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(grid->data);
    double minLand, max = findMax(grid);
    if (grid_type == COMBINE_COLOR || grid_type == COMBINE_WATER) {
        minLand = findMinLand(&elevgrid);
    } else {
        minLand= findMinLand(grid);
    }
    if (interp_bfe_EXISTS) {
        for (int i = 0; i < elevgrid.nrows; i++) {
            for (int j = 0; j < elevgrid.ncols; j++) {
                point newPoint;
                newPoint.x = i;
                newPoint.y = j;
                if (interp_bfegrid.data[i][j] != elevgrid.NODATA_value) {
                    general_draw_point(newPoint, grid, grid_type,rise +interp_bfegrid.data[i][j] ,minLand, max);

                } else {
                    general_draw_point(newPoint, grid, grid_type,rise,minLand, max);
                }
            }
        }
    } else {
        for (int i = 0; i < elevgrid.nrows; i++) {
            for (int j = 0; j < elevgrid.ncols; j++) {
                point newPoint;
                newPoint.x = i;
                newPoint.y = j;
                general_draw_point(newPoint, grid, grid_type,rise,minLand, max);
            }
        }
    }
    
}


void general_draw_point(point mypoint, Grid* grid,int grid_type, float rise, double minLand, double max) {
    double value = grid->data[(int)mypoint.x][(int)mypoint.y];
    switch (grid_type) {
        case COLOR:
            draw_point_color(value, minLand,max);
            break;
        case BINARY_COLOR:
            draw_point_binary(value);
            break;
        case BLACK_COLOR:
            draw_point_black(value, minLand,max);
            break;
        case GRAY_BLUE:
            draw_point_see_slr_better(value, minLand,max);
            break;
        case COMBINE_COLOR:
            draw_point_combine(value,minLand, rise);
            break;
        case COMBINE_WATER:
            draw_point_combine_water(value,minLand, rise);
            break;
        default:
            break;
            
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
 If there is a bfe available, the code utilizes the bfe data given to find what values will be flooded.
 Even if the bfe is given, some values in the bfe grid are NODATA_values, thus
 
 */

void combineGrids_nobfe(Grid* grid1, Grid* grid2, float rise) {
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            if (grid1->data[i][j] == elevgrid.NODATA_value) {
                currgrid.data[i][j] = elevgrid.NODATA_value;
                
            } else {
                currgrid.data[i][j] = grid2->data[i][j]-rise;
            }
        }
    }
}


void combineGrids_bfe(Grid* grid1, Grid* grid2, float rise) {
    //    assert(DRAW == SLRINTERP_BFE_ELEV || DRAW == WATER_SLRINTERP_BFE_ELEV);
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            if (grid1->data[i][j] == elevgrid.NODATA_value) {
                currgrid.data[i][j] = elevgrid.NODATA_value;
                
            } else {
                /*This point may or may not have a bfe value.
                 If we have a bfe value we add it to the rise, otherwise we assume the
                 bfe value is zero (ie just use the rise).*/
                
                if (interp_bfegrid.data[i][j] == elevgrid.NODATA_value) {
                    currgrid.data[i][j] = grid2->data[i][j]- rise;
                }else {
                    currgrid.data[i][j] = grid2->data[i][j] - (interp_bfegrid.data[i][j] + rise);
                }
            }
        }
    }
}

void draw_point_color(double value, double minLand, double max) {
    double thisMin = minLand;
    if (minLand < 0) {
        thisMin = 0;
    }
    double base = (max-thisMin)/numCategories;
    change_color_land(value, base, thisMin);
}
void draw_point_see_slr_better(double value,double minLand, double max) {
    double thisMin = minLand;
    if (minLand < 0) {
        thisMin = 0;
    }
    double base = (max-thisMin)/numCategories;
    change_color_gray(value, base, thisMin);
}

void draw_point_black(double value,double minLand, double max) {
    assert(minLand > 0);
    
    double base = (max-minLand)/numCategories;
    change_color_gray(value, base, minLand);

}
void draw_point_binary(double value) {
    if (value == 0) {
        glColor3fv(blue);
    } else {
        glColor3fv(black);
    }
}

void draw_point_combine(double value,double minLandElev, double theRise) {
    if (value > 0 || value == elevgrid.NODATA_value) {
        glColor3fv(black);
    } else {
        //assuming minimum (ie max negative number) is about zero
        value += -(minLandElev-theRise);
        double maxNum = 1+ -(minLandElev-theRise);//this is going to be a number larger than all the others
        double base = maxNum/numCategories;
        change_color_blue(value, base, 0);
    }
}
void draw_point_combine_water(double value,double minLandElev, double theRise) {
    if (value > 0){
        glColor3fv(black);
    }else if(value == elevgrid.NODATA_value) {
        glColor3fv(blue);
    } else {
        //assuming minimum (ie max negative number) is about zero
        value += -(minLandElev-theRise);
        double maxNum = 1+ -(minLandElev-theRise);//this is going to be a number larger than all the others
        double base = maxNum/numCategories;
        change_color_blue(value, base, 0);
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
    } else if (value == NEW_WATER) {
        glColor3fv(lightblue);
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

