#include <stdio.h>
#include <dos.h>
#define BYTE unsigned char

BYTE MODOTEXTO = 3;
BYTE MODOGRAFICO = 4;

unsigned char cfondo=0;
unsigned char ctexto=1;


void textcolor(unsigned char color){
	ctexto = color;
}

void textbackground(unsigned char color){
    cfondo = color;
}

void cputchar(char c){
    union REGS inregs, outregs;
	inregs.h.ah = 0x09;
    inregs.h.al = c;
	inregs.h.bl = (cfondo << 4) | ctexto;
	inregs.h.bh = 0x00;
    inregs.x.cx = 1;
	int86(0x10, &inregs, &outregs);
	return;
}

void mi_pausa(){
    union REGS inregs, outregs;
    inregs.h.ah = 8;
    int86(0x21, &inregs, &outregs);
}
 
void xy(int x, int y){
    union REGS inregs, outregs;
    inregs.h.ah = 0x02;
    inregs.h.bh = 0x00;
    inregs.h.dh = y;
    inregs.h.dl = x;
    int86(0x10,&inregs,&outregs);
    return;
}

void clrscr(int lineas, int x, int y){
    union REGS inregs, outregs;
    inregs.h.ah = 0x06;
    inregs.h.al = lineas;
    inregs.h.bh = 0x0F;
    inregs.h.ch = 0x00;
    inregs.h.cl = 0x00;
    inregs.h.dh = x;
    inregs.h.dl = y;
    int86(0x10, &inregs, &outregs);
    return;
}

int mi_getchar(){
    union REGS inregs, outregs;
    int caracter;

    inregs.h.ah = 1;
    int86(0x21, &inregs, &outregs);

    caracter = outregs.h.al;
    return caracter;
}

void mi_putchar(char c){
    union REGS inregs, outregs;

    inregs.h.ah = 2;
    inregs.h.dl = c;
    int86(0x21, &inregs, &outregs);
}

void setcursortype(int tipo_cursor){
	union REGS inregs, outregs;
	inregs.h.ah = 0x01;
	switch(tipo_cursor){
		case 0: //invisible
			inregs.h.ch = 010;
			inregs.h.cl = 000;
			printf("\nSe ha activado el tipo invisible");
			break;
		case 1: //normal
			inregs.h.ch = 010;
			inregs.h.cl = 010;
			printf("\nSe ha activado el tipo normal");
			break;
		case 2: //grueso
			inregs.h.ch = 000;
			inregs.h.cl = 010;
			printf("\nSe ha activado el tipo grueso");
			break;
	}
	int86(0x10, &inregs, &outregs);
}

void setvideomode(BYTE modo){
    union REGS inregs, outregs;
    inregs.h.ah = 0x00;
    inregs.h.al = modo;
    int86(0x10, &inregs,&outregs);
}

void getvideomode(){
    union REGS inregs, outregs;
    inregs.h.ah = 0x0F;
    int86(0x10, &inregs, &outregs);

    printf("\nModo de video actual: 0x%X", outregs.h.al);
    printf("\nNumero de columnas (solo texto): %d", outregs.h.ah);
}

char getche(){
    union REGS inregs, outregs;
    inregs.h.ah = 0x00;
    int86(0x16, &inregs, &outregs);
    return outregs.h.al;
}

void pixel(int x, int y, unsigned char color){
    union REGS inregs, outregs;
    inregs.h.ah = 0x0C;
    inregs.h.al = color;
    inregs.x.cx = x;
    inregs.x.dx = y;
    inregs.h.bh = 0x00;
    int86(0x10, &inregs, &outregs);
}


int main(){
    int tmp, x, y, color;

    printf("\nDemostracion de la funcion gotoxy(x,y)");
    mi_pausa();

    clrscr(0, 24, 79);
    xy(15,7);
    printf("*");
    mi_pausa();
    clrscr(0, 24, 79);

    xy(0,24);
    printf("Demostracion de la funcion setcursortype(tipo)");
    printf("\nSeleccione una de las opciones: Invisible (0), Normal (1) o Grueso (2)\n");
    do{
        tmp = mi_getchar() - '0'; //se le resta 0 para que reste el valor ASCI al valor que queremos realmente
    }while (tmp != 0 && tmp != 1 && tmp != 2);

    setcursortype(tmp);
    mi_pausa();

    printf("\n\nDemostracion de la funcion setvideomode(modo)");
    printf("\nSeleccione una de las opciones: Texto (3) o Video (4)\n");
    do{
        tmp = mi_getchar() - '0';
    }while (tmp != 3 && tmp != 4);
    setvideomode(tmp);
    mi_pausa();

    printf("\n\nDemostracion de la funcion getvideomode()");
    getvideomode();

    mi_pausa();
    setvideomode(3);

    printf("\n\nDemostracion de la funcion textcolor(color)");
    printf("\nElige un color para el color del texto de forma numerica\n");
    tmp = mi_getchar();
    tmp = tmp - '0';
    textcolor(tmp);
    printf("\nEsto es un texto de ejemplo, donde no cambia el color");

    printf("\n\nDemostracion de la funcion textbackground(color)");
    printf("\nElige un color para el fondo en forma numerica\n");
    tmp = mi_getchar();
    tmp = tmp - '0';
    textbackground(tmp);
    printf("\nEsto es otro texto de ejemplo, donde no cambia el fondo");

    printf("\n\nDemostracion de la funcion cputchar()");
    printf("\nEscribe un caracter para mostrarlo con el color cambiado por pantalla\n");
    tmp = mi_getchar();
    printf("\nEste es el caracter con los colores modificados: "); 
    cputchar(tmp);

    printf("\n\nDemostracion de la funcion getche()");
    printf("\nPresiona una tecla: ");
    tmp = getche();
    printf("\nHas pulsado: %c\n", tmp);

    printf("\n\nDemostracion de la funcion pixel(x,y,color)");
    printf("\nIndica la coordenada x: ");
    scanf("%d", &x);
    printf("\nIndica la coordenada y: ");
    scanf("%d", &y);
    printf("\nIndica el color con el que pintar el pixel: ");
    color = mi_getchar();
    color = color - '0';
    setvideomode(4);
    pixel(x, y, color);
    mi_pausa();
    setvideomode(3);

    return 0;
}