#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <conio.h>
#include <string.h>

/* constantes globales */
#define x 30
#define y 90
#define habitaciones 7
#define mobs 10
#define tesoros 6
#define trampas 8
#define preguntas 10
/* estructuras */
    struct room{
		int coordx,coordy,lado,altura;
	};
	
	 struct super_hero{
		int coordx,coordy,life,points;
	};

	struct monster{
		int coordx, coordy, life,prev_x,prev_y;
	};

	 struct nodo{
		int puntaje;
		struct nodo *prox;
	} *prim, *p, *a, *q;
/* variables globales */

struct room rooms[habitaciones];

struct monster monsters[mobs];

char dungeon[x][y];

int salida,semilla = 0;

int room_to_room[habitaciones];

struct super_hero hero;

int play_time;

char *preguntas_aed[preguntas][preguntas];

 

/* funciones */

/* Devuelve un número aleatorio entre minimo y maximo, incluyendo a minimo y maximo https://parzibyte.me/blog/2019/03/21/obtener-numeros-aleatorios-c/ */
int aleatorio_en_rango(int minimo, int maximo);

int crear_habitaciones(int p);

void drawrooms();

int aleatorio (int min, int max); 

int aleatorio_propio(int minimo, int maximo);

int pedir_semilla();

void caminos();

void dibujar(int pieza, int posicion);

void spawn_hero();

void move_hero();

void spawn_monsters();

void move_mobs_random();

void spawn_treasure();

void spawn_traps();

void spawn_questions();

void spawn_exit();

void cargar_preguntas();

void guardar_puntaje();

void mostrar_puntaje();

int main()

{
	char palabra_sin_uso[1];
	int i,j;
	char start_game[7];
	/* cargar matriz */
		/* generar muros */
		for (i=0; i<x; i++)
		{
			for (j=0; j<y; j++)
			{
				dungeon[i][j]= '#';
			}
		}
		/* cargar habitaciones con 0*/
		for (i=0; i<7; i++)
		{
			rooms[i].coordx = 0;
			rooms[i].coordy = 0;
			rooms[i].lado = 0;
			rooms[i].altura = 0;
		}
		/* generar habitaciones */
		for (i=0; i<7; i++)
		{
			crear_habitaciones(i);
		}
		drawrooms();
		caminos();
	/* mostrar matriz */
	printf("bienvenido a la mazmorra estas son las reglas que necesitas saber");
	printf("\n");
	printf("Leer atentamente!!");
	printf("\n");
	printf("Para interactuar con un objeto u monstruo debes moverte hacia donde esta");
	printf("\n");
	printf("para poder salir debes reunir 100 puntos");
	printf("\n");
	printf("\n");
	printf("--------glosario--------");
	printf("\n"); 
	printf("h: heroe");
	printf("\n");
	printf("T: tesoro");
	printf("\n");
	printf("p: pregunta");
	printf("\n");
	printf("m: monstruo");
	printf("\n");
	printf("*: trampa");
	printf("\n");
	printf("#: paredes");
	printf("\n");
	printf("S: salida");
	printf("\n");
	printf("\n");
	printf("debes taimear el momento exacto para golpear a un enemigo");
	printf("\n");
	printf("puedes pasar un turno de espera apretando cualquier otra tecla");
	printf("\n");
	printf("al estar cerca de un monstruo puedes recibir un golpe");
	printf("\n");
	printf("\n");
	printf("quieres empezar una partida?, ingresa 'jugar' si quieres hacerlo");
	printf("\n");
	scanf("%s", start_game);
	printf("\n");
	while ( strcmp(start_game,"jugar") == 0)
	{
		system("cls");
		spawn_hero();
		spawn_monsters();
		spawn_treasure();
		spawn_traps();
		spawn_questions();
		cargar_preguntas();
		spawn_exit();
		play_time = 1;
		while (play_time)
		{
			for (i=0; i<x; i++)
			{
				
				for (j=0; j<y; j++)
				{
					printf("%c",dungeon[i][j]);
				}
				printf("\n");
			}
			move_hero();
			move_mobs_random();
			/*me ahorro el tema de colisiones si muevo primero el heroe y de dar daño, y de recibir daño si se mueven segundo los mobs*/
			system("cls");
			if (hero.life <= 0)
			{
				play_time = 0;
				printf("se termino el juego, mejor suerte la proxima vez");
				printf("\n");
				printf("tienes unas ultimas palabras que quieras decir?... puedes explayarte un poco");
				printf("\n");
				gets(palabra_sin_uso);
				printf("ya veo, nos alegramos que lo hayas disfrutado");
				getch();
				printf("\n");
			}
		}
		/*si termino clrscren y mostrar puntos*/
		/*hacer el juego hasta 100 ptos opcion*/
		start_game[0] = '\0';
	}
	if (salida)
	{
		printf("felicidades haz logrado salir de la mazmorra!!");
		printf("\n");
	}
	printf("\n");
	/* si hero.life == 0 mensaje consolador*/
	/* si el heroe salio por la salida mensaje felicitador felicidaes haz logrado salir de la cueva!!*/
	printf("puntos: %d", hero.points);
	printf("\n");
	printf("\n");
	/* guardo en un archivo .txt el puntaje del jugador*/
	guardar_puntaje();
	/* lo recorro, cargo a una lista ordenadamente y muestro los primeros 5*/
	mostrar_puntaje();
	return 0;
}

