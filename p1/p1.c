#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank,numprocs;
    MPI_Init(argc, argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int i, done = 0, n, count;
    double PI25DT = 3.141592653589793238462643;
    double pi, x, y, z;
    MPI_Status status;
    int rep,acu,fin;

    while (!done)
    {   if(rank==0){
            printf("Enter the number of points: (0 quits) \n");
            scanf("%d",&n);
            if(n!=0){
               rep=n/numprocs; 
            }else{
                rep=0;
            }
            for(int i=1;i<numprocs;i++){
                MPI_Send(&rep,1,MPI_INT,i,0,MPI_COMM_WORLD);
                acu=acu+rep;
            }
            rep=n-acu;//para los que queden

        }else{
            MPI_Recv(&rep,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
        }
        if (rep == 0) break;

        count = 0;  

        for (i = 1; i <= rep; i++) {
            // Get the random numbers between 0 and 1
            x = ((double) rand()) / ((double) RAND_MAX);
            y = ((double) rand()) / ((double) RAND_MAX);

            // Calculate the square root of the squares
            z = sqrt((x*x)+(y*y));

            // Check whether z is within the circle
            if(z <= 1.0){
                count++;
            }

        }

        if(rank==0){
            for(int i=1;i<numprocs;i++){
                MPI_Recv(&fin,1,MPI_INT,i,0,MPI_COMM_WORLD,&status);
                count+=fin;
            }
            pi = ((double) count/(double) n)*4.0;

            printf("pi is approx. %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
        }else{
            MPI_Send(&count,1,MPI_INT,0,0,MPI_COMM_WORLD);
        }

    }
}