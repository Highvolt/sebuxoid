#include "sprite.h"

extern int HRES, VRES;

//static char *video_buffer = NULL;

/** Reads a xpm-like sprite defined in "map" (look at pixmap.h for
 * examples). Returns the address of the allocated memory where the
 * sprite was read. Updates "width" and "heigh" with the sprite
 * dimension.  Return NULL on error.
 * Assumes that VRES and HRES (the screen vertical and horizontal resolution)
 * are externaly defined.
 * 
 * Usage example, using the defined sprite in pixmap.h:
 * <pre>
 *   #include "pixmap.h" // defines  pic1, pic2, etc 
 *   int wd, hg;
 *   char *sprite = read_xpm(pic1, &wd, &hg);
 * </pre>
*/

char *read_xpm(char *map[], int *wd, int *ht)
{
	__attribute__((unused)) static char read_xpm_jcard;

	int width, height, colors;
	char sym[256];
	int  col;
	int i, j;
	char *pix, *pixtmp, *tmp, *line;
	char symbol;

	// read width, height, colors
	if (sscanf(map[0],"%d %d %d", &width, &height, &colors) != 3) {
		printf("read_xpm: incorrect width, height, colors\n");
		return NULL;
	}
	#ifdef DEBUG
	printf("%d %d %d\n", width, height, colors);
	#endif
	if (width > HRES || height > VRES || colors > 256) {
		printf("read_xpm: incorrect width, height, colors\n");
		return NULL;
	}

	*wd = width;
	*ht = height;

	for (i=0; i<256; i++)
	sym[i] = 0;

	// read symbols <-> colors 
	for (i=0; i<colors; i++) {
		if (sscanf(map[i+1], "%c %d", &symbol, &col) != 2) {
			printf("read_xpm: incorrect symbol, color at line %d\n", i+1);
			return NULL;
		}
	#ifdef DEBUG
		printf("%c %d\n", symbol, col);
	#endif
		if (col > 256) {
			printf("read_xpm: incorrect color at line %d\n", i+1);
			return NULL;
		}
		sym[col] = symbol;
	}

	// allocate pixmap memory
	pix = pixtmp = malloc(width*height);

	// parse each pixmap symbol line
	for (i=0; i<height; i++) {
		line = map[colors+1+i];
	#ifdef DEBUG
		printf("\nparsing %s\n", line);
	#endif
		for (j=0; j<width; j++) {
			tmp = memchr(sym, line[j], 256);  // find color of each symbol
			if (tmp == NULL) {
				printf("read_xpm: incorrect symbol at line %d, col %d\n", colors+i+1, j);
				return NULL;
			}
			*pixtmp++ = tmp - sym; // pointer arithmetic! back to books :-)
	#ifdef DEBUG
			printf("%c:%d ", line[j], tmp-sym);
	#endif
		}
	}

	return pix;
}



Sprite* create_sprite(char *pic[])
{
	Sprite* sprt = create_unbuffered_sprite(pic);
	if(!sprt) return NULL;
	sprt->bgmap = malloc(sprt->width * sprt->height);
	sprt->draw=false;
	return sprt;
}



Sprite* create_unbuffered_sprite(char *pic[])
{
	Sprite* sprt = malloc(sizeof(Sprite));
	if(!sprt) return NULL;
	
	int wd, ht;
	sprt->map = read_xpm(pic, &wd, &ht);
	if(!sprt->map) { free(sprt); return NULL; }
	
	sprt->x = sprt->y = sprt->xspeed = sprt->yspeed = 0;
	sprt->width = wd; sprt->height = ht;
	sprt->bgmap = NULL;
		sprt->draw=false;
	return sprt;
}



Sprite* capture_screen(int x_ini, int y_ini, int width, int height, char* base)
{
	Sprite* sprt = malloc(sizeof(Sprite));
	if(!sprt) return NULL;
	
	char* map = malloc(width * height);
	if(!map) { free(sprt); return NULL; }

	int i, k;
	for(i = 0; i < height; i++)
		for(k = 0; k < width; k++)
			*(map + width*i + k) = get_pixel(x_ini + k, y_ini + i, base);

	sprt->map = map;
	
	sprt->x = x_ini;
	sprt->y = y_ini;
	sprt->xspeed = sprt->yspeed = 0;
	sprt->width = width; sprt->height = height;
	sprt->bgmap = NULL;
	return sprt;
}



void draw_sprite(Sprite* sprt, char *base)
{
	char* dSprt = sprt->map;
	char* bgMap = sprt->bgmap;
	sprt->draw=true;
	int i,k;
	for(i=0; i < sprt->height; i++)
		for(k=0; k < sprt->width; k++)
		{
			if(bgMap) *(bgMap + sprt->width*i + k) = get_pixel(sprt->x+k, sprt->y+i, base); //guarda o valor antigo
			if(*dSprt != 0) set_pixel(sprt->x+k,sprt->y+i, *dSprt, base);  //actualiza com o valor actual
			dSprt++;
		}
}


Bool draw_ball(Sprite* sprt, char *base)
{
	//char* dSprt = sprt->map;
	//char* bgMap = sprt->bgmap;
	
	int i,k;
	for(i=0; i < sprt->height; i++)
		for(k=0; k < sprt->width; k++)
		{
			if(get_pixel(sprt->x+k, sprt->y+i, base)){
				return false;
			}

		}
	return true;
}










void delete_sprite(Sprite *sprt, char *base)
{
	char* bgMap = sprt->bgmap;
	if(!bgMap){ 
	int i, k;
	for(i=0; i < sprt->height; i++)
		for(k=0; k < sprt->width; k++)
			set_pixel(sprt->x+k,sprt->y+i, 0, base);
	
	
	return;}
	sprt->draw=false;
	int i, k;
	for(i=0; i < sprt->height; i++)
		for(k=0; k < sprt->width; k++)
			set_pixel(sprt->x+k,sprt->y+i, *(bgMap + sprt->width*i + k), base);
		
}





void destroy_sprite(Sprite *sprt)
{
	if (!sprt) return;	
	free(sprt->map);
	free(sprt->bgmap);
	free(sprt);
}









