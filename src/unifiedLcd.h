#ifndef unifiedLcd_h
#define unifiedLcd_h
#include <avr/interrupt.h>
#include <avr/io.h>
#include <math.h>
#include <stdint.h>
#include <util/delay.h>
#include "ili934x.h"
#include "unifiedColor.h"




#define LCDWIDTH	240
#define LCDHEIGHT	320

typedef enum {North, West, South, East} orientation;

typedef struct {
	uint16_t width, height;
	orientation orient;
	uint16_t x, y;
	uint16_t foreground, background;
} lcd;

extern lcd display;

typedef struct {
	uint16_t left, right;
	uint16_t top, bottom;
} rectangle;

void init_lcd(int clock_option);
void lcd_brightness(uint8_t i);
void set_orientation(orientation o);
void set_frame_rate_hz(uint8_t f);
void clear_screen();
void fill_rectangle(rectangle r, uint16_t col);
void fill_rectangle_indexed(rectangle r, uint16_t* col);
void display_char(char c);
void display_string(char *str);
void display_string_xy(char *str, uint16_t x, uint16_t y);
void display_curser_move(uint16_t x, uint16_t y);
void display_color(uint16_t fg, uint16_t bg);




 /*{Small Graphics library for the LaFortuna}
    Copyright (C) {2016}  {Nicholas Bishop}

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

/*The bezier library is a library for drawing simple curves. This library is an implementation of cubic bezier curves, please note that b-splines are not
supported.*/

/* A library for drawing anti-aliased lines using wu's algorithm */

/* The bresenham library provides an efficient way of drawing lines using the bresenham algorithm. The bresenham library
also provides an efficient function for drawing circles, based off the midpoint circle algorithm. */



/* Auxillary function for drawing bezier curves */
int16_t findBezier(double t, uint16_t x[4]);

/* A function for drawing cubic bezier curves.

Parameters:
x - An array of x coordinates needed to build the curve. x[0] represents the x coordinate of the start point of the line. x[1] represents the
x coordinate of the first control point. x[2] represents the x coordinate of the second control point. x[3] represents the x coordinate
of the end point of the line.
y - The same as above but for the y coordinates of each point.
col - The color of the curve in svgrgb565 format
segSize - A value between 0 and 1 that effects the number of straight lines used to build the curve. The closer segSize is to 0 the more, straight
lines will be used to build the curve, generally resulting in a smoother effect, but bear in mind this comes at computational cost. The smaller
the value of segSize the faster the curve will be drawn, but the more "jagged" it will be.
*/

void plotBezier(uint16_t x[4], uint16_t y[4], uint16_t col, double segSize);

/* The same function as plotBezier, except that points for each line segment are stored inside a buffer until all points have been
generated. All line segments are not drawn on the screen until all line segment points have been calculated. This means there may be a small delay
between the curve appearing on the screen and the function being called, but the curve will display all at once instead of line segment by line segment
like when plotBezier is used. This has memory implications and requires 4*(1/segSize) bytes of memory that plotBezier does not need */
void plotBezierBuffer(uint16_t x[4], uint16_t y[4], uint16_t col, double segSize);


/* auxillary functions for drawAAline */
double fraction(double val);
double remainingFraction(double val);
void swapDouble(double *x, double *y);

/* This function is used for drawing anti-aliased lines. Lines are drawn using Xiaolin Wu's algorithm so take slightly longer to load than lines drawn
using bresenham's algorithm.

Parameters:
x0 - The  coordinate of the start point of the line
y0 - The y coordinate of the start point of the line
x1 - The x coordinate of the end point of the line
y1 - The y coordinate of the end point of the line
col - The color of the line in svgrgb565 format*/
void drawAALine(double x0, double y0, double x1, double y1, uint16_t col);


/* Draws a straight line on the screen using bresenham's line algorithm

Parameters:
x1 - The  coordinate of the start point of the line
y1 - The y coordinate of the start point of the line
x2 - The x coordinate of the end point of the line
y2 - The y coordinate of the end point of the line
col - The color of the line in svgrgb565 format
*/
void drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t col);

/* Colors a pixel on the screen at the specified x and y coordinates with the specified color (in svgrgb565 format)

Parameters:
x - The  coordinate of the pixel
y - The y coordinate of the spixel
col - The new color of the pixel in svgrgb565 format
*/
void plotPixel(uint16_t x, uint16_t y, uint16_t col);

/*auxillary functions for drawing lines */
uint16_t findOctant(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void swap(int16_t *x, int16_t *y);
void switchFromOctant0(int16_t *x, int16_t *y, uint16_t octant);
void toOctant0(int16_t *x, int16_t *y, uint16_t octant);

/*Draws a circle on the screen using the midpoint circle algorithm

Parameters:
xc - The x coordinate of the centre of the circle
yc - The y coordinate of the centre of the circle
r - The radius of the circle
col - The color of the circle in svgrgb565 format
*/
void drawCircle(int xc, int yc, int r, uint16_t col);

/* auxillary function for drawing circles */
void auxCircle(uint16_t xc, uint16_t yc, uint16_t x, uint16_t y, uint16_t col);
#endif
