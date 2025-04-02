#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#define GRASS     ' '
#define EMPTY     '.'
#define WATER     '~'
#define MOUNTAIN  '^'
#define PLAYER    '*'
#define MONSTRUO  'X'

#define GRASS_PAIR     1
#define EMPTY_PAIR     1
#define WATER_PAIR     2
#define MOUNTAIN_PAIR  3
#define PLAYER_PAIR    4
#define MONSTRUO_PAIR  5
#define WELCOME_PAIR   6
#define END_PAIR       7

int is_move_okay(int y, int x);
void draw_map(void);
void movimiento_monstruo(int *y_monstruo, int *x_monstruo, int *last_move_time, int monster_speed);
void respawn_monstruo(int *y_monstruo, int *x_monstruo);
void draw_game_info(int score, long time_left, int nivel);
void subida_nivel (int n, long *start_time);
long get_current_time_us(void);
void pausa_segundos(long microsegundos, long *start_time);
void mostrar_bienvenida(int *salir);
void mostrar_fin_juego(int score, int nivel, int *jugar_de_nuevo);


int main(void)
{
    int y, x, y_monstruo, x_monstruo;
    int ch, puntuacion = 0, nivel = 0;
    int monster_speed = 500000; // 0.5 segundos inicialmente
    int last_monster_move = 0;
    long start_time, tiempo_actual, time_left, tiempo_partida = 120;
    struct timeval tv;
    int jugar_de_nuevo = 1;
    int salir = 0;
    
    /* initialize curses */
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);

    /* initialize colors */
    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }

    start_color();
    init_pair(GRASS_PAIR, COLOR_YELLOW, COLOR_GREEN);
    init_pair(WATER_PAIR, COLOR_CYAN, COLOR_BLUE);
    init_pair(MOUNTAIN_PAIR, COLOR_BLACK, COLOR_WHITE);
    init_pair(PLAYER_PAIR, COLOR_YELLOW, COLOR_WHITE);
    init_pair(MONSTRUO_PAIR, COLOR_BLACK, COLOR_RED);
    init_pair(WELCOME_PAIR, COLOR_WHITE, COLOR_GREEN);
    init_pair(END_PAIR, COLOR_WHITE, COLOR_GREEN);
    
    
    while(jugar_de_nuevo && !salir) {
    	
        mostrar_bienvenida(&salir);
        puntuacion = 0, nivel = 0;
        
    	if (!salir) {
    
	    clear();

	    /* initialize the quest map */
	    draw_map();

	    /* start player at lower-left */
	    y = LINES - 1;
	    x = 0;

	    /* inicializar la posición del monstruo random */
	    srand(time(NULL));
	    respawn_monstruo(&y_monstruo, &x_monstruo);
	    
	    gettimeofday(&tv, NULL);
	    last_monster_move = tv.tv_sec * 1000000 + tv.tv_usec;
	    
	    start_time = get_current_time_us();
	    
	    do {
	    	tiempo_actual = get_current_time_us();
		time_left = tiempo_partida - (tiempo_actual - start_time) / 1000000L;
		if (time_left < 0) time_left = 0;
		
		draw_game_info(puntuacion, time_left, nivel);
		
		
		/* Dibujar el mapa primero para "borrar" las posiciones anteriores */
		draw_map();

		/* Dibujar al jugador */
		attron(COLOR_PAIR(PLAYER_PAIR));
		mvaddch(y, x, PLAYER);
		attroff(COLOR_PAIR(PLAYER_PAIR));
		
		/* Dibujar el monstruo */
		attron(COLOR_PAIR(MONSTRUO_PAIR));
		mvaddch(y_monstruo, x_monstruo, MONSTRUO);
		attroff(COLOR_PAIR(MONSTRUO_PAIR));
		
		
		move(x,y);
		refresh();

		/* Manejar entrada del jugador */
		ch = getch();
		switch (ch) {
		    case KEY_UP: case 'w': case 'W': 
		        if (y > 0 && is_move_okay(y-1, x)) y--; 
		        break;
		    case KEY_DOWN: case 's': case 'S': 
		        if (y < LINES-1 && is_move_okay(y+1, x)) y++; 
		        break;
		    case KEY_LEFT: case 'a': case 'A': 
		        if (x > 0 && is_move_okay(y, x-1)) x--; 
		        break;
		    case KEY_RIGHT: case 'd': case 'D': 
		        if (x < COLS-1 && is_move_okay(y, x+1)) x++; 
		        break;
		}

		/* Mover monstruo según su velocidad */
		gettimeofday(&tv, NULL);
		int current_time = tv.tv_sec * 1000000 + tv.tv_usec;
		if (current_time - last_monster_move >= monster_speed) {
		
			    /* Detectar colisión DIRECTAMENTE por coordenadas */
			    if (y == y_monstruo && x == x_monstruo) {
				puntuacion++;
				refresh();
				respawn_monstruo(&y_monstruo, &x_monstruo);
				
				mvprintw(LINES/2, COLS/2 - 10, "¡Monstruo derrotado!");
		        refresh();
		        pausa_segundos(1000000, &start_time);
		        
		        
				/* Aumentar dificultad progresivamente */
				if (puntuacion == 3){
					monster_speed = 400000;
					subida_nivel(nivel, &start_time);
					nivel++;
		        }  
			       	if (puntuacion == 6){
			       		monster_speed = 300000;
			       		subida_nivel(nivel, &start_time);
					nivel++;
		        } 
				if (puntuacion == 10){
					monster_speed = 200000;
					subida_nivel(nivel, &start_time);
					nivel++;
		        } 
				if (puntuacion == 15){
					monster_speed = 150000;
					subida_nivel(nivel, &start_time);
					nivel++;
		        } 
				if (puntuacion == 20){
					monster_speed = 100000;
					subida_nivel(nivel, &start_time);
					nivel++;
		        }  
			    }
			    
		    movimiento_monstruo(&y_monstruo, &x_monstruo, &last_monster_move, monster_speed);
		}
		
		if (time_left <= 0) {
		    clear();
		    draw_game_info(puntuacion, 0, nivel);
		    mvprintw(LINES/2, COLS/2 - 10, "¡Tiempo terminado!");
		    mvprintw(LINES/2 + 1, COLS/2 - 10, "Puntuación final: %d", puntuacion);
		    mvprintw(LINES/2 + 2, COLS/2 - 10, "Nivel alcanzado: %d", nivel);
		    refresh();
		    getch();
		    break;
		}
	    
		usleep(10000); // Pequeña pausa para reducir CPU
	    }
	    while (ch != 'q' && ch != 'Q');
	    
	if (!salir) {
                mostrar_fin_juego(puntuacion, nivel, &jugar_de_nuevo);
            }
        }
    }

    endwin();
    return 0;
}

