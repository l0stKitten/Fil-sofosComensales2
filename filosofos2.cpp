#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<semaphore.h>

//Definimos globalmente un número de filosofos
#define NUM_FILOSOFOS 5

//Definimos la capacidad máxima del estómago
#define maxEstomago 5

//Definir comida en el plato
#define comidaMax 10

// Metodo filosofo comer
void *comer (void *arg);

//Método para imprimir acción
void printAccion(int acc, char* nom, int posi);

//Método para encontrar la posición del Filósofo
int posicion(char* nom);

//Método para coger los tenedores
void tomarTenedor(char* nom, int pos);

//Método para dejar los tenedores
void dejarTenedor(char* nom, int pos);

//Método para pensar
void pensar(char *nom, int pos);

//Acciones de los filosofos
//0 = pensar
//1 = tomar tenedor derecho
//2 = tomar tenedor izquierdo
//3 = comer
int accion_Filo[NUM_FILOSOFOS];

//Max Energía de cada Filósofo
int energia[NUM_FILOSOFOS];

//contador de Energía
int contEnergia[NUM_FILOSOFOS];

//Comida
int comida = comidaMax;

//Contador de restauranción de comida
int contComida = 0;

//Estómagos
int estomagos[NUM_FILOSOFOS];

//Estado de los Filósofos 0 pensar / 3 comer
int estadoF[NUM_FILOSOFOS];

//Mutex
pthread_mutex_t mutex;

//Mutex / tenedores
pthread_mutex_t palillos[NUM_FILOSOFOS];

//Semáforo para la comida
sem_t comida_M;

//Semáforo para Energía
sem_t energia_M;

//Nombres de los filósofos
char nomFilo[10][20] = {"Confucio", "Pitágoras", "Platón", "Sócrates", "Epicurio", "Tales", "Heráclito", "Diógenes", "Sófocles", "Zenón"};

int main(void){
	
	int i;
	int j;

	//inicializamos la semilla del rand()
	srand(time(NULL));
	printf("Total de Comida: %d \n", comida);
	
	pthread_t filosofos[NUM_FILOSOFOS];
		
	//Mutex
	//pthread_mutex_init(&mutex, NULL);

	//Semáforo (semáforo, 0thread/1procesos, inicialización del semáforo)
	sem_init(&comida_M, 0, 1);	
	sem_init(&energia_M, 0, 1);

	for (i = 0; i < NUM_FILOSOFOS; i++){
		accion_Filo[i] = 0;
		//Energía de los filósofos es aleatoria entre 1 y 10
		energia[i] = (rand() % 10) + 1;
		contEnergia[i] = 0;

		estadoF[i] = 0;
		estomagos[i] = 1;
		pthread_mutex_init(&palillos[i], NULL);
	}

	//Creacion de los filosofos
	for (i = 0; i < NUM_FILOSOFOS; i++){
		pthread_create(&filosofos[i], NULL, &comer, &nomFilo[i]);
		printf("CREANDO FILOSOFOS %s ..... Energía %d \n", nomFilo[i], energia[i]);
		sleep(1);
	}

	printf("Se crearon todos los filosofos\n");
	
	//Joint del arreglo de filosofos
	for (j = 0; j < NUM_FILOSOFOS; j++){

		pthread_join(filosofos[j], NULL);	
	}
	
	pthread_mutex_destroy(&mutex);
	sem_destroy(&comida_M);
	sem_destroy(&energia_M);
	printf("Total Comida: %d \n", comida);

	return 0;
}

//Imprimir acción
void printAccion(int acc, char* nom, int posi){
	if (acc == 0){
		printf("%d Filósofo %s está pensando\n", posi, nom);
	}else if(acc == 1){
		printf("%d Filósofo %s levanta tenedor derecho\n", posi, nom);
	}else if (acc == 2){
		printf("%d Filósofo %s levanta tenedor izquierdo\n", posi, nom);
	}else if (acc == 3){
		printf("%d |||||   Filósofo %s debe comer    |||||\n", posi, nom);
	}
}

//Encontrar posición del filósofo
int posicion(char* nom){
	int pos = 0;
	for (int i = 0; i < NUM_FILOSOFOS; i++){
		if (nomFilo[i] == nom){
			pos = i;
			break;
		}
	}
	return pos;
}

//Tomar el tenedor
void tomarTenedor(char* nom, int pos){
	
	if (pos%2 == 0){
		pthread_mutex_lock(&palillos[pos]);
		printAccion(1, nom, pos);
		pthread_mutex_lock(&palillos[(pos+1)%5]);
		printAccion(2, nom, pos);

	} else {
		pthread_mutex_lock(&palillos[(pos+1)%5]);
		printAccion(2, nom, pos);
		pthread_mutex_lock(&palillos[pos]);
		printAccion(1, nom, pos);
	}	
}

//Dejar el tenedor
void dejarTenedor(char* nom, int pos){

	pthread_mutex_unlock(&palillos[pos]);
	pthread_mutex_unlock(&palillos[(pos + 1) % 5]);	
	
	printAccion(0, nom, pos);
}

void pensar (char *nom, int pos){
	int maxE = energia[pos] * (-1);
	estomagos[pos] -= 1;
	contEnergia[pos] -= 1;
	printf(".........%s está pensando........ estomago: %d\n		energía:  %d\n", nom, estomagos[pos], energia[pos]);
	if (contEnergia[pos] == maxE || estomagos[pos] == 0){
		estadoF[pos] = 3;
		printAccion(3, nom, pos);
	}
}

//Acción principal del filósofo
void *comer (void *arg){

	char *nombre = (char *)arg;
	int pos = posicion(nombre);

	//Bucle infinito
	for(int i = 0; true; i++){

		if (estadoF[pos] == 0){
			sem_wait(&energia_M);
			pensar(nombre, pos);
			sem_post(&energia_M);
			sleep(4);
		} else {
			tomarTenedor(nombre, pos);
			while (estomagos[pos] != maxEstomago || contEnergia[pos] == 0){
				sem_wait(&comida_M);
				estomagos[pos] += 1;
				contEnergia[pos] += 1;
				comida--;
				printf("		%s estómago: %d\n", nombre, estomagos[pos]);
				printf("		%s energía: %d\n", nombre, contEnergia[pos]);
				if (comida <= 0){
					printf("\nSe terminó la comida, filósfo %s respone", nombre);
					comida = comidaMax;
					contComida++;
					printf("\n se restauró la comida %d veces\n\n", contComida);
					
				}
				sem_post(&comida_M);
			}
			printf("	Filósofo %s lleno\n", nombre);
			printf("\n----------Comida %d----------\n", comida);
			dejarTenedor(nombre, pos);
			estadoF[pos] = 0;
			sleep(8);
			/*if ([pos] == 0){
				srand(time(NULL));
				energia[pos] = (rand() % 10) + 1;
			}*/
		}
	}
		
	return NULL;
}
