Bool init(); //incia os perifericos a utilizar pelo programa (rtc, mouse e speaker)
int loop(int x,int y,char *base); /* 
* Esta fun��o � responsavel pelo jogo em si.
* Nesta fun��o os dados do jogo s�o tratados e as regras de jogo s�o instaladas
*/
Bool finalize(); //restaura os perificos iniciados
void load_niveis();  //carrega niveis do ficheiro "niveis.dat"
