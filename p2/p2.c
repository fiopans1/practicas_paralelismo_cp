#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int MPI_FlattreeColective(void *sendbuf, void *recvbuf, int count,MPI_Datatype datatype, MPI_Op op, int root,MPI_Comm comm){
    int error,rank,numprocs,rep;
    int *acu = recvbuf;
    MPI_Status status;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &numprocs);
    if(rank==root){
        *acu=0;
        for(int i=1;i<numprocs;i++){
            error = MPI_Recv(&rep,count,datatype,MPI_ANY_SOURCE, MPI_ANY_TAG,comm,&status);
            if(error!= MPI_SUCCESS){return error;}
            *acu +=rep;     
        }
        *acu+= *(int*) sendbuf;

    }else{
        error = MPI_Send(sendbuf,count,datatype,root,rank,comm);
        if(error!= MPI_SUCCESS){return error;}
    }
    return MPI_SUCCESS;

}
long ipow(long base, int exp)
{
    long res = 1;
    for (;;)
    {
        if (exp & 1)
            res *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return res;
}

int MPI_BinomialColectiva(void *buffer, int count, MPI_Datatype datatype,int root, MPI_Comm comm){
    int rank,numprocs,error;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &numprocs);
    MPI_Status status;
    if(rank!=0){
        error= MPI_Recv(buffer,count,datatype,MPI_ANY_SOURCE, MPI_ANY_TAG,comm,&status);
        if(error!=MPI_SUCCESS){
            return error;
        }
    }
    for(int i=0;;i++){
        if(rank<ipow(2,i)){
            if(rank+ipow(2,i)>=numprocs){
                return MPI_SUCCESS;
            }
            error=MPI_Send(buffer,count,datatype,rank+ipow(2,i),0,comm);
            if(error!=MPI_SUCCESS){
                return error;
            }

        }
    }
    return MPI_SUCCESS;
}

int main(int argc, char *argv[])
{
    int rank,numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int i, done = 0, n, count;
    double PI25DT = 3.141592653589793238462643;
    double pi, x, y, z;
    MPI_Status status;
    int rep,fin;

    while (!done)
    {   if(rank==0){
            printf("Enter the number of points: (0 quits) \n");
            scanf("%d",&n);
            //rep=0;
            fin=0;
            if(n!=0){
               rep=n/numprocs; 
            }else{
                rep=0;
            }
        }

        MPI_Bcast(&rep,1,MPI_INT,0,MPI_COMM_WORLD);
        //MPI_BinomialColectiva(&rep,1,MPI_INT,0,MPI_COMM_WORLD);

        if(rank==0){
            if(n!=0){//modificamos rep solo para 0
                rep=n-((n/numprocs)*(numprocs-1));
            }else{
                rep=0;
            }
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
        MPI_Reduce(&count,&fin,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
        //MPI_FlattreeColective(&count,&fin,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
        if(rank==0){
            pi = ((double) fin/(double) n)*4.0;

            printf("pi is approx. %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
        }

    }
    MPI_Finalize();
}