int is_move_okay(int y, int x)
{
	if (y == 0) return 0;  // La línea 0 (información) no es transitable
	
    int testch = mvinch(y, x) & A_CHARTEXT;
    return (testch == GRASS || testch == EMPTY || testch == MONSTRUO);
}

void movimiento_monstruo(int *my, int *mx, int *last_move_time, int monster_speed)
{
    int direccion = rand() % 4;
    int nuevo_y = *my, nuevo_x = *mx;

    switch (direccion) {
        case 0: if (nuevo_y > 0) nuevo_y--; break;
        case 1: if (nuevo_y < LINES-1) nuevo_y++; break;
        case 2: if (nuevo_x > 0) nuevo_x--; break;
        case 3: if (nuevo_x < COLS-1) nuevo_x++; break;
    }

    // Verificar si la nueva posición es válida
    if (is_move_okay(nuevo_y, nuevo_x)) {
        *my = nuevo_y;
        *mx = nuevo_x;
    }
    
    // Actualizar el tiempo del último movimiento
    struct timeval tv;
    gettimeofday(&tv, NULL);
    *last_move_time = tv.tv_sec * 1000000 + tv.tv_usec;
}

void respawn_monstruo(int *y_monstruo, int *x_monstruo)
{
    do {
        *y_monstruo = rand() % LINES;
        *x_monstruo = rand() % COLS;
    } while (!is_move_okay(*y_monstruo, *x_monstruo));
}


void draw_map(void)
{
    int y, x;

    /* draw the quest map */
    /* background */
    attron(COLOR_PAIR(GRASS_PAIR));
    for (y = 1; y < LINES; y++) {
        mvhline(y, 0, GRASS, COLS);
    }
    attroff(COLOR_PAIR(GRASS_PAIR));

    /* mountains, and mountain path */
    attron(COLOR_PAIR(MOUNTAIN_PAIR));
    for (x = COLS / 2; x < COLS * 3 / 4; x++) {
        mvvline(1, x, MOUNTAIN, LINES - 1);
    }
    attroff(COLOR_PAIR(MOUNTAIN_PAIR));

    attron(COLOR_PAIR(GRASS_PAIR));
    mvhline(LINES / 4, 0, GRASS, COLS);
    attroff(COLOR_PAIR(GRASS_PAIR));

    /* lake */
    attron(COLOR_PAIR(WATER_PAIR));
    for (y = 2; y < LINES / 2; y++) {
        mvhline(y, 1, WATER, COLS / 3);
    }
    attroff(COLOR_PAIR(WATER_PAIR));
}


long get_current_time_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}


void subida_nivel (int n, long *start_time){
	mvprintw(LINES/2, COLS/2 - 10, "¡Nivel %d completado!", n);
	mvprintw(LINES/2 + 2, COLS/2 - 10, "Ahora los monstruos se hacen más fuertes");
    refresh();
    pausa_segundos(2000000, start_time);
}


