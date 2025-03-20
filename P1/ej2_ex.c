#include <dos.h> 
#define BYTE unsigned char


void video_mode (BYTE modo){
    union REGS inregs, outregs;
    inregs.h.ah = 0x00;
    inregs.h.al = modo;
    int86(0x10,&inregs, &outregs);
    return;
}

// Función para dibujar un píxel en la pantalla
void dibujar_pixel(int x, int y, unsigned char color) {
    union REGS inregs, outregs;
    inregs.h.ah = 0x0C;  // Función para dibujar un píxel
    inregs.h.al = color; // Color del píxel
    inregs.x.cx = x;     // Coordenada X
    inregs.x.dx = y;     // Coordenada Y
    int86(0x10, &inregs, &outregs);  // Llamar a la interrupción 10h de la BIOS
}


void pause(){
    union REGS inregs, outregs;
    inregs.h.ah = 1;
    int86(0x21, &inregs, &outregs);
}

int main() {
    int x;
    int y;
    video_mode(4);

    // Dibujar el techo de la casa (triángulo)
    for (x = 100; x <= 200; x++) {
        dibujar_pixel(x, 50, 2);  // Base del techo 
    }
    for (y = 50; y >= 20; y--) {
        dibujar_pixel(100 + (50 - (y - 20)), y, 2);  // Lado izquierdo del techo 
        dibujar_pixel(200 - (50 - (y - 20)), y, 2);  // Lado derecho del techo
    }

    // Dibujar las paredes de la casa (rectángulo)
    for (x = 100; x <= 200; x++) {
        for (y = 50; y <= 120; y++) {
            dibujar_pixel(x, y, 3);  // Paredes (blanco)
        }
    }

    // Dibujar la puerta (rectángulo pequeño)
    for (x = 130; x <= 170; x++) {
        for (y = 80; y <= 120; y++) {
            dibujar_pixel(x, y, 1);  // Puerta (cian)
        }
    }

    // Dibujar las ventanas (dos cuadrados pequeños)
    for (x = 110; x <= 130; x++) {
        for (y = 60; y <= 80; y++) {
            dibujar_pixel(x, y, 1);  // Ventana izquierda (cian)
        }
    }
    for (x = 170; x <= 190; x++) {
        for (y = 60; y <= 80; y++) {
            dibujar_pixel(x, y, 1);  // Ventana derecha (cian)
        }
    }

    // Dibujar el suelo (línea horizontal)
    for (x = 80; x <= 220; x++) {
        dibujar_pixel(x, 120, 2);  // Suelo
    }


    pause();

    video_mode(3);

    return 0;
}