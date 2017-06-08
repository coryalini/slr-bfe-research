//
//  drawing.cpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#include "drawing.hpp"
enum {color = 0, blackC = 1, binary = 2 };

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
//GLfloat lightyellow[3] = {0.988,1.0,0.898};
GLfloat green1[3] = {0.945, 1.0, 0.905};
GLfloat greenmid[3] = {0.749, 0.918, 0.737};
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

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); //clear the matrix
    float coloring = 0;
    if (interp_bfe_EXISTS) {
        switch (DRAW) {
            case slr:
                changeCurrGrid(&slrgrid);
                coloring = color;
                break;
            case originterp_bfe:
                changeCurrGrid(&originterp_bfegrid);
                coloring = blackC;
                break;
            case interp_bfe:
                changeCurrGrid(&interp_bfegrid);
                coloring = blackC;
                break;
            case slrinterp_bfe:
                changeCurrGrid(&interp_bfeslrgrid);
                printf("%lf \n",interp_bfeslrgrid.data[1000][1000]);
                printf("%lf \n",currGrid.data[1000][1000]);

                
                coloring = color;
                break;
            case elev:
                changeCurrGrid(&elevgrid);
                coloring = color;
                break;
            case slr_elev:
                combineGrids(&slrgrid, &elevgrid, NEW_WATER);
                coloring = binary;
                break;
            case slrinterp_bfe_elev:
                combineGrids(&interp_bfeslrgrid, &elevgrid, NEW_WATER);
                coloring = binary;
                break;
            case slrinterp_bfeminusslr:
                diffGrids(&interp_bfeslrgrid, &slrgrid, NEW_WATER);
                coloring = binary;
                break;
                
            default:
                break;
        }
        
    } else {
        if (DRAW == elev) {
            changeCurrGrid(&elevgrid);
            coloring = color;
        } else if (DRAW == slr_elev) {
            combineGrids(&slrgrid, &elevgrid, NEW_WATER);
            coloring = color;
        } else {
            changeCurrGrid(&slrgrid);
            coloring = color;
        }
    }
    draw_grid(&currGrid, coloring);
    
    /* execute the drawing commands */
    glFlush();
}
void diffGrids(Grid* grid1, Grid* grid2, double diff) {
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            
            double currRise = rise;
            if (DRAW == slrinterp_bfe_elev) {
                currRise = interp_bfegrid.data[i][j] - rise;
            }
            double value1 = grid1->data[i][j];
            double value2 = grid2->data[i][j]-currRise;
            if (value1 == diff && value2 != diff) {
                currGrid.data[i][j] = 0;
            } else {
                currGrid.data[i][j] = 1;
            }
        }
    }
}

void combineGrids(Grid* grid1, Grid* grid2, double combine) {
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            double value1 = grid1->data[i][j];\
            double currRise = rise;
            if (DRAW == slrinterp_bfe_elev) {
                currRise = interp_bfegrid.data[i][j];
            }
            double value2 = grid2->data[i][j] - currRise;
            double value;
            if (value1 != elevgrid.NODATA_value && value1 != NEW_WATER) {
                value = value1 - value2;
            } else {
                value = value1;
            }
            if (value == combine) {
                currGrid.data[i][j] = 1;
            } else {
                currGrid.data[i][j] = 0;
            }
        }
    }
    
}
void draw_grid(Grid* grid, int grid_type) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(grid->data);
    findMaxMin(grid);
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            point newPoint;
            newPoint.x = i;
            newPoint.y = j;
            general_draw_point(newPoint, grid, grid_type);
        }
    }
}


