#include <sys/nearptr.h>
#include <go32.h>
#include "video-graphics.h"
#include "utypes.h"
#include <stddef.h>
#include <dpmi.h>
#include <math.h>
#include <conio.h>
#include <stdlib.h>

int VRES=0, HRES=0;
int VMIN=0;


/** Enter graphics mode, enabling near pointers and mapping video physical memory
 * to program virtual address.
 *
 * Returns a generic pointer pointing to video memory address or NULL on error. 
 * "mode" specifies the VESA graphics mode to use, and
 * the mapping information is returned through "map".
 *
 * Also initializes two global variables, VRES and HRES,
 */
 
 unsigned long get_address(int mode){
     __dpmi_regs regs;
	unsigned long dosbuf, address=0;

	dosbuf = __tb & 0xFFFFF; /* dosbuf é a área de transferência */
	dosmemput(&address, 4, dosbuf+40); /*apaga bytes 40 a 43 dessa área*/

	regs.x.ax = 0x4F01; /* VBE get mode info */
	regs.x.di = dosbuf & 0xF; /* LSB da área de transferência */
	regs.x.es = (dosbuf >> 4) & 0xFFFF; /*MSB da área de transferência*/
	regs.x.cx = mode; /*informação sobre o modo 105h*/
	__dpmi_int(0x10, &regs); /* DOS int 10h */
	if (regs.h.ah) {
	return 0;
	 } /* tratamento de erro, modo não suportado */

	dosmemget(dosbuf+40, 4, &address); /* lê o endereço da memória vídeo */
	if (!address) {
	return 0;
	} /* tratamento de erro, modo não suportado */
	
	return address;
 
 }
 
char * enter_graphics(int mode, __dpmi_meminfo *map) {

	__dpmi_regs regs; /*__dpmi_regs é uma estrutura que representa os registos do CPU */
	regs.x.ax = 0x4F02; /* registo AX do CPU com valor 0x4f02 */
	regs.x.bx = mode | 1<<14; /* registo BX do CPU com valor mode com o bit 14 a 1*/
	__dpmi_int(0x10, &regs);
	
	switch(mode){
		case 0x101: HRES=640; VRES=480; break;
		case 0x103: HRES=800; VRES=600; break;
		case 0x105: HRES=1024; VRES=768; break;
		case 0x107: HRES=1280; VRES=1024; break;
			
	}
	VMIN=VRES-10;
	
	__djgpp_nearptr_enable(); /* permite acesso à memória física usando apontadores */

	unsigned long address= get_address(mode);
	if(!address){
		return NULL;
	}
	/* endereço físico da memória vídeo, usar o valor correcto */
	map->address = address;

	/* tamanho do bloco de memória, usar apenas o necessário */
	map->size = VRES*HRES;
	__dpmi_physical_address_mapping(map);
	
	return (char *)(map->address + __djgpp_conventional_base);
}

/** Unmap video memory, disable near pointer and returns to text mode
 */
void leave_graphics(__dpmi_meminfo map) {
	__dpmi_free_physical_address_mapping(&map); /* liberta mapeamento */
	__djgpp_nearptr_disable();
	__dpmi_regs regs;
	regs.x.ax = 0x0003; /* registo AX do CPU com valor 0x03 */
	__dpmi_int(0x10, &regs); /* gera interrupção software 0x10, entrando no modo texto */


}

/** Draws a pixel of color "color" at screen coordinates x, y at memory address "base"
 */
void set_pixel(int x, int y, int color, char *base) {

	if(x>=0 && x<HRES && y>=0 && y<VRES){

	*(base + HRES*y + x)=color;
	
}
}

/** Returns the pixel color at screen coordinates x, y at memory address "base"
 */
int get_pixel(int x, int y, char *base) {
	if(x>0 && x<HRES && y>0 && y<VRES){
		return *(base + HRES*y + x);
	}
return -1;
}

/** Set graphics memory at memory address "base" to "color".
 */
void clear_screen(char color, char *base) {
	int x=0, y=0;
	while(y<=VRES){
		while(x<=HRES){
			set_pixel(x,y,color, base);
			++x;
		}
		++y;
		x=0;
	}
}

