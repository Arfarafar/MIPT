#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>

int main(int argc, char **argv)
{

  int rank, size;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0){

    double sum = 0;
    double buffer;
    MPI_Status status;

    for(int i = 1; i < size; ++i)
    {

      MPI_Recv(&buffer, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      sum += buffer;
    }
    printf("sum %f", sum);

  }
  else {

      char* extstr;
      long N = strtol(argv[1], &extstr, 0);

      int n = (N - 1) / (size - 1) + 1;
      int ibeg = (rank - 1) * n + 1;
      int iend = rank * n;

      double sum = 0;
      for(int i = ibeg; i <= ((iend > N) ? N : iend); i++)
      {
        sum += 1.0 / (double)i ;
        printf("%f ", 1.0 / (double) i);
        fflush(0);
      }

      printf("\n N = %d begin = %d end = %d,  sum = %f \n", N, ibeg, (iend > N) ? N: iend, sum);
      MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }


  MPI_Finalize();

  return 0;
}
