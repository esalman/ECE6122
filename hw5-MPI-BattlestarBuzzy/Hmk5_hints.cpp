// Demonstrate simple MPI program
// This one uses non-blocking ISend/Irecv
// George F. Riley, Georgia Tech, Fall 2011


#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

using namespace std;


int main(int argc, char**argv)
{
    int  numtasks, rank, rc;

    rc = MPI_Init(&argc, &argv);

    if (rc != MPI_SUCCESS)
    {
        printf("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Seed the random number generator to get different results each time
    srand(rank);
    if (rank == 0)
    {
        // Load in.dat file
        LoadInputFile()
    }

    // Broadcast to yellowjackets
    MPI_Bcast();

    
    // Loop through the number of time steps
    for (int round = 0; round < nTimeSteps; ++round)
    {
        if (rank == 0)
        {
            // Calculate Buzzy new location
            CalculateBuzzyXYZ();
            MPI_Allgather();
        }
        else
        {
            // Calculate yellow jacket new location
            CalculateYellowJacketXYZ();
            MPI_Allgather();
        }
    }

    MPI_Finalize();
}
