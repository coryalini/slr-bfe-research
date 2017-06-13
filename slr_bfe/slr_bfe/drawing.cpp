//
//  drawing.cpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#include "drawing.hpp"
enum {COLOR = 0, BLACK_COLOR = 1, BINARY_COLOR = 2, COMBINE_COLOR = 3,COMBINE_COLOR_BFE = 4, COMBINE_WATER = 5, COMBINE_WATER_BFE = 6};

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

GLfloat blue1[3] = {0.568,0.776,0.792};
GLfloat blue2[3] = {0.572,0.784,0.8};
GLfloat blue3[3] = {0.529,0.737,0.756};
GLfloat blue4[3] = {0.455,0.655,0.690};
GLfloat blue5[3] = {0.380,0.568,0.619};
GLfloat blue6[3] = {0.305,0.486,0.553};


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
                combineGrids_nobfe(&slrgrid, &elevgrid);
                coloring = COMBINE_COLOR;
                break;
            case WATER:
                waterGrid(&elevgrid);
                coloring = BINARY_COLOR;
                break;
            case WATER_SLR_ELEV:
                combineGrids_nobfe(&slrgrid, &elevgrid);
                coloring = COMBINE_WATER;
                break;
            case ORIG_BFE:
                setCurrGrid(&originterp_bfegrid);
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
                combineGrids_bfe(&slr_interp_bfegrid, &elevgrid);
                coloring = COMBINE_COLOR_BFE;
                break;
            case WATER_SLRINTERP_BFE_ELEV:
                combineGrids_bfe(&slr_interp_bfegrid, &elevgrid);
                coloring = COMBINE_WATER_BFE;
                break;
                
            case SLRINTERP_BFEMINUSSLR:
                diffGrids(&slr_interp_bfegrid, &slrgrid, NEW_WATER);
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
            case SLR_ELEV:
                combineGrids_nobfe(&slrgrid, &elevgrid);
                coloring = COMBINE_COLOR;
                break;
            case WATER:
                waterGrid(&elevgrid);
                coloring = BINARY_COLOR;
                break;
                
            case WATER_SLR_ELEV:
                combineGrids_nobfe(&slrgrid, &elevgrid);
                coloring = COMBINE_WATER;
                break;
            default:
                break;
        }
    }
    draw_grid(&currGrid, coloring);
    
    /* execute the drawing commands */
    glFlush();
}

void diffGrids(Grid* grid1, Grid* grid2, double diff) {
    assert(DRAW == SLRINTERP_BFEMINUSSLR);
    
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            
            double currRise = interp_bfegrid.data[i][j] + rise;
            double value1 = grid1->data[i][j];
            double value2 = grid2->data[i][j];
            if (value1 == elevgrid.NODATA_value) {
                currGrid.data[i][j] = elevgrid.NODATA_value;
            }else if (value1 == diff && value2 != diff) {
                currGrid.data[i][j] = value2-currRise;
            } else {
                currGrid.data[i][j] = 1;
            }
        }
    }
}


void waterGrid(Grid* grid) {
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            if(grid->data[i][j] == grid->NODATA_value) {
                currGrid.data[i][j] = 0;
            } else {
                currGrid.data[i][j] = 1;
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
            currGrid.data[i][j] = grid->data[i][j];
        }
    }
}

void combineGrids_nobfe(Grid* grid1, Grid* grid2) {
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            if (grid1->data[i][j] == elevgrid.NODATA_value) {
                currGrid.data[i][j] = elevgrid.NODATA_value;
                
            } else {
                currGrid.data[i][j] = grid2->data[i][j]- rise;
            }
        }
    }
}


