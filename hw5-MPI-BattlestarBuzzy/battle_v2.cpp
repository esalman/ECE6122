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
#include <cmath>
using namespace std;

// status
#define ACTIVE 1
#define DOCKED 2
#define DESTROYED 0

// sendArray length: status + 3x location + 3x thrust + 3x velocity
#define SLEN 10
// number of sendArrays 
#define SNUM 8

// other constants
#define MASS 10000
#define RMIN 0.8
#define RMAX 1.2

#define PRECISION 6

// load data from in.dat
// Outputs:
    // nTimeSteps
    // maxThrust
    // receiveArray: status + 3x location + 3x thrust + 3x velocity
void loadInputFile( int &nTimeSteps, double &maxThrust, double receiveArray[] )
{
    int xyzCount = 0;
    double dirX, dirY, dirZ;
    double speed;

    ifstream fid ("in.dat");
    
    fid >> nTimeSteps;
    fid >> maxThrust;

    for ( int i = 0; i < SNUM; i++ ) 
    {
        receiveArray[xyzCount++] = ACTIVE;
        // xyz
        fid >> receiveArray[xyzCount++];
        fid >> receiveArray[xyzCount++];
        fid >> receiveArray[xyzCount++];
        // force
        // buzzy has not thrust
        if ( i == 0 )
        {
            receiveArray[xyzCount++] = 0;
            receiveArray[xyzCount++] = 0;
            receiveArray[xyzCount++] = 0;
        }
        else {
            receiveArray[xyzCount++] = maxThrust;
            receiveArray[xyzCount++] = maxThrust;
            receiveArray[xyzCount++] = maxThrust;
        }
        // velocity
        fid >> speed;
        fid >> dirX;
        fid >> dirY;
        fid >> dirZ;
        receiveArray[xyzCount++] = speed * dirX;
        receiveArray[xyzCount++] = speed * dirY;
        receiveArray[xyzCount++] = speed * dirZ;
    }
    fid.close();
}

// Calculate Buzzy location and velocity
// Input:
//     receiveArray: first 10 elements of receiveArray contain buzzy previous step info
// Output:
//     sendArray: new buzzy location and velocity. status=1, force=0.
void CalculateBuzzyXYZ( double receiveArray[], double sendArray[] )
{
    // previous location
    double x0 = receiveArray[1];
    double y0 = receiveArray[2];
    double z0 = receiveArray[3];
    // previous speed
    double vx = receiveArray[7];
    double vy = receiveArray[8];
    double vz = receiveArray[9];

    // calculate new location
    // t=1s in the formula here
    double x = x0 + vx;
    double y = y0 + vy;
    double z = z0 + vz;

    // update buzzy location
    sendArray[1] = x;
    sendArray[2] = y;
    sendArray[3] = z;
    // set buzzy thrust
    sendArray[4] = 0;
    sendArray[5] = 0;
    sendArray[6] = 0;
    // update buzzy speed
    sendArray[7] = receiveArray[7];
    sendArray[8] = receiveArray[8];
    sendArray[9] = receiveArray[9];
}

// Determine target thrust/force in a particular dimension based on current location/velocity
// of the yellow jacket and buzzy. currently everything crash but it could be improved
// Input:
//     receiveArray: basaed on yj rank, 10 elements of receiveArray contain yj previous step info
//     rank
//     maxThrust
// Output:
//     sendArray: new yj location, velocity and force.
double targetForce( double x0, double x, double maxThrust, double v0, double vb )
{
    double targetF;

    // what velocity I want to target
    double targetV = v0 + ( maxThrust / MASS );

    // if new location based on target velocity is still far away from buzzy, apply max thrust
    if ( abs( x0 - x ) > targetV )
    {
        if ( x0 - x > 0 )
        {
            targetF = maxThrust;
        }
        else
        {
            targetF = -maxThrust;
        }
    }
    else
    {
        // otherwise calculate target thrust 
        // target velocity
        targetV = x0 - x;
        // target acceleration
        double a = targetV - v0;
        // target thrust
        targetF = MASS * a;
    }
    return targetF;
}