int pedir_semilla() {
		semilla += 1000;
		if (semilla == 200000)
		{
			semilla = 0;
		}
		return semilla;
}

int aleatorio_en_rango(int minimo, int maximo){
	srand(pedir_semilla());
	return minimo + rand() / (RAND_MAX / (maximo - minimo + 1) + 1);
}

int aleatorio_propio(int minimo, int maximo){
	srand( time(NULL) + pedir_semilla());
	return (rand() % maximo) + minimo; 
}

int aleatorio (int min, int max) 
{
         return min +(int)(((max-min+1.0)*rand())/(RAND_MAX+1.0));
}

int crear_habitaciones(int p) {
	
	int coordx, coordy, lado, altura, room_p, room_i, alta, choque_eje_x, choque_eje_y, i, j, k, paredes_pegadas_x, paredes_pegadas_y;
	
	/* mientras no genere una habitacion no deberia de salir del bucle*/
	alta= 0;
	while (!alta)
	{ 
		/* creo las coordenadas aleatorias */
		coordx = aleatorio_propio(0, x);
		coordy = aleatorio_propio(0,y);
		lado = aleatorio_propio(2,4); 
		altura = aleatorio_propio(4,7); 
		alta = 1; /* fijo alta a verdadero */
		
		/* tengo que chequear que ninguna de las habitaciones choque contra las paredes limites de la dungeon*/
		/* despues tengo que chequear que no choque con el resto de habitaciones*/
		/* recien ahi es un alta valida*/
		
		if ((coordx-lado > 0) && (coordx+lado < x) && (coordy-altura > 0) && (coordy+altura < y)) 
		{
				for (i=0; i<habitaciones; i++) /* comparamos con el resto de habitaciones */
				{
					if (i != p) /* si la comparacion no es con la propia habitacion*/
					{
						/* estamos comparando con el resto de habitaciones creadas que no son la misma */
						if ((rooms[i].coordx != 0) && (rooms[i].coordx != 0) && (rooms[i].coordx != 0) && (rooms[i].coordx != 0))
						{
							/* hay que chequear si las paredes chocan para poder habilitar*/
							/* guardo los valores ocupados por ambas habitaciones en cada eje*/
							/* si chocan en ambos ejes es porque en la interseccion se superponen */
							
							choque_eje_x  = 0;
							choque_eje_y = 0;
							paredes_pegadas_x = 0;
							paredes_pegadas_y = 0;
							
							/* eje x */
							room_p =(coordx+lado)-(coordx-lado)+1;
							int arrx1[room_p];
							room_i =(rooms[i].coordx+rooms[i].lado)-(rooms[i].coordx-rooms[i].lado)+1;
							int arrx2[room_i];
							
							/* guardo valores */
							for (j=0; j<room_p; j++)
							{
								arrx1[j]= (coordx - lado + j); 
								
							}
							
							for (j=0; j<room_i; j++)
							{
								arrx2[j]= (rooms[i].coordx - rooms[i].lado + j);
							}
														
							/* chequeo choque y paredes pegadas*/
							for (j=0; j<room_p; j++)
							{
									for (k=0; k<room_i; k++)
									{
											if ( (arrx1[j]) == (arrx2[k]) )
											{
												choque_eje_x = 1;
												
											}
											if (((arrx1[j] - arrx2[k] ) == 1) || ((arrx1[j] - arrx2[k] ) == -1) )
											{
												paredes_pegadas_x=1;
											}
									}	
							}	
							
							/* eje y */
							
							room_p = (coordy+altura)-(coordy-altura)+1;
							int arry1[room_p];
							room_i = (rooms[i].coordy+rooms[i].altura)-(rooms[i].coordy-rooms[i].altura)+1;
							int arry2[room_i];
							
							/* guardo valores */
							for (j=0; j<room_p; j++)
							{
								arry1[j] = (coordy - altura + j);
							}
							for (j=0; j<room_i; j++)
							{
								arry2[j] = (rooms[i].coordy - rooms[i].altura + j);
							}
							/* chequeo choques y paredes pegadas */	
							for( j=0; j<room_p; j++)
							{
								for( k=0; k<room_i; k++)
								{
									if ( arry1[j] == arry2[k] )
									{
											choque_eje_y = 1;
									}
									if (((arry1[j] - arry2[k] ) == 1) || ((arry1[j] - arry2[k] ) == -1) )
									{
										paredes_pegadas_y=1;
									}
								}
							}
							
							
							/* ahora hacemos el chequeo, queremos que si no chocan habilitar la habitacion */
							if (( choque_eje_x && choque_eje_y) || (paredes_pegadas_x && paredes_pegadas_y))
							{
						
								alta = 0;
								
							}	
							/* no deberiamos salir de este bucle si alta es 0(falso) por lo que deberia volver
							       a repetirse todo el proceso hasta generar una combinacion valida*/
					}	
				}	
			}
		}
		else
		{
			alta = 0;
		}	
	}
	/* ya debimos haber salido del bucle por lo cual podemos dar de alta la habitacion */
	/* un chequeo por las dudas */
	if (alta)
	{
		rooms[p].coordx = coordx;
		rooms[p].coordy = coordy;
		rooms[p].lado = lado;
		rooms[p].altura = altura;
	}
	
	return 0;
}