void combineGrids_bfe(Grid* grid1, Grid* grid2) {
    assert(DRAW == SLRINTERP_BFE_ELEV || DRAW == WATER_SLRINTERP_BFE_ELEV);
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            if (grid1->data[i][j] == elevgrid.NODATA_value) {
                currGrid.data[i][j] = elevgrid.NODATA_value;
                
            } else {
                /*This point may or may not have a bfe value.
                 If we have a bfe value we add it to the rise, otherwise we assume the
                 bfe value is zero (ie just use the rise).*/
                
                if (interp_bfegrid.data[i][j] == elevgrid.NODATA_value) {
                    currGrid.data[i][j] = grid2->data[i][j]- rise;
                }else {
                    currGrid.data[i][j] = grid2->data[i][j] - (interp_bfegrid.data[i][j] + rise);
                }
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
    if (grid_type == COLOR) {
        draw_point_color(value);
    } else if (grid_type == BINARY_COLOR) {
        draw_point_binary(value);
    } else if (grid_type == BLACK_COLOR){
        draw_point_black(value);
    } else if (grid_type == COMBINE_COLOR){
        draw_point_combine(value,rise);
    } else if (grid_type == COMBINE_COLOR_BFE){
        draw_point_combine(value,rise + interp_bfegrid.data[(int)mypoint.x][(int)mypoint.y]);
    } else if(grid_type == COMBINE_WATER) {
        draw_point_combine_water(value,rise);
    } else {
        draw_point_combine_water(value,rise + interp_bfegrid.data[(int)mypoint.x][(int)mypoint.y]);
        
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
    double base;
    double thisMin = minLand;
    if (minLand < 0) {
        thisMin = 0;
    }
    base = (max-thisMin)/numCategories;
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

void draw_point_black(double value) {
    assert(minLand > 0);
    double base = (max-minLand)/numCategories;
    if (value == elevgrid.NODATA_value) {
        glColor3fv(lightblue);
    } else if (value < minLand+base) {
        glColor3fv(interpolate_colors(gray1, gray2,value,minLand,minLand+base));
    } else if (value < (minLand+2 * base)) {
        glColor3fv(interpolate_colors(gray2, gray3,value,minLand+base,minLand+2*base));
    } else if (value < (minLand+3 * base)) {
        glColor3fv(interpolate_colors(gray3, gray4,value,minLand+2*base,minLand+3*base));
    } else if (value < (minLand+4 * base)) {
        glColor3fv(interpolate_colors(gray4, gray5,value,minLand+3*base,minLand+4*base));
    } else if (value < (minLand+5 * base)) {
        glColor3fv(interpolate_colors(gray5, gray6,value,minLand+4*base,minLand+5*base));
    }  else {
        glColor3fv(interpolate_colors(gray6, black,value,minLand+5*base,minLand+6*base));
    }
}
void draw_point_binary(double value) {
    if (value == 0) {
        glColor3fv(blue);
    } else {
        glColor3fv(black);
    }
}

void draw_point_combine(double value, double theRise) {
    if (value > 0 || value == elevgrid.NODATA_value) {
        glColor3fv(black);
    } else {
        //assuming minimum (ie max negative number) is about zero
        value += -(minLandElev-theRise);
        double maxNum = 1+ -(minLandElev-theRise);
        double base = maxNum/numCategories;
        if (value < base) {
            glColor3fv(interpolate_colors(blue6, blue5,value,0,base));
        } else if (value < (2 * base)) {
            glColor3fv(interpolate_colors(blue5, blue4,value,base,2*base));
        } else if (value < (3 * base)) {
            glColor3fv(interpolate_colors(blue4, blue3,value,2*base,3*base));
        } else if (value < (4 * base)) {
            glColor3fv(interpolate_colors(blue3, blue2,value,3*base,4*base));
        } else if (value < (5 * base)) {
            glColor3fv(interpolate_colors(blue2, blue1,value,4*base,5*base));
        }  else {
            glColor3fv(interpolate_colors(blue1, lightblue,value,5*base,6*base));
        }
    }
}
void draw_point_combine_water(double value, double theRise) {
    if (value > 0){
        glColor3fv(black);
    }else if(value == elevgrid.NODATA_value) {
        glColor3fv(blue);
    } else {
        //assuming minimum (ie max negative number) is about zero
        value += -(minLandElev-theRise);
        double maxNum = 1+ -(minLandElev-theRise);
        double base = maxNum/numCategories;
        if (value < base) {
            glColor3fv(interpolate_colors(blue6, blue5,value,0,base));
        } else if (value < (2 * base)) {
            glColor3fv(interpolate_colors(blue5, blue4,value,base,2*base));
        } else if (value < (3 * base)) {
            glColor3fv(interpolate_colors(blue4, blue3,value,2*base,3*base));
        } else if (value < (4 * base)) {
            glColor3fv(interpolate_colors(blue3, blue2,value,3*base,4*base));
        } else if (value < (5 * base)) {
            glColor3fv(interpolate_colors(blue2, blue1,value,4*base,5*base));
        }  else {
            glColor3fv(interpolate_colors(blue1, lightblue,value,5*base,6*base));
        }
    }
}


GLfloat* interpolate_colors(GLfloat* lowerColor, GLfloat* upperColor,double value,double lowerBound,double upperBound) {
    static GLfloat newColor[3] = {-1,-1,-1};
    for(int i = 0; i < 3; i ++) {
        newColor[i] = (double)lowerColor[i] + ((value-lowerBound)/(upperBound-lowerBound))*(double)(upperColor[i]-lowerColor[i]);
    }
    return newColor;
    
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
//    assert(slr_interp_bfegrid.data);
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
//            draw_point_slr_slr_interp_bfe(newPoint, &slr_interp_bfegrid,&slrgrid, currRiseB);
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




