#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <mpi.h>


#define r       "[rank=%d]"
#define PI    3.141592653589793238462643383279502884L 


const double a = 2;

const double x0 = 0;
const double x1 = 1;

const double t0 = 0;
const double t1 = 1;

double dx;
double dt;

double precise(double x, double t)
{
    if (x >= 2 * t)
        return x * t - t * t / 2 + cos(PI * (2 * t - x));
    else
        return x * t - t * t / 2 + (2 * t - x) * (2 * t - x) / 8 + exp(x / 2 - t);
}

void print_res(double* u, int x_size, int y_size)
{
    for(int y = y_size - 1; y >= 0; y--)
    {
        for (int x = 0; x < x_size; x++)
            printf("%10.4lf ", u[x*y_size + y]); 
        printf("\n");
    }
}

void print_column(int rank, double* u, int column, int x_size, int y_size)
{
    for (int i = 0; i < y_size; i++)
        printf(r"u[%d][%d] = %lf\n", rank, column, i, u[column*x_size + i]); 
}


void max_error(int rank, double* u, int x_size, int y_size)
{
    double max = -1;
    for(int y = y_size - 1; y >= 0; y--)
        for (int x = 0; x < x_size; x++)
        {
            double t = fabs(u[x*y_size+ y] - precise(x*dx, y*dt));
            if (t > max) max = t;
        }

    printf(r" max error: %10.8lf\n", rank, max);
}


void print_row(int rank, double* u, int row, int x_size, int y_size)
{ for (int i = 0; i < x_size; i++)
        printf(r"u[%d][%d] = %lf\n", rank, i, row, u[i*x_size + row]); 
}


double f(double x, double t)
{
    return x + t;
}

double psi(double t)
{
    return exp(-t);
}

double phi(double x)
{
    return cos(PI * x);
}



double left_corner(double u_0_0, double u_1_0, double f_val)
{
    return u_1_0 - dt * (2*(u_1_0 - u_0_0) / dx - f_val);
}

double cross(double u_0_1, double u_1_0, double u_2_1, double f_val)
{
    return  u_1_0 - dt*( 2 * (u_2_1 - u_0_1) / dx - 2 * f_val); 
}