/* quizas cambiar en la comparacion de habitaciones por enteros*/
/* dibujar las habitaciones en la dungeon*/
/* crear los caminos */
/* dibujarlos */

void drawrooms() {
	int lado, altura, h,i,j;
	for (i=0; i<habitaciones; i++)
	
	{
		lado= ((rooms[i].coordx)+(rooms[i].lado)) - ((rooms[i].coordx)-(rooms[i].lado)) ;
		altura= ((rooms[i].coordy)+(rooms[i].altura)) -((rooms[i].coordy)-(rooms[i].altura)) ;
		for (j=0; j<lado; j++)
		{
			for (h=0; h<altura; h++)
			{
				dungeon[(rooms[i].coordx)-(rooms[i].lado)+j][(rooms[i].coordy)-(rooms[i].altura)+h] = ' ';
			}
		}
		
	}
	
}

void caminos(){
	int i,j,min,distancia,position;
	for (i=0; i<habitaciones; i++)
	{
		min = 1000;
		for (j=0; j<habitaciones; j++)
		{
			if ( i != j)
			{
				distancia = sqrt(pow((rooms[j].coordx - rooms[i].coordx),2) + pow((rooms[j].coordy-rooms[i].coordy),2));
				if (distancia < min)
				{
					min = distancia;
					position = j;
				}
			}
		}
		if (room_to_room[position] != i)
		{
			
		}
		else
		{
			room_to_room[i] = aleatorio_propio(0,6);
		}
		
	}
	for (i=0; i<habitaciones; i++)
	{
		dibujar(i,room_to_room[i]);
	}
}
 
