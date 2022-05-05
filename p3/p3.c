#include <stdio.h>
#include <sys/time.h>
#include <mpi.h>
#include <stdlib.h>
#define DEBUG 1



#define N 1024


int main(int argc, char *argv[] ) {
    int rank,numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int i, j;
    float *matrix_Aux = (float *) malloc(sizeof(float)*(N/numprocs)*N);
    float *vector=(float *) malloc(sizeof(float)*N);
    float *result=(float *) malloc(sizeof(float)*N);//este se podria optimizar un poco más el tamaño pero eso ya a mayores

    struct timeval  tv1, tv2,tv3,tv4,tv5,tv6,tv7;
    if(rank==0){
        float *matrix = (float *) malloc(sizeof(float)*N*N);
        /* Initialize Matrix and Vector */
        for(i=0;i<N;i++) {
        vector[i] = i;
            for(j=0;j<N;j++) {
                matrix[i*N+j] = i+j;
            }
        }
        gettimeofday(&tv5, NULL);
        MPI_Bcast(vector,N,MPI_FLOAT,0,MPI_COMM_WORLD);
        MPI_Scatter(matrix,(N/numprocs)*N,MPI_FLOAT,matrix_Aux,(N/numprocs)*N,MPI_FLOAT,0,MPI_COMM_WORLD);
        gettimeofday(&tv3, NULL);

        for(i=((numprocs*(N/numprocs))-1);i<N;i++) {//calcula el resto
            result[i]=0;
            for(j=0;j<N;j++) {
                result[i] += matrix[i*N+j]*vector[j];
            }
        }
        gettimeofday(&tv4, NULL);
        free(matrix);
    }else{
        gettimeofday(&tv3, NULL);
        MPI_Bcast(vector,N,MPI_FLOAT,0,MPI_COMM_WORLD);
        MPI_Scatter(NULL,0,MPI_FLOAT,matrix_Aux,(N/numprocs)*N,MPI_FLOAT,0,MPI_COMM_WORLD);
        gettimeofday(&tv4, NULL);
    }
    gettimeofday(&tv1, NULL);

    for(i=0;i<(N/numprocs);i++) {
        result[i]=0;
        for(j=0;j<N;j++) {
            result[i] += matrix_Aux[i*N+j]*vector[j];
        }
    }

    gettimeofday(&tv2, NULL);
    if(rank==0){
        float *result2=(float *) malloc(sizeof(float)*N);
        gettimeofday(&tv6, NULL);
        MPI_Gather(result,(N/numprocs),MPI_FLOAT,result2,(N/numprocs),MPI_FLOAT,0,MPI_COMM_WORLD);
        gettimeofday(&tv7, NULL);
        for(i=((numprocs*(N/numprocs))-1);i<N;i++){
            result2[i]=result[i];
        }
        int microseconds = (tv2.tv_usec - tv1.tv_usec)+ 1000000 * (tv2.tv_sec - tv1.tv_sec);
        int microseconds2 = (tv4.tv_usec - tv3.tv_usec)+ 1000000 * (tv4.tv_sec - tv3.tv_sec);
        int microseconds3 = (tv3.tv_usec - tv5.tv_usec)+ 1000000 * (tv3.tv_sec - tv5.tv_sec);
        int microseconds4 = (tv7.tv_usec - tv6.tv_usec)+ 1000000 * (tv7.tv_sec - tv6.tv_sec);


        //Display result
        if (DEBUG){
        for(i=0;i<N;i++) {
            printf(" %f \t ",result2[i]);
        }
        } else {
        printf ("Tiempo de trabajo del proceso %d (seconds) = %lf\n",rank, (double) (microseconds2+microseconds)/1E6);
        printf ("Tiempo comunicaciones del proceso %d (seconds) = %lf\n",rank, (double) (microseconds3+microseconds4)/1E6);
        }    
        free(result2);
        printf("\n");
    }else{
        gettimeofday(&tv6, NULL);
        MPI_Gather(result,(N/numprocs),MPI_FLOAT,NULL,0,MPI_FLOAT,0,MPI_COMM_WORLD);
        gettimeofday(&tv7, NULL);  
        int microseconds = (tv2.tv_usec - tv1.tv_usec)+ 1000000 * (tv2.tv_sec - tv1.tv_sec);
        int microseconds2 = (tv4.tv_usec - tv3.tv_usec)+ 1000000 * (tv4.tv_sec - tv3.tv_sec);
        int microseconds4 = (tv7.tv_usec - tv6.tv_usec)+ 1000000 * (tv7.tv_sec - tv6.tv_sec); 
        if(!DEBUG){
            printf ("Tiempo de trabajo del proceso %d (seconds) = %lf\n",rank, (double) (microseconds)/1E6);
            printf ("Tiempo comunicaciones del proceso %d (seconds) = %lf\n",rank, (double) (microseconds4+microseconds2)/1E6);
        }    
    }
    free(matrix_Aux);
    free(vector);
    free(result);
    MPI_Finalize();
    return 0;
}