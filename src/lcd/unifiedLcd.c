#include "font.h"
#include "unifiedLcd.h"


lcd display;

void init_lcd(int clock_option) {
    /* Enable extended memory interface with 10 bit addressing */
    /* 8MHz clock, no prescaling (DS, p. 48) */
    if (clock_option==1)
    {
    CLKPR = (1 << CLKPCE);
    CLKPR = 0;
    }
    XMCRB = _BV(XMM2) | _BV(XMM1);
    XMCRA = _BV(SRE);
    DDRC |= _BV(RESET);
    DDRB |= _BV(BLC);
    _delay_ms(1);
    PORTC &= ~_BV(RESET);
    _delay_ms(20);
    PORTC |= _BV(RESET);
    _delay_ms(120);
    write_cmd(DISPLAY_OFF);
    write_cmd(SLEEP_OUT);
    _delay_ms(60);
    write_cmd_data(INTERNAL_IC_SETTING,          0x01);
    write_cmd(POWER_CONTROL_1);
        write_data16(0x2608);
    write_cmd_data(POWER_CONTROL_2,              0x10);
    write_cmd(VCOM_CONTROL_1);
        write_data16(0x353E);
    write_cmd_data(VCOM_CONTROL_2, 0xB5);
    write_cmd_data(INTERFACE_CONTROL, 0x01);
        write_data16(0x0000);
    write_cmd_data(PIXEL_FORMAT_SET, 0x55);     /* 16bit/pixel */
    set_orientation(West);
    clear_screen();
    display.x = 0;
    display.y = 0;
    display.background = BLACK;
    display.foreground = WHITE;
    write_cmd(DISPLAY_ON);
    _delay_ms(50);
    write_cmd_data(TEARING_EFFECT_LINE_ON, 0x00);
    EICRB |= _BV(ISC61);
    PORTB |= _BV(BLC);
}

void lcd_brightness(uint8_t i) {
    /* Configure Timer 2 Fast PWM Mode 3 */
    TCCR2A = _BV(COM2A1) | _BV(WGM21) | _BV(WGM20);
    TCCR2B = _BV(CS20);
    OCR2A = i;
}

void set_orientation(orientation o) {
    display.orient = o;
    write_cmd(MEMORY_ACCESS_CONTROL);
    if (o==North) {
        display.width = LCDWIDTH;
        display.height = LCDHEIGHT;
        write_data(0x48);
    }
    else if (o==West) {
        display.width = LCDHEIGHT;
        display.height = LCDWIDTH;
        write_data(0xE8);
    }
    else if (o==South) {
        display.width = LCDWIDTH;
        display.height = LCDHEIGHT;
        write_data(0x88);
    }
    else if (o==East) {
        display.width = LCDHEIGHT;
        display.height = LCDWIDTH;
        write_data(0x28);
    }
    write_cmd(COLUMN_ADDRESS_SET);
    write_data16(0);
    write_data16(display.width-1);
    write_cmd(PAGE_ADDRESS_SET);
    write_data16(0);
    write_data16(display.height-1);
}



void set_frame_rate_hz(uint8_t f) {
    uint8_t diva, rtna, period;
    if (f>118)
        f = 118;
    if (f<8)
        f = 8;
    if (f>60) {
        diva = 0x00;
    } else if (f>30) {
        diva = 0x01;
    } else if (f>15) {
        diva = 0x02;
    } else {
        diva = 0x03;
    }
    /*   !!! FIXME !!!  [KPZ-30.01.2015] */
    /*   Check whether this works for diva > 0  */
    /*   See ILI9341 datasheet, page 155  */
    period = 1920.0/f;
    rtna = period >> diva;
    write_cmd(FRAME_CONTROL_IN_NORMAL_MODE);
    write_data(diva);
    write_data(rtna);
}

