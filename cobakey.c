#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>              //uint64_t
#include <sys/socket.h>             //Socket related constants
#include <sys/un.h>                 //Unix domain constants
#include <netinet/in.h>             //Socket related constants
#include <termios.h>

int fbfd = 0;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
long int screensize = 0;
char *fbp = 0;
int x = 0, y = 0;
long int location = 0;

void init();
inline uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo *vinfo);
void draw(int x, int y, uint32_t pixel);
void draw_line(int x1, int y1, int x2, int y2, uint32_t pixel);
void clearscreen();
void shoot();

int main() {
    init();
	clearscreen();
	while(1){
		if (!kbhit()){
		
		}
		else {
			shoot();
			break;
		}
	}
	munmap(fbp, screensize);
    close(fbfd);
    return 0;
}

void shoot(){
    clearscreen();
    draw_line(100,500,700,20,pixel_color(100,0,0,&vinfo));
    usleep(10000);
    clearscreen();
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

void clearscreen(){
    int x,y;
    for (x=0; x<750; x++){
        for (y=0; y<500; y++){
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                           (y+vinfo.yoffset) * finfo.line_length;
            *(fbp + location) = 255;        // blue
            *(fbp + location + 1) = 248;      // green
            *(fbp + location + 2) = 224;      // red
            *(fbp + location + 3) = 0;      // transparency
        }
    }
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