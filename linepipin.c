#include "line.h"


long int location = 0;
unsigned char *fbp = 0;
struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
int idx = 0;
line currentLine;

void init(line l) {
	currentLine = l;
	idx=0;
}

line createLine(color col, int len) {
	line temp; int i;
	for (i=0;i<len;i++) {
		temp.col.R[i]=col.R[i];
	}
	for (i=0;i<len;i++) {
		temp.col.G[i]=col.G[i];
	}
	for (i=0;i<len;i++) {
		temp.col.B[i]=col.B[i];
	}
	for (i=0;i<len;i++) {
		temp.col.Alpha[i]=col.Alpha[i];
	}
	temp.length = ln;
	return temp;
}


void drawPixel(int x, int y, unsigned char R, unsigned char G, unsigned char B, unsigned char alpha){
    location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
               (y+vinfo.yoffset) * finfo.line_length;
    
    //INI GUE GA NGERTI, DAPET DARI INTERNET!!!!
    if (vinfo.bits_per_pixel == 32) {
    *(fbp + location) = (unsigned char) (B * alpha/255 + (*(fbp + location)) * (255 - alpha)/255);        // Some blue
    *(fbp + location + 1) = (unsigned char) (G * alpha/255 + (*(fbp + location + 1)) * (255 - alpha)/255);     //  green
    *(fbp + location + 2) = (unsigned char) (R * alpha/255 + (*(fbp + location + 2)) * (255 - alpha)/255);    //  red
    *(fbp + location + 3) = alpha+(*(fbp + location + 3))*(1-alpha)/255;      // alpha
    } else  { //assume 16bpp
        int b = B/8;
        int g = G/8;     // A little green
        int r = R/8;    // A lot of red
        unsigned short int t = r<<11 | g << 5 | b;
        *((unsigned short int*)(fbp + location)) = t;
    }
}

void plotting(int x, int y) {
	drawPixel(x,y,currentLine.col.R[idx],currentLine.col.G[idx],currentLine.col.B[idx],currentLine.col.Alpha[idx]);
	idx++;
	if (idx>=currentLine.length) {
		idx=0;
	}
}

void drawLine(int x1, int x2, int y1, int y2, line l) {
	init(l);
	int y, x;
	float dx, dy, m;
	if (x2 != x1) { //garis tdk vertikal
		//HELLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLPPPPP!!!
		if (x2 > x1) {
			dx = x2-x1; dy = y2-y1;
			m = dy/dx;
			y = y1;
			//HELLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLPPPPP!!!
		}
		else {

		}
			
	} else { //x1=x2, garis vertikal
		if (y2 > y1) {
			x = x1;
			for (y=y1; y<=y2; y++) {
				plotting(x,y);
			}
		} else {
			x = x1;
			for (y=y2; y<=y1; y++) {
				plotting(x,y);
			}
		}
	}
		
}