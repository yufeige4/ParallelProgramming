/* File:       
 *    Pi_Estimation_MonteCarloSimulation_MPI.c
 *
 * Purpose:    
 *    A estimated pi calculation program by monte carlo simulation method that uses MPI
 *
 * Compile:    
 *    mpicc -g -Wall -o mpi_test Pi_Estimation_MonteCarloSimulation_MPI.c
 * Usage:        
 *    mpiexec -n<number of processes> ./mpi_test
 * 
 * Input:      
 *    total number of simulation points
 * Output:     
 *    estimated pi value
 *
 * Note: 
 * Algorithm:  
 *    used monte carlo simulation method to estimate pi.
 *    However, distribute the mission evenly into total_number/number_of_processes parts to each process to speed up the procedure.
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char* argv[]){
    int comm_sz, my_rank;

    long long total_count;
    long long shared_count;
    long long inside_count = 0;
    long long total_inside_count = 0;
    long long toss = 0;
    int i = 0;
    double x,y,squared_dist,pi_estimate;


    srand((unsigned)time(NULL));
    

    MPI_Init(&argc,&argv);
    // shared part 
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
    MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);


    if(my_rank==0){
        // handle input
        printf("Enter the total number of tosses: (entered number should be divisible by number of threads)\n");
        scanf("%lli",&total_count);
        shared_count =  total_count/comm_sz;
        // broadcast the shared count 
        for(i=1;i<comm_sz;++i){
            MPI_Send(&shared_count,1,MPI_LONG_LONG,i,0,MPI_COMM_WORLD);
        }
        while(toss<shared_count){
            ++toss;
            x = 2.0*rand()/RAND_MAX-1;
            y = 2.0*rand()/RAND_MAX-1;
            squared_dist = x*x + y*y;
            if(squared_dist<=1){
                ++inside_count;
            }
        }
    }else{
        MPI_Recv(&shared_count,1,MPI_LONG_LONG,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        while(toss<shared_count){
            ++toss;
            x = 2.0*rand()/RAND_MAX-1;
            y = 2.0*rand()/RAND_MAX-1;
            squared_dist = x*x + y*y;
            if(squared_dist<=1.0){
                ++inside_count;
            }
        }
    }
    // wait all processes to finish their calculation
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(&inside_count,&total_inside_count,1,MPI_LONG_LONG,MPI_SUM,0,MPI_COMM_WORLD);
    if(my_rank==0){
        pi_estimate = 4.0*total_inside_count/(double)total_count;
        fflush(stdout);
        printf("pi estimate = %.8f\n",pi_estimate);
    }

    MPI_Finalize();
    // end shared
    return 0;
}
