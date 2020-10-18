#include <stdio.h>
#include <malloc.h>
#include <mpi.h>
#include <math.h>
const double eps = 0.000001;
const int n0 = 100;
double func(double x)
{
    return (1 - exp(0.7 / x)) / (2 + x);
}
int main(int argc, char **argv)
{
    int commsize, rank;
    int n = n0, k;
    double sq[2], delta = 1;

    int a = 1;
    int b = 2;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double time = MPI_Wtime();
    for (k = 0; delta > eps; n *= 2, k ^= 1) {
        int points_per_proc = n / commsize;
        int lb = rank * points_per_proc;
        int ub = (rank == commsize - 1) ? (n - 1) : (lb + points_per_proc - 1);
        double h = (b - a) / n;
        double s = 0.0;
        for (int i = lb; i <= ub; i++)
            s += func(a + h * (i + 0.5));
        MPI_Allreduce(&s, &sq[k], 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        sq[k] *= h;
        if (n > n0)
            delta = fabs(sq[k] - sq[k ^ 1]) / 3.0;
    }
    time = MPI_Wtime() - time;
    if (rank == 0) {
        //printf("Result Pi: %.12f; Runge rule: EPS %e, n %d\n Time: %f\n", sq[k] * sq[k], eps, n / 2, time);
        printf("Time: %f\n", time);
    }
    MPI_Finalize();
    return 0;
}