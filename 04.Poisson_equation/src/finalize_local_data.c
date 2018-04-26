#include <stdlib.h>
#include "data_types.h"

void finalize_local_data(Local_Data local_data)
{
    free(local_data.curr_matrix);
    free(local_data.Neighbors.top);
    free(local_data.Neighbors.bottom);
    free(local_data.Neighbors.left);
    free(local_data.Neighbors.right);
}