/* This files provides address values that exist in the system */

#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00

#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for animation */
#define BOX_LEN 2
#define NUM_BOXES 8

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Begin part1.s for Lab 7

volatile int pixel_buffer_start; // global variable

// private functions 

void swap(int * a, int * b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void plot_pixel(int x, int y, short int line_color)
{
    // check that pixel is in bounds
    if (x < 0 || x >= RESOLUTION_X || y < 0 || y >= RESOLUTION_Y) 
        return;
    else 
        *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void draw_line(int x0, int y0, int x1, int y1, short int color) 
{
    bool is_steep = abs(y1 - y0) > abs(x1 - x0);
    
    if (is_steep) 
    {
        swap(&x0, &y0);
        swap(&x1, &y1);
    } 
        
    if (x0 > x1) 
    {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }

    int deltax = x1 - x0;
    int deltay = abs(y1 - y0); 
    int error = -(deltax / 2);
    int y = y0;
    int y_step = (y0 < y1)? 1 : -1;
    
    for (int x = x0; x < x1; ++x)
    {
        is_steep ? plot_pixel(y, x, color) : plot_pixel(x, y, color);

        error = error + deltay;
        
        if (error > 0) 
        {
            y = y + y_step;
            error = error - deltax;
        }
    }    
}

void clear_screen() 
{
    for (int y = 0; y < RESOLUTION_Y; y++)
    {
        int x0 = 0, x1 = RESOLUTION_X ;
        draw_line(x0, y, x1, y, WHITE);
    }
}

void animate_line(volatile int * pixel_ctrl_ptr) {
    // config
    // set the back buffer to the same address as the front buffer
    *(int *)(pixel_ctrl_ptr + 0x1) = pixel_buffer_start;
    
    volatile int * status_reg = (int *)(pixel_ctrl_ptr + 0x3);

    int increment = 1;
    int y = 0;

    // draw the first line 
    draw_line(0, 0, RESOLUTION_X, 0, 0);

    while (true)
    {
        // check if the line is at the display boundary and change its direction
        if (y == 0)
            increment = 1;
        else if (y == RESOLUTION_Y - 1)
            increment = -1;
        
        while (*status_reg & 1); // wait until S bit of status_reg is 0

        // status reg is 0; display new line

        // clear old line by drawing a white line over it
        int x_min = 0, x_max = RESOLUTION_X - 1;
        
        draw_line(x_min, y, x_max, y, WHITE);
        // set S bit to 1
        *pixel_ctrl_ptr = 1;

        while (*status_reg & 1); // wait until S bit is 0
        
        y += increment; // increment y value
        
        draw_line(x_min, y, x_max, y, 0x0); // draw a new black line at the new row
        *pixel_ctrl_ptr = 1;
    }
}

// main program execution 

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;

    clear_screen();
    animate_line(pixel_ctrl_ptr);

    draw_line(0, 0, 150, 150, 0x001F);   // this line is blue
    draw_line(150, 150, 319, 0, 0x07E0); // this line is green
    draw_line(0, 239, 319, 239, 0xF800); // this line is red
    draw_line(319, 0, 0, 239, 0xF81F);   // this line is a pink color
}

// code not shown for clear_screen() and draw_line() subroutines