#include <stdio.h>
#include "mpi.h"

int main(int argc, char **argv)
{

        int rank, size;

        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        if (rank == 0){


                int var = 0;
                MPI_Status status;


                printf("myrank = %d, var = %d \n", rank, var);
                MPI_Send(&var, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

                MPI_Recv(&var, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                var++;
                printf("myrank = %d, var = %d \n", rank, var) ;



        }
        else {

                int var = 0;
                MPI_Status status;

                MPI_Recv(&var, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                var++;
                printf("myrank = %d, var = %d \n", rank, var);

                MPI_Send(&var, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);




        }


        MPI_Finalize();

        return 0;
}
