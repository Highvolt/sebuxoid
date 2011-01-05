#include <dpmi.h>
#include <string.h>
#include <stdio.h>
#include "utypes.h"
#include "video-graphics.h"
#include <dos.h>
#include <conio.h>
//#include "xpm.h"
#include "sprite.h"
#include <string.h>
#include "lab5.h"
#include "music.h"
#include "timer.h"
#include <math.h>


void string_graphics(char a[],int x, int y, int color, char *base,int factor){
	int i=0;
	int tam=strlen(a);
	for(i=0;i<tam;i++){
		print_text(a[i],x,y,color,base,factor);
		x=x+8*factor;
	}
	

}

void create_ball(int x, int y, char *base){
	draw_circle(x, y, 5, YELLOW, base);
}








int main(){
	load_niveis();

    init();
	
	
	
	
    int mode=0x105;
    __dpmi_meminfo map;
    char *vram;

    vram = enter_graphics(mode, &map);

    if( vram == NULL ) {
	printf("Não entrou em  modo grafico");
	return -1;
	}

	
	// char a[20]="sebuxoid";
	// string_graphics(a,220,020, GREEN,vram,7);
	//char b[6]="belos";
	//char c[6]="mulos";
	//string_graphics(b,220,220, GREEN,vram,2);
	//string_graphics(c,280,320, GREEN,vram,3);
	
	delay(500);

	//create_ball(100,100,vram);
	loop(200, get_vres()-60,vram);
	

	
	

	
	
	/*
	int b=200;
	for(b=200; b<225; b++){
	clear_screen(b, vram);
	delay(500);
	}
	*/
	
	
	
	
	


	
	
	leave_graphics(map);
	finalize();
	
	return 0;

}