// Calculate yellow jacket (yj) location, force and velocity
// Input:
//     receiveArray: basaed on yj rank, 10 elements of receiveArray contain yj previous step info
//     rank
//     maxThrust
// Output:
//     sendArray: new yj location, velocity and force.
void CalculateYellowJacketXYZ( double receiveArray[], int rank, double maxThrust, double sendArray[] )
{
    // grab yj index for the receiveArray
    int idx = rank * SLEN;

    // dead/docked logic
    if ( receiveArray[idx] != 1 )
    {
        copy( receiveArray+idx, receiveArray+idx+SLEN, sendArray );
        return;
    }
    
    double vx0 = receiveArray[idx+7]; // yj initial x velocity
    double vxb = receiveArray[7]; // buzzy initial x velocity
    double x0 = receiveArray[idx+1]; // yj initial x location
    double xb = receiveArray[1]; // buzzy initial x location
    
    // force vector determination logic
    double targetFx = targetForce( xb, x0, maxThrust, vx0, vxb );

    // add random noise to force
    double rx = RMIN + ( static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(RMAX-RMIN))) );
    double fx = rx * targetFx;
    if ( fx > maxThrust ) fx = maxThrust;
    if ( fx < -maxThrust ) fx = -maxThrust;
    // get new position and speed of yj
    // t=1s in the formula here
    double ax = fx / MASS;
    double vx = vx0 + ax;
    double x =  x0 + vx + 0.5 * ax;

    double vy0 = receiveArray[idx+8]; // yj initial x velocity
    double vyb = receiveArray[8]; // buzzy initial x velocity
    double y0 = receiveArray[idx+2]; // yj initial x location
    double yb = receiveArray[2]; // buzzy initial x location
    
    // force vector determination logic
    double targetFy = targetForce( yb, y0, maxThrust, vy0, vyb );

    // same for y
    double ry = RMIN + ( static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(RMAX-RMIN))) );
    double fy = ry * targetFy;
    if ( fy > maxThrust ) fy = maxThrust;
    if ( fy < -maxThrust ) fy = -maxThrust;
    double ay = fy / MASS;
    double vy = vy0 + ay;
    double y = y0 + vy + 0.5 * ay;

    double vz0 = receiveArray[idx+9]; // yj initial x velocity
    double vzb = receiveArray[9]; // buzzy initial x velocity
    double z0 = receiveArray[idx+3]; // yj initial x location
    double zb = receiveArray[3]; // buzzy initial x location
    
    // force vector determination logic
    double targetFz = targetForce( zb, z0, maxThrust, vz0, vzb );

    // same for z
    double rz = RMIN + ( static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(RMAX-RMIN))) );
    double fz = rz * targetFz;
    if ( fz > maxThrust ) fz = maxThrust;
    if ( fz < -maxThrust ) fz = -maxThrust;
    double az = fz / MASS;
    double vz = vz0 + az;
    double z = z0 + vz + 0.5 * az;

    // retain status, set new location, thrust and speed
    sendArray[0] = receiveArray[idx];
    sendArray[1] = x;
    sendArray[2] = y;
    sendArray[3] = z;
    sendArray[4] = fx;
    sendArray[5] = fy;
    sendArray[6] = fz;
    sendArray[7] = vx;
    sendArray[8] = vy;
    sendArray[9] = vz;
}

// check if any yj collided with each other in the previous step
// Input:
//     receiveArray: based on yj rank, 10 elements of receiveArray contain yj previous step info
//     rank: yj rank
// Output:
//     receiveArray: appropriate status indices in receiveArray updated 
void collisionStatus( double receiveArray[], int rank )
{
    int idxa = rank * SLEN;
    
    // is already dead? docked?
    if ( receiveArray[idxa] != 1 )
    {
        return;
    }

    // get yj location
    double xa = receiveArray[idxa+1];
    double ya = receiveArray[idxa+2];
    double za = receiveArray[idxa+3];

    // compare will all other yj location
    for ( int i = 1; i <= SNUM; i++ )
    {
        // can't collide with itself
        if ( i == rank )
        {
            continue;
        }

        // get other yj index
        int idxb = i * SLEN;

        // is the other yj dead?
        if ( receiveArray[idxb] != 1 )
        {
            continue;
        }

        // get the other yj location
        double xb = receiveArray[idxb+1];
        double yb = receiveArray[idxb+2];
        double zb = receiveArray[idxb+3];

        // distance calculation, collision logic
        double dist = sqrt( pow( xa-xb, 2 ) + pow( ya-yb, 2 ) + pow( za-zb, 2 ) );
        if ( dist < 250 )
        {
            // cout << "destroyed\n";
            // exit(DESTROYED);
            receiveArray[idxa] = DESTROYED;
            receiveArray[idxb] = DESTROYED;
        }
    }
}

