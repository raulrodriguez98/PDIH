#include <dos.h>
#include <stdio.h>
#include <conio.h>
#define BYTE unsigned char


void dibujar_recuadro(int x1, int y1, int x2, int y2, int color_fondo, int color_texto) {
    int i, j;

    // Establecer el color de fondo y texto
    textbackground(color_fondo);  // Color de fondo
    textcolor(color_texto);       // Color del texto

    // Dibujar los bordes horizontales
    for (i = x1; i <= x2; i++) {
        gotoxy(i, y1);
        putch(205);  // Carácter de línea horizontal
        gotoxy(i, y2);
        putch(205);
    }

    // Dibujar los bordes verticales
    for (i = y1; i <= y2; i++) {
        gotoxy(x1, i);
        putch(186);  // Carácter de línea vertical
        gotoxy(x2, i);
        putch(186);
    }

    // Dibujar las esquinas
    gotoxy(x1, y1);
    putch(201);  // Esquina superior izquierda
    gotoxy(x2, y1);
    putch(187);  // Esquina superior derecha
    gotoxy(x1, y2);
    putch(200);  // Esquina inferior izquierda
    gotoxy(x2, y2);
    putch(188);  // Esquina inferior derecha
}

int main() {
    // Limpiar la pantalla antes de dibujar el recuadro
    clrscr();

    // Ejemplo de uso de la función dibujar_recuadro
    dibujar_recuadro(1, 1, 10, 15, RED, RED);

    return 0;
}