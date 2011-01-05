gcc -c GQueue.c 
gcc -c main.c
gcc -c queue.c 
gcc -c ints.c
gcc -c kbc.c 
gcc -c timer.c
nasm -t -f coff mouse_isr.asm
nasm -t -f coff kbd_isr.asm 
nasm -t -f coff wait.asm 
gcc -c video-graphics.c 
gcc -c lab5.c
gcc -c sprite.c 


gcc -Wall GQueue.o main.o queue.o ints.o kbc.o timer.o mouse_isr.o wait.o kbc_init.o kbd_isr.o sprite.o video-graphics.o lab5.o -o teste.exe