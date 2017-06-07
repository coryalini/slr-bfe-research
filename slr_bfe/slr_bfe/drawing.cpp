//
//  drawing.cpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#include "drawing.hpp"

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

void general_draw_point(point mypoint, Grid* grid,int grid_type) {
    double value = grid->data[(int)mypoint.x][(int)mypoint.y];
    switch (grid_type) {
        case slr:
        case elev:
        case slrbfe:
            draw_point_slr(value);
            break;
        case bfe:
        case origbfe:
            draw_point_bfe(value);
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

void draw_point_slr(double value) {
    double base = max/numCategories;

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
}



void draw_point_bfe(double value) {
    
    double base = 30.0/numCategories;
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
}

void draw_point_combo(point mypoint, Grid* main, Grid* subtract,  float currRise) {
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

void draw_point_diff(point mypoint, Grid* main, Grid* subtract, float currRise) {
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

void draw_combo(Grid* main, Grid* subtract) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(slrgrid.data);
    findMaxMin(&slrgrid);
    
    float currRise = 0;
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            point newPoint;
            newPoint.x = i;
            newPoint.y = j;
            if (DRAW == 5) {
                currRise = bfegrid.data[i][j];
            }
            draw_point_combo(newPoint, main,subtract, currRise);
        }
    }
}
void draw_difference(Grid* main, Grid* subtract) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(main->data);
    assert(subtract->data);
    findMaxMin(&slrgrid);
    
    float currRiseB = 0;
    
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            point newPoint;
            newPoint.x = i;
            newPoint.y = j;
            currRiseB = bfegrid.data[i][j] - rise;
            
            draw_point_diff(newPoint, main,subtract,currRiseB);
        }
    }
}


void draw_slr_slr_bfe() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(bfeslrgrid.data);
    assert(slrgrid.data);
    findMaxMin(&slrgrid);
    
    float currRiseB = 0;
    
    for (int i = 0; i < elevgrid.nrows; i++) {
        for (int j = 0; j < elevgrid.ncols; j++) {
            point newPoint;
            newPoint.x = i;
            newPoint.y = j;
            currRiseB = bfegrid.data[i][j] - rise;
            draw_point_slr_slr_bfe(newPoint, &bfeslrgrid,&slrgrid, currRiseB);
        }
    }
}
void draw_point_slr_slr_bfe(point mypoint, Grid* main, Grid* subtract, float currRise) {
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
        double base = 30.0/numCategories;
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
        double base = max/numCategories;
        
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




