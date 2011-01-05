#include <dos.h>
#include <conio.h>
#include "lcom_il.XPM"
#include "kbc.h"
//#include "queue.h"
//#include "GQueue.h"
#include "nave.xpm"
#include "bola.xpm"
#include "sprite.h"
#include "utypes.h"
#include "video-graphics.h"
#include <string.h>
#include "gameover.XPM"
#include "rtc.h"
#include "obs1.xpm"
#include "obs2.xpm"
#include "obs3.xpm"
#include "timer.h"
#include "music.h"
#include "lcom_menu.xpm"
#include "rato.XPM"
#include <math.h>
#include "n0.xpm"
#include "n1.xpm"
#include "n2.xpm"
#include "n3.xpm"
#include "n4.xpm"
#include "n5.xpm"
#include "n6.xpm"
#include "n7.xpm"
#include "n8.xpm"
#include "n9.xpm"
_go32_dpmi_seginfo old_rtc_irq;
volatile int rtc_ints = 0, rtc_upd_ints = 0, rtc_per_ints = 0, rtc_alarm_ints = 0;


//void kbd_isr(void);
void mouse_isr(void);

//Queue keys_queue;
//GQueue* mouse_queue;

extern int mouse_byte1;
extern int mouse_byte2;
extern int mouse_byte3;
extern int pack_completo;
//extern int scancode;
//rotinas de interrupcao antigas
_go32_dpmi_seginfo old_kbd_irq, old_mouse_irq;
_go32_dpmi_seginfo old_irq;
//variaveis informacao rato
Byte MSBy1 = 0;
Byte MSBy2 = 0;
Byte MSBy3 = 0;
int dx = 0;
int dy = 0;
int xov = 0;
int yov = 0;
int lmb = 0;
int mmb = 0;
int rmb = 0;
Bool fim=false;
void gameover(char *base);
Sprite * obs[13][13];
int num_niveis=2;
int niveis[9][13][13];
void rtc_isr(void)
{
	Byte cause = read_rtc(RTC_STAT_C);
	
	if (cause & RTC_UF) // seconds interrupt
		rtc_upd_ints++;
	
	if (cause & RTC_AF) // alarm interrupt
		rtc_alarm_ints++;
	
	if (cause & RTC_PF) // periodic interrupt
		rtc_per_ints++;
		
	rtc_ints++;
	outportb(PIC1_CMD, EOI);
	outportb(PIC2_CMD, EOI);
}

Bool init()
{

	install_c_irq_handler(RTC_IRQ, rtc_isr, &old_rtc_irq);
	rtc_int_enable();
	unmask_pic(RTC_IRQ);


	//mask_pic(KBD_IRQ);
	mask_pic(MOUSE_IRQ);
	
	Bool has_mouse = kbc_init(1);
	
	//queueInit(&keys_queue);
	//mouse_queue = newGQueue(10, sizeof(MouseData));
	
	//install_asm_irq_handler(KBD_IRQ, kbd_isr, &old_kbd_irq);
	install_asm_irq_handler(MOUSE_IRQ, mouse_isr, &old_mouse_irq);
	
	//unmask_pic(KBD_IRQ);
	unmask_pic(MOUSE_IRQ);
	
	
	// timer_init(0, LCOM_MODE);
	timer_init(2, LCOM_MODE);
	//timer_load(0, get_divider(1000.0));
	// mask_pic(0);
	// install_asm_irq_handler(T0_IRQ, asm_wait, &old_irq);
	// unmask_pic(0);
	return has_mouse;
}


