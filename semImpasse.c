#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h> /*utilizada para o calculo*/
#include <unistd.h>
#include <stdbool.h> /*utilizado para verificar se a colonia possui ambos ou nenhum recurso*/

#define num_recursos 3 /*define a quantidade de recursos de cada tipo (A & B)*/

#define numero_colonias 2 /*numero de colonias(threads)*/

typedef struct colonia{
    int id;    /*representa o id de cada colonia*/
    double P0; /*representa a população inicial*/
    double r;  /*representa a taxa de crescimento*/
    double t;  /*representa o tempo de crescimento*/
    pthread_mutex_t *recursoA; /*nutriente*/
    pthread_mutex_t *recursoB; /*área de cultivo*/
} coloniaBacterias;

/*P(t) = P0 ⋅ e^(rt) */
// exp é o número de euler elevado a (r*t)
double calculo_colonia(double P0, double r, double t) {
    return P0 * exp(r * t);
}

void* criacao_colonia(void* arg) {
    coloniaBacterias* colonia = (coloniaBacterias*)arg;
    bool obteve_recursoA = false;
    bool obteve_recursoB = false;
   
    //se 0 o recurso A não estava em posse de nenhuma outra colonia e portanto poderá ser obtido, 
    // se 1 já estava em posse de outra colonia
    if (pthread_mutex_lock(colonia->recursoA) == 0) {
        obteve_recursoA = true;  // Conseguiu o recurso A
        printf("\nColonia %d obteve o recurso A\n", colonia->id);
        sleep(1);  // simula o tempo para obter o recurso B
        if (pthread_mutex_lock(colonia->recursoB) == 0) {
            obteve_recursoB = true;  // Conseguiu o recurso B
            printf("Colonia %d obteve o recurso B\n", colonia->id);
        }
    }

    // Se obteve ambos os recursos, realiza o crescimento
    if ((obteve_recursoA == true) && (obteve_recursoB == true)) {
        printf("Colonia %d comecou a crescer!!! \n", colonia->id);
        double populacao_final_colonia = calculo_colonia(colonia->P0, colonia->r, colonia->t);
        printf("A populacao final da colonia %d : %f \n\n", colonia->id, populacao_final_colonia);

        // Liberando os recursos obtidos
        pthread_mutex_unlock(colonia->recursoA);
        pthread_mutex_unlock(colonia->recursoB);
    }

    pthread_exit(NULL);
}

int main() {
    coloniaBacterias colonias[numero_colonias];
    pthread_t threads[numero_colonias];
    pthread_mutex_t recursoA[num_recursos];
    pthread_mutex_t recursoB[num_recursos];

    // Inicializando os mutexes, sendo um para cada recurso
    for (int i = 0; i < num_recursos; i++) {
        pthread_mutex_init(&recursoA[i], NULL);
        pthread_mutex_init(&recursoB[i], NULL);
    }

    // Inicializando as colônias de bactérias e criando as threads
    for (int i = 0; i < numero_colonias; i++) {
        colonias[i].id = i;
        colonias[i].P0 = 10;
        colonias[i].r = 0.75;
        colonias[i].t = 1.5;
        colonias[i].recursoA = &recursoA[num_recursos % 2]; /*acessa o recurso 1*/
        colonias[i].recursoB = &recursoB[num_recursos % 1]; /*acessa o recurso 2*/
        pthread_create(&threads[i], NULL, criacao_colonia, (void*)&colonias[i]);
    }

    // Aguardando as threads terminarem
    for (int i = 0; i < numero_colonias; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destruindo os mutexes dos recursos
    for (int i = 0; i < num_recursos; i++) {
        pthread_mutex_destroy(&recursoA[i]);
        pthread_mutex_destroy(&recursoB[i]);
    }

    return 0;
}