int main(int argc, char * argv[])
{
    int commsize, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if(!rank) fprintf(stdout, r" commsize = %d\n", rank, commsize);

    dx = 0.0001;
    dt = dx / 2;

    MPI_Barrier(MPI_COMM_WORLD);

    double time;
    if(!rank) time = MPI_Wtime();

    int count_x = (int)round((x1 - x0) / dx + 1);
    int count_t = (int)round((t1 - t0) / dt + 1);

    int range = (int)floor((double)count_x / (double)commsize);

    int start  = rank * range;
    
    int finish = rank != commsize - 1 ? start + range - 1 : count_x - 1;

    range = finish - start + 1;


    //выделение памяти для потоков

    double* u = (double*)calloc(range * count_t, sizeof(double));
    if(u == NULL)
    {
        fprintf(stderr, r" Cannot allocate memory (double) [range=%d, count_t=%d, count_x=%d, start=%d, finish=%d]\n", 
                        rank, range, count_t, count_x, start, finish);
        exit(EXIT_FAILURE);
    }

    

    //Краевые условия

    for (int i = 0; i < range; i++)
    {
        u[i * count_t + 0] = phi((i + start) * dx + x0);
    }

    
    if (rank == 0)
        for (int i = 0; i < count_t; i++)
            u[0 * count_t + i] = psi(i * dt + t0);
    
    
    //Заполняем первый слой
    
    for (int i = 1; i < range; i++)
        u[i * count_t + 1] = left_corner(u[(i - 1)*count_t + 0], 
                                         u[i * count_t + 0],
                                         f((i + start) * dx + x0, 0));
    
    if (rank)
    {

        u[0 * count_t + 1] = left_corner(phi((start - 1) * dx + x0), 
                                         u[0 * count_t + 0],
                                         f(start * dx + x0, 0));
    }
    

    //Заполняем следующие слои
    
    for (int i = 1; i < count_t - 1; i++)
    {
        //Центральные точки

        for (int j = 1; j < range - 1; j++)
        {

            u[j*count_t + (i + 1)] = cross( u[(j-1)*count_t + i],
                                            u[j*count_t +(i-1)],
                                            u[(j+1)*count_t + i],
                                            f((j + start)*dx, i*dt)); 
        }


        //Первая и последняя точки

        if(rank != 0)
        {
            if (MPI_Send(&u[0*count_t + i], 1, MPI_DOUBLE, rank - 1, i, MPI_COMM_WORLD) != MPI_SUCCESS) // отправляем свою первую
            {
                fprintf(stderr, r" Failed to send to node %d\n", rank, rank + 1);
                exit(EXIT_FAILURE);
            }
        }

        if(rank != commsize - 1)
        {
            double u_out = 0;

            if (MPI_Recv(&u_out, 1, MPI_DOUBLE, rank + 1, i, MPI_COMM_WORLD, NULL) != MPI_SUCCESS) //получаем первую соседа
            {
                fprintf(stderr, r" Failed to recieve from node %d\n", rank, rank - 1);
                exit(EXIT_FAILURE);
            }

            u[(range - 1)*count_t + (i+1)] = cross( u[(range - 2)*count_t + i],
                                                    u[(range - 1)*count_t + i - 1],
                                                    u_out,
                                                    f(finish * dx + x0, i*dt));
        
        }
        else
            u[(range-1)*count_t + (i+1)] = left_corner(u[(range-2)*count_t + i],
                                                       u[(range-1)*count_t + i], 
                                                       f(finish * dx + x0, i*dt));

        
        if(rank != commsize - 1)
        {
            if (MPI_Send(&u[(range - 1)*count_t + i], 1, MPI_DOUBLE, rank + 1, i, MPI_COMM_WORLD) != MPI_SUCCESS)//отправляем свою последнюю
            {
                fprintf(stderr, r" Failed to send to node %d\n", rank, rank + 1);
                exit(EXIT_FAILURE);
            }
        }

        if(rank != 0)
        {
            double u_out = 0;

            if (MPI_Recv(&u_out, 1, MPI_DOUBLE, rank - 1, i, MPI_COMM_WORLD, NULL) != MPI_SUCCESS) //получаем последнюю соседа
            {
                fprintf(stderr, r" Failed to recieve from node %d\n", rank, rank - 1);
                exit(EXIT_FAILURE);
            }

            u[0*count_t + (i+1)] = cross(   u_out,
                                            u[0*count_t + i - 1],
                                            u[1*count_t + i],
                                            f(start * dx + x0, i*dt));
        }
    }

    
    double* res = NULL;
    int* counts = NULL;
    int* displs = NULL;

    if(rank == 0)
    {
        res = (double*)calloc(count_x*count_t, sizeof(double));

        if(res == NULL)
        {
            fprintf(stderr, r" Cannot allocate memory (double) [count_x=%d; count_t=%d]\n", 
                            rank, count_x, count_t);
            exit(EXIT_FAILURE);
        }
    }
    

    if(!rank)
    {
        counts = (int*)calloc(commsize, sizeof(int));
        displs = (int*)calloc(commsize, sizeof(int));

        for(int i = 0; i < commsize - 1; i ++)
        {
            counts[i] = range*count_t;
            displs[i] = range * i * count_t;
            //fprintf(stdout, r" counts[%d] = %d; displs[%d] = %d\n", rank, i, counts[i], i, displs[i]);
        }

        counts[commsize-1] = count_t * count_x - (commsize - 1) * range * count_t;
        displs[commsize-1] = range * (commsize - 1) * count_t;

        //fprintf(stdout, r" counts[%d] = %d; displs[%d] = %d\n", rank, commsize-1, counts[commsize-1], commsize-1, displs[commsize-1]);
    }
    
    MPI_Gatherv(u, range*count_t, MPI_DOUBLE, res, counts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    time = MPI_Wtime() - time;

    if(rank == 0)
    {
        fprintf(stdout, "\n");
        fprintf(stdout, r" time: %lf seconds\n", rank, time);
        fprintf(stdout, r" dx = %lf, dt = %lf\n", rank, dx, dt);
        fprintf(stdout, r" count_t = %d, count_x = %d\n", rank, count_t, count_x);
        max_error(rank, res, count_x, count_t);


        free(res);
        free(counts);
        free(displs);
    }

    free(u);
    

    MPI_Finalize();

    return 0;
}
