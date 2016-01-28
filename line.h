#ifndef LINE_H
#define LINE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

typedef struct {
	//asumsi panjang max line 1500, ntar dicari tau
	unsigned char R[1500];
	unsigned char G[1500];
	unsigned char B[1500];
	unsigned char Alpha[1500];
}color;

typedef struct {
	color col;
	int length;
}line;

void init(line l);

void drawPixel(int x, int y, unsigned char R, unsigned char G, unsigned char B, unsigned char alpha);

line createLine(color col, int ln);

void plotting(int x, int y);

void drawLine(int x1, int x2, int y1, int y2, line l);

#endif