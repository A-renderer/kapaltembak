#include "frame.h"

void shoot(){
    clearscreen();
    draw_line(135,390,700,20,pixel_color(200,0,0,&vinfo));
    usleep(10000);
    clearscreen();
}

void draw_object(int size_x, int size_y, int object[size_x][size_y], int startX, int startY) {
	int pixel = 5;
    int x,y;
	for(y = startX; y < startX+size_x*pixel; y++) {
		for(x = startY; x < startY+size_y*pixel; x++) {
			if(object[(y-startX)/pixel][(x-startY)/pixel] == 0) {
				// warna langit
			}
			else if(object[(y-startX)/pixel][(x-startY)/pixel] == 1) {
				// warna hitam
				draw(x,y,pixel_color(0,0,0,&vinfo));
			}
			else if(object[(y-startX)/pixel][(x-startY)/pixel] == 2) {
				// warna biru
				draw(x,y,pixel_color(0,0,205,&vinfo));
			}
			else if(object[(y-startX)/pixel][(x-startY)/pixel] == 3) {
				// warna abu
				draw(x,y,pixel_color(128,128,128,&vinfo));
			}
			else if(object[(y-startX)/pixel][(x-startY)/pixel] == 4) {
				// warna merah
				draw(x,y,pixel_color(139,0,0,&vinfo));
			}
			else if(object[(y-startX)/pixel][(x-startY)/pixel] == 5) {
				// warna oren
				draw(x,y,pixel_color(255,140,0,&vinfo));
			}
			else if(object[(y-startX)/pixel][(x-startY)/pixel] == 6) {
				// warna kuning
				draw(x,y,pixel_color(255,255,0,&vinfo));
			}
		}
	}
}

void removePlane(int size_x, int size_y, int startX, int startY) {
	int x, y;
    for (x=startX; x<startX+size_x; x++){
        for (y=startY; y<startY+size_y; y++){
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                           (y+vinfo.yoffset) * finfo.line_length;
            *(fbp + location) = 255;        // blue
            *(fbp + location + 1) = 248;      // green
            *(fbp + location + 2) = 224;      // red
            *(fbp + location + 3) = 0;      // transparency
        }
        for (y=450; y<500; y++){
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                           (y+vinfo.yoffset) * finfo.line_length;
            *(fbp + location) = 255;        // blue
            *(fbp + location + 1) = 144;      // green
            *(fbp + location + 2) = 30;      // red
            *(fbp + location + 3) = 0;      // transparency
        }
    }
}

int isExploded(int y, int size_y) {
	return (y<550 && y>550-size_y) ? 1:0; //ntar kondisinya ditambah khbit
}

void drawPlane() {
	int size_x = 5, size_y=15, coor_y=10;
	bool isRight=true;
	while (isExploded(coor_y,size_y)==0) {
		if (isRight) {
			draw_object(size_x, size_y, plane_right,10,coor_y);
			usleep(0.0001);
			removePlane(size_x, size_y, 10, coor_y);
			coor_y+=5;
		} else {
			draw_object(size_x, size_y, plane_left,10,coor_y);
			usleep(0.0001);
			removePlane(size_x, size_y, 10, coor_y);
			coor_y-=5;
		}
	}
	
	//draw objek explosion kalo uda bener
}