Bool finalize()
{

	//fim=true;
	mask_pic(RTC_IRQ);
	rtc_int_disable();
	reinstall_c_irq_handler(RTC_IRQ, &old_rtc_irq);
	
	// mask_pic(MOUSE_IRQ);
	// delay(200);
	// reinstall_asm_irq_handler(MOUSE_IRQ, &old_mouse_irq);
	// delay(200);
	// unmask_pic(MOUSE_IRQ);
	//mask_pic(KBD_IRQ);
	//delay(200);
	//reinstall_asm_irq_handler(KBD_IRQ, &old_kbd_irq);
		
	//delay(300);
	//unmask_pic(KBD_IRQ);
//printf("keyb\n");
	delay(200);
	//deleteGQueue(mouse_queue);
	// mask_pic(T0_IRQ);
	// reinstall_asm_irq_handler(T0_IRQ, &old_irq);	
	// delay(100);
	// unmask_pic(T0_IRQ);
	
	return 0;
}


void parse_mouse_event()
{
	MSBy1 = mouse_byte1;
	MSBy2 = mouse_byte2;
	MSBy3 = mouse_byte3;
	dx = MSBy1 & XSGN ? (MSBy2 - 256) : MSBy2;
	dy = MSBy1 & YSGN ? (MSBy3 - 256) : MSBy3;
	xov = X_OVERFLOW(MSBy1);
	yov = Y_OVERFLOW(MSBy1);
	lmb = LEFT_BUTTON(MSBy1);
	mmb = MIDDLE_BUTTON(MSBy1);
	rmb = RIGHT_BUTTON(MSBy1);
}

void draw_pont(int pont,char * base){
	int i=0;
	pont=pont*10;
	//for(;pont>10;i++){
	do{
		pont=pont/10;
		int temp=pont%10;
		Sprite * num;
		switch(temp){
		case 0: num=create_unbuffered_sprite(n0); break;
		case 1: num=create_unbuffered_sprite(n1); break;
		case 2: num=create_unbuffered_sprite(n2); break;
		case 3: num=create_unbuffered_sprite(n3); break;
		case 4: num=create_unbuffered_sprite(n4); break;
		case 5: num=create_unbuffered_sprite(n5); break;
		case 6: num=create_unbuffered_sprite(n6); break;
		case 7: num=create_unbuffered_sprite(n7); break;
		case 8: num=create_unbuffered_sprite(n8); break;
		case 9: num=create_unbuffered_sprite(n9); break;
		}
		num->x=HRES-num->width-1-num->width*i;
		num->y=200;
		delete_sprite(num,base);
		draw_sprite(num,base);
		destroy_sprite(num);
		i++;
		
	
	}while(pont>=10);
	


}






Note obs1_s,obs2_s,obs3_s;


	

void startobs(){
	int i=0;
	int r=0;
	
	obs1_s.freq=440;
	obs1_s.dur=100;
	obs2_s.freq=880;
	obs2_s.dur=100;	
	obs3_s.freq=1760;
	obs3_s.dur=100;
	
	for(r=0;r<13;r++){
		for(i=0;i<13;i++){
			Sprite * temp;
			int cor=rand()%3;
			if(cor==0){
				temp=create_sprite(obs1);
				temp->nota=&obs1_s;
				}
			if(cor==1){
				temp=create_sprite(obs2);
				temp->nota=&obs2_s;
				}
			if(cor==2){
				temp=create_sprite(obs3);
				temp->nota=&obs3_s;
				}
			temp->y=11+21*r;
			temp->x=31+32*i;
			obs[r][i]=temp;
			//draw_sprite(obs[i],vram);
		}
	}
}



