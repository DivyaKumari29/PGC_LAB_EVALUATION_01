#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size;

    // Dataset
    int data[16] = {
        10, 20, 30, 40,
        50, 60, 70, 80,
        90, 100, 110, 120,
        130, 140, 150, 160
    };

    int local_data[4];

    int local_sum = 0;
    int local_max;
    int local_min;

    int total_sum;
    int global_max;
    int global_min;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Check that exactly 4 processes are used
    if (size != 4)
    {
        if (rank == 0)
        {
            printf("Please run the program using 4 MPI processes.\n");
        }

        MPI_Finalize();
        return 0;
    }

    // Distribute 4 elements to each process
    MPI_Scatter(data, 4, MPI_INT, local_data, 4, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate local statistics
    local_sum = 0;
    local_max = local_data[0];
    local_min = local_data[0];

    for (int i = 0; i < 4; i++)
    {
        local_sum += local_data[i];

        if (local_data[i] > local_max)
            local_max = local_data[i];

        if (local_data[i] < local_min)
            local_min = local_data[i];
    }

    // Combine results from all processes
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

    // Display results only from Master
    if (rank == 0)
    {
        double average = (double)total_sum / 16;

        printf("\n===== Distributed Dataset Statistics =====\n");
        printf("Dataset Size : 16\n");
        printf("MPI Processes: %d\n", size);
        printf("Sum          : %d\n", total_sum);
        printf("Average      : %.2f\n", average);
        printf("Maximum      : %d\n", global_max);
        printf("Minimum      : %d\n", global_min);
        printf("==========================================\n");
    }

    MPI_Finalize();

    return 0;
}
