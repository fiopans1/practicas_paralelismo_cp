#include <stdio.h>
#include <sys/time.h>
#include <mpi.h>
#include <stdlib.h>
#define DEBUG 1



#define N 512


int main(int argc, char *argv[] ) {
    int rank,numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int i, j;

    float *matrix = (float *) malloc(sizeof(float)*N*N);
    float *matrix_Aux = (float *) malloc(sizeof(float)*N*N);//luego adaptar tamaño de matrix_Aux a lo que recibe cada proceso
    float vector[N];
    float result[N];
    float result2[N];
    struct timeval  tv1, tv2;
    if(rank==0){
        /* Initialize Matrix and Vector */
        for(i=0;i<N;i++) {
        vector[i] = i;
            for(j=0;j<N;j++) {
                matrix[i*N+j] = i+j;
            }
        }
    }
    MPI_Bcast(&vector,N,MPI_FLOAT,0,MPI_COMM_WORLD);
    MPI_Scatter(matrix,(N/numprocs)*N,MPI_FLOAT,matrix_Aux,(N/numprocs)*N,MPI_FLOAT,0,MPI_COMM_WORLD);
    if(rank==0) gettimeofday(&tv1, NULL);

    for(i=0;i<(N/numprocs);i++) {
        result[i]=0;
        for(j=0;j<N;j++) {
            result[i] += matrix_Aux[i*N+j]*vector[j];
        }
    }

    if(rank==0) gettimeofday(&tv2, NULL);
    MPI_Gather(result,(N/numprocs),MPI_FLOAT,result2,(N/numprocs),MPI_FLOAT,0,MPI_COMM_WORLD);
    if(rank==0){
        int microseconds = (tv2.tv_usec - tv1.tv_usec)+ 1000000 * (tv2.tv_sec - tv1.tv_sec);

        //Display result
        if (DEBUG){
        for(i=0;i<N;i++) {
            printf(" %f \t ",result2[i]);
        }
        } else {
        printf ("Time (seconds) = %lf\n", (double) microseconds/1E6);
        }    
    }
    printf("\n");
    MPI_Finalize();
    return 0;
}