/** Draw a line of color "color" between point (xi,yi) and (xf,yf) at memory address "base"
*/
void draw_line(int xi, int yi, int xf, int yf, int color, char *base) {
	int difx=0, dify=0;
	int passos=0;
	difx=abs(xf-xi);
	dify=abs(yf-yi);
	if(difx==0 || dify==0){
	if(difx==0){
	while(yi<=yf){
		set_pixel(xi,yi,color,base);
		++yi;
	}
	}
	
	if(dify==0){
	while(xi<=xf){
		set_pixel(xi,yi,color,base);
		++xi;
	}
	}}
	else{
	/*
		int m=dify/difx;
		while(xi<xf){
		yi=m*xi;
		set_pixel(xi,yi,color,base);
		++xi;
		}
		*/
		if(difx>dify){ 
			passos=difx/dify;
			if(difx%dify>0.5){
			passos++;
			}
			int temp=0;
			while(xi<xf || yi<yf){
				set_pixel(xi,yi,color,base);
				if(temp==passos){
					xi++;
					temp=0;
				}
				yi++;
				temp++;
				
			}
		}
		else{
			passos=dify/difx;
			if(dify%difx>0.5){
			passos++;
			}
			int temp=0;
			while(xi<xf || yi<yf){
				set_pixel(xi,yi,color,base);
				if(temp==passos){
					yi++;
					temp=0;
				}
				xi++;
				temp++;
				
			}
		}
	}
	
}


void draw_circle(int xi, int yi, int radius, int color, char *base){
        //int xf=xi+radius, yb=yi;
        double xd=xi-radius,yd=yi-radius;
        while(xd<=xi+radius){
                while(yd<=yi+radius){
                        if((pow(xd-xi,2)+pow(yd-yi,2))<=pow((double) radius,2)){
                                set_pixel((int) xd,(int) yd,color,base);
                        }
                        yd++;
                }
                xd++;
                yd=yi-radius;
       }


}

