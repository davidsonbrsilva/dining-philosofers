#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "phil.c"

int main(int argc, char* argv[])
{
    // Declara e inicializa o ambiente
    env_t env;
    env_init(&env);

    // Passa os argumentos do programa para o ambiente
    for(int i = 0; i < argc; i++)
    {
        if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
        {
            printf("------------\n");
            printf("Jantar dos filósofos\n");
            printf("------------\n");
            printf("--help | -h\tExibe a ajuda e sai.\n");
            printf("--tot  | -t\tQuantidade de filósofos que serão criados. O padrão é 5.\n");
            printf("--run  | -r\tTempo máximo de execução em segundos de cada thread. O programa encerra quando todas terminam o processamento. Se definido como zero, executa sem limite de tempo (padrão).\n");
            printf("--min  | -i\tLimite inferior do intervalo de tempo gerado aleatoriamente (Padrão = 1).\n");
            printf("--max  | -m\tLimite suprior do intervalo de tempo gerado aleatoriamente (Padrão = 5).\n");

            return 0;
        }

        if(strcmp(argv[i], "--tot") == 0 || strcmp(argv[i], "-t") == 0)
        {
            int value = atoi(argv[i + 1]);
            env.total_phil = value;
        }
        else if(strcmp(argv[i], "--run") == 0 || strcmp(argv[i], "-r") == 0)
        {
            int value = atoi(argv[i + 1]);
            env.max_runtime = value;
        }
        else if(strcmp(argv[i], "--max") == 0 || strcmp(argv[i], "-m") == 0)
        {
            int value = atoi(argv[i + 1]);
            env.max_rand_time = value;
        }
        else if(strcmp(argv[i], "--min") == 0 || strcmp(argv[i], "-i") == 0)
        {
            int value = atoi(argv[i + 1]);
            env.min_rand_time = value;
        }
    }

    // Declara e inicializa os filósofos
    phil_t phils[env.total_phil];
    env.phils = phils;
    phil_init_all(&env);

    // Declara as threads
    pthread_t phil_threads[env.total_phil];
    pthread_t env_thread;

    // Declara os argumentos da função
    args_t args[env.total_phil];

    // Guarda o tempo de início da execução na variável start_time
    time(&env.start_time);

    // Declara variável de sinalização de erro que será utilizada
    int error = 0;

    // Cria as threads dos filósofos
    for(int i = 0; i < env.total_phil; i++)
    {
        args[i].env = &env;
        args[i].phil_i = i;

        error = pthread_create(&(phil_threads[i]), NULL, phil_thread_func, &(args[i]));

        if(error) 
        {
            printf("Erro ao criar a thread do filósofo %i. O erro retornou o status %i.\n", i, error);
            exit(EXIT_FAILURE);
        }
    }

    // Cria a thread de exibição
    error = pthread_create(&(env_thread), NULL, env_thread_func, &(env));

    if(error) 
    {
        printf("Erro ao criar a thread de exibição. O erro retornou o status %i.\n", error);
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < env.total_phil; i++)
    {
        error = pthread_join(phil_threads[i], NULL);

        if(error) 
        {
            printf("Erro ao processar a thread do filósofo %i. O erro retornou o status %i.\n", i, error);
            exit(EXIT_FAILURE);
        }
    }

    env_show(&env);

    return EXIT_SUCCESS;
}