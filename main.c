#define _POSIX_C_SOURCE 199309L
#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <time.h>


pthread_t Hilo1; 
pthread_t HiloM[4]; 
unsigned long counterSimple=0; 
unsigned long counterMulti=0;
int matrix[100][1000];
pthread_mutex_t lock; 

typedef struct 
{
    int id;
    double duracionMultiple;
    unsigned long sumaHilo;
} ParmHilo;

ParmHilo *argumentos[4];


void llenarMatrix(int low,int up){
    int lower = low;
    int upper = up;
    srand(time(NULL));
    for(int i=0; i<100; i++){
        for(int j=0; j<1000; j++){
            matrix[i][j] = (rand() % (upper - lower + 1)) + lower;
        }
    }
}

void* HiloUnico(void* arg) 
{ 

	printf("Hilo simple comienza\n"); 

    struct timespec Inicio, Fin;
    double duracion;
    
    clock_gettime(CLOCK_MONOTONIC, &Inicio);

    for(int i=0; i<100; i++){
        for(int j=0; j<1000; j++){
            counterSimple+=matrix[i][j];
        }
    } 
    
    clock_gettime(CLOCK_MONOTONIC, &Fin);

    duracion = (Fin.tv_sec - Inicio.tv_sec) + (Fin.tv_nsec - Inicio.tv_nsec) / 1000000000.0;

	printf("Hilo simple Termina\n");
    
    printf("Hilo simple suma: %lu duracion: %f \n", counterSimple,duracion);

	return NULL; 
} 

void* HiloMutex(void* arg) 
{ 
    pthread_mutex_lock(&lock); 
    ParmHilo *p=(ParmHilo *)arg;
	printf("Hilo multiple %d comienza\n",p->id); 

    struct timespec Inicio, Fin;
    double duracion;

    clock_gettime(CLOCK_MONOTONIC, &Inicio);
    int temp=(p->id-1)*25;
    
    for(int i=temp; i<temp+25; i++){
        for(int j=0; j<1000; j++){
            p->sumaHilo+=matrix[i][j];
            counterMulti+=matrix[i][j];
        }
    }
    
    clock_gettime(CLOCK_MONOTONIC, &Fin);

    duracion = (Fin.tv_sec - Inicio.tv_sec) + (Fin.tv_nsec - Inicio.tv_nsec) / 1000000000.0;

    p->duracionMultiple=duracion;

	printf("Hilo multiple %d termino\n",p->id); 
    
    printf("Hilo multiple %d sumaHilo: %lu duracionHilo: %f \n", p->id,p->sumaHilo,duracion);

    pthread_mutex_unlock(&lock);
	return NULL; 
} 

int main(void) 
{ 
    printf("Ingrese el valor superior e inferior del rango de valores para las posiciones de la matriz\n");
    printf("Valor superior:");
    int up=100;
    if (scanf("%d", &up)!=1){
        printf("Se pone valor default 100\n");
        up=100;
        while (getchar() != '\n');
    }
    printf("Valor inferior: ");
    int low=100;
    if (scanf("%d", &low)!=1){
        printf("Se pone valor default 1\n");
        low=1;
        while (getchar() != '\n');
    }
    printf("------------------Fin Configuracion --------------------\n");

    llenarMatrix(low,up);

    // printf("Matriz generada\n");
    // for(int i=0; i<100; i++){
    //     for(int j=0; j<1000; j++){
    //         printf("%d ", matrix[i][j]);
    //     }
    //     printf("\n");
    // }

    // ------------------Hilo simple --------------------

    printf("------------------Hilo simple --------------------\n");

    int resultado = pthread_create(&Hilo1, NULL, HiloUnico, NULL);
    if (resultado != 0) {
        perror("Error al crear el hilo simple \n");
        return EXIT_FAILURE;
    }

    resultado = pthread_join(Hilo1, NULL);
    if (resultado != 0) {
        perror("Error al esperar al hilo simple");
        return EXIT_FAILURE;
    }

    printf("------------------ Fin Hilo simple --------------------\n");
    // ------------------Hilo multiple --------------------

    printf("\n------------------Hilo Multiple --------------------\n");
    for(int i=0; i<4; i++){
        argumentos[i]=malloc(sizeof(ParmHilo));
        argumentos[i]->id=i+1;
        argumentos[i]->sumaHilo=0;
        argumentos[i]->duracionMultiple=0;
    }

    if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("Error al crear el muter\n"); 
        return EXIT_FAILURE; 
    } 
    
    int error[4];

    for (int i = 0; i < 4; i++)
    {
        error[i]=pthread_create(&HiloM[i], NULL, HiloMutex, argumentos[i]);
        if (error[i] != 0) {
            printf("Error al crear el hilo multiple %d\n",i+1);
        }
    }

    for (int i = 0; i < 4; i++)
    {
        error[i]=pthread_join(HiloM[i], NULL);
        if (error[i] != 0) {
            printf("Error al esperar al hilo multiple %d\n",i+1);
        }
    }
    float durationMultiple=0;
    for (int i = 0; i < 4; i++)
    {
        durationMultiple+=argumentos[i]->duracionMultiple;
    }
    printf("\n------------------Resultado Hilo Multiple--------------------\n");


    printf("Hilo multiple suma total: %lu duracion total %f \n", counterMulti,durationMultiple);
    printf("------------------Fin Hilos multiples --------------------\n");
    pthread_mutex_destroy(&lock); 


	return 0; 
} 
