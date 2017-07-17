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
const float ALPHA = 0.007;//used to change transparancy of flooding rendering

//size of OpenGL window 
const int WINDOWSIZE = 500;

//change size of point
const int POINT_SIZE  = 1.0f;

//used to convert units of grids
double ELEV_CONVERTER = 3.28084;
double BFE_CONVERTER = 1;

int interp_bfe_EXISTS = 1, DRAW = 0;

//filename of the grid
const char *elevname,*bfename,*floodedname, *writeGridname;
Grid elevgrid, bfegrid, floodedgrid, currgrid;

enum {ELEV_BFE_FLOOD = 0};
enum {THREE_COLORS =0};

static float numCategories = 6.0;//used for buckets


//COLORS
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

void getOptExecution(int argc, char* const* argv);
void testMandatoryFlags(int flag, char opt, char* argv);
void tooManyFlagError(char flag, char opt);
void helpFlag();
void printRenderCommands();

void draw_elevgrid();
void draw_sea();
void draw_flooded(double alpha);
void draw_point(int i, int j, Grid* grid);
void draw_bfe_and_flooding();

void draw_color(double value, double minLand, double max);
void draw_binary(double value);
void draw_black(double value,double minLand, double max);
void draw_blue(double value,double minLand, double max);

void change_color_land(double value, double base, double thisMin);
void change_color_gray(double value, double base, double thisMin);
void change_color_blue(double value, double base, double thisMin);
GLfloat* interpolate_colors(GLfloat* lowerColor, GLfloat* upperColor,double value,double lowerBound,double upperBound);






int main(int argc, char * argv[]) {
    
    if (argc != 1) {
        printRenderCommands();
        getOptExecution(argc, argv);
    } else {
        helpFlag();
        printRenderCommands();
        exit(0);
    }
    
    //read elevation grid from file 
    clock_t start1, end1; 
    start1 = clock();
    readGridfromFile(elevname, &elevgrid,ELEV_CONVERTER);
    end1  = clock(); 
    unsigned long msec = (end1-start1) * 1000 / CLOCKS_PER_SEC;
    printf("Reading %s took %lu seconds %lu milliseconds\n",
	   elevname, msec/1000, msec%1000);
    
    //read bfe grid from file 
    clock_t start2, end2; 
    start2 = clock();
    readGridfromFile(bfename, &bfegrid,BFE_CONVERTER);
    end2 = clock();
    unsigned long msec2 = (end2-start2) * 1000 / CLOCKS_PER_SEC;
    printf("Reading %s took %lu seconds %lu milliseconds\n",
	   bfename, msec2/1000, msec2%1000);

    //grids must have same dimensions
    if (bfegrid.ncols != elevgrid.ncols || bfegrid.nrows !=elevgrid.nrows) {
      printf("ERROR: grids dimensions not compatible!!\n"); 
      printf("%s [%ld,%ld] and %s [%ld,%ld]\n",
	     elevname, elevgrid.nrows, elevgrid.ncols, 
	     bfename, bfegrid.nrows,bfegrid.ncols);
      exit(0);
    }

    //read flooded grid from file 
    clock_t start3, end3; 
    start3 = clock();
    readGridfromFile(floodedname, &floodedgrid,BFE_CONVERTER);
    end3 = clock();
    unsigned long msec3= (end3 - start3) * 1000 / CLOCKS_PER_SEC;
    printf("Reading %s took %lu seconds %lu milliseconds\n",
	   floodedname, msec3/1000, msec3%1000);
   
    //grids must have same dimensions
    if (floodedgrid.ncols != elevgrid.ncols || floodedgrid.nrows !=elevgrid.nrows) {
      printf("ERROR: grids dimensions not compatible!\n"); 
      printf("%s [%ld,%ld] and %s [%ld,%ld]\n",
	     floodedname, floodedgrid.nrows,floodedgrid.ncols,
	     elevname, elevgrid.nrows, elevgrid.ncols);
        exit(0);
    }
    
    
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
    PRINT_HELP("-i <i>: Original bfe grid")
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
            draw_bfe_and_flooding();
            coloring = THREE_COLORS;
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
 This function is used to draws the elevgrid.
 It calls draw_color and draw_point.
 
 */
void draw_bfe() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(bfegrid.data);
//    double max = findMax(&bfegrid);
//    double minLand= findMinLand(&bfegrid);
    
    for (int i = 0; i < bfegrid.nrows; i++) {
        for (int j = 0; j < bfegrid.ncols; j++) {
            double value = bfegrid.data[i][j];
            if (value != bfegrid.NODATA_value){
                glColor3fv(black);
//                draw_black(value, minLand,max);
            } else  {
                glColor4f(blue[0],blue[1],blue[2],0);
            }
            draw_point(i,j, &bfegrid);
        }
    }
}


/*
 When called,ex: grid1 is flooded and grid2 is elev.
 
 This function sets the global variable, currGrid.
 If there is a bfe available, the code utilizes the given rise to find what values will be flooded.
 
 */

void draw_flooded(double alpha) {
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            if (floodedgrid.data[i][j] != floodedgrid.NODATA_value) {
                glColor4f(red_pink[0],red_pink[1],red_pink[2],alpha);
                
            } else {
                glColor4f(blue[0],blue[1],blue[2],0);
            }
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
void draw_bfe_and_flooding() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    draw_elevgrid();
    draw_bfe();
    draw_flooded(ALPHA);

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