void print_text(char character,int xi, int yi, int color, char *base,int factor){

	switch(character){
		case 'a': 
			draw_line(xi,yi,xi,yi+7*factor,color,base); //lado esquerdo
			draw_line(xi,yi,xi+5*factor,yi,color,base); //cima
			draw_line(xi+5*factor,yi,xi+5*factor,yi+7*factor,color,base); //lado direito
			draw_line(xi,yi+7*factor/2,xi+5*factor,yi+7*factor/2,color,base); //meio
			break;
		case 'b':
			draw_line(xi,yi,xi,yi+7*factor,color,base); //lado esquerdo
			draw_line(xi,yi,xi+5*factor,yi,color,base); //cima
			draw_line(xi,yi+7*factor,xi+5*factor,yi+7*factor,color,base); //baixo
			draw_line(xi+5*factor,yi,xi+5*factor,yi+7*factor,color,base); //lado direito
			draw_line(xi,yi+7*factor/2,xi+5*factor,yi+7*factor/2,color,base); //meio
			break;
		
		case 'c':
						draw_line(xi,yi,xi,yi+7*factor,color,base); //lado esquerdo
			draw_line(xi,yi,xi+5*factor,yi,color,base); //cima
			draw_line(xi,yi+7*factor,xi+5*factor,yi+7*factor,color,base); //baixo
			break;
		
		case 'd':	
			draw_line(xi+2*factor,yi,xi+2*factor,yi+7*factor,color,base); //vertical meio
			draw_line(xi,yi,xi+5*factor,yi,color,base); //cima
			draw_line(xi,yi+7*factor,xi+5*factor,yi+7*factor,color,base); //baixo
			draw_line(xi+5*factor,yi,xi+5*factor,yi+7*factor,color,base); //lado direito
			break;
			
		case 'e':
			draw_line(xi,yi,xi,yi+7*factor,color,base); //lado esquerdo
			draw_line(xi,yi,xi+5*factor,yi,color,base); //cima
			draw_line(xi,yi+7*factor,xi+5*factor,yi+7*factor,color,base); //baixo
			draw_line(xi,yi+7*factor/2,xi+5*factor,yi+7*factor/2,color,base); //meio
			break;
		
		case 'f':
			draw_line(xi,yi,xi,yi+7*factor,color,base); //lado esquerdo
			draw_line(xi,yi,xi+5*factor,yi,color,base); //cima
			
			draw_line(xi,yi+7*factor/2,xi+5*factor,yi+7*factor/2,color,base); //meio
			break;
			
		case 'g':
			draw_line(xi,yi,xi,yi+7*factor,color,base); //lado esquerdo
			draw_line(xi,yi,xi+5*factor,yi,color,base); //cima
			draw_line(xi,yi+7*factor,xi+5*factor,yi+7*factor,color,base); //baixo
			draw_line(xi+7*factor/2,yi+7*factor/2,xi+5*factor,yi+7*factor,color,base); //lado direito
			draw_line(xi+5*factor/2,yi+7*factor/2,xi+5*factor,yi+7*factor/2,color,base); //meio
			break;
		case 'h':
			draw_line(xi,yi,xi,yi+7*factor,color,base); //lado esquerdo
			
			draw_line(xi+5*factor,yi,xi+5*factor,yi+7*factor,color,base); //lado direito
			draw_line(xi,yi+7*factor/2,xi+5*factor,yi+7*factor/2,color,base); //meio
			break;
		
			
		case 'i':
		
			draw_line(xi+5*factor/2,yi,xi+5*factor/2,yi+7*factor,color,base); //vertical meio
			draw_line(xi,yi,xi+5*factor,yi,color,base); //cima
			draw_line(xi,yi+7*factor,xi+5*factor,yi+7*factor,color,base); //baixo
			break;
		case 'j':
			draw_line(xi,yi+7*factor/2,xi,yi+7*factor,color,base); //lado esquerdo
			draw_line(xi,yi,xi+5*factor,yi,color,base); //cima
			draw_line(xi,yi+7*factor,xi+5*factor,yi+7*factor,color,base); //baixo
			draw_line(xi+5*factor,yi,xi+5*factor,yi+7*factor,color,base); //lado direito
			draw_line(xi,yi+7*factor/2,xi+5*factor/2,yi+7*factor/2,color,base); //meio
			break;
		case 'k':
			draw_line(xi,yi,xi,yi+7*factor,color,base); //lado esquerdo
			
			draw_line(xi+4*factor,yi,xi+4*factor,yi+3*factor,color,base); //lado direito
			draw_line(xi,yi+3*factor,xi+4*factor,yi+3*factor,color,base); //meio
			draw_line(xi,yi+5*factor,xi+4*factor,yi+5*factor,color,base); //meio
			draw_line(xi+4*factor,yi+5*factor,xi+4*factor,yi+7*factor,color,base); //lado direito
			break;
		case 'l':
			draw_line(xi,yi,xi,yi+7*factor,color,base); //lado esquerdo
			draw_line(xi,yi+7*factor,xi+5*factor,yi+7*factor,color,base); //baixo
			break;
		
		case 'm':
			draw_line(xi,yi,xi,yi+7*factor,color,base); //lado esquerdo
			draw_line(xi,yi,xi+2*factor,yi,color,base); //cima
			draw_line(xi+4*factor,yi,xi+5*factor,yi,color,base); //cima
			draw_line(xi+2*factor,yi,xi+4*factor,yi+7*factor,color,base); //lado direito
			draw_line(xi,yi+7*factor/2,xi+5*factor,yi+7*factor/2,color,base); //meio
			break;
		
		case 'o':
			draw_line(xi,yi,xi,yi+7*factor,color,base); //lado esquerdo
			draw_line(xi,yi,xi+5*factor,yi,color,base); //cima
			draw_line(xi,yi+7*factor,xi+5*factor,yi+7*factor,color,base); //baixo
			draw_line(xi+5*factor,yi,xi+5*factor,yi+7*factor,color,base); //lado direito
			break;
		case 'u':
			draw_line(xi,yi,xi,yi+7*factor,color,base); //lado esquerdo
			draw_line(xi,yi+7*factor,xi+5*factor,yi+7*factor,color,base); //baixo
			draw_line(xi+5*factor,yi,xi+5*factor,yi+7*factor,color,base); //lado direito
			break;
		
		case 's':
			draw_line(xi,yi,xi,yi+7*factor/2,color,base); //lado esquerdo metade
			draw_line(xi,yi,xi+5*factor,yi,color,base); //cima
			draw_line(xi,yi+7*factor,xi+5*factor,yi+7*factor,color,base); //baixo
			draw_line(xi+5*factor,yi+7*factor/2,xi+5*factor,yi+7*factor,color,base); //lado direito metade
			draw_line(xi,yi+7*factor/2,xi+5*factor,yi+7*factor/2,color,base); //meio
			break;
		case 'x':
			draw_line(xi,yi,xi,yi+2*factor,color,base); //lado esquerdo
			draw_line(xi,yi+5*factor,xi,yi+7*factor,color,base); //lado esquerdo
			draw_line(xi+5*factor,yi,xi+5*factor,yi+2*factor,color,base); //lado direito
			draw_line(xi+5*factor,yi+5*factor,xi+5*factor,yi+7*factor,color,base); //lado direito
			draw_line(xi+5*factor/2,yi+2*factor,xi+5*factor/2,yi+5*factor,color,base); //vertical meio
			draw_line(xi,yi+2*factor,xi+5*factor,yi+2*factor,color,base); //cima
			draw_line(xi,yi+5*factor,xi+5*factor,yi+5*factor,color,base); //baixo
			break;
		
			
	}
}

int get_vres(){
return VRES;
}

void create_board(char *base){
	draw_line(30,10,30,VRES-10,WHITE,base);
	draw_line(30,10,HRES-HRES/2-64,10,WHITE,base);
    draw_line(HRES-HRES/2-64,10,HRES-HRES/2-64,VRES-10,WHITE,base);
	draw_line(30,VRES-10,HRES-HRES/2-64,VRES-10,WHITE,base);


}


/** @} end of video */ 