void dibujar(int pieza, int posicion){
	/* chequeo cual habitacion esta mas serca de la esquina superior */
	int i,j,z,t,rand;
	i=0;
	j=0;
	rand = aleatorio_propio(1,2);
	if (sqrt((pow(rooms[pieza].coordx,2) + pow(rooms[pieza].coordy,2))) < sqrt(pow(rooms[posicion].coordx,2)+pow(rooms[posicion].coordy,2)))
	{
		if (rand == 1)
		{
			if (rooms[pieza].coordy+rooms[pieza].altura <= rooms[posicion].coordy+1 )
			{	
				do
				{
					z  = rooms[pieza].coordy+rooms[pieza].altura+i;
					dungeon[rooms[pieza].coordx][z] = ' ';
					i++;
				}while ( z <= rooms[posicion].coordy+1);
			}
			else
			{
				do
				{
					z  = rooms[pieza].coordy-rooms[pieza].altura+i;
					dungeon[rooms[pieza].coordx][z] = ' ';
					i--;
				}while ( z >= rooms[posicion].coordy-1);
			}
			if (rooms[pieza].coordx <= (rooms[posicion].coordx-rooms[posicion].lado-1) )
			{
				do
				{
					j++;
					t = rooms[pieza].coordx + j;
					dungeon[t][z] = ' ';
				}while (t < (rooms[posicion].coordx-rooms[posicion].lado)-1);
			}
			else
			{
				do
				{
					j--;
					t = rooms[pieza].coordx + j;
					dungeon[t][z] = ' ';
				}while (t > (rooms[posicion].coordx+rooms[posicion].lado));
			}
		}
		else
		{
			
			if ((rooms[posicion].coordy-rooms[posicion].altura -1) >= rooms[pieza].coordy-1)
			{
				do
				{
					z = rooms[posicion].coordy-rooms[posicion].altura - i -1;
					dungeon[rooms[posicion].coordx][z] = ' ';
					i++;
				} while (z >= rooms[pieza].coordy-1);
			}
			else
			{
				do
				{
					z = rooms[posicion].coordy+rooms[posicion].altura + i + 1;
					dungeon[rooms[posicion].coordx][z] = ' ';
					i++;
				} while (z <= rooms[pieza].coordy+1);
			}
			
			if ( rooms[posicion].coordx >= rooms[pieza].coordx+rooms[pieza].lado)
			{
				do
				{		
					j--;		
					t = rooms[posicion].coordx + j;
					dungeon[t][z] = ' ';
				}while( t > rooms[pieza].coordx+rooms[pieza].lado);
			}
			else
			{
				do
				{		
					j++;
					t = rooms[posicion].coordx + j;
					dungeon[t][z] = ' ';
				}while( t < rooms[pieza].coordx-rooms[pieza].lado-1);
			}
		}
	}
	else
	{
		if (rand == 1)
		{
			if (rooms[posicion].coordy+rooms[posicion].altura <= (rooms[pieza].coordy)+1)
			{
				do 
				{
					z = rooms[posicion].coordy+rooms[posicion].altura + i;
					dungeon[rooms[posicion].coordx][z] = ' ';
					i++;
				}while (z <= (rooms[pieza].coordy)+1);
			}
			else
			{
				do 
				{
					z = rooms[posicion].coordy-rooms[posicion].altura - i;
					dungeon[rooms[posicion].coordx][z] = ' ';
					i++;
				}while (z >= (rooms[pieza].coordy)-1);
			}
			
			if (rooms[posicion].coordx <= (rooms[pieza].coordx-rooms[pieza].lado-1) )
			{
				do
				{
					j++;
					t = rooms[posicion].coordx + j;
					dungeon[t][z] = ' ';	
				}while (t < (rooms[pieza].coordx-rooms[pieza].lado)-1);
			}
			else
			{
				do
				{
					j--;
					t = rooms[posicion].coordx + j;
					dungeon[t][z] = ' ';
				}while (t > (rooms[pieza].coordx+rooms[pieza].lado));
			}
		}
		else
		{
			if((rooms[pieza].coordy-rooms[pieza].altura - 1 )>= rooms[posicion].coordy-1)
			{
				do
				{
					z = rooms[pieza].coordy-rooms[pieza].altura - i - 1;
					dungeon[rooms[pieza].coordx][z] = ' ';
					i++;
				} while (z >= rooms[posicion].coordy-1);
			}
			else
			{
				do
				{
					z = rooms[pieza].coordy+rooms[pieza].altura + i + 1;
					dungeon[rooms[pieza].coordx][z] = ' ';
					i++;
				} while (z <= rooms[posicion].coordy+1);
			}
			if (rooms[pieza].coordx >= rooms[posicion].coordx+rooms[posicion].lado)
			{
				do
				{
					j--;
					t = rooms[pieza].coordx + j;
					dungeon[t][z] = ' ';
				}while( t > rooms[posicion].coordx+rooms[posicion].lado);
			}
			else
			{
				do
				{
					j++;
					t = rooms[pieza].coordx + j;
					dungeon[t][z] = ' ';
				}while( t < rooms[posicion].coordx-rooms[posicion].lado-1);
			}
		}
	}
} 

void spawn_hero() {
	/* elijo una habitacion al azar y lo hago aparece en el centro*/
	int hazar;
	hazar = aleatorio_propio(0,6);
	hero.coordx = rooms[hazar].coordx;
	hero.coordy = rooms[hazar].coordy;
	dungeon[hero.coordx][hero.coordy] = 'h';
	hero.life = 6;
	hero.points = 0;
}

