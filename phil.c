#include "phil.h"

/*
Funções dos filósofos
*/
void* phil_thread_func(void* arguments) 
{
    args_t* args = (args_t*)arguments;

    env_t* env = args->env;
    int phil_i = args->phil_i;
    phil_t* phil = &env->phils[phil_i];

    time_t start_time;
    time_t end_time;

    while(1)
    {   
        time(&start_time);
        phil_think(phil);
        time(&end_time);
        phil->total_thinking += difftime(end_time, start_time);

        time(&start_time);
        phil_take_hashi(env, phil_i);
        time(&end_time);
        phil->total_hungry += difftime(end_time, start_time);

        time(&start_time);
        phil_eat(phil);
        time(&end_time);
        phil->total_eating += difftime(end_time, start_time);
        phil->times_he_ate++;

        phil_put_hashi(env, phil_i);

        if(env->max_runtime > 0)
        {
            if(env->runtime >= env->max_runtime)
            {
                break;
            }
        }
    }

    pthread_exit((void*) 0);
}

void phil_init(env_t* env, int phil_i)
{
    phil_t* phil = &env->phils[phil_i];

    phil->id = phil_i + 1;
    phil->status = THINKING;
    phil->times_he_ate = 0;
    phil->total_eating = 0;
    phil->total_hungry = 0;
    phil->total_thinking = 0;
    phil->next_eating = 0;
    phil->next_thinking = env_rand_time(env);
    sem_init(&phil->sem, 0, 0);
}

void phil_init_all(env_t* env)
{
    for(int i = 0; i < env->total_phil; i++)
    {
        phil_init(env, i);
    }
}

void phil_show(phil_t* phil)
{
    if(phil->status == THINKING)
    {
        printf("  %i\t\tpensando\t %i\t\t %i\t\t %i\t\t%i\n", phil->id, phil->total_thinking, phil->total_hungry, phil->total_eating, phil->times_he_ate);
    }
    else if(phil->status == HUNGRY)
    {
        printf("  %i\t\tfaminto\t\t %i\t\t %i\t\t %i\t\t%i\n", phil->id, phil->total_thinking, phil->total_hungry, phil->total_eating, phil->times_he_ate);
    }
    else if(phil->status == EATING)
    {
        printf("  %i\t\tcomendo\t\t %i\t\t %i\t\t %i\t\t%i\n", phil->id, phil->total_thinking, phil->total_hungry, phil->total_eating, phil->times_he_ate);        
    }
}

void phil_think(phil_t* phil)
{
    sleep(phil->next_thinking);
}

void phil_take_hashi(env_t* env, int phil_i)
{
    sem_wait(&env->mutex);
    env->phils[phil_i].status = HUNGRY;
    phil_try(env, phil_i);
    sem_post(&env->mutex);
    sem_wait(&env->phils[phil_i].sem);
}

void phil_try(env_t* env, int phil_i)
{
    phil_t* phils = env->phils;
    int left = phil_get_left(env, phil_i);
    int right = phil_get_right(env, phil_i);

    if(phils[phil_i].status == HUNGRY && phils[left].status != EATING && phils[right].status != EATING)
    {
        sem_post(&phils[phil_i].sem);
        phils[phil_i].next_eating = env_rand_time(env);
        phils[phil_i].status = EATING;
    }
}

void phil_eat(phil_t* phil)
{
    sleep(phil->next_eating);
}

void phil_put_hashi(env_t* env, int phil_i)
{
    int phil_left = phil_get_left(env, phil_i);
    int phil_right = phil_get_left(env, phil_i);

    sem_wait(&env->mutex);
    env->phils[phil_i].next_thinking = env_rand_time(env);
    env->phils[phil_i].status = THINKING;
    phil_try(env, phil_left);
    phil_try(env, phil_right);
    sem_post(&env->mutex);
}

int phil_get_left(env_t* env, int phil_i)
{
    return (phil_i + env->total_phil - 1) % env->total_phil;
}

int phil_get_right(env_t* env, int phil_i)
{
    return (phil_i + 1) % env->total_phil;
}

/*
Funções do ambiente
*/
void* env_thread_func(void* arguments)
{
    while(1)
    {
        env_t* env = (env_t*) arguments;
        time_t end_time;

        time(&end_time);

        env->runtime = difftime(end_time, env->start_time);

        env_show(env);
        sleep(1);
    }
}

void env_init(env_t* env)
{
    env->start_time = -1;
    env->max_rand_time = 5;
    env->max_runtime = 0;
    env->min_rand_time = 1;
    env->runtime = 0;
    env->total_phil = 5;
    env->phils = NULL;
    sem_init(&env->mutex, 0, 1);
}

void env_show(env_t* env)
{
    system("clear");
    printf("Execute com o parâmetro --help para ver a ajuda\n\n");
    printf("Executar cada thread ");

    if(env->max_runtime <= 0)
    {
        printf("infinitamente");
    }
    else
    {
        printf("por %i segundos", env->max_runtime);
    }

    printf(" | Tempo aleatório gerado entre %i e %i segundos\n", env->min_rand_time, env->max_rand_time);
    printf("Tempo total de execução: %i segundos\n\n", env->runtime);
    
    printf(" Filósofo\tStatus\t        Pensando\tFaminto\t        Comendo\t       Vezes Comeu\n");

    for(int i = 0; i < env->total_phil; i++)
    {
        phil_show(&env->phils[i]);
    }

    printf("\n");
}

int env_rand_time(env_t* env)
{
    srand(time(NULL));
    return (rand() % env->max_rand_time) + env->min_rand_time;
}
