Bool init(); //incia os perifericos a utilizar pelo programa (rtc, mouse e speaker)
int loop(int x,int y,char *base); /* 
* Esta função é responsavel pelo jogo em si.
* Nesta função os dados do jogo são tratados e as regras de jogo são instaladas
*/
Bool finalize(); //restaura os perificos iniciados
void load_niveis();  //carrega niveis do ficheiro "niveis.dat"
