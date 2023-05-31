#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "dynarray.h"

typedef struct client client;

struct client{
    int id;
    double arrival;
    double leave;
    client * next;
};

typedef struct{
    int size;
    client * first;
    client * last;
} queue;

double ** simulation(double, double, double, unsigned int, size_t *);
double sample_exponential(double);
double ** get_results(Array *, Array *);
Array * init_dynamic_array(size_t);
client * init_client(int, double);
queue * init_queue();
void enqueue(queue *, client *);
client * dequeue(queue *);

int main(){
    double T, lambda, mu;
    double ** results;
    unsigned int seed;
    int i, j;
    size_t size;

    lambda = (double) 1/4;
    mu = (double) 1/2;
    T = 1e3;
    seed = 123;

    results = simulation(T, lambda, mu, seed, &size);
    return 0;
}

double ** simulation(double T, double lambda, double mu, unsigned int seed, size_t * size){
    int id, i, n, is_serving;
    double x, t, departure_time, last_arrival, next_arrival;
    double ** results;
    client * c, * being_served;
    queue * q;
    Array * time, * system_size;

    q = init_queue();
    t = 0;
    departure_time = DBL_MAX;
    i = 0;
    is_serving = 0;
    next_arrival = sample_exponential(lambda);

    time = init_dynamic_array((size_t) 1); system_size = init_dynamic_array((size_t) 1);

    insertArray(time, 0); insertArray(system_size, q->size + is_serving);
    while(t < T){
        if(next_arrival < departure_time){
            t = next_arrival;
            c = init_client(i, t);
            enqueue(q, c);
            next_arrival += sample_exponential(lambda);
            i += 1;
        } else { 
            t = departure_time;
            is_serving = 0;
            free(being_served);
        }
        insertArray(time, t); insertArray(system_size, q->size + is_serving);

        if(1 - is_serving){
            if(q->size > 0){
                being_served = dequeue(q);
                departure_time = t + sample_exponential(mu);
                being_served->leave = departure_time;
                is_serving = 1;
            } else{
                departure_time = DBL_MAX;
            }
            
        }
    }

    c = q->first;
    client * next;
    while(c != NULL){
        next = c->next;
        free(c);
        c = next;
    }

    results = get_results(time, system_size);
    *size = time->used;

    return results;
}

double sample_exponential(double lambda){
    double z;                   
   if(lambda <= 0){
        return -1;
   } else{
        double u = (double)random()/(RAND_MAX);
        return -log(1-u)/lambda;
   }
}

Array * init_dynamic_array(size_t initial_size){
    Array * a = (Array *) malloc(sizeof(Array));
    if(a == NULL){
        return NULL;
    }
    initArray(a, initial_size);
    return a;
}

double ** get_results(Array * time, Array * system_size){
    int i, j, n;
    double ** results;

    n = time->used;
    results = (double **) malloc(n*sizeof(double *));
    if(results == NULL){
        return NULL;
    }
    for(i = 0; i < n; i++){
        results[i] = (double *) malloc(2*sizeof(double));
        if(results[i] == NULL){
            return NULL;
        }
    }

    for(i = 0; i < n; i++){
        results[i][0] = time->array[i];
        results[i][1] = system_size->array[i];
    }

    return results;
}

client * init_client(int id, double t){
    client * c;
    c = (client *) malloc(sizeof(client));
    if(c == NULL){
        return NULL;
    }
    c->id = id;
    c->arrival = t;
    c->leave = DBL_MAX;
    c->next = NULL;
    return c;
}

queue * init_queue(){
    queue * q;
    q = (queue *) malloc(sizeof(queue));
    if(q == NULL){
        return NULL;
    }
    q->size = 0;
    q->first = NULL;
    q->last = NULL;
    return q;
}

void enqueue(queue * q, client * c){
    if(q->first == NULL){
        q->first = q->last = c;
    } else{
        q->last->next = c;
        q->last = c;
    }
    q->size += 1;
    return;
}

client * dequeue(queue * q){
    client * first = q->first;
    if(q->first == q->last){
        q->first = NULL;
        q->last = NULL;
    } else{
        q->first = q->first->next;
    }
    q->size -= 1;
    return first;
}