void move_hero() {
	char direccion;
	char respuesta[100];
	int i, j;
	printf("puedes moverte utilizando la combinacion de teclas wasd, puedes realizar una accion por turno");
	printf("\n");
	printf("para dejar de jugar ingresa 'f'");
	printf("\n");
	printf("life points:");
	for (i=0;i<hero.life;i++)
	{
		printf("%c", 3);
	}
	printf("\n");
	printf("puntaje ");
	printf("%d", hero.points);
	printf("/100");
	printf("\n");
	fflush(stdin);
	direccion = getch();
	if (direccion == 'f')
	{
		play_time = 0;
	}
	else
	{
		switch (direccion)
		{
			case 'w':
				switch (dungeon[hero.coordx-1][hero.coordy])
				{
					case ' ':
						dungeon[hero.coordx][hero.coordy] = ' ';
						hero.coordx-=1;
						dungeon[hero.coordx][hero.coordy] = 'h';
						break;
					case 'm':
						for(i=0;i<mobs;i++)
						{
								if((monsters[i].coordx == hero.coordx-1 ) && (monsters[i].coordy == hero.coordy))
								{
									monsters[i].life -=1;
									printf("Monster life points left: ");
									for (j=0;j<monsters[i].life;j++)
									{
										printf("%c", 3);
									}
									getch();
									printf("\n");
									if (monsters[i].life == 0)
									{
										dungeon[monsters[i].coordx][monsters[i].coordy] = ' ';
										monsters[i].coordx = 0;
										monsters[i].coordy = 0;
										hero.points+= 10;
									}
								}
						}
						break;
					case 'T':
						dungeon[hero.coordx][hero.coordy] = ' ';
						hero.coordx-=1;
						dungeon[hero.coordx][hero.coordy] = 'h';
						hero.points+= 15;
						break;
					case '*':
						dungeon[hero.coordx][hero.coordy] = ' ';
						hero.coordx-=1;
						dungeon[hero.coordx][hero.coordy] = 'h';
						hero.life -= 1;
						break;
					case 'p':
						printf("\n");
						int rand = aleatorio_propio(0,preguntas);
						printf("Pregunta!!");
						printf("\n");
						printf("respondes en minusculas"); 
						printf("\n");
						puts(preguntas_aed[rand][0]);
						if (strcmp(gets(respuesta), preguntas_aed[rand][1]) == 0)
						{
							hero.points += 15;
						}
						else
						{
							hero.points -= 20;
						}
						dungeon[hero.coordx-1][hero.coordy] = ' ';
						break;
					case 'S':
						if (hero.points >= 100)
						{
							play_time = 0;
							salida = 1;
						}
						printf("\n");
						getch();
						break;
				}
				break;
			case 'a':
				switch (dungeon[hero.coordx][hero.coordy-1])
				{
					case ' ':
						dungeon[hero.coordx][hero.coordy] = ' ';
						hero.coordy-=1;
						dungeon[hero.coordx][hero.coordy] = 'h';
						break;
					case 'm':
						for(i=0;i<mobs;i++)
						{
								if((monsters[i].coordx == hero.coordx ) && (monsters[i].coordy == hero.coordy-1))
								{
									monsters[i].life -=1;
									printf("Monster life points left: ");
									for (j=0;j<monsters[i].life;j++)
									{
										printf("%c", 3);
									}
									getch();
									printf("\n");
									if (monsters[i].life == 0)
									{
										dungeon[monsters[i].coordx][monsters[i].coordy] = ' ';
										monsters[i].coordx = 0;
										monsters[i].coordy = 0;
										hero.points+= 10;
									}
								}
						}
						break;
					case 'T':
						dungeon[hero.coordx][hero.coordy] = ' ';
						hero.coordy-=1;
						dungeon[hero.coordx][hero.coordy] = 'h';
						hero.points+= 15;
						break;
					case '*':
						dungeon[hero.coordx][hero.coordy] = ' ';
						hero.coordy-=1;
						dungeon[hero.coordx][hero.coordy] = 'h';
						hero.life -= 1;
						break;
					case 'p':
						printf("\n");
						int rand = aleatorio_propio(0,preguntas);
						printf("Pregunta!!");
						printf("\n");
						printf("respondes en minusculas");
						printf("\n");
						puts(preguntas_aed[rand][0]);
						if (strcmp(gets(respuesta), preguntas_aed[rand][1]) == 0)
						{
							hero.points += 15;
						}
						else
						{
							hero.points -= 20;
						}
						dungeon[hero.coordx][hero.coordy-1] = ' ';
						break;
					case 'S':
						if (hero.points >= 100)
						{
							play_time = 0;
							salida = 1;
						}
						printf("\n");
						getch();
						break;
				}
				break;
			case 's':
				switch (dungeon[hero.coordx+1][hero.coordy])
				{
					case ' ':
						dungeon[hero.coordx][hero.coordy] = ' ';
						hero.coordx+=1;
						dungeon[hero.coordx][hero.coordy] = 'h';
						break;
					case 'm':
						for(i=0;i<mobs;i++)
						{
								if((monsters[i].coordx == hero.coordx+1 ) && (monsters[i].coordy == hero.coordy))
								{
									monsters[i].life -=1;
									printf("Monster life points left: ");
									for (j=0;j<monsters[i].life;j++)
									{
										printf("%c", 3);
									}
									getch();
									printf("\n");
									if (monsters[i].life == 0)
									{
										dungeon[monsters[i].coordx][monsters[i].coordy] = ' ';
										monsters[i].coordx = 0;
										monsters[i].coordy = 0;
										hero.points+= 10;
									}
								}
						}
						break;
					case 'T':
						dungeon[hero.coordx][hero.coordy] = ' ';
						hero.coordx+=1;
						dungeon[hero.coordx][hero.coordy] = 'h';
						hero.points+= 15;
						break;
					case '*':
						dungeon[hero.coordx][hero.coordy] = ' ';
						hero.coordx+=1;
						dungeon[hero.coordx][hero.coordy] = 'h';
						hero.life -= 1;
						break;
					case 'p':
						printf("\n");
						int rand = aleatorio_propio(0,preguntas);
						printf("Pregunta!!");
						printf("\n");
						printf("respondes en minusculas");
						printf("\n");
						puts(preguntas_aed[rand][0]);
						if (strcmp(gets(respuesta), preguntas_aed[rand][1]) == 0)
						{
							hero.points += 15;
						}
						else
						{
							hero.points -= 20;
						}
						dungeon[hero.coordx+1][hero.coordy] = ' ';
						break;
					case 'S':
						printf("para poder salir debes tener 100 puntos");
						if (hero.points >= 100)
						{
							play_time = 0;
							salida = 1;
						}
						printf("\n");
						printf("muevete hacia otro lado para continuar");
						getch();
						break;
				}
				break;
			case 'd':
				switch (dungeon[hero.coordx][hero.coordy+1])
				{
					case ' ':  
						dungeon[hero.coordx][hero.coordy] = ' ';
						hero.coordy+=1;
						dungeon[hero.coordx][hero.coordy] = 'h';
						break;
					case 'm':
						for(i=0;i<mobs;i++)
						{
								if((monsters[i].coordx == hero.coordx ) && (monsters[i].coordy == hero.coordy+1))
								{					
									monsters[i].life -=1;
									printf("Monster life points left: ");
									for (j=0;j<monsters[i].life;j++)
									{
										printf("%c", 3);
									}
									getch();
									printf("\n");
									if (monsters[i].life == 0)
									{
										dungeon[monsters[i].coordx][monsters[i].coordy] = ' ';
										monsters[i].coordx = 0;
										monsters[i].coordy = 0;
										hero.points+= 10;
									}
								}
						}
						break;
					case 'T':
						dungeon[hero.coordx][hero.coordy] = ' ';
						hero.coordy+=1;
						dungeon[hero.coordx][hero.coordy] = 'h';
						hero.points+= 15;
						break;
					case '*':
						dungeon[hero.coordx][hero.coordy] = ' ';
						hero.coordy+=1;
						dungeon[hero.coordx][hero.coordy] = 'h';
						hero.life -= 1;
						break;
					case 'p':
						printf("\n");
						int rand = aleatorio_propio(0,preguntas);
						printf("Pregunta!!");
						printf("\n");
						printf("respondes en minusculas");
						printf("\n");
						puts(preguntas_aed[rand][0]);
						if (strcmp(gets(respuesta), preguntas_aed[rand][1]) == 0)
						{
							hero.points += 15;
						}
						else
						{
							hero.points -= 20;
						}
						dungeon[hero.coordx][hero.coordy+1] = ' ';
						break;
					case 'S':
						if (hero.points >= 100)
						{
							play_time = 0;
							salida = 1;
						}
						printf("\n");
						getch();
						break;
				}
				break;
		}	
	}
}