void fill_rectangle(rectangle r, uint16_t col) {
    write_cmd(COLUMN_ADDRESS_SET);
    write_data16(r.left);
    write_data16(r.right);
    write_cmd(PAGE_ADDRESS_SET);
    write_data16(r.top);
    write_data16(r.bottom);
    write_cmd(MEMORY_WRITE);
/*  uint16_t x, y;
    for(x=r.left; x<=r.right; x++)
        for(y=r.top; y<=r.bottom; y++)
            write_data16(col);
*/
    uint16_t wpixels = r.right - r.left + 1;
    uint16_t hpixels = r.bottom - r.top + 1;
    uint8_t mod8, div8;
    uint16_t odm8, odd8;
    if (hpixels > wpixels) {
        mod8 = hpixels & 0x07;
        div8 = hpixels >> 3;
        odm8 = wpixels*mod8;
        odd8 = wpixels*div8;
    } else {
        mod8 = wpixels & 0x07;
        div8 = wpixels >> 3;
        odm8 = hpixels*mod8;
        odd8 = hpixels*div8;
    }
    uint8_t pix1 = odm8 & 0x07;
    while(pix1--)
        write_data16(col);

    uint16_t pix8 = odd8 + (odm8 >> 3);
    while(pix8--) {
        write_data16(col);
        write_data16(col);
        write_data16(col);
        write_data16(col);
        write_data16(col);
        write_data16(col);
        write_data16(col);
        write_data16(col);
    }
}

void fill_rectangle_indexed(rectangle r, uint16_t *col) {
    uint16_t x, y;
    write_cmd(COLUMN_ADDRESS_SET);
    write_data16(r.left);
    write_data16(r.right);
    write_cmd(PAGE_ADDRESS_SET);
    write_data16(r.top);
    write_data16(r.bottom);
    write_cmd(MEMORY_WRITE);
    for(x=r.left; x<=r.right; x++)
        for(y=r.top; y<=r.bottom; y++)
            write_data16(*col++);
}

void clear_screen() {
    display.x = 0;
    display.y = 0;
    rectangle r = {0, display.width-1, 0, display.height-1};
    fill_rectangle(r, display.background);
}

void display_char(char c) {
    uint16_t x, y;
    PGM_P fdata;
    uint8_t bits, mask;
    uint16_t sc=display.x, ec=display.x + 4, sp=display.y, ep=display.y + 7;

    /*   New line starts a new line, or if the end of the
         display has been reached, clears the display.
    */
    if (c == '\n') {
        display.x=0; display.y+=8;
        if (display.y >= display.height) { clear_screen(); }
        return;
    }

    if (c < 32 || c > 126) return;
    fdata = (c - ' ')*5 + font5x7;
    write_cmd(PAGE_ADDRESS_SET);
    write_data16(sp);
    write_data16(ep);
    for(x=sc; x<=ec; x++) {
        write_cmd(COLUMN_ADDRESS_SET);
        write_data16(x);
        write_data16(x);
        write_cmd(MEMORY_WRITE);
        bits = pgm_read_byte(fdata++);
        for(y=sp, mask=0x01; y<=ep; y++, mask<<=1)
            write_data16((bits & mask) ? display.foreground : display.background);
    }
    write_cmd(COLUMN_ADDRESS_SET);
    write_data16(x);
    write_data16(x);
    write_cmd(MEMORY_WRITE);
    for(y=sp; y<=ep; y++)
        write_data16(display.background);

    display.x += 6;
    if (display.x >= display.width) { display.x=0; display.y+=8; }
}

void display_string(char *str) {
    uint8_t i;
    for(i=0; str[i]; i++)
        display_char(str[i]);
}

void display_curser_move(uint16_t x, uint16_t y) {
    display.x = x;
    display.y = y;
}

void display_color(uint16_t fg, uint16_t bg) {
	display.foreground = fg;
	display.background = bg;

}

void display_string_xy(char *str, uint16_t x, uint16_t y) {
    uint8_t i;
    display.x = x;
    display.y = y;
    for(i=0; str[i]; i++)
        display_char(str[i]);
}

int16_t findBezier(double t, uint16_t x[4]) {
	double omt = 1-t;

	double xt = omt*omt*omt*x[0] + 3*omt*omt*t*x[1] +
				3*omt*t*t*x[2] +  t*t*t*x[3];

	return (int16_t) xt+0.5;
}

void plotBezier(uint16_t x[4], uint16_t y[4], uint16_t col, double segSize) {
	int16_t px0 = findBezier(0.0, x);
	int16_t py0 = findBezier(0.0, y);
	double t;
	for (t = segSize; t < 1.0+segSize; t+=segSize) {
		int16_t px1 = findBezier(t,x);
		int16_t py1 = findBezier(t,y);

		drawLine(px0,py0,px1, py1, col);
		px0 = px1;
		py0 = py1;
	}

}

