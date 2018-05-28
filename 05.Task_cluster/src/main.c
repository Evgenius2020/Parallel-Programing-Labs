#include "mpich/mpi.h"
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include "Info.h"

#define TASKS_POOL_SIZE 5

void *tasks_managing(void *info_raw);
void *tasks_execution(void *info_raw);

void main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    unsigned comm_id, comm_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_id);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    unsigned *tasks = malloc(sizeof(int) * TASKS_POOL_SIZE * comm_size);

    unsigned i;
    for (i = 0; i < TASKS_POOL_SIZE; i++)
    {
        tasks[i] = comm_id * TASKS_POOL_SIZE + i + 1;
    }

    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);
    pthread_t tasks_executor, tasks_manager;
    Info info;
    unsigned last_task = TASKS_POOL_SIZE - 1;
    unsigned current_task = 0;
    unsigned is_waiting = 0;
    unsigned is_pool_complete = 0;
    info.comm_id = comm_id;
    info.comm_size = comm_size;
    info.current_task = &current_task;
    info.last_task = &last_task;
    info.tasks = tasks;
    info.is_waiting = &is_waiting;
    info.is_poolComplete = &is_pool_complete;
    pthread_create(&tasks_executor, &attrs, tasks_execution, &info);
    pthread_create(&tasks_manager, &attrs, tasks_managing, &info);
    pthread_attr_destroy(&attrs);
    pthread_join(tasks_executor, NULL);
    pthread_join(tasks_manager, NULL);

    free(tasks);
    MPI_Finalize();
}