void spawn_monsters(){
	int i,rand;
	for (i=0;i<mobs;i++)
	{
		/*bucle indefinido hasta que no este encima de algo mas para generarse*/
		rand = aleatorio_propio(0,6);
		monsters[i].coordx = rooms[rand].coordx + (aleatorio_propio(0,rooms[rand].lado) - aleatorio_propio(0,rooms[rand].lado));
		monsters[i].coordy = rooms[rand].coordy + (aleatorio_propio(0,rooms[rand].altura) - aleatorio_propio(0,rooms[rand].altura));
	}
	for (i=0;i<mobs;i++)
	{
		dungeon[monsters[i].coordx][monsters[i].coordy] = 'm';
		monsters[i].life = 2;
	}
} 

void move_mobs_random(){
	int i,rand,desplazamiento_valido;
	for (i=0;i<mobs;i++)
	{
		/* bucle indefinido*/
		if (monsters[i]. life > 0)
		{
			desplazamiento_valido = 0;
			while (!desplazamiento_valido)
			{
				 
					rand = aleatorio_propio(0,3);
					switch (rand)
					{
						case 0:
							if ((dungeon[monsters[i].coordx-1][monsters[i].coordy] == '#') || (dungeon[monsters[i].coordx-1][monsters[i].coordy] == 'T') || (dungeon[monsters[i].coordx-1][monsters[i].coordy] == '*') || (dungeon[monsters[i].coordx-1][monsters[i].coordy] == 'p') || (dungeon[monsters[i].coordx-1][monsters[i].coordy] == 'S'))
							{
								desplazamiento_valido = 1;
								break;
							}
							else if(dungeon[monsters[i].coordx-1][monsters[i].coordy] == 'h')
							{
								desplazamiento_valido = 1;
								/*hacer daño al heroe*/
								hero.life -=1;
								break;
							}
							else
							{
								monsters[i].prev_x = monsters[i].coordx;
								monsters[i].prev_y = monsters[i].coordy;
								dungeon[monsters[i].coordx][monsters[i].coordy] = ' ';
								monsters[i].coordx-=1;
								dungeon[monsters[i].coordx][monsters[i].coordy] = 'm';
								desplazamiento_valido = 1;
								break;
							}
							break;
						case 1:
							if ((dungeon[monsters[i].coordx+1][monsters[i].coordy] == '#') || (dungeon[monsters[i].coordx+1][monsters[i].coordy] == 'T') || (dungeon[monsters[i].coordx+1][monsters[i].coordy] == '*') || (dungeon[monsters[i].coordx+1][monsters[i].coordy] == 'p') || (dungeon[monsters[i].coordx+1][monsters[i].coordy] == 'S'))
							{
								desplazamiento_valido = 1;
								break;
							}
							else if(dungeon[monsters[i].coordx+1][monsters[i].coordy] == 'h')
							{
								desplazamiento_valido = 1;
								/*hacer daño al heroe*/
								hero.life -=1;
								break;
							}
							else
							{
								monsters[i].prev_x = monsters[i].coordx;
								monsters[i].prev_y = monsters[i].coordy;
								dungeon[monsters[i].coordx][monsters[i].coordy] = ' ';
								monsters[i].coordx+=1;
								dungeon[monsters[i].coordx][monsters[i].coordy] = 'm';
								desplazamiento_valido = 1;
								break;
							}
							break;
						case 2:
							if ((dungeon[monsters[i].coordx][monsters[i].coordy-1] == '#') || (dungeon[monsters[i].coordx][monsters[i].coordy-1] == 'T') || (dungeon[monsters[i].coordx][monsters[i].coordy-1] == '*') || (dungeon[monsters[i].coordx][monsters[i].coordy-1] == 'p') || (dungeon[monsters[i].coordx][monsters[i].coordy-1] == 'S'))
							{
								desplazamiento_valido = 1;
								break;
							}
							else if(dungeon[monsters[i].coordx][monsters[i].coordy-1] == 'h')
							{
								desplazamiento_valido = 1;
								/*hacer daño al heroe*/
								hero.life -=1;
								break;
							}
							else
							{
								monsters[i].prev_x = monsters[i].coordx;
								monsters[i].prev_y = monsters[i].coordy;
								dungeon[monsters[i].coordx][monsters[i].coordy] = ' ';
								monsters[i].coordy-=1;
								dungeon[monsters[i].coordx][monsters[i].coordy] = 'm';
								desplazamiento_valido = 1;
								break;
							}
							break;
						case 3:
							if ((dungeon[monsters[i].coordx][monsters[i].coordy+1] == '#') || (dungeon[monsters[i].coordx][monsters[i].coordy+1] == 'T') || (dungeon[monsters[i].coordx][monsters[i].coordy+1] == '*') || (dungeon[monsters[i].coordx][monsters[i].coordy+1] == 'p') || (dungeon[monsters[i].coordx][monsters[i].coordy+1] == 'S'))
							{
								desplazamiento_valido = 1;
								break;
							}
							else if(dungeon[monsters[i].coordx][monsters[i].coordy+1] == 'h')
							{
								desplazamiento_valido = 1;
								/*hacer daño al heroe*/
								hero.life -=1;
								break;
							}
							else
							{
								monsters[i].prev_x = monsters[i].coordx;
								monsters[i].prev_y = monsters[i].coordy;
								dungeon[monsters[i].coordx][monsters[i].coordy] = ' ';
								monsters[i].coordy+=1;
								dungeon[monsters[i].coordx][monsters[i].coordy] = 'm';
								desplazamiento_valido = 1;
								break;
							}
							break;
						}
					}
		}
		
	}
}

