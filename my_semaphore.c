#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <dlfcn.h>
#include <stdatomic.h>

int (*_sem_init)(sem_t *, int, unsigned int) = NULL;
int (*_sem_wait)(sem_t *) = NULL;
int (*_sem_post)(sem_t *) = NULL;
int (*_sem_getvalue)(sem_t *, int) = NULL;


/*
 *  Graph Definition
 */

typedef enum {resource, proccess} gType;

typedef struct nodeT *nodeS;

typedef struct nodeT{
    gType nodeType;
    int nodeCode;
    nodeS nextEdge;
};

typedef struct graphT *graphS;

typedef struct graphT{
    nodeS currNode;
    graphS nextNode;
};

/*
 *  Global Variables Definition
 */

graphS resourcesGraph = NULL;

/*
 *  Graph Functions Definition
 */


/* Returns a Graph G with a single node */
graphS createGraph(gType nodeType, int nodeCode){
    graphS G;

    G = malloc(sizeof(struct graphT));

    G->currNode = malloc(sizeof(struct nodeT));
    G->currNode->nextEdge = NULL;
    G->currNode->nodeCode = nodeCode;
    G->currNode->nodeType = nodeType;

    G->nextNode = NULL;

    return G;
}

/* Inserts a node into an already existing graph G */
int insertNode(graphS G, gType nodeType, int nodeCode){
    graphS aux;
    for(aux = G; aux->nextNode != NULL; aux = aux->nextNode){
        if(aux->currNode->nodeCode == nodeCode){
            //Node already exists
            perror("Error inserting node into graph: node already exists!");
            return -1;
        }
    }

    aux->nextNode = malloc(sizeof(struct graphT));

    aux->nextNode = malloc(sizeof(struct nodeT));
    aux->nextNode->currNode->nextEdge = NULL;
    aux->nextNode->currNode->nodeCode = nodeCode;
    aux->nextNode->currNode->nodeType = nodeType;

    aux->nextNode->nextNode = NULL;

    //Successfully added node
    return 0;
}

/* Inserts an edge into an already existing node with code K */
int insertEdge(graphS G, int nodeCode, gType newNodeType, int newNodeCode){
    graphS auxGraph;
    nodeS auxNode;

    //Finds right node for edge insertion
    for(auxGraph = G; auxGraph->currNode->nodeCode != nodeCode ; auxGraph = auxGraph->nextNode);

    //If node with code nodeCode exists
    if(auxGraph != NULL){
        //Finds last edge
        for(auxNode = auxGraph->currNode; auxNode->nextEdge != NULL; auxNode = auxNode->nextEdge);
        
        auxNode->nextEdge = malloc(sizeof(struct nodeT));

        auxNode->nodeType = newNodeType;
        auxNode->nodeCode = newNodeCode;
        auxNode->nextEdge = NULL;

        //Successfully added new edge
        return 0;
    }else {
        //Failed to add new edge: node doesnt exist
        perror("Failed to add new edge: node doesnt exist");
        return -1;
    }
}

int sem_init(sem_t *sem, int pshared, unsigned int value){
    /* Inicializa o semáforo apontado em sem, indicando seu valor inicial
    especificado em value. O argumento pshared indica se o semáforo será
    compartilhados entre threads de um processos (pshared=0) ou entre
    diferentes processos (pshared<>0)*/

    int r;

    if(!_sem_init){
        _sem_init = dlsym(RTLD_NEXT, "sem_init");
    }

    printf("\tDentro da sem_init()\n");

    if(resourcesGraph == NULL){
        //Adicionar semaforo para o grafo?
        resourcesGraph = createGraph(resource, sem);
    }else{
        
        
        r = _sem_init(sem, pshared, value);
        return r;
    }
}

int sem_wait(sem_t *sem){
    /* Decrementa (lock) o semáforo apontado em sem. Se valor do semáforo
    for maior que zero, decrementa. Se igual a zero, bloqueia quem chamou
    sem_wait(). */

    int r;
    if(!_sem_wait){
        _sem_wait = dlsym(RTLD_NEXT, "sem_wait");
    }

    printf("\tDentro da sem_wait()\n");
    r = _sem_wait(sem);
    return(r);
}

int sem_post(sem_t *sem){
    /* Incrementa (unlock) semáforo apontado em sem. Se valor do semáforo
    for zero, ao incrementar, desbloqueia outras threads/processos que
    estão bloqueados em sem_wait(). */

}

int sem_getvalue(sem_t *sem, int *sval){
    /* Copia o valor atual do semáforo apontado em sem para o inteiro
    apontado em sval. Chamada não é bloqueante. */


}
