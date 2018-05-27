#include "mpich/mpi.h"
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define TASKS_POOL_SIZE 3

#define WAITING_TASKS_CODE 0
#define NO_FREE_TASKS_CODE -1

typedef struct Info
{
    unsigned *last_task;
    unsigned *current_task;
    pthread_mutex_t *mutex;
    unsigned comm_id;
    unsigned comm_size;
    unsigned *tasks;
    unsigned *is_waiting;
    unsigned *is_poolComplete;
} Info;

void *tasks_execution(void *info_raw)
{
    Info *info = info_raw;
    while (*(info->is_poolComplete) == 0)
    {
        if (*(info->current_task) <= *(info->last_task))
        {
            *(info->is_waiting) = 0;
            printf("Proc[%d].Executor: Started a task (%d sec. aprox).\n",
                   info->comm_id, info->tasks[*(info->current_task)]);
            sleep(info->tasks[*(info->current_task)]);
            *(info->current_task) += 1;
            printf("Proc[%d].Executor: Finished a task.\n", info->comm_id);
        }
        else
        {
            if (*(info->is_waiting) == 0)
            {
                printf("Proc[%d].Executor: Completed %d tasks. Waiting mode...\n",
                       info->comm_id, *(info->last_task) + 1);
                *(info->is_waiting) = 1;
            }
            else
            {
                sleep(1);
            }
        }
    }
    printf("Proc[%d].Executor: Tasks pool is complete!\n", info->comm_id);
}

int get_sender_index(int *requests, unsigned comm_size, unsigned comm_id)
{
    unsigned i;
    unsigned senders_count;
    for (i = 0; i < comm_size; i++)
    {
        if ((requests[i] != WAITING_TASKS_CODE) && (requests[i] != NO_FREE_TASKS_CODE))
        {
            if (i == comm_id)
            {
                return senders_count;
            }
            senders_count++;
        }
    }
    return -1;
}

int get_receiver_index(int *requests, unsigned comm_size, unsigned comm_id)
{
    unsigned i;
    unsigned receivers_count = 0;
    for (i = 0; i < comm_size; i++)
    {
        if (requests[i] == WAITING_TASKS_CODE)
        {
            if (i == comm_id)
            {
                return receivers_count;
            }
            receivers_count++;
        }
    }
    return -1;
}

int get_nth_receiver_index(int *requests, unsigned comm_size, unsigned n)
{
    unsigned i;
    for (i = 0; i < comm_size; i++)
    {
        if (requests[i] == WAITING_TASKS_CODE)
        {
            if (n == 0)
            {
                return i;
            }
            n--;
        }
    }
    return -1;
}

int get_nth_sender_index(int *requests, unsigned comm_size, unsigned n)
{
    unsigned i;
    for (i = 0; i < comm_size; i++)
    {
        if ((requests[i] != WAITING_TASKS_CODE) && (requests[i] != NO_FREE_TASKS_CODE))
        {
            if (n == 0)
            {
                return i;
            }
            n--;
        }
    }
    return -1;
}

void *tasks_managing(void *info_raw)
{
    Info *info = info_raw;
    unsigned total_waiting;
    int *requests = malloc(sizeof(int) * info->comm_size);
    while (1)
    {
        // ======== If all procs are waiting, then pool is complete. ====================
        MPI_Allreduce(info->is_waiting, &total_waiting, info->comm_size, MPI_INT,
                      MPI_SUM, MPI_COMM_WORLD);
        if (total_waiting == info->comm_size)
        {
            break;
        }
        // ------------------------------------------------------------------------------

        // ======== Creating a vector of requests. ======================================
        int request;
        if (*(info->is_waiting))
        {
            request = WAITING_TASKS_CODE;
        }
        else
        {
            if (*(info->current_task) < *(info->last_task))
            {
                request = info->tasks[*(info->last_task)];
            }
            else
            {
                request = NO_FREE_TASKS_CODE;
            }
        }
        MPI_Allgather(&request, 1, MPI_INT, requests, 1, MPI_INT, MPI_COMM_WORLD);
        // ------------------------------------------------------------------------------

        // ======== Requests processing. ================================================
        if (*(info->is_waiting))
        {
            // Trying to find a sharing process for this process.
            int receiver_index =
                get_receiver_index(requests, info->comm_size, info->comm_id);
            int sender_index =
                get_nth_sender_index(requests, info->comm_size, receiver_index);
            if (sender_index != -1)
            {
                *(info->last_task) += 1;
                info->tasks[*(info->last_task)] = requests[sender_index];
                printf("Manager[%d]. Received a %d-task from Proc[%d]\n",
                       info->comm_id, sender_index, requests[sender_index]);
            }
        }
        else if (*(info->current_task) != *(info->last_task))
        {
            // Trying to find a receving process for this process.
            int sender_index =
                get_sender_index(requests, info->comm_size, info->comm_id);
            int receiver_index =
                get_nth_receiver_index(requests, info->comm_size, sender_index);
            if (receiver_index != -1)
            {
                *(info->last_task) -= 1;
                printf("Manager[%d]. Sent a %d-task to Proc[%d]\n",
                       info->comm_id, requests[info->comm_id], receiver_index);
            }
        }
        // ------------------------------------------------------------------------------

        sleep(1);
    }
    *(info->is_poolComplete) = 1;
    free(requests);
}

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