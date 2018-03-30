#ifndef PHIL_H
#define PHIL_H
#define HUNGRY 0
#define THINKING 1
#define EATING 2

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

/*
Convenções:
Prefixo "env"  : variável ou função relacionada ao ambiente (de "enviroment")
Prefixo "i"    : variável ou função que indica índice
Prefixo "max"  : variável que guarda valor máximo
Prefixo "min"  : variável que guarda valor mínimo
Prefixo "next" : variável que guarda valor a ser consumido
Prefixo "phil" : variável ou função relacionada a um filósofo (de "philosofer")
Prefixo "sem"  : variável ou função relacionada a um semáforo
Prefixo "total": variável de acumulação
*/

typedef struct
{
    int id;
    int next_eating;
    int next_thinking;
    int status;
    int times_he_ate;
    int total_eating;
    int total_hungry;
    int total_thinking;
    sem_t sem;
} phil_t;

typedef struct
{
    int max_rand_time;
    int max_runtime;
    int min_rand_time;
    int runtime;
    int total_phil;
    phil_t* phils;
    sem_t mutex;
    time_t start_time;
} env_t;

typedef struct
{
    env_t* env;
    int phil_i;
} args_t;

/*
Filósofos
*/
void* phil_thread_func(void*);
void phil_init(env_t*, int);
void phil_init_all(env_t*);
void phil_show(phil_t*);
void phil_think(phil_t*);
void phil_take_hashi(env_t*, int);
void phil_try(env_t*, int);
void phil_eat(phil_t*);
void phil_put_hashi(env_t*, int);
int phil_get_left(env_t*, int);
int phil_get_right(env_t*, int);

/*
Ambiente
*/
void* env_thread_func(void*);
void env_init(env_t*);
void env_show(env_t*);
int env_rand_time(env_t*);
#endif