void load_niveis(){
	FILE * fp;
fp= fopen("niveis.dat","r");

//int p=0;
if(fp){
	int i=0;
	int t=0;
	
		for(t=0;t<num_niveis;t++){
		
		for(i=0;i<13;i++){
		fscanf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d\n",&niveis[t][i][0]
															,&niveis[t][i][1]
															,&niveis[t][i][2]
															,&niveis[t][i][3]
															,&niveis[t][i][4]
															,&niveis[t][i][5]
															,&niveis[t][i][6]
															,&niveis[t][i][7]
															,&niveis[t][i][8]
															,&niveis[t][i][9]
															,&niveis[t][i][10]
															,&niveis[t][i][11]
															,&niveis[t][i][12]
															);
		// printf("%d %d %d %d %d %d %d %d %d %d %d %d %d\n",niveis[t][i][0]
															// ,niveis[t][i][1]
															// ,niveis[t][i][2]
															// ,niveis[t][i][3]
															// ,niveis[t][i][4]
															// ,niveis[t][i][5]
															// ,niveis[t][i][6]
															// ,niveis[t][i][7]
															// ,niveis[t][i][8]
															// ,niveis[t][i][9]
															// ,niveis[t][i][10]
															// ,niveis[t][i][11]
															// ,niveis[t][i][12]
															// );
		}
	}
	}
	
fclose(fp);
}

