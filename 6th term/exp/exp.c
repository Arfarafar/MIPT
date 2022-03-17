#include <mpi.h>
#include <stdio.h>


int main(int argc, char *argv[]) {

        int rank, size;
        char* extstr;
        long N = strtol(argv[1], &extstr, 0);


        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        
        long ibeg = rank * N / size;
        long iend = (rank + 1) * N / size;

        
        long long factorial = 1;
        long double result = 0;

        long i;
        for (i = ibeg == 0 ? 1 : ibeg; i < iend; i++) {
              
                factorial *= i;
                result += (1.0 / (long double)(factorial));
        }
       

        

        if (rank != 0) {
            long long pfact;
            MPI_Recv(&pfact, 1, MPI_LONG_LONG, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            result /= (long double) pfact;
            factorial *= pfact;
            MPI_Send(&factorial, 1, MPI_LONG_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
            MPI_Send(&result, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
        }
        else {

              long double buffer;

              for(i = 1; i < size; ++i)
              {

                MPI_Recv(&buffer, 1, MPI_LONG_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                result += buffer;
              }
              printf("sum %lf", result);
        }

        
        
        MPI_Finalize();
        return 0;
}
