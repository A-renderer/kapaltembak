#include "frame.h"

bool isRight = true;
int coor_y = 10;
int coor_x = 10;
int size_y = 24;

int main() {
    init();
    clearscreen();
	isShoot = 0;
    pthread_t plane;
    pthread_create(&plane,NULL,threadHandler,NULL);
	degree = 0;
	int c;
	while(1){
		key = kbhit();
		if (!key){
		
		}
		else {
			c = getchar();
			if(c == 'd') {
				if (degree <= 87)
					degree +=1;
				target(degree);
			}
			else if(c == 'a') {
				if (degree >= 1)
					degree -=1;
				target(degree);
			}
			else if(c == '\n'){
				isShoot = shoot();
			}
				key = 0;
		}
	}
    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}

int isExploded() {
	int x2 = 20;
	int y2 = calculateX(135+degree, calculateY(135+degree));
	
	if ((isShoot == 1) && (y2 >= coor_y && y2 <=coor_y+120))
		return 1;
	else {
		isShoot = 0;
		return 0;
	}
    /*int x2 = 20; // harusnya ini titik tembakan
    int y2 = calculateX(135+degree, calculateY(135+degree)); // harusnya ini titik tembakan
    if((coor_y >= y2 && coor_y <=y2+120) && (coor_x >= x2 && coor_x <= x2+70)) return 1;
    else return 0;
	*/
}

void* threadHandler(void *threadArgs) {
    while (isExploded()==0) { // harusnya ini kondisi tertembak
        coor_y = drawPlane(coor_y,isRight);
        if(coor_y > (750-size_y*5)-10 && isRight) {
            isRight = false;
        }
        else if(coor_y < 10 && !isRight) {
            isRight = true;
        }
    }
	coor_y = drawPlane(coor_y,isRight);
	usleep(1);
    draw_object(13,14,explosion,coor_x,coor_y);
    usleep(100000);
    return NULL;
}

void init() {

    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
    }

    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");
}

inline uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo *vinfo)
{
    return (r<<vinfo->red.offset) | (g<<vinfo->green.offset) | (b<<vinfo->blue.offset);
}

void draw(int x, int y, uint32_t pixel){
    location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
        (y+vinfo.yoffset) * finfo.line_length;
    *((uint32_t*)(fbp + location)) = pixel;
}

void draw_line(int x1, int y1, int x2, int y2, uint32_t pixel)
{
    int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

    dx=x2-x1;           //Delta x
    dy=y2-y1;           //Delta y
    dxabs=abs(dx);      //Absolute delta
    dyabs=abs(dy);      //Absolute delta
    sdx=(dx>0)?1:-1; //signum function
    sdy=(dy>0)?1:-1; //signum function
    x=dyabs>>1;
    y=dxabs>>1;
    px=x1;
    py=y1;

    if (dxabs>=dyabs)
    {
        for(i=0;i<dxabs;i++)
        {
            y+=dyabs;
            if (y>=dxabs)
            {
                y-=dxabs;
                py+=sdy;
            }
            px+=sdx;
            draw(px,py,pixel);
            draw(px+1,py,pixel);
            draw(px+1,py+1,pixel);
            draw(px,py+1,pixel);
            draw(px-1,py,pixel);
            draw(px-1,py-1,pixel);
            draw(px,py-1,pixel);
            usleep(0.0001);
        }
    }
    else
    {
        for(i=0;i<dyabs;i++)
        {
            x+=dxabs;
            if (x>=dyabs)
            {
                x-=dyabs;
                px+=sdx;
            }
            py+=sdy;
            draw(px,py,pixel);
            draw(px+1,py,pixel);
            draw(px+1,py+1,pixel);
            draw(px,py+1,pixel);
            draw(px-1,py,pixel);
            draw(px-1,py-1,pixel);
            draw(px,py-1,pixel);
            usleep(0.0001);
        }
    }
}


