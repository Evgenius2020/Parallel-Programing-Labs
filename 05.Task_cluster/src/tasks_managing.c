#include "mpich/mpi.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "Info.h"

#define WAITING_TASKS_CODE 0
#define NO_FREE_TASKS_CODE -1

int get_sender_index(int *requests, unsigned comm_size, unsigned comm_id);
int get_receiver_index(int *requests, unsigned comm_size, unsigned comm_id);
int get_nth_receiver_index(int *requests, unsigned comm_size, unsigned n);
int get_nth_sender_index(int *requests, unsigned comm_size, unsigned n);

void *tasks_managing(void *info_raw)
{
    Info *info = info_raw;
    unsigned total_waiting;
    int *tasks_left = malloc(sizeof(int) * info->comm_size);
    int *requests = malloc(sizeof(int) * info->comm_size);
    while (1)
    {
        // ======== Getting the number of pending tasks. ================================
        int task_left_curr = *(info->last_task) - *(info->current_task) + 1;
        MPI_Gather(&task_left_curr, 1, MPI_INT,
                   tasks_left, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (info->comm_id == 0)
        {
            unsigned i;
            for (i = 0; i < info->comm_size; i++)
            {
                printf("%d ", tasks_left[i]);
            }
            printf("\n");
        }
        // ------------------------------------------------------------------------------

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
#ifndef UNOPTIMIZED
            if (sender_index != -1)
            {
                *(info->last_task) += 1;
                info->tasks[*(info->last_task)] = requests[sender_index];
#ifndef SILENT
                printf("Manager[%d]. Received a %d-task from Proc[%d]\n",
                       info->comm_id, sender_index, requests[sender_index]);
#endif
            }
#endif
        }
        else if (*(info->current_task) != *(info->last_task))
        {
            // Trying to find a receving process for this process.
            int sender_index =
                get_sender_index(requests, info->comm_size, info->comm_id);
            int receiver_index =
                get_nth_receiver_index(requests, info->comm_size, sender_index);
#ifndef UNOPTIMIZED
            if (receiver_index != -1)
            {
                *(info->last_task) -= 1;
#ifndef SILENT
                printf("Manager[%d]. Sent a %d-task to Proc[%d]\n",
                       info->comm_id, requests[info->comm_id], receiver_index);
#endif
            }
#endif
        }
        // ------------------------------------------------------------------------------

        sleep(1);
    }

    *(info->is_poolComplete) = 1;
    free(tasks_left);
    free(requests);
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