void general_draw_point(point mypoint, Grid* grid,int grid_type) {
    double value = grid->data[(int)mypoint.x][(int)mypoint.y];
    if (grid_type == color) {
        draw_point_color(value);
    } else if (grid_type == binary) {
        draw_point_binary(value);
    }
    else {
        draw_point_black(value);
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


void draw_point_color(double value) {
    double base = max/numCategories;
    if (value == elevgrid.NODATA_value) {
        glColor3fv(blue);
    } else if (value == NEW_WATER) {
        glColor3fv(lightblue);
    } else if (value < base) {
        glColor3fv(interpolate_colors(green1, greenmid,value,0,base));
    } else if (value < (2 * base)) {
        glColor3fv(interpolate_colors(greenmid, green2,value,base,2*base));
    } else if (value < (3 * base)) {

        glColor3fv(interpolate_colors(green2, darkyellow,value,2*base,3*base));
    } else if (value < (4 * base)) {
        glColor3fv(interpolate_colors(darkyellow, darkorange,value,3*base,4*base));
    } else if (value < (5 * base)) {
        glColor3fv(interpolate_colors(darkorange, red1,value,4*base,5*base));
    }  else {
        glColor3fv(interpolate_colors(red1, red2,value,5*base,6*base));
    }
}



void draw_point_black(double value) {
    double base = max/numCategories;
    if (value == elevgrid.NODATA_value) {
        glColor3fv(lightblue);
    } else if (value < base) {
        glColor3fv(interpolate_colors(gray1, gray2,value,0,base));
    } else if (value < (2 * base)) {
        glColor3fv(interpolate_colors(gray2, gray3,value,base,2*base));
    } else if (value < (3 * base)) {
        glColor3fv(interpolate_colors(gray3, gray4,value,2*base,3*base));
    } else if (value < (4 * base)) {
        glColor3fv(interpolate_colors(gray4, gray5,value,3*base,4*base));
    } else if (value < (5 * base)) {
        glColor3fv(interpolate_colors(gray5, gray6,value,4*base,5*base));
    }  else {
        glColor3fv(interpolate_colors(gray6, black,value,5*base,6*base));
    }
}
void draw_point_binary(double value) {
    if (value == 0) {
        glColor3fv(lightblue);
    } else {
        glColor3fv(black);
    }
}


GLfloat* interpolate_colors(GLfloat* lowerColor, GLfloat* upperColor,double value,double lowerBound,double upperBound) {
    static GLfloat newColor[3] = {-1,-1,-1};
    for(int i = 0; i < 3; i ++) {
        newColor[i] = (double)lowerColor[i] + ((value-lowerBound)/(upperBound-lowerBound))*(double)(upperColor[i]-lowerColor[i]);
        
    }
    return newColor;
    
}

void changeCurrGrid(Grid* grid){
    for (int i = 0; i < grid->nrows; i++) {
        for (int j = 0; j < grid->ncols; j++) {
            currGrid.data[i][j] = grid->data[i][j];
        }
    }
}


//void draw_point_combo(point mypoint, Grid* main, Grid* subtract,  float currRise) {
//    double value1 = main->data[(int)mypoint.x][(int)mypoint.y];
//    double value2 = subtract->data[(int)mypoint.x][(int)mypoint.y] - currRise;
//    double value;
//    if (value1 != elevgrid.NODATA_value && value1 != NEW_WATER) {
//        value = value1 - value2;
//    } else {
//        value = value1;
//    }
//    
//    if (value == NEW_WATER) {
//        glColor3fv(lightblue);
//    } else {
//        glColor3fv(black);
//    }
//    
//    float x=0, y=0;  //just to initialize with something
//    float larger,smaller;
//    if (main->nrows > main->ncols) {
//        larger = main->nrows;
//        smaller = main->ncols;
//        
//    } else {
//        larger = main->ncols;
//        smaller = main->ncols;
//    }
//    
//    x = (((mypoint.y)/(larger))*2) - smaller/larger;
//    y = -(((mypoint.x)/(larger))*2) + 1;
//    
//    glBegin(GL_POINTS);
//    glVertex2f(x, y);
//    glEnd();
//}
//
//void draw_point_diff(point mypoint, Grid* main, Grid* subtract, float currRise) {
//    double value, value1,value2;
//    value1 = main->data[(int)mypoint.x][(int)mypoint.y];//slrinterp_bfe
//    value2 = subtract->data[(int)mypoint.x][(int)mypoint.y]; //SLR
//    
//    if (value1 == NEW_WATER && value2 != NEW_WATER) {
//        value = 0;
//    } else {
//        value = 1;
//    }
//    
//    if (value == 0) {
//        glColor3fv(lightblue);
//    } else {
//        glColor3fv(black);
//    }
//    
//    float x=0, y=0;  //just to initialize with something
//    float larger,smaller;
//    if (main->nrows > main->ncols) {
//        larger = main->nrows;
//        smaller = main->ncols;
//        
//    } else {
//        larger = main->ncols;
//        smaller = main->ncols;
//    }
//    
//    x = (((mypoint.y)/(larger))*2) - smaller/larger;
//    y = -(((mypoint.x)/(larger))*2) + 1;
//    
//    glBegin(GL_POINTS);
//    glVertex2f(x, y);
//    glEnd();
//}
//


//void draw_combo(Grid* main, Grid* subtract) {
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    assert(slrgrid.data);
//    findMaxMin(&slrgrid);
//    
//    float currRise = 0;
//    for (int i = 0; i < elevgrid.nrows; i++) {
//        for (int j = 0; j < elevgrid.ncols; j++) {
//            point newPoint;
//            newPoint.x = i;
//            newPoint.y = j;
//            if (DRAW == 5) {
//                currRise = interp_bfegrid.data[i][j];
//            }
//            draw_point_combo(newPoint, main,subtract, currRise);
//        }
//    }
//}
//void draw_difference(Grid* main, Grid* subtract) {
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    assert(main->data);
//    assert(subtract->data);
//    findMaxMin(&slrgrid);
//    
//    float currRiseB = 0;
//    
//    for (int i = 0; i < elevgrid.nrows; i++) {
//        for (int j = 0; j < elevgrid.ncols; j++) {
//            point newPoint;
//            newPoint.x = i;
//            newPoint.y = j;
//            currRiseB = interp_bfegrid.data[i][j] - rise;
//            
//            draw_point_diff(newPoint, main,subtract,currRiseB);
//        }
//    }
//}
//
//
//void draw_slr_slr_interp_bfe() {
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    assert(interp_bfeslrgrid.data);
//    assert(slrgrid.data);
//    findMaxMin(&slrgrid);
//    
//    float currRiseB = 0;
//    
//    for (int i = 0; i < elevgrid.nrows; i++) {
//        for (int j = 0; j < elevgrid.ncols; j++) {
//            point newPoint;
//            newPoint.x = i;
//            newPoint.y = j;
//            currRiseB = interp_bfegrid.data[i][j] - rise;
//            draw_point_slr_slr_interp_bfe(newPoint, &interp_bfeslrgrid,&slrgrid, currRiseB);
//        }
//    }
//}
//void draw_point_slr_slr_interp_bfe(point mypoint, Grid* main, Grid* subtract, float currRise) {
//    float value, value1,value2;
//    value1 = main->data[(int)mypoint.x][(int)mypoint.y];//slrinterp_bfe
//    value2 = subtract->data[(int)mypoint.x][(int)mypoint.y]; //SLR
//    float interp_bfeval = interp_bfegrid.data[(int)mypoint.x][(int)mypoint.y];
//    
//    if (value1 == NEW_WATER && value2 != NEW_WATER) {
//        value = 0;
//    } else {
//        value = 1;
//    }
//    if (value == 0) {
//        double base = 30.0/numCategories;
//        if (value1 == elevgrid.NODATA_value) {
//            glColor3fv(blue);
//        } else if (interp_bfeval < base) {
//            glColor3fv(gray1);
//        } else if (interp_bfeval < (2 * base)) {
//            glColor3fv(gray2);
//        } else if (interp_bfeval < (3 * base)) {
//            glColor3fv(gray3);
//        } else if (interp_bfeval < (4 * base)) {
//            glColor3fv(gray4);
//        } else if (interp_bfeval < (5 * base)) {
//            glColor3fv(gray5);
//        }  else {
//            glColor3fv(gray6);
//        }
//    } else {
//        draw_point_color(value1);
//    }
//    
//    float x=0, y=0;  //just to initialize with something
//    float larger,smaller;
//    if (main->nrows > main->ncols) {
//        larger = main->nrows;
//        smaller = main->ncols;
//        
//    } else {
//        larger = main->ncols;
//        smaller = main->ncols;
//    }
//    
//    x = (((mypoint.y)/(larger))*2) - smaller/larger;
//    y = -(((mypoint.x)/(larger))*2) + 1;
//    
//    glBegin(GL_POINTS);
//    glVertex2f(x, y);
//    glEnd();
//}




