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

    while (!done)
    {   if(rank==0){
        printf("Enter the number of points: (0 quits) \n");
        scanf("%d",&n);
        int rep,rep1;
        rep=n/numprocs;
        rep1=rep;
        for(int i=1;i<numprocs;i++){
            if(n==0){

            }
            else if(rep1<n){
                MPI_Send(&rep1,1,MPI_INT,i,0,MPI_COMM_WORLD);
            }
        }else{
            //poner receive
        }
        //simplemente repartir hasta n, y el resto que lo hagas el proceso 0, y al final que todos envien el numero de puntos al proceso 0
        if (n == 0) break;

        count = 0;  

        for (i = 1; i <= n; i++) {
            // Get the random numbers between 0 and 1
	    x = ((double) rand()) / ((double) RAND_MAX);
	    y = ((double) rand()) / ((double) RAND_MAX);

	    // Calculate the square root of the squares
	    z = sqrt((x*x)+(y*y));

	    // Check whether z is within the circle
	    if(z <= 1.0)
                count++;
        }


        pi = ((double) count/(double) n)*4.0;

        printf("pi is approx. %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
    }
}