void directDraw(int x1, int y1, int x2, int y2, uint32_t pixel)
{
    int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

    dx=x2-x1;           //Delta x
    dy=y2-y1;           //Delta y
    dxabs=abs(dx);      //Absolute delta
    dyabs=abs(dy);      //Absolute delta
    sdx=(dx>0)?1:-1; //signum function
    sdy=(dy>0)?1:-1; //signum function
    x=dyabs>>1;
    y=dxabs>>1;
    px=x1;
    py=y1;

    if (dxabs>=dyabs)
    {
        for(i=0;i<dxabs;i++)
        {
            y+=dyabs;
            if (y>=dxabs)
            {
                y-=dxabs;
                py+=sdy;
            }
            px+=sdx;
            draw(px,py,pixel);
            draw(px+1,py,pixel);
            draw(px+1,py+1,pixel);
            draw(px,py+1,pixel);
            draw(px-1,py,pixel);
            draw(px-1,py-1,pixel);
            draw(px,py-1,pixel);
        }
    }
    else
    {
        for(i=0;i<dyabs;i++)
        {
            x+=dxabs;
            if (x>=dyabs)
            {
                x-=dyabs;
                px+=sdx;
            }
            py+=sdy;
            draw(px,py,pixel);
            draw(px+1,py,pixel);
            draw(px+1,py+1,pixel);
            draw(px,py+1,pixel);
            draw(px-1,py,pixel);
            draw(px-1,py-1,pixel);
            draw(px,py-1,pixel);
        }
    }
}


void clearscreen(){
    int x,y;
    for (x=0; x<750; x++){
        for (y=0; y<450; y++){
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
    draw_object(14,38,ship,379,7);
    if (isRight)
        draw_object(14, size_y, plane_right, 10, coor_y);
    else 
        draw_object(14, size_y, plane_left, 10, coor_y);
}

void cleararea(int startX, int startY, int finishX, int finishY){
    int x,y;
    for (x=startX; x<finishX; x++){
        for (y=startY; y<finishY; y++){
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                           (y+vinfo.yoffset) * finfo.line_length;
            *(fbp + location) = 255;        // blue
            *(fbp + location + 1) = 248;      // green
            *(fbp + location + 2) = 224;      // red
            *(fbp + location + 3) = 0;      // transparency
        }
    }
}

int calculateY(int x){
		return (390 - (int)(sqrt(8100-((x-135)*(x-135))))); 
}

void target(int degree){
    cleararea(133,298,227,389);
	int yPos = calculateY(135+degree);
	if (degree >= 1 || degree <=87)
		directDraw(135,390,135+degree,yPos,pixel_color(0,200,0,&vinfo));
}

int calculateX(int x, int y){
	int xPos = (int)((float)((float)((20-y)/(390-y))*(135-x))+x);
	if (xPos <= 750)
		return xPos;
	else
		return 750;
}

int calculateIfXMax(int x, int y){
	int yPos = (int)((float)((float)((750-x)/(135-x))*(390-y))+y);
	return yPos;
}


int shoot(){
    cleararea(133,298,227,392);
	int xPos = calculateX(135+degree, calculateY(135+degree));
	int yPos;
	if (xPos == 750){
		yPos = calculateIfXMax(135+degree, calculateY(135+degree));
	}
	else {
		yPos = 20;
	}
    draw_line(135,390,xPos,yPos,pixel_color(200,0,0,&vinfo));
    usleep(10000);
    clearscreen();
	target(degree);
	return 1;
}

void removeshoot(){
    draw_line(135,390,calculateX(135+degree, calculateY(135+degree)),20,pixel_color(224,248,255,&vinfo));
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
                draw(x,y,pixel_color(223,223,223,&vinfo));
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
			else if(object[(y-startX)/pixel][(x-startY)/pixel] == 7) {
                // warna putih
                draw(x,y,pixel_color(255,191,191,&vinfo));
            }
        }
    }
}

void removePlane(int size_x, int size_y, int startX, int startY) {
    int pixel = 5;
    int x, y;
    for(y = startX; y < startX+size_x*pixel; y++) {
        for(x = startY; x < startY+size_y*pixel; x++) {
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                           (y+vinfo.yoffset) * finfo.line_length;
            *(fbp + location) = 255;        // blue
            *(fbp + location + 1) = 248;      // green
            *(fbp + location + 2) = 224;      // red
            *(fbp + location + 3) = 0;      // transparency
        }
    }
}

int drawPlane(int coor_y, bool isRight) {
    int size_x = 14, size_y=24;
    if (isRight) {
        draw_object(size_x, size_y, plane_right, 10, coor_y);
        usleep(1000);
        removePlane(size_x, size_y, 10, coor_y);
        coor_y+=1;
        
    } else {
        draw_object(size_x, size_y, plane_left,10,coor_y);
        usleep(1000);
        removePlane(size_x, size_y, 10, coor_y);
        coor_y-=1;
    }
    return coor_y;
}

int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;
 
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  ch = getchar();
 
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }
 
  return 0;
}

