#include <stdlib.h>
#include "data_types.h"

void finalize_local_data(Local_Data local_data)
{
    free(local_data.phi_matrix);
    free(local_data.rho_matrix);
    free(local_data.Receive_Buffers.top);
    free(local_data.Receive_Buffers.bottom);
    free(local_data.Receive_Buffers.left);
    free(local_data.Receive_Buffers.right);
}