void plotBezierBuffer(uint16_t x[4], uint16_t y[4], uint16_t col, double segSize) {
	int16_t px0 = findBezier(0.0, x);
	int16_t py0 = findBezier(0.0, y);
	double t = segSize;
	uint16_t pointNo = (uint16_t) ((1/segSize) + 1.5); //+1 for n+1 points for n lines  and +0.5 for round up
	int16_t* xbuff = (int16_t*) malloc(sizeof(int16_t) * pointNo);
	int16_t* ybuff = (int16_t*) malloc(sizeof(int16_t) * pointNo);
	ybuff[0] = py0;
	xbuff[0] = px0;
	uint16_t i;
	for (i = 1; i < pointNo; i++) {
		int16_t px1 = findBezier(t,x);
		int16_t py1 = findBezier(t,y);

		ybuff[i] = py1;
		xbuff[i] = px1;
		px0 = px1;
		py0 = py1;
		t+= segSize;
	}

	for (i = 0; i < pointNo-1; i++ ) {
		drawLine(xbuff[i],ybuff[i],xbuff[i+1], ybuff[i+1],col);
	}

	free(ybuff);
	free(xbuff);
}

double fraction(double val) {
	if (val<0) {
		return 1 - (val - floor(val));
	}

	return val - floor(val);

}

double remainingFraction(double val) {
	return 1 - fraction(val);
}

void drawAALine(double x0, double y0, double x1, double y1, uint16_t col) {
	double steep = fabs(y1 - y0) - fabs (x1 - x0);

	if (steep > 0) {
		swapDouble(&y0, &x0);
		swapDouble(&y1, &x1);
	}

	if (x0 > x1) {
		swapDouble(&x0, &x1);
		swapDouble(&y1, &y0);
	}

	double dx = x1 - x0;
	double dy = y1 - y0;

	double gradient = dy / dx;

	int16_t xend = (int16_t) (x0 +0.5);
	double yend = y0 + gradient * (xend-x0);
	double xgap = remainingFraction(x0 + 0.5);
	uint16_t xpxl1 = (uint16_t) xend;
	uint16_t ypxl1 = (uint16_t) yend;

	if(steep > 0) {
		plotPixel(ypxl1, xpxl1, shade(col,1+remainingFraction(yend)*xgap));
		plotPixel(ypxl1+1,xpxl1, shade(col,1+fraction(yend)*xgap));
	}
	else {
		plotPixel(xpxl1, ypxl1, shade(col,1+remainingFraction(yend)*xgap));
		plotPixel(xpxl1,ypxl1+1, shade(col,1+fraction(yend)*xgap));
	}

	double intery = yend + gradient;

	xend = (int16_t) (x1 +0.5);
    yend = y1 + gradient * (xend - x1);
    xgap = remainingFraction(x1 + 0.5);
    uint16_t xpxl2 = (uint16_t) xend;
    uint16_t ypxl2 = (uint16_t) yend;
    if (steep > 0) {
        plotPixel(ypxl2  , xpxl2, shade(col,remainingFraction(yend) * xgap));
        plotPixel(ypxl2+1, xpxl2, shade(col,fraction(yend) * xgap));
	}
    else {
        plotPixel(xpxl2, ypxl2,  shade(col,remainingFraction(yend) * xgap));
        plotPixel(xpxl2, ypxl2+1, shade(col,fraction(yend) * xgap));
	}

	uint16_t i;

	for(i = xpxl1+1; i<xpxl2;i++) {
		if(steep >0) {
			plotPixel((uint16_t) intery  , i, shade(col,remainingFraction(intery)));
            plotPixel((uint16_t) intery+1, i, shade(col,fraction(intery)));
		}
		else {
			plotPixel(i,(uint16_t) intery  , shade(col,remainingFraction(intery)));
            plotPixel(i,(uint16_t) intery+1, shade(col,fraction(intery)));
		}
		intery = intery + gradient;
	}

}

void swapDouble(double *x, double *y) {
	double z;
	z = *x;
	*x = *y;
	*y  = z;
}


void drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t col) {

	plotPixel((uint16_t) x1, (uint16_t) y1, col);

	uint16_t oct = findOctant(x1,y1,x2,y2);

	toOctant0(&x1, &y1, oct);
	toOctant0(&x2, &y2, oct);

	int16_t posx;
	int16_t posy = y1;
	int16_t diffx =  x2 -  x1;
	int16_t diffy =   y2 -  y1;
	int16_t twoDiffy = 2 *diffy;
	int32_t p = (int32_t) twoDiffy -  (int32_t) diffx;
	int16_t tempx;
	int16_t tempy;


	for (posx = x1+1; posx <= x2; posx++) {
		if (p < 0) {
			p = p + twoDiffy;
		}



		else {
			posy++;
			p = p + twoDiffy - 2*diffx;
		}

		tempx = posx;
		tempy = posy;

		switchFromOctant0(&tempx, &tempy, oct);

		plotPixel((uint16_t)tempx , (uint16_t) tempy, col);

	}
}

void switchFromOctant0(int16_t *x, int16_t *y, uint16_t octant) {
	switch(octant)  {
     case 0: break;
     case 1:
		swap(x,y);
		break;
     case 2:
		*y = -*y;
		swap(x,y);
		break;
     case 3:
		*x = -*x;
		break;
     case 4:
		*x = -*x;
		*y = -*y;
		break;
     case 5:
		*x = -*x;
		*y = -*y;
		swap(x,y);
		break;
     case 6:
		*x = -*x;
		swap(x,y);
		break;
     case 7:
		*y = -*y;
		break;
	}
}

void toOctant0(int16_t *x, int16_t *y, uint16_t octant) {
	switch(octant)  {
     case 0: break;
     case 1:
		swap(x,y);
		break;
     case 6:
		*y = -*y;
		swap(x,y);
		break;
     case 3:
		*x = -*x;
		break;
     case 4:
		*x = -*x;
		*y = -*y;
		break;
     case 5:
		*x = -*x;
		*y = -*y;
		swap(x,y);
		break;
     case 2:
		*x = -*x;
		swap(x,y);
		break;
     case 7:
		*y = -*y;
		break;
	}
}

void swap(int16_t *x, int16_t *y) {
	uint16_t z;
	z = *x;
	*x = *y;
	*y  = z;
}

uint16_t findOctant(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
	int16_t dy = y2 - y1;
	int16_t dx = x2 - x1;

	if (fabs(dy) > fabs(dx)) {
		if (dy < 0) {

			if (dx < 0) {
				return 5;
			}

			else {
				return 6;
			}
		}

		else {
			if (dx < 0) {
				return 2;
			}

			else {
				return 1;
			}
		}
	}

	else {
		if (dy < 0) {

			if (dx < 0) {
				return 4;
			}

			else {
				return 7;
			}
		}

		else {
			if (dx < 0) {
				return 3;
			}

			else {
				return 0;
			}
		}
	}
}



void plotPixel(uint16_t x, uint16_t y, uint16_t col) {

	write_cmd(COLUMN_ADDRESS_SET);
	write_data16(x);
	write_data16(x);
	write_cmd(PAGE_ADDRESS_SET);
	write_data16(y);
	write_data16(y);
	write_cmd(MEMORY_WRITE);
	write_data16(col);

}

void drawCircle(int x0, int y0, int radius, uint16_t col)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        plotPixel(x0 + x, y0 + y, col);
        plotPixel(x0 + y, y0 + x, col);
        plotPixel(x0 - y, y0 + x, col);
        plotPixel(x0 - x, y0 + y, col);
        plotPixel(x0 - x, y0 - y, col);
        plotPixel(x0 - y, y0 - x, col);
        plotPixel(x0 + y, y0 - x, col);
        plotPixel(x0 + x, y0 - y, col);

        if (err <= 0)
        {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

void auxCircle(uint16_t xc, uint16_t yc, uint16_t x, uint16_t y, uint16_t col) {
	plotPixel(xc+x, yc+y, col);
	plotPixel(xc-x, yc+y, col);
	plotPixel(xc+x, yc-y, col);
	plotPixel(xc-x, yc-y, col);
	plotPixel(xc+y, yc+x, col);
	plotPixel(xc-y, yc+x, col);
	plotPixel(xc+y, yc-x, col);
	plotPixel(xc-y, yc-x, col);
}
