#ifndef _VIDEO_GRAPHICS_H_
#define _VIDEO_GRAPHICS_H_

#include <dpmi.h>



/** @defgroup Video Video 
 * @{
 *
 * Entering/leaving/utility video functions
 */

/*
 * there are two global variables, declared (but not defined) here
 */
extern int HRES;  /**< global variable with the horizontal resolution in pixels */
extern int VRES;  /**< global variable with the vertical resolution in pixels */
extern int VMIN;
/** Enter graphics mode, enabling near pointers and mapping video physical memory
 * to program virtual address.
 *
 * Returns a generic pointer pointing to video memory address or NULL on error. 
 * "mode" specifies the VESA graphics mode to use, and
 * the mapping information is returned through "map".
 *
 * Also initializes two global variables, VRES and HRES,
 */
char * enter_graphics(int mode, __dpmi_meminfo *map);

/** Unmap video memory, disable near pointer and returns to text mode
 */
void leave_graphics(__dpmi_meminfo map);

/** Draws a pixel of color "color" at screen coordinates x, y at memory address "base"
 */
void set_pixel(int x, int y, int color, char *base);

/** Returns the pixel color at screen coordinates x, y at memory address "base"
 */
int get_pixel(int x, int y, char *base);

/** Set graphics memory at memory address "base" to "color".
 */
void clear_screen(char color, char *base);

/** Draw a line of color "color" between point (xi,yi) and (xf,yf) at memory address "base"
*/
void draw_line(int xi, int yi, int xf, int yf, int color, char *base);

void draw_circle(int xi, int yi, int radius, int color, char *base);

void print_text(char character,int xi, int yi, int color, char *base,int factor);
void create_board();
int get_vres();
/** @} end of video */ 

#endif
