/*
Author:         Mustafa Salman
Class:          ECE 6122
Last modified:  10/30/2019

Description:    Battlestar Buzzy has just finished defending GaTech from the evil 
                space bulldogs. It was a fierce battle and only seven yellow jackets 
                are still operational and need to dock as soon as possible. The yellow 
                jackets are all damaged, flying blind, and having propulsion issues. 
                Our mission is to develop a distributed MPI program that will be 
                able to guide the yellow jackets safely back to Buzzy so that they can dock.
*/

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <iomanip>
using namespace std;

#define ACTIVE 1
#define DOCKED 2
#define DESTROYED 0

#define SLEN 7
#define SNUM 8

void loadInputFile( int &nTimeSteps, double &maxThrust, double xyzArray[], double speedVector[], double dirVector[] )
{
    int xyzCount = 0;
    int dirCount = 0;
    int speedCount = 0;

    ifstream fid ("sin.dat");
    
    fid >> nTimeSteps;
    fid >> maxThrust;

    for ( int i = 0; i < SNUM; i++ ) 
    {
        xyzArray[xyzCount++] = ACTIVE;           // status
        fid >> xyzArray[xyzCount++];        // x
        fid >> xyzArray[xyzCount++];        // y
        fid >> xyzArray[xyzCount++];        // z
        xyzArray[xyzCount++] = 0;           // Fx
        xyzArray[xyzCount++] = 0;           // Fy
        xyzArray[xyzCount++] = 0;           // Fz
        fid >> speedVector[speedCount++];   // speed
        fid >> dirVector[dirCount++];       // dir x
        fid >> dirVector[dirCount++];       // dir y
        fid >> dirVector[dirCount++];       // dir z
    }
    fid.close();
}

void CalculateBuzzyXYZ( double receiveArray[], double speedVector[], double dirVector[], double sendArray[] )
{
    // buzzy rank=0
    int idx = 0 * SLEN;
    
    // do nothing
    copy( receiveArray+idx, receiveArray+idx+7, sendArray );
}

void CalculateYellowJacketXYZ( double receiveArray[], int rank, double sendArray[] )
{
    // grab yj index from the receiveArray
    int idx = rank * SLEN;

    // do nothing
    copy( receiveArray+idx, receiveArray+idx+7, sendArray );
}

int main( int argc, char *argv[] )
{
    int  numtasks, rank, rc;
    int nTimeSteps;
    double maxThrust;
    double sendArray[SLEN];
    double receiveArray[SLEN*SNUM];
    double initXYZVector[3*SNUM];
    double initDirVector[3*SNUM];
    double initSpeedVector[SNUM];
    int precision = 6;

    cout.precision(precision);
    cout << scientific;

    rc = MPI_Init(&argc, &argv);
    if (rc != MPI_SUCCESS)
    {
        cout << "Error starting MPI program. Terminating.\n";
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // seed the random number generator to get different results each time
    srand(rank);

    if ( rank == 0 )
    {
        loadInputFile( nTimeSteps, maxThrust, receiveArray, initSpeedVector, initDirVector );
    }
    
    // Broadcast all the initial variables
    MPI_Bcast( &nTimeSteps, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( &maxThrust, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( &receiveArray, SLEN*SNUM, MPI_DOUBLE, 0, MPI_COMM_WORLD );
    MPI_Bcast( &initSpeedVector, SNUM, MPI_DOUBLE, 0, MPI_COMM_WORLD );
    MPI_Bcast( &initDirVector, 3*SNUM, MPI_DOUBLE, 0, MPI_COMM_WORLD );

    // loop through the number of time steps
    for (int round = 1; round < nTimeSteps; ++round)
    {
        if (rank == 0)
        {
            // calculate Buzzy new location
            CalculateBuzzyXYZ(receiveArray, initSpeedVector, initDirVector, sendArray);

            for ( int i = 1; i < SNUM; i++ ) 
            {
                int idx = ( i * SLEN );
                cout << i << ", " << (int)receiveArray[idx] << ", ";
                cout << receiveArray[idx+1] << ", " << receiveArray[idx+2] << ", " << receiveArray[idx+3] << ", ";
                cout << receiveArray[idx+4] << ", " << receiveArray[idx+5] << ", " << receiveArray[idx+6] << "\n";
            }
        }
        else
        {
            // calculate yellow jacket new location
            CalculateYellowJacketXYZ(receiveArray, rank, sendArray);
        }
        
        MPI_Allgather( sendArray, SLEN, MPI_DOUBLE, receiveArray, SLEN, MPI_DOUBLE, MPI_COMM_WORLD );
    }

    MPI_Finalize();
}