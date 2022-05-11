#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<semaphore.h>

//Definimos globalmente un número de filosofos
#define NUM_FILOSOFOS 5

// Metodo filosofo comer
void *comer (void *arg);

//Método para imprimir acción
void printAccion(int acc, char* nom, int posi);

//Método para encontrar la posición del Filósofo
int posicion(char* nom);

//Método para coger los tenedores
bool tomarTenedor(char* nom);

//Método para dejar los tenedores
void dejarTenedor(char* nom);

//Recurso principal
int variable = 0;

//Tenedores
int tenedores[NUM_FILOSOFOS];

//Estado de tenedores
int esTenedor[NUM_FILOSOFOS];

//Acciones de los filosofos
//0 = pensar
//1 = tomar tenedor derecho
//2 = tomar tenedor izquierdo
//3 = comer
int accion_Filo[NUM_FILOSOFOS];

//Mutex
pthread_mutex_t mutex;
//Semáforo
//sem_t mutex;

//Nombres de los filósofos
char nomFilo[10][20] = {"Confucio", "Pitágoras", "Platón", "Sócrates", "Epicurio", "Tales", "Heráclito", "Diógenes", "Sófocles", "Zenón"};

int main(void){
	//Array de nombres de filósofos
	
	int i;
	int j;

	//Estado de los nombres
	int estNom[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	
	printf("Variable: %d \n", variable);
	
	pthread_t filosofos[NUM_FILOSOFOS];
	//int identificadores[NUM_FILOSOFOS];
	
	//Mutex
	pthread_mutex_init(&mutex, NULL);
	//Semáforo
	//sem_init(&mutex, 0, 1);
	//Inicialización de los tenedores y estado de los filosofos
	for (i = 0; i < NUM_FILOSOFOS; i++){
		tenedores[i] = i;
		accion_Filo[i] = 0;
		esTenedor[i] = 0;
	}

	//Creacion de los filosofos
	for (i = 0; i < NUM_FILOSOFOS; i++){
		//Si el nombre no ha sido usado, NUM_FILOSOFOS < 10
		if (i%10 == 0){
			j = 0;
		}
		if (estNom[j] == 0){
			pthread_create(&filosofos[i], NULL, &comer, &nomFilo[i]);
			estNom[i] += 1;
		//Si el nombre ha sido usado, NUM_FILOSOFOS > 10
		}else{
			char num[1];
			snprintf(num, sizeof(num), "%d", estNom[j]);
			strcat(nomFilo[j], num);
			pthread_create(&filosofos[i], NULL, &comer, &nomFilo[j]);
			estNom[i] += 1;
		}
		j++;
		printf("CREANDO FILOSOFOS\n");
	}

	printf("Se crearon todos los filosofos\n");

	//Create Inicial
	//pthread_create(&epicurio, NULL, &filosofo, &filo);
	//pthread_create(&confucio, NULL, &filosofo, &filo2);

	//Llamada a la función filosofo, no es necesario, dado que 
	//es el comportamiento del thread.
	//filosofo(filo);
	//filosofo(filo2);
	
	//Joint del arreglo de filosofos
	for (i = 0; i < NUM_FILOSOFOS; i++){

		pthread_join(filosofos[i], NULL);	
	}
	//pthread_join(epicurio, NULL);
	//pthread_join(confucio, NULL);
	
	pthread_mutex_destroy(&mutex);
	printf("Variable: %d \n", variable);

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
		printf("%d Filósofo %s está comiendo\n", posi, nom);
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
bool tomarTenedor(char* nom){


	int pos = posicion(nom);
	bool tenedorD = false;
	bool tenedorI = false;
	bool comer = false;
	
	if (esTenedor[pos] == 0){
		esTenedor[pos] = 1;
		tenedorD = true;
		printAccion(1, nom, pos);
	}
	
	if (esTenedor[(pos+1)%5] == 0){
		esTenedor[(pos+1)%5] = 1;
		tenedorI = true;
		printAccion(2, nom, pos);
	}

	if (tenedorD == true && tenedorI == true){
		printAccion(3, nom, pos);
		comer = true;
	}else{
		printf("%d El filósofo %s no puede comer\n", pos, nom);
	}

	return comer;		
}

//Dejar el tenedor
void dejarTenedor(char* nom){
	int pos = posicion(nom);
	esTenedor[pos] = 0;
	esTenedor[(pos+1)%5] = 0;
	printAccion(0, nom, pos);
}

//Se hace uso del recurso compartido
void *comer (void *arg){

	char *nombre = (char *)arg;
	//int ident = *((int *)arg);
	/*for (int i=1; i<=3; i++){
		variable = variable + 1;
		printf("Filósofo %s está comiendo : %d . Variable = %d \n", nombre, i, variable);
	}*/

	//Bucle infinito
	for(int i = 0; true; i++){	
		//Levanta Tenedores
		pthread_mutex_lock(&mutex);
		bool com = tomarTenedor(nombre);
		if (com == true){
			sleep(1);
			dejarTenedor(nombre);
		}
		pthread_mutex_unlock(&mutex);
		sleep(2);
	}
	
	
	return NULL;
}
