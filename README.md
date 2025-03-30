<center>
<h1> Hoja de trabajo #3 </h1> <br>
2011-14771 Otoniel Alexander Hernández Arias<br>  
Ingeniería en Ciencias y sistemas  <br>
Universidad de San Carlos de Guatemala.<br>  
Facultad de Ingeniería  <br>
Sistemas operativos 2 <br>
Primer Semestre 2025  <br>
28 de marzo de 2025  <br>
</center>

# Solucion Comparacion de rangos

![1](./Captura%20de%20pantalla%20de%202025-03-29%2022-37-47.png)

Al ejecutar el codigo llenamos la matriz con valores en el rango de 5 a 1 y podemos observar en la ejecucion del hilo simple como en el hilo multiple es la misma suma no difieren en los valores sumados, al ejecutar los hilos multiples con mutex podemos ver que cada vez que termina un hilo imprime lo que sumo el a su variable contador global y el tiempo que duro la ejecucion de dicho hilo al terminar los 4 hilos se suman solamente los tiempos de ejecucion de cada hilo para la ejecucion total.

En esta ejecucion podemos ver la siguiente tabla con la duracion de los hilos

| Duracion hilo simple | Duracion hilo multiple |
| -------------------- | ---------------------- |
| 0.000833 s           | 0.000797 s             |

En la mayoria de rangos pequeños casi siempre, tal vez en una proporcion de 6 de 10 veces el valor del hilos multiple es menor que la de un hilo simple.

Ahora con un rango mucho mas grande como mostramos en las tres siguiente capturas

![2](./Captura%20de%20pantalla%20de%202025-03-29%2022-39-21.png)

| Duracion hilo simple | Duracion hilo multiple |
| -------------------- | ---------------------- |
| 0.000169 s           | 0.000268 s             |

![3](./Captura%20de%20pantalla%20de%202025-03-29%2022-40-34.png)

| Duracion hilo simple | Duracion hilo multiple |
| -------------------- | ---------------------- |
| 0.000863 s           | 0.000780 s             |

![4](./Captura%20de%20pantalla%20de%202025-03-29%2022-40-52.png)

| Duracion hilo simple | Duracion hilo multiple |
| -------------------- | ---------------------- |
| 0.000175 s           | 0.000776 s             |

Podemos observar que solo una vez el hilo multiple tiene una menor duracion que el simple y esto pasa un poco mas seguido que con rangos pequeños.

# Observacion de rendimientos

Podemos ver que con rangos pequeños los hilos multiples duran en realizar el trabajo menos que un hilo simple pero ya en valores mas variados y mayores no pasa eso lo que si podemos observar cuando los valores son mas grandes dentro de los elementos de la matriz puede que la operacion de la suma tarde mas y eso hace una diferencia significativa en la tardanza entre la de un hilo simple y los multiples hilos. 

Otro elemento que puede estar afectan en la duracion mucho mayor de los hilos multiples que el de hilo simple es que al usar mutex los hilos se bloquean las recursos compartidos por lo que solo uno puede realizar la tarea a al vez, creando un cuello de botella, y que al momento de crear un hilo el sistema operativo es el que asigna al procesador para realizar la tarea, y dependiendo de prioridad y la posicion en las lista de ejecucion del planificador este se le asigna tiempo para la ejecucion entonces esa tardanza entre realizar la tarea y pedir tiempo de ejecucion en el planificador del sistema operativo al separar la tarea se tarda mas si no hay recursos disponibles, esto podria ser el amplia diferencia que tiene la ultima captura. 

# Discusion sobre multihilo

## Beneficios

- Los hilos multiples pueden ser beneficiosos para tareas pequeñas y rapidas, donde el paralelismos puede compensar la sobrecarga de la crecion y gestion de hilos
- El rendimiento de los hilos múltiples depende del tamaño de la tarea, la carga del sistema operativo y la disponibilidad de recursos
- Si una tarea se puede dividirse en subtares independientes se pueden aprovechar los recursos
- Pueden mejorar la eficiencia general de un programa y la capacidad de respuesta
- Si la tareas son mutuamente excluyentes los multihilos son mucho mejor que una hilo simple porque tiene una mayor eficiencia

## Inconvenientes

- Al usar multihilos que acceden a recursos compartidos es necesario un mecanismo como mutex para evitar condiciones de carrera pero esto introduce un carga mayor al sistemas debido a que los hilos deben esperar a que se terminen otros
- Si se usa mucho mecanismos de sincronizacion como mutex se corre el riesgo de que en vez de que sea paralelismo se convierte en selializacion.
- Si no usaramos un metodo de sincronizacion como mutex o los otros que existen tenemos el riesgo que los recursos compartidos se corrompan y no se realice bien la tarea
- Las tareas multihilos son dificiles de depurar ya que los errores no son los mismos porque son dependientes de la ejecucion
- El diseño para realizar una tarea en multihilo puede agregar complejidad porque no es lo mismo que una tarea se realice por una unidad de trabajo que por varias unidades de trabajo.
- Los hilos multiples cuando la tareas son grandes y complejas, con sincronizacion de mutex pueden superar los beneficios del paralelismo y empeorar el rendimiento


# Codigo en C
```c
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
    
    printf("Hilo multiple %d sumaHilo: %lu 
    duracionHilo: %f \n", p->id,p->sumaHilo,duracion);

    pthread_mutex_unlock(&lock);
	return NULL; 
} 

int main(void) 
{ 
    printf("Ingrese el valor superior e inferior del rango 
    de valores para las posiciones de la matriz\n");
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

```