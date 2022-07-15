/*
Author:         Mustafa Salman
Class:          ECE 6122
Last modified:  09/22/2019

Description:    This program takes as a command line argument the name of a data 
                file containing a triangle. Then it finds by starting at the top 
                of the triangle below and moving to adjacent numbers on the row 
                below, the maximum total from top to bottom. Output is written to
                output3.txt.
*/

#include <fstream>
#include <iostream>
#include <vector>
#include <assert.h> // for checking if the input data file name has been specified
using namespace std;

int main( int argc, char** argv )
{
    int nrow, sum, t1;
    int maxSum = 0;
    vector< vector< int > > triangle;
    vector< int > row;

    assert( argc >= 2 );    // for checking if the input data file name has been specified
    ifstream fid ( argv[1] );
    
    // read the triangle row number and values from file
    fid >> nrow;
    for ( int i = 0; i < nrow; i++ ) 
    {
        row.clear();
        for ( int j = 0; j <= i; j++ ) 
        {
            fid >> t1;
            row.push_back( t1 );
        }
        triangle.push_back( row );
    }
    fid.close();

    // solve with dynamic programming: starting from bottom row iterate through
    // consecutive pairs to find which has larger sum and assign to the previous row
    for ( int i = nrow-1; i >= 0; i-- ) 
    {
        for ( int j = 0; j < i; j++ ) 
        {
            triangle[i-1][j] += max( triangle[i][j], triangle[i][j+1] );
        }
    }

    // output to file
    ofstream fod;
    fod.open ("output3.txt");
    fod << triangle[0][0];
    fod.close();

    return 0;
}
