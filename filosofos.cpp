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
#define maxEstomago 2

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

//Método para pensar
void pensar(char *nom);

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

//Comida
int comida = 10;

//Plato --true=lleno, --false=vacio
bool plato = true;

//Contador de restauranción de comida
int contComida = 0;

//Estómagos
int estomagos[NUM_FILOSOFOS];

//Mutex
pthread_mutex_t mutex;

//Nombres de los filósofos
char nomFilo[10][20] = {"Confucio", "Pitágoras", "Platón", "Sócrates", "Epicurio", "Tales", "Heráclito", "Diógenes", "Sófocles", "Zenón"};

int main(void){
	//Array de nombres de filósofos
	
	int i;
	int j;

	
	printf("Total de Comida: %d \n", comida);
	
	pthread_t filosofos[NUM_FILOSOFOS];
		
	//Mutex
	pthread_mutex_init(&mutex, NULL);
	
	for (i = 0; i < NUM_FILOSOFOS; i++){
		tenedores[i] = i;
		accion_Filo[i] = 0;
		esTenedor[i] = 0;
	}

	//Creacion de los filosofos
	for (i = 0; i < NUM_FILOSOFOS; i++){
		pthread_create(&filosofos[i], NULL, &comer, &nomFilo[i]);
		printf("CREANDO FILOSOFOS %s  \n", nomFilo[i]);
		sleep(1);
	}

	printf("Se crearon todos los filosofos\n");
	
	//Joint del arreglo de filosofos
	for (j = 0; j < NUM_FILOSOFOS; j++){

		pthread_join(filosofos[j], NULL);	
	}
	
	pthread_mutex_destroy(&mutex);
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

void pensar (char *nom){
	int pos = posicion(nom);
	estomagos[pos] -= 1;
	printf(".........%s está pensando........ estomago: %d\n", nom, estomagos[pos]);
}

//Acción principal del filósofo
void *comer (void *arg){

	char *nombre = (char *)arg;
	int pos = posicion(nombre);

	//Bucle infinito
	for(int i = 0; true; i++){	
		pthread_mutex_lock(&mutex);

		//Comienzan pensando
		pensar(nombre);
		//Levanta Tenedores
		bool com = tomarTenedor(nombre);
		if (com == true){
			sleep(5);
			if (comida <= 0){
				printf("\nSe terminó la Comida, filósofo %s repone", nombre);
				comida = 10;
				contComida++;
				printf("\nRestauró la comida %d veces\n\n", contComida);
			}
			while (estomagos[pos] != maxEstomago){
				estomagos[pos] += 1;
				comida--;
				printf("		%s estómago: %d\n", nombre, estomagos[pos]);
			}
			printf("		Filósofo %s lleno\n", nombre);
			printf("\n-------Comida: %d---------\n", comida);
			dejarTenedor(nombre);
		}
		/*int tenedorUsado = 0;
		for(int i = 0; i < NUM_FILOSOFOS; i++){
			if (esTenedor[i] = 1){
				tenedorUsado++;
			}
		}
		printf("\n____TenedorUsado %d_____\n", tenedorUsado);
		if (tenedorUsado == NUM_FILOSOFOS){
			for (int i = 0; i < NUM_FILOSOFOS; i++){
				esTenedor[i] = 0;
			}
			printf("Moderador:	Todos dejen los tenedores\n");
		}	*/
		pensar(nombre);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
	
	
	return NULL;
}