int loop(int x,int y,char *base) {
int lastyb=0;
Bool forced=false;
	int bolartc=4;		
	// Note teste;
		// teste.freq=10000;
	// teste.dur=1000;
						 // play_note(&teste);
					 // speaker_on();
					// delay(1000);
					 // speaker_off();
					 
	


	
	
	///**************
	Bool hold=true;
	int nivel=1;
	int accu=0;
	int rtci;
	int vidas=3;
	int pecasdespv=0;
	Sprite * vida[vidas];
	int i=0;
	for(i=0;i<vidas;i++){
		Sprite * temp;
		temp=create_sprite(bola_xpm);
		temp->x=HRES-temp->width-1-temp->width*i;
		temp->y=300;
		vida[i]=temp;
	
	}
	int last_sc = 0;
			Note bateu;
			int cont=0;
			int rtcp=0;
			Bool toca=false;
			startobs(base);
			int numpecas=0;
			int pecasdes=0;
			Sprite * Pont;
				Pont=create_sprite(pontuacao);
				Pont->x=770;
				Pont->y=0;
				draw_sprite(Pont,base);
			Sprite * nave;
				nave=create_sprite(nave_xpm);
				nave->x=x;
				nave->y=y;
				draw_sprite(nave,base);
				//printf("NAVE\n");
			Sprite * bola;
				bola=create_sprite(bola_xpm);
				bola->x=x+(nave->height/2);
				bola->y=y-bola->height-1;
				bola->yspeed=-1;
				bola->xspeed=1;
				draw_sprite(bola,base);
			//printf("bola\n");
			int x_old;
			int y_old;
			
			int r=0;	
		
		
	
	
	
	////*************

		Note musica[200];
	FILE * fp;
fp= fopen("musica.txt","r");
int p=0;
if(fp){
int n;
	int t=0;
	while(t>=0){

	fscanf(fp,"%d %d\n",&t,&n);
	if(t>=0){
	int freq=440 * pow(2.0,(n-57)/12.0);
	musica[p].dur=t;
	musica[p].freq=freq;
	//printf("%d %d \n",musica[p].freq,musica[p].dur);
	p++;
	}
	}
}
fclose(fp);

	int notas_totais=p;
	int z=0;


	Sprite * menuz;
	Sprite * cursor;
	cursor=create_sprite(cu);
	menuz=create_sprite(menuzito);
	int op=0;
	do{
	
			
	cursor->x=HRES/2;
	cursor->y=VRES/2;
	menuz->x=0;
	menuz->y=0;
	clear_screen(BLACK, base);
	draw_sprite(menuz,base);
	draw_sprite(cursor,base);
	op=0;
		while(op==0){
		
			rtc_per_ints=0;
				
				for(z=0;z<notas_totais && op==0;z++){
				//printf("%d : %d %d \n",z,musica[z].dur,musica[z].freq);
				play_note(&musica[z]);
				speaker_on();
				int est1=rtc_per_ints;
				while((abs(rtc_per_ints-est1)<musica[z].dur) && op==0){
			
				
										if(pack_completo)
									//if(!isEmptyGQueue(mouse_queue))
									{	
										int dxi=dx/2;
										int dyi=dy/2;
								
										parse_mouse_event();
										delete_sprite(cursor,base);
										if((cursor->x+dxi+1)>31 && (cursor->x+dxi+cursor->width)<1024){
											
											cursor->x=cursor->x+dxi;
											if((cursor->y-dyi+1)>10 && (cursor->y-dyi+cursor->height)<758){
											cursor->y=cursor->y-dyi;
											
														
										
									
										//delete_sprite(bola,base);
													//printf("x.%d y.%d\n",cursor->x,cursor->y);
													draw_sprite(cursor,base);
										//draw_sprite(bola,base);
										//printf("nova x: %d y: %d",x,y);
										
										}
										}
										
										//draw_line(x,y,x+10,y,GREEN,base);

										//printf("x:%d y:%d",x,y);
										//process_mouse_event_text();
										if(xov==0 && yov==0){
										if(lmb==1){
										lmb=0;
										if(cursor->x>=390 && cursor->x<=580){
											if(cursor->y>=312 && cursor->y<=358){
											op=1;
											}
											if(cursor->y>=510 && cursor->y<=561){
											op=2;
											}
										}
										}}
										pack_completo = 0;
									}
			
				
				
				}
				speaker_off();
				
				}
				
		

		
		
		}
		speaker_off();
		pack_completo = 0;
		delete_sprite(menuz,base);
		//destroy_sprite(menuz);
		clear_screen(BLACK, base);
		if(op==1){
			create_board(base);		
			accu=0;
			vidas=3;
			for(nivel=0;nivel<num_niveis && vidas>0; nivel++){
			clear_screen(BLACK, base);
			create_board(base);

			pecasdespv=0;	
			hold=true;		
			last_sc = 0;
			cont=0;
			rtcp=0;
			pecasdes=0;
			numpecas=0;
			bateu.freq=440;
			bateu.dur=100;
			toca=false;
			startobs(base);
	
				Pont->x=770;
				Pont->y=0;
				draw_sprite(Pont,base);
				nave->x=200;
				nave->y=VRES-60;
				draw_sprite(nave,base);
	
				bola->x=x+(nave->height/2);
				bola->y=y-bola->height-6;
				bola->yspeed=-1;
				bola->xspeed=1;
				draw_sprite(bola,base);
				lastyb=bola->y+1;	
			
			 i=0;
			 r=0;				 
			for(r=0;r<13;r++){
				for(i=0;i<13;i++){
					//printf("%d\n",i);
					if(niveis[nivel][r][i]!=0)
						draw_sprite(obs[r][i],base);
				}
			}
			
			for(r=0;r<13;r++){
				for(i=0;i<13;i++){
				
					if(obs[r][i]->draw==true){
						numpecas++;
					}
				}
			}
			
			Note * valores[numpecas];				 
							 
			for(i=0;i<vidas;i++){
			
				draw_sprite(vida[i],base);
	
			}	 
							 
							 
			
			rtci=rtc_per_ints;
			
			do
			{
			if(abs(rtci-rtc_per_ints)>500 && hold==true){
			hold=false;
			if(bola->yspeed>0){
			bola->yspeed=-bola->yspeed;
			delete_sprite(bola,base);
			bola->y=bola->y+bola->yspeed;
			bola->x=bola->x+bola->xspeed;
			}
			}
			if(pecasdespv==0){
			bolartc=4;
}			
			if(pecasdespv==10){
				bolartc=3;
				}
				if(pecasdespv==35){
				bolartc=2;
				}
				if(pecasdespv==50){
				bolartc=1;
				}
				
				if(toca==true){
					if(rtc_per_ints!=rtcp){
					cont++;
					rtcp=rtc_per_ints;
					//printf(" %d\n ",cont);
					}
					
				}
				if(cont>=bateu.dur){
					
					speaker_off();
					cont=0;
					toca=false;
					
				}
				
				if(pack_completo)
				//if(!isEmptyGQueue(mouse_queue))
				{	
					int dxi=dx/2;
					int dyi=dy/2;

					parse_mouse_event();
					// if(xov==0 && yov==0){
					// if(mmb==1){
					// fim=true;
					// }
					// }
					
					if((x+dxi+1)>31 && (x+dxi+nave->width)<(HRES-HRES/2-64)){
						
						x=x+dxi;
						if((y-dyi+1)>10 && (y-dyi+nave->height)<758){
						y=y-dyi;
						
									delete_sprite(nave,base);
									
						
					
					if(((bola->x+bola->width)>=x  && bola->x<=(x+nave->width)) && ((bola->y+bola->height+1)>=(y) && (bola->y-1)<=(y+nave->height))){
		                        // if(nave->y<y)
								// nave->y=bola->y+bola->height+1;
								// else{
								// nave->y=bola->y+bola->height-nave->height-1;
								// }

		                        }else{
								if(hold){
								delete_sprite(bola,base);
								bola->x=x;
								bola->y=nave->y-bola->height-6;
								draw_sprite(bola,base);
								}
					nave->x=x;
					nave->y=y;
					      }
					//delete_sprite(bola,base);
					draw_sprite(nave,base);
					//draw_sprite(bola,base);
					//printf("nova x: %d y: %d",x,y);
					
					}
					}
					
					//draw_line(x,y,x+10,y,GREEN,base);

					//printf("x:%d y:%d",x,y);
					//process_mouse_event_text();
					pack_completo = 0;
					x=nave->x;
					y=nave->y;
				}
				
				
				
				
				
				if(!hold){
					if(rtc_per_ints>=bolartc){
					delete_sprite(bola,base);
					x_old=bola->x;
					y_old=bola->y;
					//bola->y+=bola->yspeed;
					//bola->x+=bola->xspeed;
				
					//if(!draw_ball(bola,base)){
					int oldxs=bola->xspeed;
					int oldys=bola->yspeed;
					//bola->x=x_old;
					//bola->y=y_old;
					// if(get_pixel((bola->x+bola->xspeed)+bola->width+1,bola->y+bola->height,base)!=0 || get_pixel((bola->x+bola->xspeed)-1,bola->y,base)!=0){

						// bola->yspeed=-bola->yspeed;
						// }else{
							// if(get_pixel((bola->x+bola->xspeed),bola->y,base)!=0){
							// bola->yspeed=bola->yspeed;
							// bola->xspeed=-bola->xspeed;
							// }else{
								// if(get_pixel(bola->x,(bola->y+bola->yspeed),base)!=0){
								// bola->xspeed=bola->xspeed;
								// bola->yspeed=-bola->yspeed;
						
								// }
								// else{
									// if(get_pixel(bola->x+bola->width,(bola->y+bola->yspeed)+bola->height+1,base)!=0){
									// bola->xspeed=-bola->xspeed;
								// }
								// }
					// }}
					
					
					Bool Vertical=false;
					Bool Horizontal=false;
					int i=0;
					for(i=0;i<bola->width;i++){
						//if(bola->yspeed>0){
							if(get_pixel(bola->x+bola->xspeed+i,bola->y+bola->yspeed+1+bola->height,base)!=0){
								Vertical=true;
								i=bola->width;
							//}
							}
						//else{
							if(get_pixel(bola->x+bola->xspeed+i,bola->y+bola->yspeed-1,base)!=0){
								Vertical=true;
								i=bola->width;
							}
							//}
							if(bola->y+bola->yspeed<=11){
							Vertical=true;
							bola->y=12;
							i=bola->width;}
							
							// if(((bola->x+bola->width)>=nave->x  && bola->x<=(nave->x+nave->width)) && ((bola->y+bola->height+2)>=(nave->y) && (bola->y-3)<=(nave->y+nave->height))){
									// if(((bola->y+bola->height+2)>=(nave->y) && (bola->y-3)<=(nave->y+nave->height))){
									// Vertical=true;
									// if(bola->yspeed>0){
									// bola->y=nave->y-bola->height-8;
									// bola->yspeed=-1;
									// forced=true;
									// i=bola->width;
									// }
									
									// }
							// }
					}
					
					for(i=0;i<bola->height;i++){
						//if(bola->xspeed>0){
							if(get_pixel(bola->x+bola->xspeed+1+bola->width,bola->y+bola->yspeed+i,base)!=0){
								Horizontal=true;
								i=bola->height;
							}
							//}
						// else{
							if(get_pixel(bola->x+bola->xspeed-1,bola->y+bola->yspeed+i,base)!=0){
								Horizontal=true;
								i=bola->height;
							//}
							}
					}
					if(Horizontal && !(forced)){
						bola->xspeed=-bola->xspeed;
					}else{
					if(Vertical && !(forced)){
						bola->yspeed=-bola->yspeed;
					}else{
					forced=false;}
					}

						
						if((bola->y+oldys-31)/20<13 && (bola->y+oldys-31)/20>=0){
							//printf("bateu: %d",(bola->y+oldys-31)/20);
							 if(((bola->x+oldxs-31)/32)<13){
								//printf(" x: %d\n",((bola->x+oldxs-31)/32) );
								 if(obs[(bola->y+oldys-31)/20][((bola->x+oldxs-31)/32)]->draw==true){
									 delete_sprite(obs[(bola->y+oldys-31)/20][((bola->x+oldxs-31)/32)],base);
									 valores[pecasdes]=obs[(bola->y+oldys-31)/20][((bola->x+oldxs-31)/32)]->nota;
									 pecasdes++;
									 pecasdespv++;
									 play_note(obs[(bola->y+oldys-31)/20][((bola->x+oldxs-31)/32)]->nota);
									 speaker_on();
									 //printf("\t\t\t\t\tspeaker on\n");
									 toca=true;
									 cont=0;
									 rtcp=0;
									 bola->yspeed=-bola->yspeed;
									 draw_pont(accu+pecasdes*10,base);
								 }
							 }
						}
					
					
					//}
							bola->x=x_old;
					bola->y=y_old;
					bola->y+=bola->yspeed;
					bola->x+=bola->xspeed;
					if(bola->y==lastyb)
						bola->y+=bola->yspeed*2;
					lastyb=bola->y;
					draw_sprite(bola,base);
					
					rtc_per_ints=0;
					}
				}
				
				
				
				
				
				
				
				if((bola->y+bola->height+2)>=(VRES-20)){
				vidas--;
				pecasdespv=0;
				delete_sprite(vida[vidas],base);
				delete_sprite(bola,base);
				delete_sprite(nave,base);
				x=nave->x=200;
				y=nave->y=VRES-60;
				draw_sprite(nave,base);
				bola->x=nave->x+nave->x/2;
				bola->y=nave->y-bola->height-6;
				bola->yspeed=-bola->yspeed;
				draw_sprite(bola,base);
				hold=true;
				rtci=rtc_per_ints;
				
				}
				
				
			} while(numpecas>pecasdes && vidas>0);
			if(vidas<=0){
			gameover(base);
			}
			for(i=0;i<pecasdes;i++){
			 play_note(valores[i]);
			speaker_on();
			int est=rtc_per_ints;
			while(abs(rtc_per_ints-est)<valores[i]->dur){
			}
			speaker_off();
			}
			accu=accu+pecasdes*10;
		}
		}
	}while(op!=2);


	return 0;
}


void gameover(char *base){
	Sprite * gameoveri;
	gameoveri=create_sprite(gameover_xpm);
	gameoveri->x=HRES-HRES/2-gameoveri->width/2;
	gameoveri->y=VRES-VRES/2-gameoveri->height/2;
	draw_sprite(gameoveri,base);
		
		delay(100);
	
	
	

}
