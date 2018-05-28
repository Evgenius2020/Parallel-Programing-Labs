#include <stdio.h>
#include <unistd.h>
#include "Info.h"

void *tasks_execution(void *info_raw)
{
    Info *info = info_raw;
    while (*(info->is_poolComplete) == 0)
    {
        if (*(info->current_task) <= *(info->last_task))
        {
            *(info->is_waiting) = 0;
#ifndef SILENT
            printf("Proc[%d].Executor: Started a task (%d sec. aprox).\n",
                   info->comm_id, info->tasks[*(info->current_task)]);
#endif
            sleep(info->tasks[*(info->current_task)]);
            *(info->current_task) += 1;
#ifndef SILENT
            printf("Proc[%d].Executor: Finished a task.\n", info->comm_id);
#endif
        }
        else
        {
            if (*(info->is_waiting) == 0)
            {
#ifndef SILENT

                printf("Proc[%d].Executor: Completed %d tasks. Waiting mode...\n",
                       info->comm_id, *(info->last_task) + 1);
#endif
                *(info->is_waiting) = 1;
            }
            else
            {
                sleep(1);
            }
        }
    }
#ifndef SILENT
    printf("Proc[%d].Executor: Tasks pool is complete!\n", info->comm_id);
#endif
}