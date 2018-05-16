#include "mpich/mpi.h"
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define TASKS_POOL_SIZE 5

typedef struct Info
{
    unsigned *last_task;
    pthread_mutex_t *mutex;
    unsigned comm_id;
    unsigned comm_size;
    unsigned *tasks;
    unsigned *additional_task;
    unsigned *isFinished;
} Info;

void *tasks_execution(void *info_raw)
{
    Info *info = info_raw;
    unsigned current_task = 0;

    while (current_task <= *(info->last_task))
    {
        printf("Proc[%d].Executor: Started a task (%d sec. aprox).\n", info->comm_id, info->tasks[current_task]);
        sleep(info->tasks[current_task]);
        current_task++;
        printf("Proc[%d].Executor: Finished a task.\n", info->comm_id);
    }
    printf("Proc[%d].Executor: Completed %d tasks.\n", info->comm_id, *(info->last_task) + 1);
    *(info->isFinished) = 1;
}

void *tasks_managing(void *info_raw)
{
    Info *info = info_raw;
    unsigned totalFinished;
    while (1)
    {
        MPI_Allreduce(info->isFinished, &totalFinished, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        if (totalFinished != info->comm_size)
        {
            if (*(info->isFinished))
            {
                printf("Proc[%d].Manager: Maybe I can help them ?..\n", info->comm_id);
            }
            sleep(1);
            continue;
        }
        break;
    }
}

void main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    unsigned comm_id, comm_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_id);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    unsigned *tasks = malloc(sizeof(int) * TASKS_POOL_SIZE);

    for (int i = 0; i < TASKS_POOL_SIZE; i++)
    {
        tasks[i] = comm_id * TASKS_POOL_SIZE + i + 1;
    }

    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);
    pthread_t tasks_executor, tasks_manager;
    Info info;
    unsigned last_task = TASKS_POOL_SIZE - 1;
    unsigned isFinished = 0;
    info.comm_id = comm_id;
    info.comm_size = comm_size;
    info.last_task = &last_task;
    info.tasks = tasks;
    info.isFinished = &isFinished;
    pthread_create(&tasks_executor, &attrs, tasks_execution, &info);
    pthread_create(&tasks_manager, &attrs, tasks_managing, &info);
    pthread_attr_destroy(&attrs);
    pthread_join(tasks_executor, NULL);
    pthread_join(tasks_manager, NULL);

    free(tasks);
    MPI_Finalize();
}