void spawn_treasure(){
	int i,rand,a,b,alta;
	for (i=0;i<tesoros;i++)
	{
		alta=0;
		while (!alta)
		{
			rand = aleatorio_propio(0,6);
			a = rooms[rand].coordx + (aleatorio_propio(0,rooms[rand].lado) - aleatorio_propio(0,rooms[rand].lado));
			b = rooms[rand].coordy + (aleatorio_propio(0,rooms[rand].altura) - aleatorio_propio(0,rooms[rand].altura));
			if ((dungeon[a][b] != '#') && (dungeon[a][b] != 'h') && (dungeon[a][b] != 'm') && (dungeon[a][b] = 'T'))
			{
				dungeon[a][b] = 'T';
				alta = 1;
			}
		}
	}
}

void spawn_traps(){
	int i,rand,a,b,alta;
	for (i=0;i<trampas;i++)
	{
		alta=0;
		while (!alta)
		{
			rand = aleatorio_propio(0,6);
			a = rooms[rand].coordx + (aleatorio_propio(0,rooms[rand].lado) - aleatorio_propio(0,rooms[rand].lado));
			b = rooms[rand].coordy + (aleatorio_propio(0,rooms[rand].altura) - aleatorio_propio(0,rooms[rand].altura));
			if ((dungeon[a][b] != '#') && (dungeon[a][b] != 'h') && (dungeon[a][b] != 'm') && (dungeon[a][b] != 'T'))
			{
				dungeon[a][b] = '*';
				alta = 1;
			}
		}
	}
}

void spawn_questions(){
	int i,rand,a,b,alta;
	for (i=0;i<preguntas;i++)
	{
		alta=0;
		while (!alta)
		{
			rand = aleatorio_propio(0,6);
			a = rooms[rand].coordx + (aleatorio_propio(0,rooms[rand].lado) - aleatorio_propio(0,rooms[rand].lado));
			b = rooms[rand].coordy + (aleatorio_propio(0,rooms[rand].altura) - aleatorio_propio(0,rooms[rand].altura));
			if ((dungeon[a][b] != '#') && (dungeon[a][b] != 'h') && (dungeon[a][b] != 'm') && (dungeon[a][b] != 'T') && (dungeon[a][b] != '*') && (dungeon[a][b] != 'p'))
			{
				dungeon[a][b] = 'p';
				alta = 1;
			}
		}
	}
}

