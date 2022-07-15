/*
Author:         Mustafa Salman
Class:          ECE 6122
`Last modified:  09/22/2019

Description:    This program takes as a command line argument the name of a data 
                file containing a triangle. Then it finds by starting at the top 
                of the triangle below and moving to adjacent numbers on the row 
                below, the maximum total from top to bottom. Output is written to
                output3.txt.
*/

#include <assert.h> // for checking if the input data file name has been specified
#include <cstring>
#include <fstream>
#include <future>
#include <iostream>
#include <thread>
#include <vector>
using namespace std;

// function for determining n choose k
// provides a closed form solution to the lattice path problem
void nChooseK( unsigned long long n, unsigned long long k, promise<unsigned long long> && p )
{
    if ( k > n )
    {
        p.set_value( 0 );
    }
    else 
    {
        unsigned long long ret = 1;
        for ( unsigned long long i = 1; i <= k; i++ ) 
        {
            ret *= n--;
            ret /= i;
        }
        p.set_value( ret );
    }    
}

int main( int argc, char** argv )
{
    int h, w, k;
    
    assert( argc == 5 );    // for checking if the input has been specified properly
    if ( strcmp( argv[1], "-h" ) == 0 ) h = stoi(argv[2]);
    if ( strcmp( argv[1], "-w" ) == 0 ) w = stoi(argv[2]);
    if ( strcmp( argv[3], "-h" ) == 0 ) h = stoi(argv[4]);
    if ( strcmp( argv[3], "-w" ) == 0 ) w = stoi(argv[4]);
    
    k = min( h, w );

    promise<unsigned long long> p1;
    future<unsigned long long> f1 = p1.get_future();
    // spawn a new thread which calls nChooseK
    thread foo1 ( &nChooseK, w+h, k, move(p1) );

    // syncronize threads
    foo1.join();
    // retrieve result
    auto res = f1.get();
    
    // output to file
    ofstream fod;
    fod.open ("output1.txt");
    fod << res << '\n';
    fod.close();

    return 0;
}
