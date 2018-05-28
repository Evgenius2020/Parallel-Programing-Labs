#ifndef INFO
#define INFO

// Local values of process variables. Used to synchronize process threads.
typedef struct Info
{
    unsigned comm_id;
    unsigned comm_size;
    unsigned *tasks;
    unsigned *last_task;
    unsigned *current_task;
    unsigned *is_waiting;
    unsigned *is_poolComplete;
} Info;

#endif