void spawn_exit() {
	int max,i,dist,faress,a,b,alta;
	max = 0;
	for (i=0;i<habitaciones; i++)
	{	
		dist = sqrt(pow((rooms[i].coordx-hero.coordx),2)+pow((rooms[i].coordy-hero.coordy),2));
		if ( max < dist)
		{
			max = dist;
			faress = i;
		} 
	}
	alta=0;
	while (!alta)
	{
		a = rooms[faress].coordx + (aleatorio_propio(0,rooms[faress].lado) - aleatorio_propio(0,rooms[faress].lado));
		b = rooms[faress].coordy + (aleatorio_propio(0,rooms[faress].altura) - aleatorio_propio(0,rooms[faress].altura));
		if ((dungeon[a][b] != '#') && (dungeon[a][b] != 'h') && (dungeon[a][b] != 'm') && (dungeon[a][b] != 'T') && (dungeon[a][b] != '*') && (dungeon[a][b] != 'p'))
		{
			dungeon[a][b] = 'S';
			alta = 1;
		}
	}
}

void cargar_preguntas(){
	preguntas_aed[0][0] = "Cual es el metodo para recorrer archivos mas rapido?";
	preguntas_aed[0][1] = "indexado";
	preguntas_aed[1][0] = "Cual es la forma extensa del if stament en lenguaje C? (escribir todo junto separado por guiones -)";
	preguntas_aed[1][1] = "if-elseif-else";
	preguntas_aed[2][0] = "Que es mas rapido un algoritmo se ejecuta a una velocidad lineal o logaritmica? (responder con 1 o 2)";
	preguntas_aed[2][1] = "2";
	preguntas_aed[3][0] = "tipo de dato numerico simple en pseudocodigo";
	preguntas_aed[3][1] = "entero";
	preguntas_aed[4][0] = "accion usada para input en pseudocodigo";
	preguntas_aed[4][1] = "leer";
	preguntas_aed[5][0] = "accion para tener un output en pseudocodigo";
	preguntas_aed[5][1] = "esc";
	preguntas_aed[6][0] = "una memoria que se ocupa y desocupa en tiempo de ejecucion, como se llamaria?";
	preguntas_aed[6][1] = "dinamica";
	preguntas_aed[7][0] = "tipo de dato nuevo usado para navegar entre listas";
	preguntas_aed[7][1] = "puntero";
	preguntas_aed[8][0] = "lenguaje de programacion en el que fue escrito este algoritmo";
	preguntas_aed[8][1] = "c";
	preguntas_aed[9][0] = "en que almacenamiento se guarda un archivo para que perdure en el tiempo, decir solamente tipo";
	preguntas_aed[9][1] = "externo";
}
/* crear una salida que a los tantos puntos se habilite poder usarla y asi salis y termina el juego*/

/* las preguntas las voy a resolver de alguna manera*/
/* tengo que hacer un mejor menu al inicio con glosario y eso antes de empezar a jugar*/
/* tengo que solucionar que hago cuando 2 monstruos colisionan por alguna razon se bugean*/
/* el daño quien lo toma */
/* dar puntos al jugador por matar un monstruo*/

 void guardar_puntaje() { 
	FILE *fr;
	char numero[4];
	fr = fopen("puntaje.txt","a");
	itoa(hero.points, numero,10);
	if (fr != NULL)
	{
		fputc(' ', fr);
		fputs(numero,fr);		
	}
	else
	{
		printf("puntaje no guardado");
	}
	fflush(fr);
	fclose(fr);
}

void mostrar_puntaje() {
	char cadena[4];
	int numero;
	FILE *fp;
	fp = fopen("puntaje.txt","r");
	if (fp == NULL)
	{
		perror("lo siento no se pudieron mostrar los puntajes");
	}
	else
	{
		prim = NULL;
		while ((feof(fp)) == 0)
		{
				
				/* cargar lista */
				fscanf(fp, "%s", cadena);
				numero = atoi(cadena);
				p = malloc(sizeof(struct nodo));
				p->puntaje = numero;
				q = prim;
				a= NULL;
				while ((q != NULL) && (q->puntaje > numero))
				{
					a=q;
					q= q->prox;
				}
				if (a == NULL)
				{
					/*primer elemento*/ /*lista vacia*/
					p->prox = prim;
					prim = p;
				}
				else
				{
					/*caso intermedio*/ /*ultimo elemento*/
					p->prox = q;
					a->prox = p;
				}
		}
		numero = 0;
		p = prim;
		printf("Top 5 highest score: ");
		printf("\n");
		while ((p != NULL) && (numero<5))
		{
			numero += 1;
			printf("%d", p->puntaje);
			printf("\n");
			p= p->prox;
		}
		while (prim != NULL)
		{
			q = prim;
			prim = q->prox;
			free(q);
		}
	}
	fflush(fp);
	fclose(fp);
}
