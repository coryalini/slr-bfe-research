//
//  drawing.hpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#ifndef drawing_hpp
#define drawing_hpp
#include "grid.hpp"


#ifdef __APPLE__
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif


#include <iostream>
#include <assert.h>


extern GLfloat red_pink[3];
extern GLfloat black[3];

//GLfloat blue[3] = {0.204, 0.533, 1};
//GLfloat lightblue[3] = {0.25, 0.8, 1.0};
//GLfloat green1[3] = {0.475, 0.925, 0.06};
//GLfloat green2[3] = {0.08, 0.611, 0.219};
//GLfloat darkyellow[3] = {0.611, 0.611, 0.08};
//GLfloat darkorange[3] = {0.611,0.349,0.08};
//GLfloat red1[3] = {0.611,0.08,0.08};
//GLfloat red2[3] = {0.262,0.10,0.039};


extern GLfloat blue[3];
extern GLfloat lightblue[3];
extern GLfloat green1[3];
extern GLfloat green2[3];

extern GLfloat darkyellow[3];
extern GLfloat darkorange[3];
//GLfloat red1[3] = {0.271,0.314,0.231};

extern GLfloat red1[3];
extern GLfloat red2[3];


extern GLfloat gray1[3];
extern GLfloat gray2[3];
extern GLfloat gray3[3];
extern GLfloat gray4[3];
extern GLfloat gray5[3];
extern GLfloat gray6[3];

enum {ELEV = 0, SLR = 1, SLR_ELEV = 2,SLR_GRAY =3, WATER = 4, WATER_SLR_ELEV = 5, ORIG_BFE = 6, INTERP_BFE = 7,SLRINTERP_BFE = 8, SLRINTERP_BFE_ELEV = 9,WATER_SLRINTERP_BFE_ELEV = 10,SLRINTERP_BFEMINUSSLR = 11};
enum {COLOR = 0, BLACK_COLOR = 1, BINARY_COLOR = 2, COMBINE_COLOR = 3,COMBINE_COLOR_BFE = 4, COMBINE_WATER = 5, COMBINE_WATER_BFE = 6, GRAY_BLUE= 7};

static float numCategories = 6.0;

void waterGrid(Grid* grid);
void setCurrGrid(Grid* grid);
void combineGrids_nobfe(Grid* grid1, Grid* grid2);
void combineGrids_bfe(Grid* grid1, Grid* grid2);
void diffGrids(Grid* grid1, Grid* grid2, double diff);


void draw_grid(Grid* grid,int grid_type);
void general_draw_point(point mypoint, Grid* grid,int grid_type);

void draw_point_color(double value);
void draw_point_black(double value);
void draw_point_binary(double value);
void draw_point_combine(double value, double theRise);
void draw_point_combine_water(double value, double theRise);
void draw_point_see_slr_better(double value);



GLfloat* interpolate_colors(GLfloat* lowerColor, GLfloat* upperColor,double value,double lowerBound,double upperBound);

//void draw_point_combo(point mypoint, Grid* main, Grid* subtract,  float currRise);
//void draw_point_diff(point mypoint, Grid* main, Grid* subtract,  float currRise);
//void draw_point_slr_slr_interp_bfe(point mypoint, Grid* main, Grid* subtract, float currRise);

//void draw_combo(Grid* main, Grid* subtract);
//void draw_difference(Grid* main, Grid* subtract);
//void draw_slr_slr_interp_bfe();

#endif /* drawing_hpp */