void draw_game_info(int score, long time_left, int nivel) {
    // Dibujar la información en la primera línea
    attron(A_BOLD);
    mvhline(0, 0, ' ', COLS);
    mvprintw(0, 0, "Puntuacion: %d", score);
    mvprintw(0, COLS/2 - 7, "Nivel: %d", nivel);
    mvprintw(0, COLS - 30, "Tiempo: %02ld", time_left);
    mvprintw(0, COLS - 8, "Salir: Q");
    attroff(A_BOLD);
}

void pausa_segundos(long microsegundos, long *start_time) {
    long tiempo_inicio_pausa = get_current_time_us();
    usleep(microsegundos);
    *start_time += (get_current_time_us() - tiempo_inicio_pausa);
}


void mostrar_bienvenida(int *salir) {
    nodelay(stdscr, FALSE);
    clear();
    attron(COLOR_PAIR(WELCOME_PAIR));
    
    int height = 18;
    int width = 60;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    
    // Dibujar borde
    for(int y = start_y; y < start_y + height; y++) {
        mvaddch(y, start_x, ACS_VLINE);
        mvaddch(y, start_x + width - 1, ACS_VLINE);
    }
    for(int x = start_x; x < start_x + width; x++) {
        mvaddch(start_y, x, ACS_HLINE);
        mvaddch(start_y + height - 1, x, ACS_HLINE);
    }
    mvaddch(start_y, start_x, ACS_ULCORNER);
    mvaddch(start_y, start_x + width - 1, ACS_URCORNER);
    mvaddch(start_y + height - 1, start_x, ACS_LLCORNER);
    mvaddch(start_y + height - 1, start_x + width - 1, ACS_LRCORNER);
    
    // Título del juego centrado
    mvprintw(start_y + 2, start_x + (width - 15)/2, "NCursed Knight");
    
    // Información de creadores
    mvprintw(start_y + 4, start_x + 4, "Creado por:");
    mvprintw(start_y + 5, start_x + 6, "Gabriel Vico Arboledas");
    mvprintw(start_y + 6, start_x + 6, "Raul Rodriguez Rodriguez");
    
    // Controles - alineado a la izquierda
    mvprintw(start_y + 8, start_x + 4, "Controles:");
    mvprintw(start_y + 9, start_x + 6, "W o tecla Arriba    - Mover arriba");
    mvprintw(start_y + 10, start_x + 6, "S o tecla Abajo     - Mover abajo");
    mvprintw(start_y + 11, start_x + 6, "A o tecla Izquierda - Mover izquierda");
    mvprintw(start_y + 12, start_x + 6, "D o tecla Derecha   - Mover derecha");
    mvprintw(start_y + 13, start_x + 6, "Q                   - Salir del juego");
    
    // Mensaje de continuar - alineado con "Controles"
    mvprintw(start_y + 15, start_x + 4, "Presiona cualquier tecla para continuar");
    
    attroff(COLOR_PAIR(WELCOME_PAIR));
    refresh();
    
    int ch = getch();
    nodelay(stdscr, TRUE);
    if (ch == 'q' || ch == 'Q') {
        *salir = 1;
    }
}

void mostrar_fin_juego(int score, int nivel, int *jugar_de_nuevo) {
    int ch;
    clear();
    
    do {
        attron(COLOR_PAIR(END_PAIR));
        
        int height = 12;
        int width = 50;
        int start_y = (LINES - height) / 2;
        int start_x = (COLS - width) / 2;
        
        // Dibujar borde
        for(int y = start_y; y < start_y + height; y++) {
            mvaddch(y, start_x, ACS_VLINE);
            mvaddch(y, start_x + width - 1, ACS_VLINE);
        }
        for(int x = start_x; x < start_x + width; x++) {
            mvaddch(start_y, x, ACS_HLINE);
            mvaddch(start_y + height - 1, x, ACS_HLINE);
        }
        mvaddch(start_y, start_x, ACS_ULCORNER);
        mvaddch(start_y, start_x + width - 1, ACS_URCORNER);
        mvaddch(start_y + height - 1, start_x, ACS_LLCORNER);
        mvaddch(start_y + height - 1, start_x + width - 1, ACS_LRCORNER);
        
        // Título del juego centrado
        mvprintw(start_y + 2, start_x + (width - 15)/2, "NCursed Knight");
        mvprintw(start_y + 4, start_x + (width - 20)/2, "Puntuacion final: %d", score);
        mvprintw(start_y + 5, start_x + (width - 20)/2, "Nivel alcanzado: %d", nivel);
        mvprintw(start_y + 7, start_x + (width - 14)/2, "¡Felicidades!");
        mvprintw(start_y + height - 3, start_x + (width - 20)/2, "¿Jugar de nuevo (S/N)?");
        
        attroff(COLOR_PAIR(END_PAIR));
        refresh();
        
        ch = getch();
        if (ch == 'q' || ch == 'Q') {
            *jugar_de_nuevo = 0;
            return;
        }
    } while(ch != 's' && ch != 'S' && ch != 'n' && ch != 'N');
    
    *jugar_de_nuevo = (ch == 's' || ch == 'S');
}











