#include "frame.h"

bool isRight = true;
int coor_y = 10;
int size_y = 24;

int main() {
    init();
    clearscreen();
    while(1){
        key = kbhit();
        if (!key){
            coor_y = drawPlane(coor_y,isRight);
            if(coor_y > (750-size_y*5)-10 && isRight) {
                isRight = false;
            }
            else if(coor_y < 10 && !isRight) {
                isRight = true;
            }
        }
        else {
            shoot();
            key = 0;
            getchar();
        }
    }
    munmap(fbp, screensize);
    close(fbfd);
    return 0;
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
    draw_object(14,38,ship,380,10);
    if (isRight)
        draw_object(14, size_y, plane_right, 10, coor_y);
    else 
        draw_object(14, size_y, plane_left, 10, coor_y);
}

void shoot(){
    clearscreen();
    draw_line(135,390,700,20,pixel_color(200,0,0,&vinfo));
    usleep(10000);
    clearscreen();
}

void removeshoot(){
    draw_line(135,390,700,20,pixel_color(224,248,255,&vinfo));
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

int isExploded(int y, int size_y) {
    return (y<550 && y>550-size_y) ? 1:0; //ntar kondisinya ditambah khbit
}

int isCorner(int y, int size_y) {
    return (y>550-size_y) ? 1:0; //ntar kondisinya ditambah khbit
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

