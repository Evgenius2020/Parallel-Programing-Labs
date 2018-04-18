#include <stdio.h>

#define X_max 10
#define X_step 0.1

double phi(int x)
{
    return x * x * x * X_step + 30 * x * x * X_step;
}

void main()
{
    double phi_v[X_max];
    double rho_v[X_max];
    unsigned i;
    printf("Printing phi vector:\n");
    for (i = 0; i < X_max; i++)
    {
        phi_v[i] = phi(i);
        printf("%.2e ", phi_v[i]);        
    }
    printf("\n\n");

    printf("Printing rho vector:\n");
    for (i = 0; i < X_max; i++)
    {
        double prev_phi = (i == 0) ? phi(-1) : phi_v[i - 1];
        double curr_phi = phi_v[i];
        double next_phi = (i == X_max - 1) ? phi(X_max) : phi_v[i + 1];
        rho_v[i] = (prev_phi - 2 * curr_phi + next_phi) / (X_step * X_step);
        printf("%.2e ", rho_v[i]);
    }
    printf("\n\n");

    printf("Printing phi vector, obtained from rho:\n");
    for (i = 0; i < X_max; i++)
    {
        double prev_phi = (i == 0) ? phi(-1) : phi_v[i - 1];
        double next_phi = (i == X_max - 1) ? phi(X_max) : phi_v[i + 1];
        phi_v[i] = (prev_phi + next_phi - (X_step * X_step) * rho_v[i]) / 2;
        printf("%.2e ", phi_v[i]);
    }
    printf("\n\n");
}