// check if any yj collided with buzzy in the previous step
// Input:
//     receiveArray: based on yj rank, 10 elements of receiveArray contain yj previous step info.
//         first 10 elements contain buzzy info
//     rank: yj rank
// Output:
//     receiveArray: appropriate status indices in receiveArray updated 
void dockStatus( double receiveArray[], int rank )
{
    // which yj
    int idxa = rank * SLEN;
    
    // is already dead? docked?
    if ( receiveArray[idxa] != 1 )
    {
        return;
    }
    
    // get location of yj
    double xa = receiveArray[idxa+1];
    double ya = receiveArray[idxa+2];
    double za = receiveArray[idxa+3];

    // get velocity  of yj
    double vxa = receiveArray[idxa+7];
    double vya = receiveArray[idxa+8];
    double vza = receiveArray[idxa+9];

    // buzzy location
    double xb = receiveArray[1];
    double yb = receiveArray[2];
    double zb = receiveArray[3];

    // buzzy velocity
    double vxb = receiveArray[7];
    double vyb = receiveArray[8];
    double vzb = receiveArray[9];

    // get distance between buzzy and yj
    double dist = sqrt( pow( xa-xb, 2 ) + pow( ya-yb, 2 ) + pow( za-zb, 2 ) );
    // angle calculation
    double dotProd = vxa * vxb + vya * vyb + vza * vzb;
    double abs_va = sqrt( pow( vxa, 2 ) + pow( vya, 2 ) + pow( vza, 2 ) );
    double abs_vb = sqrt( pow( vxb, 2 ) + pow( vyb, 2 ) + pow( vzb, 2 ) );
    double cosTheta = dotProd / ( abs_va * abs_vb );
    
    if ( dist < 50 )
    {
        if ( abs_va < 1.1*abs_vb && cosTheta > 0.8 )
        {
            // cout << "DOCKED " << rank << "\n";
            // exit(DOCKED);
            receiveArray[idxa] = DOCKED;
        }
        else
        {
            // cout << "DESTROYED " << rank << "\n";
            // exit(DESTROYED);
            receiveArray[idxa] = DESTROYED;
        }
    }
}

void reportStatus( double receiveArray[] )
{
    for ( int i = 1; i < SNUM; i++ ) 
    {
        int idx = ( i * SLEN );
        cout << i << ", " << (int)receiveArray[idx] << ", ";
        cout << receiveArray[idx+1] << ", " << receiveArray[idx+2] << ", " << receiveArray[idx+3] << ", ";
        cout << receiveArray[idx+4] << ", " << receiveArray[idx+5] << ", " << receiveArray[idx+6] << "\n";
    }
}

// main function executed in all MPI processes, different functionality based on rank
int main( int argc, char *argv[] )
{
    int rank, rc;
    int nTimeSteps;
    double maxThrust;
    double sendArray[SLEN];
    double receiveArray[SLEN*SNUM];
    int precision = PRECISION;

    // set floating point output precision
    cout.precision(precision);
    cout << scientific;

    // initialize MPI
    rc = MPI_Init(&argc, &argv);
    if (rc != MPI_SUCCESS)
    {
        cout << "Error starting MPI program. Terminating.\n";
        MPI_Abort(MPI_COMM_WORLD, rc);
    }
    
    // get rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // seed the random number generator
    srand(rank);

    // load input data in the main process
    if ( rank == 0 )
    {
        loadInputFile( nTimeSteps, maxThrust, receiveArray );
    }
    
    // broadcast all the initial variables
    MPI_Bcast( &nTimeSteps, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( &maxThrust, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD );
    MPI_Bcast( &receiveArray, SLEN*SNUM, MPI_DOUBLE, 0, MPI_COMM_WORLD );

    // loop through the number of time steps
    for (int round = 1; round <= nTimeSteps; ++round)
    {
        if ( rank == 0 )
        {
            // calculate Buzzy new location
            CalculateBuzzyXYZ(receiveArray, sendArray);
            
            reportStatus( receiveArray );
        }
        else
        {
            // update yj status
            collisionStatus(receiveArray, rank);
            dockStatus(receiveArray, rank);

            // calculate yellow jacket new location
            CalculateYellowJacketXYZ(receiveArray, rank, maxThrust, sendArray);
        }
        
        // gather all status, location, force and velocity
        MPI_Allgather( sendArray, SLEN, MPI_DOUBLE, receiveArray, SLEN, MPI_DOUBLE, MPI_COMM_WORLD );

        // if all yj is dead/docked, abort
        int undead = 0;
        for ( int i = 1; i < SNUM; i++ )
        {
            int idx = ( i * SLEN );
            if ( receiveArray[idx] == 1 )
            {
                undead += 1;
            }
        }
        if ( undead == 0 )
        {
            // print final status
            if ( rank == 0 ) 
            {
                reportStatus( receiveArray );
            }
            MPI_Abort(MPI_COMM_WORLD, 0);
        }
    }

    MPI_Finalize();
    return 0;
}