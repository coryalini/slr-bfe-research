//
//  drawing.hpp
//  slr
//
//  Created by Cory Alini on 6/4/17.
//  Copyright © 2017 gis.coryalini. All rights reserved.
//

#ifndef drawing_hpp
#define drawing_hpp
#include "bridge.hpp"


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

enum {slr = 0, bfe = 1, slrbfe = 2, elev = 3, slr_elev = 4,slrbfe_elev = 5, slrbfeminusslr = 6, slrbfe_slr =7, origbfe = 8};

static float numCategories = 6.0;


void draw_point_slr(double value);
void draw_point_bfe(double value);
void draw_point_combo(point mypoint, Grid* main, Grid* subtract,  float currRise);
void draw_point_diff(point mypoint, Grid* main, Grid* subtract,  float currRise);
void draw_point_slr_slr_bfe(point mypoint, Grid* main, Grid* subtract, float currRise);


void draw_grid(Grid* grid,int grid_type);
void draw_combo(Grid* main, Grid* subtract);
void draw_difference(Grid* main, Grid* subtract);
void draw_slr_slr_bfe();

#endif /* drawing_hpp */



